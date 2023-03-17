using GrpcClient;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Input;

namespace IoT1.Model
{

    class Connection : ICommand
    {
        struct connstatus
        {
            public string msg;
            public bool active;
        }


        private readonly ConnectViewModel _connectViewModel;

        public event EventHandler CanExecuteChanged;

        public Connection(ConnectViewModel connectViewModel)
        {
            _connectViewModel = connectViewModel;

            _connectViewModel.PropertyChanged += OnViewModelChanged;
            
        }

        ~Connection()
        {
            Console.WriteLine("Destructor");

        }

        private void OnViewModelChanged(object sender, PropertyChangedEventArgs e)
        {
           
           CanExecuteChanged?.Invoke(this, new EventArgs());
            
        }

        public bool CanExecute(object parameter)
        {
            return _connectViewModel.client == null;
        }

        public void Execute(object parameter)
        {
            if (_connectViewModel.client != null)
                return;
            try
            {

                var addr = _connectViewModel.IpAddress;
                addr += ":6501";
                _connectViewModel.client = new Client(addr);

                _ = _connectViewModel.client.GetServerId();
                                

                _connectViewModel.IpAddressEnabled = false;

            }
            catch(Exception e)
            {
                _ = MessageBox.Show(e.Message.ToString(),"Error connecting to endpoint", MessageBoxButton.OK, MessageBoxImage.Error);
                _connectViewModel.client = null;
            }

        }
    }


    class StopConnection : ICommand
    {
        struct connstatus
        {
            public string msg;
            public bool active;
        }


        private readonly ConnectViewModel _connectViewModel;

        public event EventHandler CanExecuteChanged;

        public StopConnection(ConnectViewModel connectViewModel)
        {
            _connectViewModel = connectViewModel;

            _connectViewModel.PropertyChanged += OnViewModelChanged;

        }

        private void OnViewModelChanged(object sender, PropertyChangedEventArgs e)
        {

            CanExecuteChanged?.Invoke(this, new EventArgs());

        }

        public bool CanExecute(object parameter)
        {
            return _connectViewModel.client != null;
        }

        public void Execute(object parameter)
        {
            if (_connectViewModel.client == null)
                return;
            try
            {
                ((StreamPackets)_connectViewModel.StartStopComm).CancelSource();
                _connectViewModel.client.CloseConnection();
                _connectViewModel.client = null;
                _connectViewModel.IpAddressEnabled = true;
            }
            catch (Exception e)
            {
                _ = MessageBox.Show(e.Message.ToString(), "Error connecting to endpoint", MessageBoxButton.OK, MessageBoxImage.Error);
                _connectViewModel.client = null;
            }

        }
    }
}
