using System;
using System.Net;
using System.Windows;
using Emgu.CV;
using Emgu.CV.Structure;

namespace ClientFaceGestures
{
    /// <summary>
    /// Interaction logic for MultimediaUserControl.xaml
    /// </summary>
    public partial class MultimediaUserControl
    {
        private MainWindow _mainWindow;

        public MultimediaUserControl()
        {
            InitializeComponent();
        }

        private void CurFrameInitialized(object sender, EventArgs e)
        {
            using (Image<Bgr, byte> curFrame = new Image<Bgr, byte>(Properties.Resources.mf))
            {
                CurFrame.Source = curFrame.Bitmap.ToWpfBitmap();
            }
        }

        private void MultimediaUserControlLoaded(object sender, RoutedEventArgs e)
        {
            _mainWindow = Window.GetWindow(this) as MainWindow;
        }

        private void CurFrameDrop(object sender, DragEventArgs e)
        {
            string[] files = (string[])e.Data.GetData(DataFormats.FileDrop);

            _mainWindow.Title = Properties.Resources.MainWindowTitle + " - " + files[0];

            if (_mainWindow.MediaHandler != null)
            {
                _mainWindow.MediaHandler.Dispose();
                _mainWindow.CamZeroMenuItem.Header = "_Start capture";
            }

            try
            {
                if (_mainWindow.Connection.IsOpen())
                    _mainWindow.Connection.Close();

                IPAddress ip = IPAddress.Parse("127.0.0.1");
                _mainWindow.Connection.Open(new IPEndPoint(ip, 6000));
            }
            catch (Exception e2)
            {
                _mainWindow.ResultUC.TextBoxServerMsg.Text = e2.Message + "\n" + _mainWindow.ResultUC.TextBoxServerMsg.Text;
            }

            VideoLengthSlider.Value = 0;
            _mainWindow.MediaHandler = new MediaHandler(files[0]);
            _mainWindow.Timer.Start();
        }
    }
}
