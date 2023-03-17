using IOT.Monitoring;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Channels;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Input;
using System.Windows.Threading;

namespace IoT1.Model
{
    class StreamPackets : ICommand
    {
        private ConnectViewModel connectViewModel;
        private readonly PacketModel packetModel;
        private static BackgroundWorker bgWorker;
        private static CancellationTokenSource source;

        public StreamPackets(ConnectViewModel connectViewModel, PacketModel packetModel)
        {
            this.connectViewModel = connectViewModel;
            this.packetModel = packetModel;
            this.packetModel.PropertyChanged += OnViewModelChanged;
            this.connectViewModel.PropertyChanged += OnViewModelChanged;
            bgWorker = new BackgroundWorker
            {
                WorkerReportsProgress = true,
                WorkerSupportsCancellation = true,
            };


            bgWorker.ProgressChanged += ProgressChanged;
            bgWorker.RunWorkerCompleted += WorkCompleted;
        }

        private void OnViewModelChanged(object sender, PropertyChangedEventArgs e)
        {

            CanExecuteChanged?.Invoke(this, new EventArgs());

        }

        public void CancelSource()
        {
            source.Cancel();
            bgWorker.DoWork -= ReadAllPackets;
            
        }

        public bool IsExecuting { get; private set; }

        public event EventHandler CanExecuteChanged;

        public bool CanExecute(object parameter)
        {
            return !IsExecuting;
        }

        public async void Execute(object parameter)
        {
            try
            {
                IsExecuting = true;
                await ExecuteAsync(parameter);
            }
            finally
            {
            }
        }

        public Task ExecuteAsync(object parameter)
        {
            
            if (connectViewModel.client == null || bgWorker.IsBusy)
                return Task.FromResult(false);

            bgWorker.DoWork += ReadAllPackets;
            bgWorker.RunWorkerAsync(1);
            
            return Task.FromResult(true);
        }

        void ReadAllPackets(object sender, DoWorkEventArgs args)
        {

            Channel<Packet> buffer = Channel.CreateUnbounded<Packet>();

            source = new CancellationTokenSource();

            _ = Task.Run(async () =>
            {
                try
                {
                    await connectViewModel.client.ReadPackets(source.Token, buffer.Writer);
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

            var buf = (ChannelReader<Packet>)args.Result;


            Application.Current.Dispatcher.BeginInvoke(DispatcherPriority.Normal, new Action(async () =>
            {
                while(await buf.WaitToReadAsync())
                {
                    var data = await buf.ReadAsync();

                    packetModel.AddPacket(data.Id, data);
                                        
                }
                IsExecuting = false;

            }));

        }
        
        
    }
}
