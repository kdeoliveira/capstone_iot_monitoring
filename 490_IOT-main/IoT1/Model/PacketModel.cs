using IOT.Monitoring;
using NSwag.Collections;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace IoT1.Model
{
    public class PacketModel : INotifyPropertyChanged
    {
        public ObservableDictionary<int, Packet> _packets = new ObservableDictionary<int, Packet>();


        public PacketModel()
        {
        }

        public Packet GetPackets(int id)
        {
            if(this._packets.ContainsKey(id))
                return this._packets[id];
            return new Packet();
        }

        public void AddPacket(int id, Packet packet)
        {
            this._packets[id] = packet;

            PropertyChanged?.Invoke(id, new PropertyChangedEventArgs(packet.Data.ToStringUtf8()));
        }

        #region INotifyPropertyChanged Members

        public event PropertyChangedEventHandler PropertyChanged;

        #endregion
    }
}
