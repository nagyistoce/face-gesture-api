using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;

namespace ClientFaceGestures.UI
{
    /// <summary>
    /// Interaction logic for NetworkOptionsWindow.xaml
    /// </summary>
    public partial class NetworkOptionsWindow : Window
    {
        public NetworkOptionsWindow()
        {
            InitializeComponent();
        }

        public string ServerIp { get; set; }
        public int ServerPort { get; set; }

        private void OkButtonClick(object sender, RoutedEventArgs e)
        {
            ServerIp = ServerAddressTextBox.Text;
            ServerPort = Convert.ToInt32( ServerPortTextBox.Text );
            Hide();
        }

        private void CancelButtonClick(object sender, RoutedEventArgs e)
        {
            Hide();
        }

        public override string ToString()
        {
            return "tcp://" + ServerIp + ":" + ServerPort;
        }
    }
}
