#include "ServerPCH.h"
#include "NetworkManagerServer.h"

#include "PacketQueue.h"

NetworkManagerServer& NetworkManagerServer::GetInstance() {
	static NetworkManagerServer sInstance;
	return sInstance;
}

void NetworkManagerServer::InitIOCP() {
	// Overlapped IO 위한 listen socket 생성
	m_listenSocket.m_socket = Socket::CreateWSASocket();
	
	cout << "listenSocket 생성 완료" << endl;

	if (m_listenSocket.Bind("0.0.0.0", 50000) == SOCKET_ERROR) {
		cout << "bind error: " << WSAGetLastError() << endl;
		return;
	}

	cout << "bind 완료" << endl;

	if (listen(m_listenSocket.m_socket, 10) == SOCKET_ERROR) {
		cout << "listen error: " << WSAGetLastError() << endl;
		return;
	}

	cout << "listen 완료" << endl;

	// IOCP 생성
	mh_iocp = CreateIoCompletionPort(
		INVALID_HANDLE_VALUE, 
		nullptr, 
		reinterpret_cast<ULONG_PTR>(nullptr), 
		m_threadCount);

	cout << "IOCP 생성 완료" << endl;

	// IOCP에 listen socket 추가
	if (CreateIoCompletionPort(
		reinterpret_cast<HANDLE>(m_listenSocket.m_socket),
		mh_iocp, 
		reinterpret_cast<ULONG_PTR>(nullptr), 
		0) == nullptr) {
		cout << "Add IOCP error: " << GetLastError() << endl;
		//cout << "Add IOCP error: " << WSAGetLastError() << endl;
		return;
	}

	cout << "IOCP에 listenSocket 등록 완료" << endl;
	
	// WSAIoctl로부터 AcceptEx 함수 포인터 할당
	GUID guidAcceptEx = WSAID_ACCEPTEX;
	DWORD dwBytes;

	if (WSAIoctl(
		m_listenSocket.m_socket,			// 소켓 API라 필요한 arg인 것 같은데... 아직 왜 필요한지 모르겠음.
		SIO_GET_EXTENSION_FUNCTION_POINTER,	// AcceptEx 함수 포인터를 얻기 위한 제어 코드
		&guidAcceptEx,						// 얻고자 하는 함수 이름의 지정된 값(WSAID_ACCEPTEX) 사용.
		sizeof(guidAcceptEx),
		&m_AcceptEx,						// 요청에 대한 출력 버퍼. AcceptEx 함수 포인터 출력.
		sizeof(m_AcceptEx),
		&dwBytes,							// 출력버퍼로 출력된 개수. <- 애매
		nullptr,							// lpOverlapped: WSAOVERLAPPED 구조체 포인터. 지금 불필요.
		nullptr) == SOCKET_ERROR) {			// lpCompletionRoutine: 작업 완료 후 호출할 루틴 전달 가능. // 지금 불필요.

		cout << "WSAIoctl error: " << WSAGetLastError() << endl;
		}

	if (m_AcceptEx == nullptr) {
		cout << "Getting AcceptEx ptr failed." << endl;

		return;
	}

	cout << "WSAIoctl 반환 및 m_AcceptEx 함수 포인터 획득 완료" << endl;

	AcceptEx();
}
void NetworkManagerServer::AcceptEx()
{
	m_clientCandidateSocket.m_socket = Socket::CreateWSASocket();

	bool acceptExStatus = m_AcceptEx(
		m_listenSocket.m_socket,					// listenSocket
		m_clientCandidateSocket.m_socket,			// Accept가 이루어지면 client socket으로 변한다.
		m_lpOutputBuf,								// 첫 번째 데이터 블록, 서버 로컬주소, 클라 원격 주소 수신 버퍼.
													// 이 변수와 아래 3개의 바이트 수 정보 변수는 GetAcceptExSockaddrs() 함수의 인자로
													// 로컬/원격 sockaddr 정보를 파싱할 때에 사용될 수 있다.
		0,											// 수신에 사용할 데이터 바이트 수. 0이면 데이터는 받지 않고 accept만 하겠다는 의미.
		sizeof(sockaddr_in) + 16,					// 로컬 주소 정보를 위해 예약된 바이트 수. 전송 프로토콜의 최대 길이보다 16만큼 커야 한다.
		sizeof(sockaddr_in) + 16,					// 원격 주소 정보를 위해 예약된 바이트 수. 위와 동일.
		&m_dwBytes,									// 받은 바이트 수. 불필요.
		&m_listenSocket.m_readOverlappedStruct);	// lpOverlapped: 요청을 처리하는 데 사용되는 OVERLAPPED 구조체. NULL 불가!
	// 에러가 없다면 ret은 TRUE이다.

	if (acceptExStatus == false)
	{
		int errorCode = WSAGetLastError();

		if (errorCode == ERROR_IO_PENDING) // 아직 IO 처리 중. 정상 상태
		{
		}
		else
		{
			cout << "AcceptEx() Error" << endl;
			cout << errorCode << endl;
		}
	}
	else
	{
		cout << "m_AcceptEx 함수 수행 완료" << endl;
	}
}
void NetworkManagerServer::ProcessIOCPEvent()
{
	GetCompletionStatus();

	// 받은 이벤트 각각을 처리합니다.
	for (int i = 0; i < m_iocpEvent.m_eventCount; i++)
	{
		auto& readEvent = m_iocpEvent.m_events[i];
		ULONG_PTR completionKey = readEvent.lpCompletionKey;
		if (completionKey == 0) // listenSocket. AcceptEx에 의해 신규 client 접속
		{
			if (ProcessAcceptedClientSocketIOCP() == false)
				return;
		}
		else // clientSocket
		{
			shared_ptr<Socket> p_clientSocket = m_clientsMap[completionKey];

			// event가 WSASend의 완료에 의해 발생했다면, 무시하자.
			if (readEvent.lpOverlapped == &p_clientSocket->m_sendOverlappedStruct)
			{
				continue;
			}

			unsigned int receivedBytes = readEvent.dwNumberOfBytesTransferred;
			if (receivedBytes == 0) // sendBytes == 0일 때 clientSocket 제거 로직 필요
			{
				closesocket(p_clientSocket->m_socket);
				cout << "close socket: " << completionKey << endl;
				continue;
			}

			ReceivePacketsIOCP(p_clientSocket, receivedBytes);
		}
	}
}
bool NetworkManagerServer::GetCompletionStatus()
{
	bool ret = GetQueuedCompletionStatusEx(
		mh_iocp,							// IOCP 객체
		m_iocpEvent.m_events,					// 처리가 완료된 event를 수신하는 배열
		MAX_IOCP_EVENT_COUNT,				// 최대 event 개수
		(ULONG*)&m_iocpEvent.m_eventCount,	// 수신한 event 개수를 받을 변수
		m_timeoutMs,							// 다시 분석 필요
		FALSE);								// fAlertable: ?????

	if (ret == false)	// 실패 시
	{
		int errorCode = WSAGetLastError();
		
		if (errorCode == WSA_WAIT_TIMEOUT)	// timeoutMs 동안 event가 발생하지 않았다.
											// 별도 처리할 내용 없음
		{}
		else
		{
			cout << "GetQueuedCompletionStatusEx 실패" << endl;
			cout << errorCode << endl;
		}
		
		m_iocpEvent.m_eventCount = 0;			// 실패 시 수동으로 변경 필요 코드
	}

	return ret;
}
bool NetworkManagerServer::ProcessAcceptedClientSocketIOCP()
{
	cout << "listen Socket!!!" << endl;

	// m_clientCandidateSocket로부터 신규 client socket을 iocp에 추가
	// listenSocket과 동일한 context로 clientSocket을 최적화
	setsockopt(
		m_clientCandidateSocket.m_socket,
		SOL_SOCKET,
		SO_UPDATE_ACCEPT_CONTEXT,
		reinterpret_cast<const char*>(&m_listenSocket),
		sizeof(m_listenSocket));

	// SOCKET 타입은 UINT_PTR일 뿐이다. 아래와 같이 값을 대입하고
	// 이후에 m_clientCandidateSocket을 다시 listen에 사용하더라도
	// m_clientCandidateSocket 변수는 새로운 clientSocket의 주소를 가지고 있을 것이다.
	// // ac, 104, ... 이런 값으로 출력.
	// 어쨌든 연결될 때마다 다른 값이다.
	shared_ptr<Socket> clientSocket = make_shared<Socket>();
	clientSocket->m_socket = m_clientCandidateSocket.m_socket;

	// 신규 client를 IOCP에 추가
	const ULONG_PTR completionKey = reinterpret_cast<ULONG_PTR>(clientSocket.get());
	cout << "completionKey: " << completionKey << endl;

	if (AddSocketIOCP(clientSocket, completionKey) == nullptr) {
		cout << "Add IOCP error: " << WSAGetLastError() << endl;
		return false;
	}

	// 이후 completionKey로 clientSocket 참조 위해 map에 저장해둔다.
	m_clientsMap.insert({ completionKey, clientSocket });

	// 다시 listenSocket을 accept로 변경
	// listenSocket.AcceptEx() 형태로 쓰는 게 좋을 것 같다. 추후 리팩터링 진행.
	AcceptEx();

	// 연결한 clientSocket을 recv로 전환
	Recv(clientSocket);

	return true;
}
HANDLE NetworkManagerServer::AddSocketIOCP(std::shared_ptr<Socket> clientSocket, const ULONG_PTR completionKey)
{
	return CreateIoCompletionPort(
		reinterpret_cast<HANDLE>(clientSocket->m_socket),
		mh_iocp,
		completionKey,
		0);
}
void NetworkManagerServer::ReceivePacketsIOCP(std::shared_ptr<Socket> p_clientSocket, unsigned int receivedBytes)
{
	Packet packet{ p_clientSocket->m_receiveBuffer, receivedBytes };
	auto& receiveQueue = PacketQueue::GetReceiveStaticInstance();
	receiveQueue.PushCopy(packet);

	// 다시 수신 대기
	Recv(p_clientSocket);
}
int NetworkManagerServer::Recv(shared_ptr<Socket> clientSocket)
{
	WSABUF b;
	b.buf = clientSocket->m_receiveBuffer;
	b.len = clientSocket->MAX_RECEIVE_LENGTH;

	DWORD& numberOfBytesReceived = clientSocket->m_numberOfBytesReceived;
	
	// overlapped I/O가 진행되는 동안 여기 값이 채워집니다.
	clientSocket->m_readFlags = 0;

	int retCode = WSARecv(
		clientSocket->m_socket,
		&b,										// lpBuffers.
		1,										// dwBufferCount. lpBuffers 배열의 구조체 개수.
		&numberOfBytesReceived,					// lpNumberOfBytesRecvd. TCP같은 연결지향형에서
		&clientSocket->m_readFlags,
		&clientSocket->m_readOverlappedStruct,
		NULL);									// lpCompletionRoutine. 수신 작업 완료 루틴에 대한 포인터.

	return retCode;
}
void NetworkManagerServer::SendPacketsIOCP()
{
	auto& sendQueue = PacketQueue::GetSendStaticInstance();

	while (sendQueue.Empty() == false)
	{
		auto data = sendQueue.Front();
		unsigned int sentBytes = data->GetLength();
		cout << "sentBytes: " << sentBytes << endl;
		
		// broadcast
		for (auto clientSocketPair : m_clientsMap)
		{
			auto clientSocket = clientSocketPair.second;
			clientSocket->SetSendBuffer(data->GetBuffer(), sentBytes);
			Send(clientSocket, sentBytes);
		}
	}
}
int NetworkManagerServer::Send(shared_ptr<Socket> clientSocket, size_t len)
{
	WSABUF b;
	b.buf = clientSocket->m_sendBuffer;
	b.len = static_cast<ULONG>(len);

	int retCode = WSASend(
		clientSocket->m_socket,
		&b,
		1,
		&clientSocket->m_numberOfBytesSent,
		clientSocket->m_sendFlags,
		&clientSocket->m_sendOverlappedStruct,
		NULL);

	return retCode;
}

NetworkManagerServer::NetworkManagerServer()
{
	if (WSAStartup(MAKEWORD(2, 2), &m_wsa) != 0)
	{
		cout << "WSAStartup failed" << endl;
		
		return;
	}
	cout << "WSAStartup" << endl;
}
NetworkManagerServer::~NetworkManagerServer() {
	WSACleanup();
	cout << "WSACleanup" << endl;
}
