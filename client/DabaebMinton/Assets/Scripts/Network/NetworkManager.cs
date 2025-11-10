using System;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;
using UnityEngine;

using static NetworkUtils;

using PacketSequenceNumber = System.UInt16;
public enum PacketType
{
    PT_None,
    PT_Hello,
    PT_ReplicationData,
    PT_RPC,
    PT_Disconnect,
    PT_Max
};

public class NetworkManager : MonoBehaviour
{
    private static NetworkManager _instance;

    public static NetworkManager Instance
    {
        get
        {
            if (_instance == null)
            {
                Debug.Log("NetworkManager GameObject가 아직 생성되지 않았습니다. instance는 null을 return합니다.");
            }
            return _instance;
        }
    }

    private TcpClient _client;
    private UdpClient _udpClient;
    private NetworkStream _stream;
    private byte[] _receiveBuffer = new byte[1024];
    private OutputMemoryBitStream _outBuffer = new OutputMemoryBitStream();
    public OutputMemoryBitStream OutBuffer { get { return _outBuffer; } }

    public string _serverIP = "127.0.0.1"; // 서버 IP 주소
    public int _serverPort = 50000;       // 서버 포트 번호

    public string OnlinePlayServerIp { get; set; } = "";
    public UInt16 OnlinePlayServerPort { get; set; } = 0;
    public UInt64 OnlinePlaySessionId { get; set; } = 0;
    public UInt32 OnlinePlayPlayerId { get; set; } = 0;

    private ReplicationManager _replicationManager;
    private DeliveryNotificationManager _deliveryNotificationManager;
    public RPCManager _rpcManager;

    public Action onHelloFromServer;

    private NetworkManager() { }

    // 연결 시작
    public void ConnectToServerTcp()
    {
        try
        {
            _client = new TcpClient();
            _client.Connect(_serverIP, _serverPort); // 서버에 연결
            _stream = _client.GetStream();

            Debug.Log("서버에 연결되었습니다.");

            StartListening(); // 데이터 수신 대기
        }
        catch (Exception ex)
        {
            Debug.LogError("서버 연결 실패: " + ex.Message);
        }
    }

    // 데이터 수신 대기
    private async void StartListening()
    {
        try
        {
            while (_client != null && _client.Connected)
            {
                int bytesRead = await _stream.ReadAsync(_receiveBuffer, 0, _receiveBuffer.Length);
                // bytesRead가 있기 때문에, 현재까지 읽어 들인 bytes를 세면 마지막 데이터로 0을 주지 않아도 된다.
                if (bytesRead > 0)
                {
                    Debug.Log($"bytesRead: {bytesRead}");
                    InputMemoryBitStream outStream = new InputMemoryBitStream(_receiveBuffer, bytesRead);
                    Debug.Log($"_receiveBuffer.ToString(): {BitConverter.ToString(outStream.StreamBuffer)}");

                    PacketType pt = (PacketType)outStream.ReadBits(GetRequiredBits((int)PacketType.PT_Max))[0];
                    Debug.Log($"pt: {pt.ToString()}");

                    switch (pt)
                    {
                        case PacketType.PT_Hello:
                            break;

                        case PacketType.PT_ReplicationData:
                            _replicationManager.ProcessReplicationAction(outStream);
                            break;

                        case PacketType.PT_Disconnect:
                            break;

                        default: break;
                    }


                    //ProcessIncomingDataTest(outStream);
                }
            }
        }
        catch (Exception ex)
        {
            Debug.LogError("서버와의 연결이 종료되었습니다: " + ex.Message);
            Debug.Log(ex.StackTrace.ToString());
        }
    }

    public void ConnectToOnlinePlayServer()
    {
        try
        {
            _udpClient = new UdpClient();
            _udpClient.Connect(OnlinePlayServerIp, OnlinePlayServerPort); // 서버에 연결

            // 수신 대기 시작
            RecvFromOnlinePlayServer();

        }
        catch (Exception ex)
        {
            Debug.LogError("서버 연결 실패: " + ex.Message);
        }
    }

    private async void RecvFromOnlinePlayServer()
    {
        try
        {
            while (true)
            {
                UdpReceiveResult result = await _udpClient.ReceiveAsync();
                byte[] receivedData = result.Buffer;
                if (receivedData.Length > 0)
                {
                    InputMemoryBitStream inStream = new InputMemoryBitStream(receivedData, receivedData.Length);

                    ProcessPacket(inStream);
                }
            }
        }
        catch (Exception ex)
        {
            Debug.LogError("서버와의 연결이 종료되었습니다: " + ex.Message);
            Debug.Log(ex.StackTrace.ToString());
        }
    }

    void ProcessPacket(InputMemoryBitStream inStream)
    {
        PacketType packetType = (PacketType)inStream.ReadBits(GetRequiredBits((int)PacketType.PT_Max))[0];
        Debug.Log($"Packet Type: {packetType.ToString()}");

        PacketSequenceNumber packetSequenceNumber = BitConverter.ToUInt16(inStream.ReadBits(16));
        Debug.Log($"PacketSequenceNumber: {packetSequenceNumber}");

        _deliveryNotificationManager.AddPendingAck(packetSequenceNumber);

        switch (packetType)
        {
            case PacketType.PT_Hello:
                ProcessHello(inStream);
                break;
            case PacketType.PT_ReplicationData:
                _replicationManager.ProcessReplicationAction(inStream);
                break;
            case PacketType.PT_Disconnect:
                break;
            default: break;
        }
    }

    private void ProcessHello(InputMemoryBitStream inStream)
    {
        Debug.Log("Hello 패킷 수신 완료.");
        _replicationManager.ProcessReplicationAction(inStream);

        // Hello Packet이 만약 여러 개라면 모두 처리하지 않은 상태에서 Invoke는 좋지 않다.
        // 모든 Hello가 처리된 것을 감시하다가 이걸 호출하는 형태의 감시자가 있어야 한다.
        onHelloFromServer.Invoke();
    }

    private async void SendToOnlinePlayServer()
    {
        try
        {
            int len = (_outBuffer.Count + 7) / 8;
            byte[] dataToSend = new byte[len];
            Buffer.BlockCopy(_outBuffer.StreamBuffer, 0, dataToSend, 0, len);

            _outBuffer.Clear();

            await _udpClient.SendAsync(dataToSend, len);
        }
        catch (Exception ex)
        {
            Debug.LogError("서버에 메시지 전송 실패: " + ex.Message);
            Debug.Log(ex.StackTrace.ToString());
        }
    }

    private void ProcessIncomingDataTest(InputMemoryBitStream inputBitStream)
    {
        while (true)
        {
            int packetHeader = inputBitStream.ReadBits(2)[0];
            if (packetHeader == 0)
            {
                break;
            }

            if (packetHeader == 1)
            {
                UnwrapAddNewChat(inputBitStream);
            }
            else if (packetHeader == 2)
            {
                UnwrapSetUsername(inputBitStream);
            }
        }
    }

    public async void SendPacketsEveryPacketPeriodUdp()
    {
        while (true)
        {
            await Task.Delay(100); // 100ms 간격으로 패킷 전송
            SendPacketsEveryPacketPeriod();
        }
    }

    public void SendPacketsToServer()
    {
        if (null == _client || false == _client.Connected)
        {
            PrintNotConnectedToServerMessage();
            return;
        }

        try
        {
            Debug.Log($"before _outBuffer.ToString(): {BitConverter.ToString(_outBuffer.StreamBuffer)}");
            _outBuffer.WriteBits(0, 2);
            Debug.Log($"after _outBuffer.ToString(): {BitConverter.ToString(_outBuffer.StreamBuffer)}");
            _stream.Write(_outBuffer.StreamBuffer, 0, _outBuffer.Count); // 메시지 전송
            _outBuffer.InitBuffer();
        }
        catch (Exception ex)
        {
            Debug.LogError("메시지 전송 실패: " + ex.Message);
        }
    }

    public void SendHello()
    {
        Debug.Log("[NetworkManager] SendHello called.");

        _outBuffer.Clear();

        // Session ID
        _outBuffer.Write(OnlinePlaySessionId);

        // Acks
        _outBuffer.Write(false);

        // Packet Type
        _outBuffer.WriteBits((int)PacketType.PT_Hello, GetRequiredBits(PacketType.PT_Max));

        SendToOnlinePlayServer();
    }

    public void SendPacketsEveryPacketPeriod()
    {
        if (_deliveryNotificationManager.hasAcks() == false &&
            _rpcManager.hasRpcToSend() == false)
        {
            return;
        }

        if (_deliveryNotificationManager.hasAcks())
        {
            Debug.Log("Sending Acks to server.");
        }

        _outBuffer.Clear();

        _outBuffer.Write(OnlinePlaySessionId);
        _deliveryNotificationManager.WriteAckData(_outBuffer);

        if (_rpcManager.hasRpcToSend())
        {
            _outBuffer.WriteBits((int)PacketType.PT_RPC, GetRequiredBits(PacketType.PT_Max));
            _outBuffer.Append(_rpcManager.GetStream());

            _rpcManager.Clear();
        }
        else
        {
            _outBuffer.WriteBits((int)PacketType.PT_None, GetRequiredBits(PacketType.PT_Max));
        }

        SendToOnlinePlayServer();
    }

    private void PrintNotConnectedToServerMessage()
    {
        Debug.LogError("서버에 연결되지 않았습니다.");
    }

    private void UnwrapAddNewChat(InputMemoryBitStream inputBitStream)
    {
        int stringLength = inputBitStream.ReadBits(8)[0];
        Debug.Log($"stringLength: {stringLength}");
        string newChat = Encoding.UTF8.GetString(inputBitStream.ReadBits(stringLength * 8));
        Debug.Log($"newChat: {newChat}");

        FindAnyObjectByType<ChatController>().AddNewChat(newChat);
    }

    private void UnwrapSetUsername(InputMemoryBitStream inputBitStream)
    {
        int stringLength = inputBitStream.ReadBits(8)[0];
        Debug.Log($"stringLength: {stringLength}");
        string newUsername = Encoding.UTF8.GetString(inputBitStream.ReadBits(stringLength * 8));
        Debug.Log($"newUsername: {newUsername}");
        FindAnyObjectByType<UIController>().SetUsername(newUsername);
    }

    private void Awake()
    {
        if (_instance != null && _instance != this)
        {
            Destroy(gameObject);
            return;
        }

        _instance = this;
        DontDestroyOnLoad(gameObject);

        _replicationManager = ReplicationManager.Instance;
        _deliveryNotificationManager = new DeliveryNotificationManager();
        _rpcManager = new RPCManager();
    }

    private void Start()
    {
        SendPacketsEveryPacketPeriodUdp();
    }

    // 연결 종료
    private void OnApplicationQuit()
    {
        if (_client != null)
        {
            _stream?.Close();
            _client?.Close();
        }
    }
}
