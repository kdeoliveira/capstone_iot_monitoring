using System;
using System.Collections.Generic;
using System.Data.SqlClient;
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
    /// Interaction logic for LoginPage.xaml
    /// </summary>
    public partial class LoginPage : Page
    {
        public LoginPage()
        {
            InitializeComponent();
        }

        private void btnSubmit_Click(object sender, RoutedEventArgs e)
        {

            // For Windows auth -
            // @"Data Source=(MachineName)\InstanceName);Initial Catalog = (DBName); Integrated Security = True;"
            // For SQL Server auth -
            // @"Data Source=(MachineName)\InstanceName);Initial Catalog = (DBName); UserID = Username; Password = (Password);"
            SqlConnection sqlCon = new SqlConnection(@"Data Source=(localdb)\Local; Database = LoginDB;Integrated Security = True;");

            try
            {
                if (sqlCon.State == System.Data.ConnectionState.Closed)
                    sqlCon.Open();
                String query = "SELECT COUNT(1) FROM  tblUser WHERE  Username = @Username AND Password =@Password";
                SqlCommand sqlCmd = new SqlCommand(query, sqlCon);
                sqlCmd.CommandType = System.Data.CommandType.Text;

                sqlCmd.Parameters.AddWithValue("@Username", txtUsername.Text);
                sqlCmd.Parameters.AddWithValue("@Password", txtPassword.Password);
                int count = Convert.ToInt32(sqlCmd.ExecuteScalar());

                if (count == 1)
                {
                    MainWindow dashboard = new MainWindow();
                    dashboard.Show();
                    MessageBox.Show("Login Successfull    ");
                    this.NavigationService.Navigate(null);
                }

                else
                {
                    MessageBox.Show("Invalid Username/Password");
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);

            }

            finally
            {
                sqlCon.Close();
            }
        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {
            RegisterPage registerPage = new RegisterPage();
            this.NavigationService.Navigate(registerPage);

        }
    }
}
