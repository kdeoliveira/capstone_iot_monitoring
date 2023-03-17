using System;
using System.Collections.ObjectModel;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using GrpcClient;
using System.ComponentModel;
using System.Windows.Threading;
using IOT.Monitoring;
using System.Threading.Channels;
using System.Threading;
using System.IO.Ports;
using System.Net.Sockets;
using System.Runtime.Remoting.Contexts;

namespace IoT1
{
    /// <summary>
    /// Interaction logic for LandingWindow.xaml
    /// </summary>
    public partial class LandingWindow : Window
    {
        public LandingWindow()
        {
            InitializeComponent();

            co_2.Text = "419.03 ppm";
            temp.Text = "28 C";
            oxygen.Text = "85 mm";


            bgWorker = new BackgroundWorker
            {
                WorkerReportsProgress = true,
                WorkerSupportsCancellation = true,
            };


            bgWorker.ProgressChanged += ProgressChanged;
            bgWorker.RunWorkerCompleted += WorkCompleted;
            


        }

        private void Button_Click_Dashboard(object sender, RoutedEventArgs e)
        {
            LandingWindow landingWindow = new LandingWindow();
            landingWindow.Show();
            this.Close();
        }

        struct connstatus
        {
            public string msg;
            public bool active;
        }
        static Client client = null;
        private static BackgroundWorker bgWorker;
        private static CancellationTokenSource source = null;


        private void Connect_Click(object sender, RoutedEventArgs e)
        {

            if (endpoint_addr.Text.Length == 0 || bgWorker.IsBusy)
            {
                return;
            }

            bgWorker.DoWork += ConnectToClient;

            bgWorker.RunWorkerAsync(endpoint_addr.Text);

        }


        //bgWorker tasks
        void ConnectToClient(object sender, DoWorkEventArgs args)
        {

            Dispatcher.BeginInvoke(new Action(() => {
                endpoint_conn_status.Content = "";
                endpoint_addr.IsEnabled = false;
                Connect.IsEnabled = false;
            }), DispatcherPriority.Background);

            var addr = (string)args.Argument;
            addr += ":6501";
            client = new Client(addr);
            connstatus result = new connstatus { };
            try
            {
                var res = client.GetServerId();

                result.msg = res;
                result.active = true;
            }
            catch (Exception e)
            {
                result.msg = e.Message;
                result.active = false;
            }

            args.Result = result;

        }

        void ReadDevicePackets(object sender, DoWorkEventArgs args)
        {

            if (client == null)
                return;
            var id = (int)args.Argument;

            List<Packet> response = new List<Packet>();

            try
            {
                response = client.GetListOfPackets(id);

            }
            catch (Exception e)
            {
                Console.WriteLine(e.Message);
            }

            args.Result = response;

        }

        void ReadAllPackets(object sender, DoWorkEventArgs args)
        {

            if (client == null)
                return;
            Channel<Packet> buffer = Channel.CreateUnbounded<Packet>();

            source = new CancellationTokenSource();

            _ = Task.Run(async () =>
            {
                try
                {
                    await client.ReadPackets(source.Token, buffer.Writer);
                }
                catch (Exception e)
                {
                    Console.WriteLine(e.Message);
                }
                finally
                {
                    source.Cancel(false);
                    buffer.Writer.TryComplete();
                }
            });

            args.Result = buffer.Reader;
        }

        static void ProgressChanged(object sender, ProgressChangedEventArgs args)
        {

        }

        void WorkCompleted(object sender, RunWorkerCompletedEventArgs args)
        {
            if (args.Result == null)
                return;

            if (typeof(connstatus) == args.Result.GetType())
            {
                var res = (connstatus)args.Result;
                Dispatcher.BeginInvoke(new Action(() => {

                    if (res.active)
                    {
                        var bc = new BrushConverter();
                        endpoint_conn_status.Content = "Connected -- Server v" + res.msg;
                        endpoint_conn_status.Background = (Brush)bc.ConvertFrom("#00FF00");

                    }
                    else
                    {
                        endpoint_addr.IsEnabled = true;
                        endpoint_conn_status.Content = res.msg;
                        Connect.IsEnabled = true;
                    }
                }), DispatcherPriority.Background);
                bgWorker.DoWork -= ConnectToClient;
            }
            else if (typeof(List<Packet>) == args.Result.GetType())
            {
                var res = (List<Packet>)args.Result;

                TextBox[] textBoxes = {  co_2, temp, oxygen };

                Dispatcher.BeginInvoke(new Action(() => {

                    if (res.Count == 0)
                    {
                        return;
                    }
                    else
                    {
                        for (int i = 0; i < res.Count(); i++)
                        {
                            textBoxes[res[i].Id % textBoxes.Length].Text = res[i].Data.ToStringUtf8();
                        }
                    }
                }), DispatcherPriority.Background);

            }
            else
            // if(typeof(ChannelReader<Packet>) == args.Result.GetType())
            {
                var buf = (ChannelReader<Packet>)args.Result;

                _ = Dispatcher.BeginInvoke(new Action(async () =>
                {
                    TextBox[] textBoxes = { co_2, temp, oxygen };
                    //Ellipse[] ellipses = { heart_rate_st, co_2_st, temp_st, oxygen_st };
                    var bc = new BrushConverter();


                    int i = 0;
                    for (int j = 0; j < textBoxes.Length; j++)
                    {
                        textBoxes[j].IsEnabled = false;
                        //ellipses[j].Fill = (Brush)bc.ConvertFrom("#00FF00");
                    }
                    while (await buf.WaitToReadAsync())
                    {
                        var data = await buf.ReadAsync();
                        textBoxes[data.Id % textBoxes.Length].Text = data.Data.ToStringUtf8();
                    }

                    for (int j = 0; j < textBoxes.Length; j++)
                    {
                        textBoxes[j].IsEnabled = true;
                        // ellipses[j].Fill = (Brush)bc.ConvertFrom("#FF0000");
                    }
                    StartBtn.IsEnabled = true;
                    StopBtn.IsEnabled = false;

                }), DispatcherPriority.Normal);

            }





        }

        private void endpoint_addr_MouseEnter(object sender, MouseEventArgs e)
        {
            endpoint_addr.Text = "";
        }

        private void StartBtn_Click(object sender, RoutedEventArgs e)
        {
            if (endpoint_addr.IsEnabled || bgWorker.IsBusy)
            {
                return;
            }
            Dispatcher.BeginInvoke(new Action(() => {
                StartBtn.IsEnabled = false;
                StopBtn.IsEnabled = true;
            }), DispatcherPriority.Background);

            bgWorker.DoWork += ReadAllPackets;

            bgWorker.RunWorkerAsync(1);
        }

        private void StopBtn_Click(object sender, RoutedEventArgs e)
        {
            if (source == null)
                return;
            source.Cancel();
            bgWorker.DoWork -= ReadAllPackets;
        }

        private void endpoint_addr_TextChanged(object sender, TextChangedEventArgs e)
        {

        }
        
    }
}

