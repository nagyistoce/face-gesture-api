using System.Windows;

namespace ClientFaceGestures.UI
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
                PExpression = PExpression | 8;

            if (EstimateHeadPoseCheckBox.IsChecked == true)
                PExpression = PExpression | 16;

            Hide();
        }

        private void EstimateHeadPoseCheckBoxClick(object sender, RoutedEventArgs e)
        {
            if (EstimateHeadPoseCheckBox.IsChecked == true)
                DetectFaceCheckBox.IsChecked = AamFitFaceCheckBox.IsChecked = true;
        }

        private void EstimateMouthStateCheckBoxClick(object sender, RoutedEventArgs e)
        {
            if (EstimateMouthStateCheckBox.IsChecked == true)
                DetectFaceCheckBox.IsChecked = AamFitMouthCheckBox.IsChecked = DetectMouthCheckBox.IsChecked = true;
        }

        private void EstimateGazeDirectionCheckBoxClick(object sender, RoutedEventArgs e)
        {
            if (EstimateGazeDirectionCheckBox.IsChecked == true)
                DetectFaceCheckBox.IsChecked = AamFitFaceCheckBox.IsChecked = true;
        }

        private void BlinkingDetectionOpticalFlowCheckBoxClick(object sender, RoutedEventArgs e)
        {
            if (BlinkingDetectionOpticalFlowCheckBox.IsChecked == true)
                DetectFaceCheckBox.IsChecked = DetectLeftEyeCheckBox.IsChecked = DetectRightEyeCheckBox.IsChecked = true;
        }

        private void BlinkingDetectionSkinColorCheckBoxClick(object sender, RoutedEventArgs e)
        {
            if (BlinkingDetectionSkinColorCheckBox.IsChecked == true)
                DetectFaceCheckBox.IsChecked = DetectLeftEyeCheckBox.IsChecked = DetectRightEyeCheckBox.IsChecked = true;
        }

        private void AamFitMouthCheckBoxClick(object sender, RoutedEventArgs e)
        {
            if (AamFitMouthCheckBox.IsChecked == true)
                DetectFaceCheckBox.IsChecked = DetectMouthCheckBox.IsChecked = true;
            else
                EstimateMouthStateCheckBox.IsChecked = false;
        }

        private void AamFitFaceCheckBoxClick(object sender, RoutedEventArgs e)
        {
            if (AamFitFaceCheckBox.IsChecked == true)
                DetectFaceCheckBox.IsChecked = true;
            else
                EstimateHeadPoseCheckBox.IsChecked = EstimateGazeDirectionCheckBox.IsChecked = false;
        }

        private void DetectNoseCheckBoxClick(object sender, RoutedEventArgs e)
        {
            if(DetectNoseCheckBox.IsChecked == true)
                DetectFaceCheckBox.IsChecked = true;
        }

        private void DetectMouthCheckBoxClick(object sender, RoutedEventArgs e)
        {
            if (DetectMouthCheckBox.IsChecked == true)
                DetectFaceCheckBox.IsChecked = true;
            else
                EstimateMouthStateCheckBox.IsChecked = AamFitMouthCheckBox.IsChecked = false;
        }

        private void DetectRightEyeCheckBoxClick(object sender, RoutedEventArgs e)
        {
            if (DetectRightEyeCheckBox.IsChecked == true)
                DetectFaceCheckBox.IsChecked = true;
            else
                BlinkingDetectionOpticalFlowCheckBox.IsChecked = BlinkingDetectionSkinColorCheckBox.IsChecked = false;
        }

        private void DetectLeftEyeCheckBoxClick(object sender, RoutedEventArgs e)
        {
            if (DetectLeftEyeCheckBox.IsChecked == true)
                DetectFaceCheckBox.IsChecked = true;
            else
                BlinkingDetectionOpticalFlowCheckBox.IsChecked = BlinkingDetectionSkinColorCheckBox.IsChecked = false;
        }

        private void DetectFaceCheckBoxClick(object sender, RoutedEventArgs e)
        {
            if (DetectFaceCheckBox.IsChecked == false)
                DetectLeftEyeCheckBox.IsChecked =
                    DetectRightEyeCheckBox.IsChecked =
                    DetectMouthCheckBox.IsChecked = DetectNoseCheckBox.IsChecked = AamFitFaceCheckBox.IsChecked = AamFitMouthCheckBox.IsChecked = false;

            DetectLeftEyeCheckBoxClick(null, null);
            DetectRightEyeCheckBoxClick(null, null);
            DetectMouthCheckBoxClick(null, null);
            DetectNoseCheckBoxClick(null, null);
            AamFitFaceCheckBoxClick(null, null);
            AamFitMouthCheckBoxClick(null, null);
        }
    }
}
