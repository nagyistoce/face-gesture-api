using System;
using System.Collections.Generic;
using System.Drawing;
using Emgu.CV.Structure;

namespace ClientFaceGestures
{
    public class Features
    {
        public Rectangle FaceRect { get; set; }
        public Rectangle NoseRect { get; set; }
        public Rectangle MouthRect { get; set; }
        public Rectangle LeftEyeRect { get; set; }
        public Rectangle RightEyeRect { get; set; }

        public CircleF GazeCenter { get; set; }

        public List<CircleF> FaceAamPoints { get; set; }
        public List<CircleF> MouthAamPoints { get; set; }

        public float HMoving { get; set; }
        public float VMoving { get; set; }

        public Rectangle[] Directions { get; private set; }

        public PointF[] ModelPoints { get; private set; }
        //public PointF[] FaceModelPoints { get; private set; }

        public float[] RotationMatrix { get; set; }
        public float[] TranslationVector { get; set; }
        public float Distance { get; set; }

        public void SetRectangles(int width, int height)
        {
            int w3 = width / 3, h3 = height / 3;

            Directions = new Rectangle[9];

            Directions[0] = new Rectangle(0, 0, w3, h3);
            Directions[1] = new Rectangle(w3, 0, w3, h3);
            Directions[2] = new Rectangle(2 * w3, 0, w3, h3);

            Directions[3] = new Rectangle(0, h3, w3, h3);
            Directions[4] = new Rectangle(w3, h3, w3, h3);
            Directions[5] = new Rectangle(2 * w3, h3, w3, h3);

            Directions[6] = new Rectangle(0, 2 * h3, w3, h3);
            Directions[7] = new Rectangle(w3, 2 * h3, w3, h3);
            Directions[8] = new Rectangle(2 * w3, 2 * h3, w3, h3);
        }

        public bool IsElementRect(Rectangle r, PointF p)
        {
	        return ( p.X >= r.X ) && ( p.X <= r.X + r.Width ) && ( p.Y >= r.Y ) && ( p.Y <= r.Y + r.Height );
        }

        public void SetModelPoints()
        {
            MCvPoint3D32f[] modelPoints3D = new MCvPoint3D32f[4];
            ModelPoints = new PointF[4];

            modelPoints3D[0] = new MCvPoint3D32f(0.0f, 0.0f, 0.0f);
            modelPoints3D[1] = new MCvPoint3D32f(40.0f, 0.0f, 0.0f);
            modelPoints3D[2] = new MCvPoint3D32f(0.0f, -40.0f, 0.0f);
            modelPoints3D[3] = new MCvPoint3D32f(0.0f, 0.0f, 50.0f);

            for( int i = 0; i < modelPoints3D.Length; i++ )
	        {
                MCvPoint3D32f viewPoint3D = new MCvPoint3D32f
                {
                    x = RotationMatrix[0] * modelPoints3D[i].x +
                        RotationMatrix[1] * modelPoints3D[i].y +
                        RotationMatrix[2] * modelPoints3D[i].z + TranslationVector[0],
                    y = RotationMatrix[3] * modelPoints3D[i].x +
                        RotationMatrix[4] * modelPoints3D[i].y +
                        RotationMatrix[5] * modelPoints3D[i].z + TranslationVector[1],
                    z = RotationMatrix[6] * modelPoints3D[i].x +
                        RotationMatrix[7] * modelPoints3D[i].y +
                        RotationMatrix[8] * modelPoints3D[i].z + TranslationVector[2]
                };

	            ModelPoints[i] = new PointF(0.0f, 0.0f);

                if( Math.Abs(viewPoint3D.z - 0) > float.Epsilon )
                {
                    ModelPoints[i].X = 760.0f * viewPoint3D.x / viewPoint3D.z; 
                    ModelPoints[i].Y = 760.0f * viewPoint3D.y / viewPoint3D.z; 	
                }
	        }

            float xDiff = ModelPoints[0].X - FaceRect.X;
            float yDiff = ModelPoints[0].Y - ( FaceRect.Y /*+ FaceRect.Height*/ );

            for (int i = 0; i < modelPoints3D.Length; i++)
            {
                ModelPoints[i].X -= xDiff;
                ModelPoints[i].Y -= yDiff;
            }
        }

        //public void SetFaceModelPoints()
        //{
        //    MCvPoint3D32f[] modelPoints3D = new MCvPoint3D32f[58];
        //    FaceModelPoints = new PointF[58];

        //    modelPoints3D[0] = new MCvPoint3D32f(0.000f, 0.000f, 0.000f); // 0
        //    modelPoints3D[1] = new MCvPoint3D32f(1.500f, -2.750f, 1.250f); // 1
        //    modelPoints3D[2] = new MCvPoint3D32f(3.000f, -5.250f, 2.500f); // 2
        //    modelPoints3D[3] = new MCvPoint3D32f(4.500f, -7.500f, 3.750f); // 3
        //    modelPoints3D[4] = new MCvPoint3D32f(6.000f, -9.250f, 5.000f); // 4
        //    modelPoints3D[5] = new MCvPoint3D32f(7.500f, -10.500f, 6.250f); // 5
        //    modelPoints3D[6] = new MCvPoint3D32f(9.500f, -10.750f, 7.500f); // 6
        //    modelPoints3D[7] = new MCvPoint3D32f(11.500f, -10.500f, 6.250f); // 7
        //    modelPoints3D[8] = new MCvPoint3D32f(13.000f, -9.250f, 5.000f); // 8
        //    modelPoints3D[9] = new MCvPoint3D32f(14.500f, -7.500f, 3.750f); // 9
        //    modelPoints3D[10] = new MCvPoint3D32f(16.000f, -5.250f, 2.500f); // 10
        //    modelPoints3D[11] = new MCvPoint3D32f(17.500f, -2.750f, 1.250f); // 11
        //    modelPoints3D[12] = new MCvPoint3D32f(19.000f, 0.000f, 0.000f); // 12
        //    modelPoints3D[13] = new MCvPoint3D32f(15.500f, 8.750f, 2.500f); // 13
        //    modelPoints3D[14] = new MCvPoint3D32f(14.500f, 10.000f, 2.500f); // 14
        //    modelPoints3D[15] = new MCvPoint3D32f(13.500f, 10.250f, 2.500f); // 15
        //    modelPoints3D[16] = new MCvPoint3D32f(12.500f, 10.000f, 2.500f); // 16
        //    modelPoints3D[17] = new MCvPoint3D32f(11.500f, 8.750f, 2.500f); // 17
        //    modelPoints3D[18] = new MCvPoint3D32f(12.500f, 8.500f, 2.500f); // 18
        //    modelPoints3D[19] = new MCvPoint3D32f(13.500f, 8.250f, 2.500f); // 19
        //    modelPoints3D[20] = new MCvPoint3D32f(14.500f, 8.250f, 2.500f); // 20
        //    modelPoints3D[21] = new MCvPoint3D32f(3.500f, 8.750f, 2.500f); // 21
        //    modelPoints3D[22] = new MCvPoint3D32f(4.500f, 10.000f, 2.500f); // 22
        //    modelPoints3D[23] = new MCvPoint3D32f(5.500f, 10.250f, 2.500f); // 23
        //    modelPoints3D[24] = new MCvPoint3D32f(6.500f, 10.000f, 2.500f); // 24
        //    modelPoints3D[25] = new MCvPoint3D32f(7.500f, 8.750f, 2.500f); // 25
        //    modelPoints3D[26] = new MCvPoint3D32f(6.500f, 8.500f, 2.500f); // 26
        //    modelPoints3D[27] = new MCvPoint3D32f(5.500f, 8.250f, 2.500f); // 27
        //    modelPoints3D[28] = new MCvPoint3D32f(4.500f, 8.250f, 2.500f); // 28
        //    modelPoints3D[29] = new MCvPoint3D32f(11.500f, 11.250f, 7.500f); // 29
        //    modelPoints3D[30] = new MCvPoint3D32f(13.500f, 12.750f, 7.500f); // 30
        //    modelPoints3D[31] = new MCvPoint3D32f(15.500f, 12.500f, 7.500f); // 31
        //    modelPoints3D[32] = new MCvPoint3D32f(17.000f, 11.500f, 6.250f); // 32
        //    modelPoints3D[33] = new MCvPoint3D32f(18.000f, 9.250f, 5.000f); // 33
        //    modelPoints3D[34] = new MCvPoint3D32f(7.500f, 11.250f, 7.500f); // 34
        //    modelPoints3D[35] = new MCvPoint3D32f(5.500f, 12.750f, 7.500f); // 35
        //    modelPoints3D[36] = new MCvPoint3D32f(3.500f, 12.500f, 7.500f); // 36
        //    modelPoints3D[37] = new MCvPoint3D32f(2.000f, 11.500f, 6.250f); // 37
        //    modelPoints3D[38] = new MCvPoint3D32f(1.000f, 9.250f, 5.000f); // 38
        //    modelPoints3D[39] = new MCvPoint3D32f(6.250f, -2.000f, 5.000f); // 39
        //    modelPoints3D[40] = new MCvPoint3D32f(8.500f, -0.500f, 6.250f); // 40
        //    modelPoints3D[41] = new MCvPoint3D32f(9.500f, -0.750f, 7.500f); // 41
        //    modelPoints3D[42] = new MCvPoint3D32f(10.500f, -0.500f, 6.250f); // 42
        //    modelPoints3D[43] = new MCvPoint3D32f(12.750f, -2.000f, 5.000f); // 43
        //    modelPoints3D[44] = new MCvPoint3D32f(10.500f, -3.000f, 6.250f); // 44
        //    modelPoints3D[45] = new MCvPoint3D32f(9.500f, -2.750f, 7.500f); // 45
        //    modelPoints3D[46] = new MCvPoint3D32f(8.500f, -3.000f, 6.250f); // 46
        //    modelPoints3D[47] = new MCvPoint3D32f(9.000f, 8.500f, 6.250f); // 47
        //    modelPoints3D[48] = new MCvPoint3D32f(8.000f, 5.500f, 6.250f); // 48
        //    modelPoints3D[49] = new MCvPoint3D32f(7.250f, 4.500f, 6.250f); // 49
        //    modelPoints3D[50] = new MCvPoint3D32f(7.500f, 2.500f, 6.250f); // 50
        //    modelPoints3D[51] = new MCvPoint3D32f(8.500f, 2.250f, 6.250f); // 51
        //    modelPoints3D[52] = new MCvPoint3D32f(9.500f, 2.000f, 6.250f); // 52
        //    modelPoints3D[53] = new MCvPoint3D32f(10.500f, 2.250f, 6.250f); // 53
        //    modelPoints3D[54] = new MCvPoint3D32f(11.500f, 2.500f, 6.250f); // 54
        //    modelPoints3D[55] = new MCvPoint3D32f(11.750f, 4.500f, 6.250f); // 55
        //    modelPoints3D[56] = new MCvPoint3D32f(11.000f, 5.500f, 6.250f); // 56
        //    modelPoints3D[57] = new MCvPoint3D32f(10.000f, 8.500f, 6.250f); // 57

        //    for (int i = 0; i < modelPoints3D.Length; i++)
        //    {
        //        MCvPoint3D32f viewPoint3D = new MCvPoint3D32f
        //        {
        //            x = RotationMatrix[0] * modelPoints3D[i].x +
        //                RotationMatrix[1] * modelPoints3D[i].y +
        //                RotationMatrix[2] * modelPoints3D[i].z + TranslationVector[0],
        //            y = RotationMatrix[3] * modelPoints3D[i].x +
        //                RotationMatrix[4] * modelPoints3D[i].y +
        //                RotationMatrix[5] * modelPoints3D[i].z + TranslationVector[1],
        //            z = RotationMatrix[6] * modelPoints3D[i].x +
        //                RotationMatrix[7] * modelPoints3D[i].y +
        //                RotationMatrix[8] * modelPoints3D[i].z + TranslationVector[2]
        //        };

        //        FaceModelPoints[i] = new PointF(0.0f, 0.0f);

        //        if (Math.Abs(viewPoint3D.z - 0) > float.Epsilon)
        //        {
        //            FaceModelPoints[i].X = 760.0f * viewPoint3D.x / viewPoint3D.z;
        //            FaceModelPoints[i].Y = 760.0f * viewPoint3D.y / viewPoint3D.z;
        //        }
        //    }

        //    float xDiff = FaceModelPoints[0].X - 50;
        //    float yDiff = FaceModelPoints[0].Y - 100;

        //    for (int i = 0; i < modelPoints3D.Length; i++)
        //    {
        //        FaceModelPoints[i].X -= xDiff;
        //        FaceModelPoints[i].Y -= yDiff;
        //    }
        //}
    }
}
