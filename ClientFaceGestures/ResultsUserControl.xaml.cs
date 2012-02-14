using System;
using System.Collections.Generic;
using System.Drawing;
using System.Globalization;
using System.Windows;
using Emgu.CV;
using Emgu.CV.Structure;

namespace ClientFaceGestures
{
    /// <summary>
    /// Interaction logic for ResultsUserControl.xaml
    /// </summary>
    public partial class ResultsUserControl
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

        public void AppendServerMsg(string message)
        {
            DateTime time = DateTime.Now;
            const string format = "[HH:mm]: ";

            TextBoxServerMsg.Text = time.ToString(format) + message + Environment.NewLine + TextBoxServerMsg.Text;
        }

        public void ProcessResults(string message, Image<Bgr, byte> curFrame)
        {
            List<ResultElement> elements = new List<ResultElement>();
            string trimmedMessage = message.Trim('#');
            string[] messageParts = trimmedMessage.Split('#');

            foreach (string p in messageParts)
            {
                string[] fp = p.Split('|');

                if(String.CompareOrdinal(fp[0], "FACERECT")==0)
                {
                    elements.Add(new ResultElement
                                     {
                        Name = "Face Rectangle (x, y, w, h)",
                        Value = fp[1] + ", " + fp[2] + ", " + fp[3] + ", " + fp[4]
                    });

                    DrawRectangle(fp, curFrame);
                }
                else if (String.CompareOrdinal(fp[0], "NOSERECT") == 0)
                {
                    elements.Add(new ResultElement
                                     {
                        Name = "Nose Rectangle (x, y, w, h)",
                        Value = fp[1] + ", " + fp[2] + ", " + fp[3] + ", " + fp[4]
                    });

                    DrawRectangle(fp, curFrame);
                }
                else if (String.CompareOrdinal(fp[0], "LEFTEYERECT") == 0)
                {
                    elements.Add(new ResultElement
                                     {
                        Name = "Left Eye Rectangle (x, y, w, h)",
                        Value = fp[1] + ", " + fp[2] + ", " + fp[3] + ", " + fp[4]
                    });

                    DrawRectangle(fp, curFrame);
                }
                else if (String.CompareOrdinal(fp[0], "RIGHTEYERECT") == 0)
                {
                    elements.Add(new ResultElement
                                     {
                        Name = "Right Eye Rectangle (x, y, w, h)",
                        Value = fp[1] + ", " + fp[2] + ", " + fp[3] + ", " + fp[4]
                    });

                    DrawRectangle(fp, curFrame);
                }
                else if (String.CompareOrdinal(fp[0], "MOUTHRECT") == 0)
                {
                    elements.Add(new ResultElement
                                     {
                        Name = "Mouth Rectangle (x, y, w, h)",
                        Value = fp[1] + ", " + fp[2] + ", " + fp[3] + ", " + fp[4]
                    });

                    DrawRectangle(fp, curFrame);
                }
                else if(String.CompareOrdinal(fp[0], "FACEAAM") == 0)
                {
                    elements.Add(new ResultElement
                    {
                        Name = "Face AAM ",
                        Value = ( ( fp.Length - 1 ) / 2 ).ToString(CultureInfo.InvariantCulture) + " points"
                    });

                    DrawCircle(fp, curFrame);
                }
                else if (String.CompareOrdinal(fp[0], "MOUTHAAM") == 0)
                {
                    elements.Add(new ResultElement
                    {
                        Name = "Mouth AAM ",
                        Value = ((fp.Length - 1) / 2).ToString(CultureInfo.InvariantCulture) + " points"
                    });

                    DrawCircle(fp, curFrame);
                }
                else if (String.CompareOrdinal(fp[0], "MOUTHSTATE") == 0)
                {
                    string state = Convert.ToSingle(fp[1], new CultureInfo("en-GB")) > 0.38 ? "OPEN" : "CLOSE";

                    elements.Add(new ResultElement
                    {
                        Name = "Mouth State ",
                        Value = fp[1] + ": " + state
                    });
                }
                else if (String.CompareOrdinal(fp[0], "GAZE") == 0)
                {
                    elements.Add(new ResultElement
                    {
                        Name = "Gaze (x, y, dx, dy)",
                        Value = fp[1] + ", " + fp[2] + ", " + fp[3] + ", " + fp[4]
                    });
                }
                else if (String.CompareOrdinal(fp[0], "HEADPOSE") == 0)
                {
                    elements.Add(new ResultElement
                    {
                        Name = "Head Pose (rx, ry, rz, tx, ty, tz)",
                        Value = fp[1] + ", " + fp[2] + ", " + fp[3] + ", " + fp[4] + ", " + fp[5] + ", " + fp[6]
                    });
                }
            }

            ResultsDataGrid.ItemsSource = elements;
        }

        private void DrawRectangle(string[] p, Image<Bgr, byte> curFrame)
        {
            Rectangle r = new Rectangle(Convert.ToInt32(p[1]), Convert.ToInt32(p[2]), Convert.ToInt32(p[3]), Convert.ToInt32(p[4]));

            curFrame.Draw(r, new Bgr(Color.Red), 2);
        }

        private void DrawCircle(string[] p, Image<Bgr, byte> curFrame)
        {
            for (int i = 1; i < p.Length - 1; i+=2)
            {
                PointF center = new PointF(Convert.ToSingle(p[i], new CultureInfo("en-GB")), Convert.ToSingle(p[i + 1], new CultureInfo("en-GB")));
                CircleF r = new CircleF(center, 2);

                curFrame.Draw(r, new Bgr(Color.Red), 2);
            }
        }
    }

    public class ResultElement
    {
        public string Name { get; set; }
        public string Value { get; set; }
    }
}
