#include "ClientApp.h"

#include "ExampleProject/Network/ClientPacketHandler.h"
#include "ExampleProject/Network/ClientRPCHandler.h"
#include "ExampleProject/Network/LocalCharacter.h"

#include "UECustomNet/NetClientInterface.h"
#include "UECustomNet/Packet.h"
#include "UECustomNet/PlayerSocket.h"
#include "UECustomNet/Containers/String.h"

CClientApp::~CClientApp() {
    Shutdown();
}

void CClientApp::Startup(CString const& sHost, int nPort, CString const& sPassword) {
    if(m_pNetClient) {
        return;
    }
    
    m_pNetClient = GetNetClientInterface();
    m_pPacketHandler = new CClientPacketHandler{};
    m_pRPCHandler = new CClientRPCHandler{};
    m_pCharactersStore = new TRemoteCharactersStore{};

    m_pNetClient->Startup();
    m_pNetClient->SetHost(*sHost);
    m_pNetClient->SetPort(nPort);
    if(!sPassword.IsEmpty()) {
        m_pNetClient->SetPassword(sPassword);
    }
    m_pNetClient->SetPacketHandler(PacketHandlerWrapper);
    UE_LOG(LogTemp, Warning, TEXT("[Client:Startup] Network socket started up"));

    m_pPacketHandler->Register();
    UE_LOG(LogTemp, Warning, TEXT("[Client:Startup] Packet handler registered"));

    m_pRPCHandler->Register();
    UE_LOG(LogTemp, Warning, TEXT("[Client:Startup] RPC handler registered"));

    m_pNetClient->Connect();
    UE_LOG(LogTemp, Warning, TEXT("[Client:Startup] Client connecting to %hs:%d..."), *sHost, nPort);
}

void CClientApp::Shutdown() {
    if(!m_pNetClient) {
        return;
    }
    
    m_pNetClient->Shutdown(0);

    SAFE_DELETE(m_pCharactersStore);
    SAFE_DELETE(m_pRPCHandler);
    SAFE_DELETE(m_pRPCHandler);
    DestroyNetClientInterface(m_pNetClient);
}

void CClientApp::Tick() const {
    m_pNetClient->Process();
    CLocalCharacter::Process();
}

void CClientApp::PacketHandlerWrapper(CPacket* pPacket) {
    if (!GetInstance()->GetPacketHandler()->HandlePacket(pPacket) &&
        !GetInstance()->GetRPCHandler()->HandlePacket(pPacket)) {
		auto const& nPacket = pPacket->nPacket;
		auto const& nPlayer = pPacket->pPlayerSocket->GetPlayerId();
        UE_LOG(LogTemp, Warning, TEXT("[Server:Warning] Unhandled packet (packet: %i, player: %i)"), nPacket, nPlayer);
    }
}
