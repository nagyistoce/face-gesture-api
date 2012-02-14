using System;
using System.Windows;
using Emgu.CV;
using Emgu.CV.CvEnum;
using Emgu.CV.Structure;

namespace ClientFaceGestures
{
    public class MediaHandler
    {
        private readonly Capture _capture;

        public MediaHandler()
        {
            try
            {
                _capture = new Capture {FlipHorizontal = true};

                CamIndex = 0;
                FPS = _capture.GetCaptureProperty(CAP_PROP.CV_CAP_PROP_FPS);
                FrameWidth = _capture.GetCaptureProperty(CAP_PROP.CV_CAP_PROP_FRAME_WIDTH);
                FrameHeight = _capture.GetCaptureProperty(CAP_PROP.CV_CAP_PROP_FRAME_HEIGHT);
                FrameCount = -1;
                Millisec = _capture.GetCaptureProperty(CAP_PROP.CV_CAP_PROP_POS_MSEC);
            }
            catch (NullReferenceException e)
            {
                MessageBox.Show(e.Message);
                _capture = null;
            }
        }

        public MediaHandler(int pCamIndex = 0)
        {
            try
            {
                _capture = new Capture(pCamIndex) {FlipHorizontal = true};
                _capture.SetCaptureProperty(CAP_PROP.CV_CAP_PROP_FRAME_WIDTH, 800.0 );
                _capture.SetCaptureProperty(CAP_PROP.CV_CAP_PROP_FRAME_HEIGHT, 600.0);

                CamIndex = pCamIndex;
                FPS = _capture.GetCaptureProperty(CAP_PROP.CV_CAP_PROP_FPS);
                FrameWidth = _capture.GetCaptureProperty(CAP_PROP.CV_CAP_PROP_FRAME_WIDTH);
                FrameHeight = _capture.GetCaptureProperty(CAP_PROP.CV_CAP_PROP_FRAME_HEIGHT);
                FrameCount = -1;
                Millisec = _capture.GetCaptureProperty(CAP_PROP.CV_CAP_PROP_POS_MSEC);
            }
            catch (NullReferenceException e)
            {
                MessageBox.Show(e.Message);
                _capture = null;
            }
        }

        public MediaHandler(string pPath)
        {
            try
            {
                _capture = new Capture(pPath) { FlipHorizontal = true };

                CamIndex = -1;
                FPS = _capture.GetCaptureProperty(CAP_PROP.CV_CAP_PROP_FPS);
                FrameWidth = _capture.GetCaptureProperty(CAP_PROP.CV_CAP_PROP_FRAME_WIDTH);
                FrameHeight = _capture.GetCaptureProperty(CAP_PROP.CV_CAP_PROP_FRAME_HEIGHT);
                FrameCount = _capture.GetCaptureProperty(CAP_PROP.CV_CAP_PROP_FRAME_COUNT);
                Millisec = _capture.GetCaptureProperty(CAP_PROP.CV_CAP_PROP_POS_MSEC);
            }
            catch (NullReferenceException e)
            {
                MessageBox.Show(e.Message);
                _capture = null;
            }
        }

        public int CamIndex { get; private set; }
        public double FPS { get; private set; }
        public double FrameWidth { get; private set; }
        public double FrameHeight { get; private set; }
        public double FrameNo { get; private set; }
        public double FrameCount { get; private set; }
        public double Millisec { get; private set; }
        public Image<Bgr, byte> Frame { get; private set; }

        public Image<Bgr, byte> QueryFrame()
        {
            Frame = _capture != null ? _capture.QueryFrame() : null;

            FrameNo++;

            return Frame;
        }

        public void SetPosFrame(double pos)
        {
            if (_capture != null && pos > 0.0 && pos < FrameCount)
            {
                _capture.SetCaptureProperty(CAP_PROP.CV_CAP_PROP_POS_AVI_RATIO, pos);
            }
        }

        public void Dispose()
        {
            if (_capture != null)
            {
                _capture.Dispose();
            }
        }
    }
}