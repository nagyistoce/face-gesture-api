using System;
using System.Windows;
using System.Windows.Controls;

namespace ClientFaceGestures
{
    /// <summary>
    /// Interaction logic for ResultsUserControl.xaml
    /// </summary>
    public partial class ResultsUserControl : UserControl
    {
        private MainWindow _mainWindow;

        public ResultsUserControl()
        {
            InitializeComponent();

            //http://www.c-sharpcorner.com/uploadfile/mahesh/datagrid-in-wpf/        
        }

        private void UserControlLoaded(object sender, RoutedEventArgs e)
        {
            _mainWindow = Window.GetWindow(this) as MainWindow;
        }

        public void AppendServerMsg(string message)
        {
            DateTime time = DateTime.Now;
            const string format = "[HH:mm]: ";

            TextBoxServerMsg.Text = time.ToString(format) + message + Environment.NewLine + TextBoxServerMsg.Text;
        }

        public void ProcessResults(string message)
        {
            
        }
    }
}
