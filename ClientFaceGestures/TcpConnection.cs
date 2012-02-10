using System.Net;
using System.Net.Sockets;

namespace ClientFaceGestures
{
    public class TcpConnection
    {
        private TcpClient _tcpClient;
        private NetworkStream _networkStream;

        public bool IsOpen()
        {
            return _tcpClient != null && _tcpClient.Client.Connected;
        }

        public bool Open(IPEndPoint ipEndPoint)
        {
            _tcpClient = new TcpClient();
            _tcpClient.Client.SetSocketOption(SocketOptionLevel.Socket, SocketOptionName.ReuseAddress, true);
            _tcpClient.Connect(ipEndPoint);

            if (_tcpClient.Connected)
            {
                _networkStream = _tcpClient.GetStream();
                return true;
            }
            return false;
        }

        public void Close()
        {
            if (_tcpClient == null || !_tcpClient.Connected) return;

            _tcpClient.Client.Shutdown(SocketShutdown.Both);
            _networkStream.Close();
            _tcpClient.Close();
        }

        public void Send(byte[] data)
        {
            _networkStream.Write(data, 0, data.Length);
            _networkStream.Flush();
        }

        public void Send(byte[] data, int length)
        {
            _networkStream.Write(data, 0, length);
            _networkStream.Flush();
        }

        public byte[] Receive()
        {
            int loop = 0;

            while (loop < 1000)
            {
                if (_networkStream.DataAvailable)
                {
                    byte[] data = new byte[_tcpClient.Available];
                    _networkStream.Read(data, 0, data.Length);
                    return data;
                }
                System.Threading.Thread.Sleep(1);
                loop++;
            }
            return null;
        }
    }
}
