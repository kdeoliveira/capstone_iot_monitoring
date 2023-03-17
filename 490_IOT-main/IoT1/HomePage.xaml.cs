using IoT1.Model;
using System;
using System.Collections.Generic;
using System.ComponentModel;
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
using System.Windows.Threading;

namespace IoT1
{
    /// <summary>
    /// Interaction logic for HomePage.xaml
    /// </summary>
    public partial class HomePage : Page
    {
        public HomePage(ConnectViewModel ctx)
        {
            InitializeComponent();

            DataContext = ctx;

            
            ctx.packetModel.PropertyChanged += new PropertyChangedEventHandler((x, y) =>
            {
                Application.Current.Dispatcher.BeginInvoke(DispatcherPriority.Background, new Action(() => {
                    switch ((int)x)
                    {
                        case 0:
                            heart_rate.Text = String.Format("{0:0.## BPM}", float.Parse(y.PropertyName));
                            break;
                        case 1:
                            co_2.Text = String.Format("{0:0.## ppm}", float.Parse(y.PropertyName));
                            break;
                        case 2:
                            temp.Text = String.Format("{0:0.## C}", float.Parse(y.PropertyName));
                            break;
                        case 4:
                            oxygen.Text = String.Format("{0:0.## mm}", float.Parse(y.PropertyName));
                            break;
                    }
                }));
                
            });


            //heart_rate.Text = packet.GetPackets(0).Data.ToStringUtf8();
            //co_2.Text = "419.03 ppm";
            //temp.Text = "28 C";
            //oxygen.Text = "85 mm";

            // Subscribe to the TextChanged event of the heart rate TextBox
            heart_rate.TextChanged += HeartRate_TextChanged;
        }
        private void HeartRate_TextChanged(object sender, TextChangedEventArgs e)
        {
            // Parse the heart rate value
            if (int.TryParse(heart_rate.Text.Replace("BPM", "").Trim(), out int heartRate))
            {
                // Check if heart rate is below 60 or above 140
                if (heartRate < 60 || heartRate > 140)
                {
                    // Set text color to red
                    heart_rate.Foreground = Brushes.Red;
                }
                else
                {
                    // Set text color to default
                    heart_rate.Foreground = Brushes.Black;
                }
            }
        }





    }
}
