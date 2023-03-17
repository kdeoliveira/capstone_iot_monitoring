using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
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

namespace IoT1
{
    /// <summary>
    /// Interaction logic for UserPage.xaml
    /// </summary>
    public partial class UserPage : Page
    {
        private ObservableCollection<User> _users = new ObservableCollection<User>();

        public UserPage()
        {
            InitializeComponent();
            DataContext = this;
            Users = new ObservableCollection<User>();
        }

        public ObservableCollection<User> Users { get; set; } = new ObservableCollection<User>();

        private void btnAddUser_Click(object sender, RoutedEventArgs e)
        {
            var userInputWindow = new UserInputWindow(Users, this);
            userInputWindow.Show();
        }

        public void AddUser(User user)
        {
            Users.Add(user);
        }
    }

}
