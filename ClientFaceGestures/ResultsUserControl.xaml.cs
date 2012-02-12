using System;
using System.Collections.Generic;
using System.Windows;

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

        public void ProcessResults(string message)
        {
            List<ResultElement> elements = new List<ResultElement>();
            string trimmedMessage = message.Trim('#');
            string[] messageParts = trimmedMessage.Split('#');

            foreach (string p in messageParts)
            {
                string[] fp = p.Split('|');

                if(fp[0].CompareTo("FACERECT")==0)
                {
                    elements.Add(new ResultElement
                                     {
                        Name = "Face Rectangle (x, y, w, h)",
                        Value = fp[1] + ", " + fp[2] + ", " + fp[3] + ", " + fp[4]
                    });
                }
                else if (fp[0].CompareTo("NOSERECT") == 0)
                {
                    elements.Add(new ResultElement
                                     {
                        Name = "Nose Rectangle (x, y, w, h)",
                        Value = fp[1] + ", " + fp[2] + ", " + fp[3] + ", " + fp[4]
                    });
                }
                else if (fp[0].CompareTo("LEFTEYERECT") == 0)
                {
                    elements.Add(new ResultElement
                                     {
                        Name = "Left Eye Rectangle (x, y, w, h)",
                        Value = fp[1] + ", " + fp[2] + ", " + fp[3] + ", " + fp[4]
                    });
                }
                else if (fp[0].CompareTo("RIGHTEYERECT") == 0)
                {
                    elements.Add(new ResultElement
                                     {
                        Name = "Right Eye Rectangle (x, y, w, h)",
                        Value = fp[1] + ", " + fp[2] + ", " + fp[3] + ", " + fp[4]
                    });
                }
                else if (fp[0].CompareTo("MOUTHRECT") == 0)
                {
                    elements.Add(new ResultElement
                                     {
                        Name = "Mouth Rectangle (x, y, w, h)",
                        Value = fp[1] + ", " + fp[2] + ", " + fp[3] + ", " + fp[4]
                    });
                }

            }

            ResultsDataGrid.ItemsSource = elements;
        }
    }

    public class ResultElement
    {
        public string Name { get; set; }
        public string Value { get; set; }
    }
}
