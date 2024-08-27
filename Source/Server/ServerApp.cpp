#include "ServerApp.h"

#include <ctime>
#include <Windows.h>

#include "NetServerInterface.h"
#include "ServerRPCHandler.h"
#include "ServerPacketHandler.h"
#include "CharacterFlow.h"
#include "Packet.h"

CServerApp::~CServerApp() {
    Shutdown();
}

void CServerApp::Startup(unsigned short nPort, int nMaxPlayers, CString const &sHostAddress) {
    if (m_pNetServer) {
        return;
    }

    m_pNetServer = GetNetServerInterface();
    m_pPacketHandler = new CServerPacketHandler{};
    m_pRPCHandler = new CServerRPCHandler{};
    m_pCharacterFlow = new CCharacterFlow{};

    m_pNetServer->Startup(nPort, nMaxPlayers, sHostAddress);
    m_pNetServer->SetPacketHandler(PacketHandlerWrapper);
    printf("[Server:Startup] Network socket started up\n");

    m_pPacketHandler->Register();
    printf("[Server:Startup] Packets handler registered\n");

    m_pRPCHandler->Register();
    printf("[Server:Startup] RPC handler registered\n");
}

void CServerApp::Shutdown() {
    if (!m_pNetServer) {
        return;
    }

    m_pNetServer->Shutdown(0);

    SAFE_DELETE(m_pCharacterFlow);
    SAFE_DELETE(m_pRPCHandler);
    SAFE_DELETE(m_pPacketHandler);
    DestroyNetServerInterface(m_pNetServer);
}

[[noreturn]] void CServerApp::Run(un::Size nSleepTime) const {
    while (true) {
        m_pNetServer->Process();
        m_pCharacterFlow->ProcessCharactersInGrid();
        Sleep(nSleepTime);
    }
}

void CServerApp::PacketHandlerWrapper(CPacket const *pPacket) {
    if (!GetInstance()->GetPacketHandler()->HandlePacket(pPacket) &&
        !GetInstance()->GetRPCHandler()->HandlePacket(pPacket)) {
        auto const &nPacket = pPacket->nPacket;
        auto const &nPlayer = pPacket->pPlayerSocket->GetPlayerId();
        printf("[Server:Warning] Unhandled packet (packet: %i, player: %i)\n", nPacket, nPlayer);
    }
}
