using System.Windows;

namespace ClientFaceGestures
{
    /// <summary>
    /// Interaction logic for ProcessOptionsWindow.xaml
    /// </summary>
    public partial class ProcessOptionsWindow
    {
        public ProcessOptionsWindow()
        {
            InitializeComponent();

            PTracking = 0;
            PExpression = 0;
        }

        public int PTracking { get; set; }
        public int PExpression { get; set; }

        private void CancelButtonClick(object sender, RoutedEventArgs e)
        {
            Hide();
        }

        private void OkButtonClick(object sender, RoutedEventArgs e)
        {
            PTracking = 0;
            PExpression = 0;

            if( DetectFaceCheckBox.IsChecked == true )
                PTracking = PTracking | 1;

            if( DetectLeftEyeCheckBox.IsChecked == true )
                PTracking = PTracking | 2;

            if( DetectRightEyeCheckBox.IsChecked == true )
                PTracking = PTracking | 4;

            if( DetectNoseCheckBox.IsChecked == true )
                PTracking = PTracking | 8;

            if( DetectMouthCheckBox.IsChecked == true )
                PTracking = PTracking | 16;

            if( AamFitFaceCheckBox.IsChecked == true )
                PTracking = PTracking | 32;

            if( AamFitMouthCheckBox.IsChecked == true )
                PTracking = PTracking | 64;


            if( BlinkingDetectionSkinColorCheckBox.IsChecked == true )
                PExpression = PExpression | 1;

            if( BlinkingDetectionOpticalFlowCheckBox.IsChecked == true )
                PExpression = PExpression | 2;

            if( EstimateGazeDirectionCheckBox.IsChecked == true )
                PExpression = PExpression | 4;

            if (EstimateMouthStateCheckBox.IsChecked == true)
                PTracking = PTracking | 8;

            if (EstimateHeadPoseCheckBox.IsChecked == true)
                PExpression = PExpression | 16;

            Hide();
        }
    }
}
