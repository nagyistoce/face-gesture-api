using System;
using System.Globalization;

namespace ClientFaceGestures.UI
{
    /// <summary>
    /// Interaction logic for NetworkOptionsWindow.xaml
    /// </summary>
    public partial class NetworkOptionsWindow
    {
        public NetworkOptionsWindow()
        {
            InitializeComponent();

            ServerIp = "127.0.0.1";
            ServerPort = 6000;

            ServerIpTextBox.Text = ServerIp;
            ServerPortTextBox.Text = ServerPort.ToString(CultureInfo.InvariantCulture);
        }

        public string ServerIp { get; set; }
        public int ServerPort { get; set; }

        public override string ToString()
        {
            return "tcp://" + ServerIp + ":" + ServerPort;
        }

        private void OkButtonClick(object sender, System.Windows.RoutedEventArgs e)
        {
            ServerIp = ServerIpTextBox.Text;
            ServerPort = Convert.ToInt32(ServerPortTextBox.Text);

            Hide();
        }

        private void CancelButtonClick(object sender, System.Windows.RoutedEventArgs e)
        {
            Hide();
        }
    }
}
