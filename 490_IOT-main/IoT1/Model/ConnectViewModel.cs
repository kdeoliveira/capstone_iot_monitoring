using GrpcClient;
using IOT.Monitoring;
using NSwag.Collections;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Input;

namespace IoT1.Model
{
    public class ConnectViewModel : INotifyPropertyChanged
    {
        public Client client = null;

        public PacketModel packetModel;

        public ConnectViewModel(PacketModel packetModel)
        {
            _connectTogRPC = new Connection(this);
            _StartStopComm = new StreamPackets(this, packetModel);
            _StopEndpoint = new StopConnection(this);
            this.packetModel = packetModel;
        }

        ~ConnectViewModel()
        {
        }

        public event PropertyChangedEventHandler PropertyChanged;

        public void NotifyPropertyChanged(string propertyName)
        {
            this.PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }

        private string _IpAddress = "192.168.2.156";

        public string IpAddress
        {
            get { return _IpAddress; }
            set { 
                _IpAddress = value;
                NotifyPropertyChanged(nameof(IpAddress));
            }
        }

        private bool _IpAddressEnabled = true;

        public bool IpAddressEnabled
        {
            get { return _IpAddressEnabled; }
            set { 
                _IpAddressEnabled = value; 
                NotifyPropertyChanged(nameof(IpAddressEnabled));
            }
        }

        private ICommand _connectTogRPC;

        public ICommand ConnectTogRPC
        {
            get
            {
                return _connectTogRPC;
            }
        }

        private ICommand _StartStopComm;

        public ICommand StartStopComm
        {
            get
            {
                return _StartStopComm;
            }
        }

        private ICommand _StopEndpoint;

        public ICommand StopEndpoint
        {
            get
            {
                return _StopEndpoint;
            }
        }




    }
}
