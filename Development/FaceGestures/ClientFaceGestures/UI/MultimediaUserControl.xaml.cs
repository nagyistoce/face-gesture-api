using System;
using System.Net;
using System.Windows;
using Emgu.CV;
using Emgu.CV.Structure;

namespace ClientFaceGestures.UI
{
    /// <summary>
    /// Interaction logic for MultimediaUserControl.xaml
    /// </summary>
    public partial class MultimediaUserControl
    {
        public MultimediaUserControl()
        {
            InitializeComponent();
        }

        public MainWindow MainWindow { get; set; }

        private void CurFrameInitialized(object sender, EventArgs e)
        {
            using (Image<Bgr, byte> curFrame = new Image<Bgr, byte>(Properties.Resources.mf))
            {
                CurFrame.Source = curFrame.Bitmap.ToWpfBitmap();
            }
        }

        private void MultimediaUserControlLoaded(object sender, RoutedEventArgs e)
        {
            MainWindow = Window.GetWindow(this) as MainWindow;
        }

        private void CurFrameDrop(object sender, DragEventArgs e)
        {
            string[] files = (string[])e.Data.GetData(DataFormats.FileDrop);

            MainWindow.Title = Properties.Resources.MainWindowTitle + " - " + files[0];

            if (MainWindow.MediaHandler != null)
            {
                MainWindow.MediaHandler.Dispose();
                MainWindow.CamZeroMenuItem.Header = "_Start capture";
            }

            try
            {
                if (MainWindow.Connection.IsOpen())
                {
                    MainWindow.Connection.Close();
                    MainWindow.ResultUC.AppendServerMsg("Successfully disconnected from the server [tcp://127.0.0.1:6000].");
                }

                IPAddress ip = IPAddress.Parse("127.0.0.1");
                MainWindow.Connection.Open(new IPEndPoint(ip, 6000));
                MainWindow.ResultUC.AppendServerMsg("A connection was successfully established with the server [tcp://127.0.0.1:6000]");
            }
            catch (Exception e2)
            {
                MainWindow.ResultUC.TextBoxServerMsg.Text = "[" + DateTime.Now.Hour + ":" + DateTime.Now.Minute + "] - " + 
                    e2.Message + "\n" + MainWindow.ResultUC.TextBoxServerMsg.Text;
            }

            VideoLengthSlider.Value = 0;
            MainWindow.MediaHandler = new MediaHandler(files[0]);
            MainWindow.Timer.Start();
        }

        private void StartCaptureButtonClick(object sender, RoutedEventArgs e)
        {
            if (StartCaptureButton.Content.Equals("Start"))
                MainWindow.StartCaptureInit(0, 0, "");
            else
                MainWindow.StopCaptureInit();
        }
    }
}
