using System;
using System.Collections.Generic;
using System.Drawing;
using System.Globalization;
using System.Windows;
using Emgu.CV;
using Emgu.CV.Structure;
using Point = System.Drawing.Point;

namespace ClientFaceGestures.UI
{
    /// <summary>
    /// Interaction logic for ResultsUserControl.xaml
    /// </summary>
    public partial class ResultsUserControl
    {
        public ResultsUserControl()
        {
            InitializeComponent();
        }

        public Features Features { get; set; }
        public MainWindow MainWindow { get; set; }
        public MCvFont Font { get; set; }

        private void UserControlLoaded(object sender, RoutedEventArgs e)
        {
            MainWindow = Window.GetWindow(this) as MainWindow;
            Features = new Features();
            Font = new MCvFont(Emgu.CV.CvEnum.FONT.CV_FONT_HERSHEY_COMPLEX, 0.1, 1.0);
        }

        public void AppendServerMsg(string message)
        {
            DateTime time = DateTime.Now;
            const string format = "[HH:mm]: ";

            TextBoxServerMsg.Text = time.ToString(format) + message + Environment.NewLine + TextBoxServerMsg.Text;
        }

        public Features ProcessResults(string message, Image<Bgr, byte> curFrame)
        {
            List<ResultElement> elements = new List<ResultElement>();
            string trimmedMessage = message.Trim('#');
            string[] messageArray = trimmedMessage.Split('#');

            foreach (string messagePart in messageArray)
            {
                string[] val = messagePart.Split('|');

                if(String.CompareOrdinal(val[0], "FACERECT")==0)
                {
                    elements.Add(new ResultElement
                                     {
                        Name = "Face Rectangle (x, y, w, h)",
                        Value = val[1] + ", " + val[2] + ", " + val[3] + ", " + val[4]
                    });

                    Features.FaceRect = ProcessRectangle(val, curFrame, Color.Red);
                }
                else if (String.CompareOrdinal(val[0], "NOSERECT") == 0)
                {
                    elements.Add(new ResultElement
                                     {
                        Name = "Nose Rectangle (x, y, w, h)",
                        Value = val[1] + ", " + val[2] + ", " + val[3] + ", " + val[4]
                    });

                    Features.NoseRect = ProcessRectangle(val, curFrame, Color.Red);
                }
                else if (String.CompareOrdinal(val[0], "LEFTEYERECT") == 0)
                {
                    elements.Add(new ResultElement
                                     {
                        Name = "Left Eye Rectangle (x, y, w, h)",
                        Value = val[1] + ", " + val[2] + ", " + val[3] + ", " + val[4]
                    });

                    Features.LeftEyeRect = ProcessRectangle(val, curFrame, Color.Red);
                }
                else if (String.CompareOrdinal(val[0], "RIGHTEYERECT") == 0)
                {
                    elements.Add(new ResultElement
                                     {
                        Name = "Right Eye Rectangle (x, y, w, h)",
                        Value = val[1] + ", " + val[2] + ", " + val[3] + ", " + val[4]
                    });

                    Features.RightEyeRect = ProcessRectangle(val, curFrame, Color.Red);
                }
                else if (String.CompareOrdinal(val[0], "MOUTHRECT") == 0)
                {
                    elements.Add(new ResultElement
                                     {
                        Name = "Mouth Rectangle (x, y, w, h)",
                        Value = val[1] + ", " + val[2] + ", " + val[3] + ", " + val[4]
                    });

                    Features.MouthRect = ProcessRectangle(val, curFrame, Color.Red);
                }
                else if(String.CompareOrdinal(val[0], "FACEAAM") == 0)
                {
                    elements.Add(new ResultElement
                    {
                        Name = "Face AAM ",
                        Value = ( ( val.Length - 1 ) / 2 ).ToString(CultureInfo.InvariantCulture) + " points"
                    });

                    Features.FaceAamPoints = ProcessCircles(val, curFrame, Color.Blue);
                }
                else if (String.CompareOrdinal(val[0], "MOUTHAAM") == 0)
                {
                    elements.Add(new ResultElement
                    {
                        Name = "Mouth AAM ",
                        Value = ((val.Length - 1) / 2).ToString(CultureInfo.InvariantCulture) + " points"
                    });

                    Features.MouthAamPoints = ProcessCircles(val, curFrame, Color.Blue);
                }
                else if (String.CompareOrdinal(val[0], "MOUTHSTATE") == 0)
                {
                    string state = Convert.ToSingle(val[1], new CultureInfo("en-GB")) > 0.38 ? "OPEN" : "CLOSE";

                    elements.Add(new ResultElement
                    {
                        Name = "Mouth State ",
                        Value = val[1] + ": " + state
                    });
                }
                else if (String.CompareOrdinal(val[0], "GAZE") == 0)
                {
                    elements.Add(new ResultElement
                    {
                        Name = "Gaze (x, y, dx, dy)",
                        Value = val[1] + ", " + val[2] + ", " + val[3] + ", " + val[4]
                    });

                    //Features.GazeCenter = ProcessCircle(val[1], val[2], curFrame, Color.Green);
                    ProcessGaze(val, curFrame, Color.LawnGreen);
                }
                else if (String.CompareOrdinal(val[0], "HEADPOSE") == 0)
                {
                    elements.Add(new ResultElement
                    {
                        Name = "Head Pose (rx, ry, rz, tx, ty, tz)",
                        Value = val[1] + ", " + val[2] + ", " + val[3] + ", " + val[4] + ", " + val[5] + ", " + val[6]
                    });
                }
            }

            ResultsDataGrid.ItemsSource = elements;

            return Features;
        }

        private Rectangle ProcessRectangle(string[] p, Image<Bgr, byte> curFrame, Color rgb)
        {
            Rectangle r = new Rectangle(Convert.ToInt32(p[1]), Convert.ToInt32(p[2]), 
                Convert.ToInt32(p[3]), Convert.ToInt32(p[4]));

            curFrame.Draw(r, new Bgr(rgb), 2);

            return r;
        }

        private List<CircleF> ProcessCircles(string[] p, Image<Bgr, byte> curFrame, Color rgb)
        {
            List<CircleF> points = new List<CircleF>();

            for (int i = 1; i < p.Length - 1; i+=2)
            {
                PointF centerPoint = new PointF(Convert.ToSingle(p[i], new CultureInfo("en-GB")), 
                    Convert.ToSingle(p[i + 1], new CultureInfo("en-GB")));
                points.Add(new CircleF(centerPoint, 2));

                curFrame.Draw(points[i/2], new Bgr(rgb), 2);
            }

            return points;
        }

        //private CircleF ProcessCircle(string x, string y, Image<Bgr, byte> curFrame, Color rgb)
        //{
        //    PointF centerPoint = new PointF(Convert.ToSingle(x, new CultureInfo("en-GB")), 
        //        Convert.ToSingle(y, new CultureInfo("en-GB")));
        //    CircleF circle = new CircleF(centerPoint, 2);

        //    curFrame.Draw(circle, new Bgr(rgb), 2);

        //    return circle;
        //}

        private void ProcessGaze(string[] p, Image<Bgr, byte> curFrame, Color rgb)
        {
            PointF centerPoint = new PointF(Convert.ToSingle(p[1], new CultureInfo("en-GB")),
                Convert.ToSingle(p[2], new CultureInfo("en-GB")));
            Features.GazeCenter = new CircleF(centerPoint, 2);

            PointF faceCenter = new PointF(0.0f, 0.0f);
            
            foreach (CircleF c in Features.FaceAamPoints)
            {
                faceCenter.X += c.Center.X;
                faceCenter.Y += c.Center.Y;
            }

            faceCenter.X /= Features.FaceAamPoints.Count;
            faceCenter.Y /= Features.FaceAamPoints.Count;

            curFrame.Draw(new CircleF(faceCenter, 2), new Bgr(rgb), 2);
            curFrame.Draw(new LineSegment2DF(faceCenter, Features.GazeCenter.Center), new Bgr(rgb), 2);
            curFrame.Draw(Features.GazeCenter, new Bgr(rgb), 2);

            Features.HMoving = Convert.ToSingle(p[3], new CultureInfo("en-GB"));
            Features.VMoving = Convert.ToSingle(p[4], new CultureInfo("en-GB"));

            MCvFont f = Font;

            if(Features.HMoving < 0)
                curFrame.Draw("HORIZONTAL: LEFT", ref f, new Point(10, 20), new Bgr(255, 255, 255));
            else if(Features.HMoving > 0)
                curFrame.Draw("HORIZONTAL: RIGHT", ref f, new Point(10, 20), new Bgr(255, 255, 255));
            else
                curFrame.Draw("HORIZONTAL: FORWARD", ref f, new Point(10, 20), new Bgr(255, 255, 255));

            if(Features.VMoving < 0)
                curFrame.Draw("VERTICAL:    UP", ref f, new Point(10, 40), new Bgr(255, 255, 255));
            else if(Features.VMoving > 0)
                curFrame.Draw("VERTICAL:    DOWN", ref f, new Point(10, 40), new Bgr(255, 255, 255));
            else
                curFrame.Draw("VERTICAL:    FORWARD", ref f, new Point(10, 40), new Bgr(255, 255, 255));
        }
    }

    public class ResultElement
    {
        public string Name { get; set; }
        public string Value { get; set; }
    }
}
