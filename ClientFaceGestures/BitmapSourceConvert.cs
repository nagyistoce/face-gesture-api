using System.Drawing;
using System.Drawing.Imaging;
using System.IO;
using System.Windows.Media.Imaging;
//using System.Windows;

namespace ClientFaceGestures
{
    public static class BitmapConversion
    {
        public static Bitmap ToWinFormsBitmap(this BitmapSource bitmapsource)
        {
            using (var stream = new MemoryStream())
            {
                BitmapEncoder enc = new BmpBitmapEncoder();
                enc.Frames.Add(BitmapFrame.Create(bitmapsource));
                enc.Save(stream);

                using (var tempBitmap = new Bitmap(stream))
                {
                    // According to MSDN, one "must keep the stream open for the lifetime of the Bitmap."
                    // So we return a copy of the new bitmap, allowing us to dispose both the bitmap and the stream.
                    return new Bitmap(tempBitmap);
                }
            }
        }

        public static MemoryStream BitmapToStream(this Bitmap bitmap)
        {
            using (var stream = new MemoryStream())
            {
                bitmap.Save(stream, ImageFormat.Jpeg);
                stream.Position = 0;

                return stream;
            }
        }

        public static BitmapSource ToWpfBitmap(this Bitmap bitmap)
        {
            using (var stream = new MemoryStream())
            {
                var result = new BitmapImage();

                bitmap.Save(stream, ImageFormat.Bmp);

                //MessageBox.Show((stream.Capacity / 1024.0 ).ToString() + " kb");

                stream.Position = 0;
                result.BeginInit();
                // According to MSDN, "The default OnDemand cache option retains access to the stream until the image is needed."
                // Force the bitmap to load right now so we can dispose the stream.
                result.CacheOption = BitmapCacheOption.OnLoad;
                result.StreamSource = stream;
                result.EndInit();
                result.Freeze();
                return result;
            }
        }
    }
}