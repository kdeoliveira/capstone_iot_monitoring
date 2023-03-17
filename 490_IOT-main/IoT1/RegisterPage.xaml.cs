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
    /// Interaction logic for RegisterPage.xaml
    /// </summary>
    public partial class RegisterPage : Page
    {
        public RegisterPage()
        {
            InitializeComponent();
        }

        SqlConnection conn = new SqlConnection(@"Data Source=(localdb)\Local;Initial Catalog=LoginDB;Integrated Security=True");

        private void Button_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                SqlCommand cmd = new SqlCommand("INSERT INTO tblUser VALUES (@Username, @Password)", conn);
                cmd.CommandType = System.Data.CommandType.Text;
                cmd.Parameters.AddWithValue("@Username", txt_Username.Text);
                cmd.Parameters.AddWithValue("@Password", txt_Password.Password);
                conn.Open();
                cmd.ExecuteNonQuery();
                conn.Close();
                MessageBox.Show("Registration Completed ");

            }

            catch (SqlException Ex)
            {

                MessageBox.Show(Ex.Message);

            }

        }
    }
}
