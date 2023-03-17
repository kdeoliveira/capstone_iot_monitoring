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
using System.Windows.Shapes;

namespace IoT1
{
    /// <summary>
    /// Interaction logic for UserInputWindow.xaml
    /// </summary>
    public partial class UserInputWindow : Window
    {
        private readonly ObservableCollection<User> _users;
        private readonly UserPage _userPage;

        public UserInputWindow(ObservableCollection<User> users, UserPage userPage)
        {
            InitializeComponent();
            _users = users;
            _userPage = userPage;
            this.Owner = Application.Current.MainWindow;
        }


        private void BtnSave_Click(object sender, RoutedEventArgs e)
        {
            // Validate the input fields
            if (string.IsNullOrEmpty(txtFirstName.Text))
            {
                MessageBox.Show("Please enter a first name.");
                return;
            }
            if (string.IsNullOrEmpty(txtLastName.Text))
            {
                MessageBox.Show("Please enter a last name.");
                return;
            }
            int agentID;
            if (!int.TryParse(txtAgentID.Text, out agentID))
            {
                MessageBox.Show("Please enter a valid agent ID.");
                return;
            }
            int stationNumber;
            if (!int.TryParse(txtStationNumber.Text, out stationNumber))
            {
                MessageBox.Show("Please enter a valid station number.");
                return;
            }

            // Create a new instance of the User class
            var user = new User
            {
                FirstName = txtFirstName.Text,
                LastName = txtLastName.Text,
                AgentID = agentID,
                StationNumber = stationNumber
            };

            _userPage.AddUser(user);

            // Close the dialog box
            Close();
        }



        private void BtnCancel_Click(object sender, RoutedEventArgs e)
        {
            Close();
        }
    }


}
