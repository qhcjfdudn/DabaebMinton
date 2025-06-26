using System;
using System.Net.Sockets;
using System.Text;
using Unity.Collections;
using UnityEngine;

public class NetworkManagerUI : MonoBehaviour
{
    private static NetworkManagerUI _instance;

    public static NetworkManagerUI Instance
    {
        get
        {
            if (_instance == null)
            {
                Debug.Log("NetworkManagerUI GameObject가 아직 생성되지 않았습니다. instance는 null을 return합니다.");
            }
            return _instance;
        }
    }

    private TcpClient _client;
    private NetworkStream _stream;
    private byte[] _receiveBuffer = new byte[1024];
    private OutputMemoryBitStream _outBuffer = new OutputMemoryBitStream();
    public OutputMemoryBitStream OutBuffer { get { return _outBuffer; } }

    public string _serverIP = "127.0.0.1"; // 서버 IP 주소
    public int _serverPort = 50000;       // 서버 포트 번호

    private void Awake()
    {
        if (_instance != null && _instance != this)
        {
            Destroy(gameObject);
            return;
        }

        _instance = this;
    }

    private void Start()
    {
        ConnectToServer();
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

    // 연결 시작
    public void ConnectToServer()
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
                if (bytesRead > 0)
                {
                    InputMemoryBitStream inputBitStream = new InputMemoryBitStream(_receiveBuffer, bytesRead);
                    Debug.Log($"_receiveBuffer.ToString(): {BitConverter.ToString(_receiveBuffer)}");
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
            }
        }
        catch (Exception ex)
        {
            Debug.LogError("서버와의 연결이 종료되었습니다: " + ex.Message);
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
}