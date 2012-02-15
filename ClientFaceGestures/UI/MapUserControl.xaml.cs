using System;
using System.Windows;
using Microsoft.Maps.MapControl.WPF;

namespace ClientFaceGestures.UI
{
    /// <summary>
    /// Interaction logic for MapUserControl.xaml
    /// </summary>
    public partial class MapUserControl
    {
        private MainWindow _mainWindow;

        public MapUserControl()
        {
            InitializeComponent();
        }

        public Map Map
        {
            get { return MyMap; }
        }

        private void MapViewChangeOnFrame(object sender, MapEventArgs e)
        {
            if (SliderZoom != null)
            {
                SliderZoom.ValueChanged -= SliderZoomValueChanged;
                SliderZoom.Value = MyMap.ZoomLevel;
                SliderZoom.ValueChanged += SliderZoomValueChanged;

                if (ButtonZoomPlus != null && Math.Abs(SliderZoom.Value - 20) < Double.Epsilon)
                    ButtonZoomPlus.IsEnabled = false;
                else if (ButtonZoomPlus != null)
                    ButtonZoomPlus.IsEnabled = true;

                if (ButtonZoomMinus != null && Math.Abs(SliderZoom.Value - 1) < Double.Epsilon)
                    ButtonZoomMinus.IsEnabled = false;
                else if (ButtonZoomMinus != null)
                    ButtonZoomMinus.IsEnabled = true;
            }
        }

        private void SliderZoomValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
            if (MyMap != null)
            {
                MyMap.ViewChangeEnd -= MapViewChangeOnFrame;
                MyMap.ZoomLevel = SliderZoom.Value;
                MyMap.ViewChangeEnd += MapViewChangeOnFrame;
            }
        }

        private void ButtonModeAerialClick(object sender, RoutedEventArgs e)
        {
            MyMap.Mode = new AerialMode();

            ButtonModeAerial.IsEnabled = false;
            ButtonModeHybrid.IsEnabled = true;
            ButtonModeRoad.IsEnabled = true;
        }

        private void ButtonModeRoadClick(object sender, RoutedEventArgs e)
        {
            MyMap.Mode = new RoadMode();

            ButtonModeAerial.IsEnabled = true;
            ButtonModeHybrid.IsEnabled = true;
            ButtonModeRoad.IsEnabled = false;
        }

        private void ButtonModeHybridClick(object sender, RoutedEventArgs e)
        {
            MyMap.Mode = new AerialMode(true);

            ButtonModeAerial.IsEnabled = true;
            ButtonModeHybrid.IsEnabled = false;
            ButtonModeRoad.IsEnabled = true;
        }

        private void MapLoaded(object sender, RoutedEventArgs e)
        {
            _mainWindow = Window.GetWindow(this) as MainWindow;
        }

        private void ButtonZoomPlusClick(object sender, RoutedEventArgs e)
        {
            if (MyMap != null)
            {
                MyMap.ViewChangeEnd -= MapViewChangeOnFrame;
                MyMap.ZoomLevel += 1.0;
                MyMap.ViewChangeEnd += MapViewChangeOnFrame;
            }
        }

        private void ButtonZoomMinusClick(object sender, RoutedEventArgs e)
        {
            if (MyMap != null)
            {
                MyMap.ViewChangeEnd -= MapViewChangeOnFrame;
                MyMap.ZoomLevel -= 1.0;
                MyMap.ViewChangeEnd += MapViewChangeOnFrame;
            }
        }
    }
}