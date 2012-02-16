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
            modelPoints3D[1] = new MCvPoint3D32f(50.0f, 0.0f, 0.0f);
            modelPoints3D[2] = new MCvPoint3D32f(0.0f, 50.0f, 0.0f);
            modelPoints3D[3] = new MCvPoint3D32f(0.0f, 0.0f, 50.0f);

            for( int i = 0; i < modelPoints3D.Length; i++ )
	        {
	            MCvPoint3D32f viewPoint3D = new MCvPoint3D32f
	                                            {
	                                                x = RotationMatrix[0]*modelPoints3D[i].x +
	                                                    RotationMatrix[1]*modelPoints3D[i].y +
	                                                    RotationMatrix[2]*modelPoints3D[i].z + TranslationVector[0],
	                                                y = RotationMatrix[3]*modelPoints3D[i].x +
	                                                    RotationMatrix[4]*modelPoints3D[i].y +
	                                                    RotationMatrix[5]*modelPoints3D[i].z + TranslationVector[1],
	                                                z = RotationMatrix[6]*modelPoints3D[i].x +
	                                                    RotationMatrix[7]*modelPoints3D[i].y +
	                                                    RotationMatrix[8]*modelPoints3D[i].z + TranslationVector[2]
	                                            };




	            ModelPoints[i] = new PointF(0.0f, 0.0f);

                if( Math.Abs(viewPoint3D.z - 0) > float.Epsilon )
                {
                    ModelPoints[i].X = 760.0f * viewPoint3D.x / viewPoint3D.z; 
                    ModelPoints[i].Y = 760.0f * viewPoint3D.y / viewPoint3D.z; 	
                }
	        }

            float xDiff = ModelPoints[0].X - FaceRect.X;
            float yDiff = ModelPoints[0].Y - ( FaceRect.Y + FaceRect.Height );

            for (int i = 0; i < modelPoints3D.Length; i++)
            {
                ModelPoints[i].X -= xDiff;
                ModelPoints[i].Y -= yDiff;
            }
        }
    }
}
