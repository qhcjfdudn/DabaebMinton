#include "ServerPCH.h"
#include "NetworkManagerServer.h"

#include "OutputMemoryBitStream.h"
#include "InputMemoryBitStream.h"
#include "PacketQueue.h"

#include "Constant.h"
#include "GetRequiredBits.h"
#include "ClientInfo.h"
#include "SockAddress.h"

#include "PacketGenerator.h"

NetworkManagerServer& NetworkManagerServer::GetInstance()
{
	static NetworkManagerServer sInstance;
	return sInstance;
}

void NetworkManagerServer::InitIOCP()
{
	// IOCP 생성
	mh_iocp = CreateIoCompletionPort(
		INVALID_HANDLE_VALUE, 
		nullptr, 
		reinterpret_cast<ULONG_PTR>(nullptr), 
		m_threadCount);

	spdlog::info("[NetworkManagerServer::InitIOCP] IOCP creation complete.");

	CreateListenSocket();
	m_listenSocket.SetCompletionKey(0);

	// listenSocket으로부터 AcceptEx, GetAcceptExSockAddrs 확장 함수 획득 위한 WSAIoctl 함수 호출
	GetLPFN();

	// IOCP에 listen socket 추가
	if (CreateIoCompletionPort(
		reinterpret_cast<HANDLE>(m_listenSocket.m_socket),
		mh_iocp,
		m_listenSocket.GetCompletionKey(),
		0) == nullptr)
	{
		spdlog::error("[NetworkManagerServer::InitIOCP] Add IOCP error: {}", WSAGetLastError());
		return;
	}

	spdlog::info("[NetworkManagerServer::InitIOCP] listenSocket attach complete.");

	AcceptEx();

	CreateRUDPSocket();
	const ULONG_PTR completionKey = reinterpret_cast<ULONG_PTR>(&m_rudpSocket);
	m_rudpSocket.SetCompletionKey(completionKey);

	// IOCP에 RUDP socket 추가
	if (CreateIoCompletionPort(
		reinterpret_cast<HANDLE>(m_rudpSocket.m_socket),
		mh_iocp,
		m_rudpSocket.GetCompletionKey(),
		0) == nullptr)
	{
		spdlog::error("[NetworkManagerServer::InitIOCP] Add IOCP error: {}", WSAGetLastError());
		return;
	}

	spdlog::info("[NetworkManagerServer::InitIOCP] RUDP Socket completionKey: {}", completionKey);

	RecvFrom();

	spdlog::info("[NetworkManagerServer::InitIOCP] RUDPSocket attach complete.");
}
void NetworkManagerServer::AcceptEx()
{
	m_clientCandidateSocket.m_socket = Socket::CreateWSASocketHandle(SocketProtocolType::SPT_TCP);

	auto& overlapped = m_listenSocket._recvOverlappedDto;
	ZeroMemory(&overlapped, sizeof(overlapped));

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
		&overlapped._overlapped);	// lpOverlapped: 요청을 처리하는 데 사용되는 OVERLAPPED 구조체. NULL 불가!
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
void NetworkManagerServer::GetAcceptExSockAddrs(shared_ptr<Socket> client)
{
	// UDP 통신을 하려면 remote의 addr 정보가 필요하다. socket으로부터 얻자.
	sockaddr_in* localAddr = nullptr;
	sockaddr_in* clientAddr = nullptr;
	int localAddrLen, clientAddrLen;

	m_GetAcceptExSockAddrs(
		m_lpOutputBuf,
		0,
		sizeof(sockaddr_in) + 16,
		sizeof(sockaddr_in) + 16,
		reinterpret_cast<sockaddr**>(localAddr),
		&localAddrLen,
		reinterpret_cast<sockaddr**>(clientAddr),
		&clientAddrLen
	);

	client->SetRemoteAddress(*clientAddr);
}
void NetworkManagerServer::ProcessIOCPEvent()
{
	GetCompletionStatus();

	// 받은 이벤트 각각을 처리합니다.
	for (int i = 0; i < m_iocpEvent.m_eventCount; i++)
	{
		auto& readEvent = m_iocpEvent.m_events[i];
		ULONG_PTR completionKey = readEvent.lpCompletionKey;
		auto& lpOverlapped = readEvent.lpOverlapped;
		unsigned int receivedBytes = readEvent.dwNumberOfBytesTransferred;

		if (completionKey == m_listenSocket.GetCompletionKey()) // AcceptEx에 의해 신규 client 접속
		{
			if (ProcessAcceptedClientSocketIOCP() == false)
				return;
		}
		else if (completionKey == m_rudpSocket.GetCompletionKey())
		{
			auto& recvOverlapped = m_rudpSocket._recvOverlappedDto;

			if (lpOverlapped != &recvOverlapped._overlapped)	// sendOverlapped의 완료 이벤트인가?
			{
				NotifySendOverlappedCompletionEvent(lpOverlapped);
				
				// sendOverlapped의 완료 결과에 오류가 발생한다면 여기에서
				// WSAGetOverlappedResult() 호출로 검출 가능

				continue;
			}

			// recv packet을 처리하는 코드

			if (receivedBytes == 0)
			{
				// DisconnectClient
			}
			else if (receivedBytes > 0)
			{
				// remote sockaddr을 받아서 ClientInfo를 알아야 한다.

				auto inStreamPtr = make_shared<InputMemoryBitStream>(m_rudpSocket._sendOverlappedDto._Buffer, receivedBytes << 3);
				_receivedQueue.push(inStreamPtr);
			}

			RecvFrom();
		}
		else // clientSocket
		{
			shared_ptr<Socket> p_clientSocket = m_clientsMap[completionKey];
			auto& recvOverlapped = p_clientSocket->_recvOverlappedDto;

			// event가 WSASend의 완료에 의해 발생했다면, 무시하자.
			if (lpOverlapped != &recvOverlapped._overlapped)
			{
				continue;
			}

			// 에러 발생 시 receivedBytes는 0일 수 있다. 종료 이외 대응 필요
			if (receivedBytes == 0)
			{
				int recvBytes;
				bool isDisconnection = WSAGetOverlappedResult(p_clientSocket->m_socket,
					&recvOverlapped._overlapped,
					reinterpret_cast<LPDWORD>(&recvBytes),
					FALSE,
					&recvOverlapped._overlappedFlags);

				if (isDisconnection)
				{
					closesocket(p_clientSocket->m_socket);
					m_clientsMap.erase(completionKey);

					spdlog::info("[NetworkManagerServer::ProcessIOCPEvent] close socket: {}", completionKey);

					continue;
				}
				else
				{
					spdlog::error("[NetworkManagerServer::ProcessIOCPEvent] 0 Bytes received. WSAErrorCode: {}", WSAGetLastError());
				}

				continue;
			}

			spdlog::info("[NetworkManagerServer::ProcessIOCPEvent] receivedBytes: {}", receivedBytes);

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
	spdlog::info("[NetworkManagerServer::ProcessAcceptedClientSocketIOCP] New Client accepted.");

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
	clientSocket->SetProtocolType(SocketProtocolType::SPT_TCP);

	// 신규 client를 IOCP에 추가
	const ULONG_PTR completionKey = reinterpret_cast<ULONG_PTR>(clientSocket.get());
	spdlog::info("[NetworkManagerServer::ProcessAcceptedClientSocketIOCP] completionKey: {}", completionKey);

	if (AddSocketIOCP(clientSocket) == nullptr)
	{
		cout << "Add IOCP error: " << WSAGetLastError() << endl;
		return false;
	}

	// 이후 completionKey로 clientSocket 참조 위해 map에 저장해둔다.
	m_clientsMap.insert({ completionKey, clientSocket });

	// 다시 listenSocket을 accept로 변경
	// listenSocket.AcceptEx() 형태로 쓰는 게 좋을 것 같다. 추후 리팩터링 진행.
	AcceptEx();

	// 연결한 clientSocket을 recv로 전환

	if (clientSocket->GetProtocolType() == SocketProtocolType::SPT_TCP)
		Recv(clientSocket);
	else if (clientSocket->GetProtocolType() == SocketProtocolType::SPT_RUDP)
		RecvFrom();
	else
	{
		spdlog::error("[NetworkManagerServer::ProcessAcceptedClientSocketIOCP] Invalid SocketProtocolType.");
		return false;
	}

	return true;
}
HANDLE NetworkManagerServer::AddSocketIOCP(std::shared_ptr<Socket> socket)
{
	return CreateIoCompletionPort(
		reinterpret_cast<HANDLE>(socket->m_socket),
		mh_iocp,
		socket->GetCompletionKey(),
		0);
}
void NetworkManagerServer::ReceivePacketsIOCP(std::shared_ptr<Socket> p_clientSocket, unsigned int receivedBytes)
{
	auto& recvOverlapped = p_clientSocket->_recvOverlappedDto;
	Packet packet{ recvOverlapped._Buffer, receivedBytes };
	auto& receiveQueue = PacketQueue::GetReceiveStaticInstance();
	receiveQueue.PushCopy(packet);

	// 다시 수신 대기
	if (p_clientSocket->GetProtocolType() == SocketProtocolType::SPT_TCP)
		Recv(p_clientSocket);
	else if (p_clientSocket->GetProtocolType() == SocketProtocolType::SPT_RUDP)
		RecvFrom();
	else
		spdlog::error("[NetworkManagerServer::ReceivePacketsIOCP] Invalid SocketProtocolType.");
}
int NetworkManagerServer::Send(shared_ptr<Socket> clientSocket, size_t len)
{
	auto& sendOverlapped = clientSocket->_sendOverlappedDto;

	ZeroMemory(&sendOverlapped._overlapped, sizeof(sendOverlapped._overlapped));
	
	WSABUF& b = sendOverlapped._wsabuf;
	b.buf = reinterpret_cast<CHAR*>(sendOverlapped._Buffer);
	b.len = static_cast<ULONG>(len);

	int retCode = WSASend(
		clientSocket->m_socket,
		&b,
		1,
		&sendOverlapped._numberOfBytesTransfered,
		sendOverlapped._overlappedFlags,
		&sendOverlapped._overlapped,
		NULL);

	return retCode;
}
int NetworkManagerServer::Recv(shared_ptr<Socket> clientSocket)
{
	auto& recvOverlapped = clientSocket->_recvOverlappedDto;

	ZeroMemory(&recvOverlapped, sizeof(recvOverlapped));
	
	WSABUF& b = recvOverlapped._wsabuf;
	b.buf = reinterpret_cast<CHAR*>(recvOverlapped._Buffer);
	b.len = sizeof(recvOverlapped._Buffer);

	int retCode = WSARecv(
		clientSocket->m_socket,
		&b,											// lpBuffers.
		1,											// dwBufferCount. lpBuffers 배열의 구조체 개수.
		&recvOverlapped._numberOfBytesTransfered,	// lpNumberOfBytesRecvd. TCP같은 연결지향형에서
		&recvOverlapped._overlappedFlags,
		&recvOverlapped._overlapped,
		NULL);										// lpCompletionRoutine. 수신 작업 완료 루틴에 대한 포인터.

	return retCode;
}

int NetworkManagerServer::SendTo(ClientInfo* client, const OutputMemoryBitStream& stream)
{
	auto& overlapped = GetNextSendOverlapped();
	ZeroMemory(&overlapped, sizeof(overlapped));
	memcpy(overlapped._Buffer, stream.GetBufferPtr(), stream.GetByteLength());

	WSABUF& b = overlapped._wsabuf;
	b.buf = reinterpret_cast<CHAR*>(overlapped._Buffer);
	b.len = static_cast<ULONG>(stream.GetByteLength());
	
	int retCode = WSASendTo(m_rudpSocket.m_socket,
		&b,
		1,
		&overlapped._numberOfBytesTransfered,
		overlapped._overlappedFlags,
		reinterpret_cast<const sockaddr*>(&client->GetSockAddress().GetSockAddr()),
		sizeof(sockaddr_in),
		&overlapped._overlapped,
		nullptr
	);

	if (retCode != 0)
	{
		spdlog::error("[NetworkManagerServer::SendTo] WSASendTo error: {}", WSAGetLastError());
	}

	return retCode;
}

int NetworkManagerServer::SendTo(ClientInfo* client)
{
	// Check TimedOut Packets
	client->GetDeliveryNotificationManager().ProcessTimedOutPackets();

	// Replication State
	SendReplicationStatePacketToClient(client);

	// RPCs
	SendRpcPacketToClient(client);

	return 0;
}

int NetworkManagerServer::RecvFrom()
{
	auto& overlapped = m_rudpSocket._recvOverlappedDto;

	ZeroMemory(&overlapped, sizeof(overlapped));
	
	WSABUF& b = overlapped._wsabuf;
	b.buf = reinterpret_cast<CHAR*>(overlapped._Buffer);
	b.len = sizeof(overlapped._Buffer);


	int lpFromLen = sizeof(m_rudpSocket.m_remoteAddr);

	int retCode = WSARecvFrom(
		m_rudpSocket.m_socket,
		&b,													// lpBuffers
		1,															// dwBufferCount
		&overlapped._numberOfBytesTransfered,						// lpNumberOfBytesRecvd
		&overlapped._overlappedFlags,								// lpFlags
		reinterpret_cast<sockaddr*>(&m_rudpSocket.m_remoteAddr),
		&lpFromLen,													// lpFromLen
		&overlapped._overlapped,									// lpOverlapped
		nullptr														// lpCompletionRoutine
	);
	
	if (retCode != 0) {
		int errorCode = WSAGetLastError();

		if (errorCode == ERROR_IO_PENDING) {
			/* [정상]. host로 전달된 packet이 없어 pending */
		}
		else {
			spdlog::error("[NetworkManagerServer::RecvFrom] WSARecvFrom error: {}", errorCode);
		}
	}

	return retCode;
}

void NetworkManagerServer::SendOutgoingPackets()
{
	// Check TimedOut Packets
	for (auto& kv : _sockAddressToClientInfoMap)
	{
		auto clientInfo = kv.second;
		clientInfo->GetDeliveryNotificationManager().ProcessTimedOutPackets();
	}

	// Replication State
	for (auto& kv : _sockAddressToClientInfoMap)
	{
		auto clientInfo = kv.second;
		SendReplicationStatePacketToClient(clientInfo.get());
	}

	// RPCs
	for (auto& kv : _sockAddressToClientInfoMap)
	{
		auto clientInfo = kv.second;
		SendRpcPacketToClient(clientInfo.get());
	}
}

void NetworkManagerServer::SendReplicationStatePacketToClient(ClientInfo* client)
{
	DeliveryNotificationManager& dnm = client->GetDeliveryNotificationManager();
	ReplicationManager& nrm = client->GetReplicationManager();

	PacketGenerator packetGenerator{ &dnm, &nrm, PacketType::PT_ReplicationData };

	client->GetReplicationManager().Write(packetGenerator);

	for (auto& stream : packetGenerator.GetAllStreams())
	{
		SendTo(client, stream);
	}
}

void NetworkManagerServer::SendRpcPacketToClient(ClientInfo* client)
{
	PacketGenerator packetGenerator{
		&client->GetDeliveryNotificationManager(),
		& client->GetReplicationManager(),
		PacketType::PT_RPC };
}

void NetworkManagerServer::ProcessReceivePacket()
{
	while (_receivedQueue.empty() == false)
	{
		auto inStream = _receivedQueue.front(); _receivedQueue.pop();
		
	}
}

NetworkManagerServer::NetworkManagerServer()
{
	if (WSAStartup(MAKEWORD(2, 2), &m_wsa) != 0)
	{
		cout << "WSAStartup failed" << endl;
		
		return;
	}
	spdlog::info("[NetworkManagerServer::NetworkManagerServer] WSAStartup.");

	InitRudpSendOverlappedPool();
}
NetworkManagerServer::~NetworkManagerServer() {
	WSACleanup();
	spdlog::info("[NetworkManagerServer::~NetworkManagerServer] WSACleanup.");
}

void NetworkManagerServer::InitRudpSendOverlappedPool()
{
	constexpr int size = sizeof(_sendOverlappedDtoPool) / sizeof(OverlappedDto);
	_lpOverlappedToSendOverlappedDtoPoolIdxMap.reserve(size << 2);
	for (int idx = 0; idx < size; ++idx)
	{
		ULONG_PTR key = reinterpret_cast<ULONG_PTR>(&_sendOverlappedDtoPool[idx]._overlapped);
		_lpOverlappedToSendOverlappedDtoPoolIdxMap.emplace(key, idx);
		_sendOverlappedQueue.push(idx);
	}
}

OverlappedDto& NetworkManagerServer::GetNextSendOverlapped()
{
	while (_sendOverlappedQueue.empty())
	{
		spdlog::warn("[NetworkManagerServer::GetNextSendOverlapped] _sendOverlappedQueue is empty. Wait Network send ok sign.");

		// 추후 condition_variable 통한 접근으로 변경 필요
		std::this_thread::sleep_for(100ms);
	}
	int idx = _sendOverlappedQueue.front(); _sendOverlappedQueue.pop();

	spdlog::debug("[NetworkManagerServer::GetNextSendOverlapped] nextSendOverlappedDto: _sendOverlappedDtoPool[{}]", idx);

	return _sendOverlappedDtoPool[idx];
}

void NetworkManagerServer::NotifySendOverlappedCompletionEvent(LPOVERLAPPED lpOverlapped)
{
	ULONG_PTR lpOverlappedKey = reinterpret_cast<ULONG_PTR>(lpOverlapped);
	int idx = _lpOverlappedToSendOverlappedDtoPoolIdxMap[lpOverlappedKey];
	_sendOverlappedQueue.push(idx);

	spdlog::debug("[NetworkManagerServer::NotifySendOverlappedCompletionEvent] _sendOverlappedDtoPool[{}] is complete.", idx);
}

void NetworkManagerServer::CreateListenSocket()
{
	// Overlapped IO 위한 listen socket 생성
	m_listenSocket.m_socket = Socket::CreateWSASocketHandle(SocketProtocolType::SPT_TCP);
	m_listenSocket.SetProtocolType(SocketProtocolType::SPT_TCP);

	spdlog::info("[NetworkManagerServer::CreateListenSocket] Socket creation complete.");

	if (m_listenSocket.Bind("0.0.0.0", 50000) == SOCKET_ERROR) {
		spdlog::error("[NetworkManagerServer::CreateListenSocket] bind error: {}", WSAGetLastError());
		return;
	}

	spdlog::info("[NetworkManagerServer::CreateListenSocket] bind complete.");

	if (listen(m_listenSocket.m_socket, 10) == SOCKET_ERROR) {
		spdlog::error("[NetworkManagerServer::CreateListenSocket] listen error: {}", WSAGetLastError());
		return;
	}

	spdlog::info("[NetworkManagerServer::CreateListenSocket] Listen Start.");
}

void NetworkManagerServer::CreateRUDPSocket()
{
	m_rudpSocket.m_socket = Socket::CreateWSASocketHandle(SocketProtocolType::SPT_RUDP);
	m_rudpSocket.SetProtocolType(SocketProtocolType::SPT_RUDP);

	spdlog::info("[NetworkManagerServer::CreateRUDPSocket] RUDP Socket Creation Complete.");
	
	if (m_rudpSocket.Bind("0.0.0.0", 50000) == SOCKET_ERROR) {
		spdlog::error("[NetworkManagerServer::CreateRUDPSocket] bind error: {}", WSAGetLastError());
		return;
	}
}

void NetworkManagerServer::GetLPFN()
{
	GUID guidAcceptEx = WSAID_ACCEPTEX;
	GUID guidGetAcceptExSockAddrs = WSAID_GETACCEPTEXSOCKADDRS;
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

		spdlog::error("[NetworkManagerServer::GetLPFN] WSAIoctl error: {}", WSAGetLastError());
	}

	if (m_AcceptEx == nullptr) {
		spdlog::error("[NetworkManagerServer::GetLPFN] Getting AcceptEx ptr failed.");

		return;
	}

	spdlog::info("[NetworkManagerServer::GetLPFN] Init AcceptEx function complete.");

	if (WSAIoctl(
		m_listenSocket.m_socket,
		SIO_GET_EXTENSION_FUNCTION_POINTER,
		&guidGetAcceptExSockAddrs,
		sizeof(guidGetAcceptExSockAddrs),
		&m_GetAcceptExSockAddrs,
		sizeof(m_GetAcceptExSockAddrs),
		&dwBytes,
		nullptr,
		nullptr) == SOCKET_ERROR) {

		spdlog::error("[NetworkManagerServer::GetLPFN] WSAIoctl error: {}", WSAGetLastError());
	}

	spdlog::info("[NetworkManagerServer::GetLPFN] Init GetAcceptExSockAddrs function complete.");
}

GameObject* NetworkManagerServer::GetGameObject(const NetworkId_t networkId) const
{
	return _linkingContext.GetGameObject(networkId);
}

NetworkId_t NetworkManagerServer::RegisterGameObject(shared_ptr<GameObject> gameObject)
{
	return _linkingContext.RegisterGameObject(gameObject);
}

void NetworkManagerServer::UnregisterGameObject(const NetworkId_t networkId)
{
	_linkingContext.UnregisterGameObject(networkId);
}

void NetworkManagerServer::ClearAllGameObjects()
{
	_linkingContext.Clear();
}

ClientInfo* NetworkManagerServer::CreateClientInfo(std::string_view ip, const uint16_t port)
{
	ClientInfo* ret = nullptr;

	SockAddress key{ ip.data(), port };

	_sockAddressToClientInfoMapMutex.lock();
	if (_sockAddressToClientInfoMap.find(key) != _sockAddressToClientInfoMap.end())
	{
		ret = _sockAddressToClientInfoMap[key].get();
		_sockAddressToClientInfoMapMutex.unlock();
		
		spdlog::info("[NetworkManagerServer::CreateClientInfo] {}:{} client already exists.", ip, port);
		
		return ret;
	}

	shared_ptr<ClientInfo> ci = make_shared<ClientInfo>(key);

	_sockAddressToClientInfoMap.emplace(key, ci);
	_sockAddressToClientInfoMapMutex.unlock();

	return ci.get();
}

bool NetworkManagerServer::RemoveClientInfo(ClientInfo* clientInfo)
{
	if (clientInfo == nullptr)
		return false;
	
	const auto& key = clientInfo->GetSockAddress();

	_sockAddressToClientInfoMapMutex.lock();
	if (_sockAddressToClientInfoMap.find(key) == _sockAddressToClientInfoMap.end())
	{
		_sockAddressToClientInfoMapMutex.unlock();

		spdlog::warn("[NetworkManagerServer::RemoveClientInfo] clientInfo is dangling pointer.");

		return false;
	}

	_sockAddressToClientInfoMap.erase(key);
	_sockAddressToClientInfoMapMutex.unlock();

	return true;
}

ClientInfo* NetworkManagerServer::GetClientInfo(std::string_view ip, const uint16_t port)
{
	const SockAddress key{ ip.data(), port };

	_sockAddressToClientInfoMapMutex.lock();
	if (_sockAddressToClientInfoMap.find(key) == _sockAddressToClientInfoMap.end())
	{
		_sockAddressToClientInfoMapMutex.unlock();
		spdlog::warn("[NetworkManagerServer::GetClientInfo] {}:{} client does not exist.", ip, port);

		return nullptr;
	}

	ClientInfo* ret = _sockAddressToClientInfoMap[key].get();

	_sockAddressToClientInfoMapMutex.unlock();

	return ret;
}
