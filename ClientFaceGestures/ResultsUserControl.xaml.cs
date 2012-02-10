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
        }

        private void UserControlLoaded(object sender, RoutedEventArgs e)
        {
            _mainWindow = Window.GetWindow(this) as MainWindow;
        }
    }
}
