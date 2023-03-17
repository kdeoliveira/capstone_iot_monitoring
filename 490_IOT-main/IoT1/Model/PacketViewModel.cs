using IOT.Monitoring;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace IoT1.Model
{
    class PacketViewModel : INotifyPropertyChanged
    {
        public PacketViewModel()
        {
            _packets = new ObservableCollection<PacketModel>();

        }

        public event PropertyChangedEventHandler PropertyChanged;

        public void NotifyPropertyChanged(string propertyName)
        {
            this.PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }


        private readonly ObservableCollection<PacketModel> _packets;

        public IEnumerable<PacketModel> Packet => _packets;


    }
}
