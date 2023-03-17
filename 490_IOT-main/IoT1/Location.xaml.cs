using System;
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
using Microsoft.Maps.MapControl.WPF;

namespace IoT1
{
    /// <summary>
    /// Interaction logic for Location.xaml
    /// </summary>
    public partial class Location : Page
    {
        public Location()
        {
            InitializeComponent();
        }

        private void Update_Click(object sender, RoutedEventArgs e)
        {
            double latitude, longitude;

            if(double.TryParse(LatitudeTextBox.Text,out latitude) && double.TryParse(LongitudeTextBox.Text,out longitude))
            {
               Microsoft.Maps.MapControl.WPF.Location location = new Microsoft.Maps.MapControl.WPF.Location(latitude, longitude);
                TestLocation.Center= location;
                TestLocation.ZoomLevel= 20;

                Pushpin pushpin= new Pushpin();
                pushpin.Location = location;
                TestLocation.Children.Add(pushpin);
            }
        }
    }
}
