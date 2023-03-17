using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Windows;

namespace IoT1
{
    public class NotificationsViewModel : INotifyPropertyChanged
    {
        private ObservableCollection<Notification> _notifications = new ObservableCollection<Notification>();

        public ObservableCollection<Notification> Notifications
        {
            get { return _notifications; }
            set
            {
                _notifications = value;
                OnPropertyChanged(nameof(Notifications));
            }
        }

        public void AddNotification(Notification notification)
        {
            Application.Current.Dispatcher.Invoke(() =>
            {
                Notifications.Insert(0, notification);
            });
        }

        public event PropertyChangedEventHandler PropertyChanged;

        protected void OnPropertyChanged(string propertyName)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }
    }

}
