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
    }
}
