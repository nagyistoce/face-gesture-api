using System;
using System.ComponentModel;
using System.Globalization;
using System.IO;
using System.Net;
using System.Runtime.InteropServices;
using System.Text;
using System.Windows;
using System.Windows.Threading;
using Emgu.CV;
using Emgu.CV.Structure;
using Microsoft.Win32;

namespace ClientFaceGestures
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow
    {
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 256)]
        private TcpipImageInformation ImageInformation;

        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
        private struct TcpipImageInformation
        {
            public byte control_ID;
            public uint frame_count;
            public int depth;
            public int nChannels;
            public int width;
            public int height;
            public int pExpression;
            public int pTracking;
            // [FieldOffset(21)]
            public int size;
        }

        public MainWindow()
        {
            InitializeComponent();

            Timer = new DispatcherTimer();
            Timer.Tick += ProcessFrame;
            Connection = new TcpConnection();

            ProcessOptionsWindow = new ProcessOptionsWindow();
        }

        public MediaHandler MediaHandler { get; set; }
        public DispatcherTimer Timer { get; set; }
        public TcpConnection Connection { get; set; }
        public ProcessOptionsWindow ProcessOptionsWindow { get; set; }

        public byte[] RawSerializeEx(object anything)
        {
            int rawsize = Marshal.SizeOf(anything);
            byte[] rawdatas = new byte[rawsize];
            GCHandle handle = GCHandle.Alloc(rawdatas, GCHandleType.Pinned);
            IntPtr buffer = handle.AddrOfPinnedObject();
            Marshal.StructureToPtr(anything, buffer, false);
            handle.Free();
            return rawdatas;
        } 

        private void WindowLoaded(object sender, RoutedEventArgs e)
        {
            WindowMain.Title = Properties.Resources.MainWindowTitle;
        }

        public void ProcessFrame(object sender, EventArgs et)
        {
            MemoryStream ms = null;

            try
            {
                Image<Bgr, byte> curFrame = MediaHandler.QueryFrame();

                if (curFrame == null)
                {
                    StopCaptureInit();
                    throw (new Exception("frame == NULL!"));
                }

                ms = curFrame.Bitmap.BitmapToStream();

                ImageInformation.control_ID = 0xaa;
                ImageInformation.frame_count = UInt32.Parse(MediaHandler.FrameNo.ToString(CultureInfo.InvariantCulture));
                ImageInformation.depth = (int)Image<Bgr, byte>.CvDepth;
                ImageInformation.nChannels = curFrame.NumberOfChannels;
                ImageInformation.width = curFrame.Width;
                ImageInformation.height = curFrame.Height;
                ImageInformation.pTracking = ProcessOptionsWindow.PTracking;
                ImageInformation.pExpression = ProcessOptionsWindow.PExpression;
                ImageInformation.size = ms.GetBuffer().Length;

                // Küldés szervernek, erre a válasz "ACK1".
                byte[] data = RawSerializeEx(ImageInformation);
                Connection.Send(data, Marshal.SizeOf(ImageInformation));

                byte[] recv;
                if ((recv = Connection.Receive()) != null)
                {
                    string sRecv = Encoding.ASCII.GetString(recv);
                    if (sRecv == "ACK1")
                    {
                        Connection.Send(ms.GetBuffer());
                        //ResultUC.TextBoxServerMsg.Text = sRecv + "\n" + ResultUC.TextBoxServerMsg.Text;                 
                    }
                    else if (sRecv == "ERR1")
                    {
                        System.Threading.Thread.Sleep(10);
                        throw (new Exception("TCPIP recv() error - ERR1."));
                    }
                }
                else
                {
                    throw (new Exception("TCPIP recv() error #1 - recv() == NULL."));
                }

                ms.Flush();
                ms.Close();

                if ((recv = Connection.Receive()) != null)
                {
                    string sRecv = Encoding.ASCII.GetString(recv);
                    if (String.CompareOrdinal(sRecv.Substring(0, 4), "ERR2") != 0)
                    {
                        // Az aktuális frame kifeszítése a WPF Image controlra
                        MultimediaUC.CurFrame.Source = curFrame.Bitmap.ToWpfBitmap();
                        ResultUC.TextBoxServerMsg.Text = sRecv + "\n" + ResultUC.TextBoxServerMsg.Text;
                    }
                    else
                    {
                        throw (new Exception("TCPIP recv() error - ERR2."));
                    }
                }
                else
                {                    
                    System.Threading.Thread.Sleep(10);
                    throw (new Exception("TCPIP recv() error #2 - recv() == NULL"));
                }                       
            }
            catch (Exception e)
            {
                Timer.Stop();

                if (ms != null)
                {
                    ms.Flush();
                    ms.Close();
                }

                ResultUC.TextBoxServerMsg.Text = e.Message + "\n" + ResultUC.TextBoxServerMsg.Text;
            }

            double playRate = Math.Abs(MediaHandler.FrameCount - 0) > Double.Epsilon ? MediaHandler.FrameNo/MediaHandler.FrameCount : 0;
            MultimediaUC.VideoLengthSlider.Value = MultimediaUC.VideoLengthSlider.Maximum * playRate;
        }

        private void WindowClosing(object sender, CancelEventArgs e)
        {
            if (MediaHandler != null) 
                MediaHandler.Dispose();

            if (Connection != null) 
                Connection.Close();

            ProcessOptionsWindow.Close();
        }

        private void StartCamZeroClick(object sender, RoutedEventArgs e)
        {
            MultimediaUC.VideoLengthSlider.Value = 0;

            if (CamZeroMenuItem.Header.Equals("_Start capture"))
                StartCaptureInit(0, 0, "");
            else
                StopCaptureInit();
        }

        private void MenuItemOpenVideoClick(object sender, RoutedEventArgs e)
        {
            // Configure open file dialog box
            var dlg = new OpenFileDialog
                          {
                              FileName = "Document",
                              DefaultExt = ".avi",
                              Filter = "Movie files (.avi)|*.asf;*.avi;*.flv;*.m2ts;*.mpg;*.mpeg;*.wmv" +
                                       "|All Files|*.*"
                          };

            // Show open file dialog box
            var result = dlg.ShowDialog();

            // Process open file dialog box results
            if (result == true)
            {
                StartCaptureInit(1, 0, dlg.FileName);
            }
        }

        private void StartCaptureInit(int type, int camIndex, string filename)
        {
            if (MediaHandler != null)
            {
                Timer.Stop();
                MediaHandler.Dispose();
            }

            try
            {
                if( Connection.IsOpen() )
                    Connection.Close();

                IPAddress ip = IPAddress.Parse("127.0.0.1");
                Connection.Open(new IPEndPoint(ip, 6000));
            }
            catch (Exception e)
            {
                ResultUC.TextBoxServerMsg.Text = e.Message + "\n" + ResultUC.TextBoxServerMsg.Text;
            }
            

            if (type == 0)
            {
                MediaHandler = new MediaHandler(camIndex);

                MultimediaUC.VideoLengthSlider.Value = 0;

                CamZeroMenuItem.Header = "_Stop capture";
                WindowMain.Title = Properties.Resources.MainWindowTitle + " - Webcam #" + camIndex;
            }
            else
            {
                MediaHandler = new MediaHandler(filename);

                MultimediaUC.VideoLengthSlider.Value = 0;

                CamZeroMenuItem.Header = "_Start capture";
                WindowMain.Title = Properties.Resources.MainWindowTitle + " - " + filename;
            }

            Timer.Start();
        }

        private void StopCaptureInit()
        {
            if (MediaHandler != null)
            {
                Timer.Stop();
                MediaHandler.Dispose();
            }

            Connection.Close();

            CamZeroMenuItem.Header = "_Start capture";
            MultimediaUC.VideoLengthSlider.Value = 0;
            WindowMain.Title = Properties.Resources.MainWindowTitle;

            using (var multimediaBackground = new Image<Bgr, byte>(Properties.Resources.mf))
            {
                MultimediaUC.CurFrame.Source = multimediaBackground.Bitmap.ToWpfBitmap();
            }
        }

        private void MenuItemProcessingSettingsClick(object sender, RoutedEventArgs e)
        {
            ProcessOptionsWindow.ShowDialog();
        }
    }
}