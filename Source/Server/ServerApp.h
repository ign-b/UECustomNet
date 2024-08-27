#pragma once

#include "PlayerSocket.h"

class CNetServerInterface;
class CServerPacketHandler;
class CServerRPCHandler;
class CCharacterFlow;
class CPacket;

class CServerApp {
protected:
    CNetServerInterface *m_pNetServer;
    CServerPacketHandler *m_pPacketHandler;
    CServerRPCHandler *m_pRPCHandler;
    CCharacterFlow *m_pCharacterFlow;

public:
    CServerApp() : m_pNetServer(nullptr),
                   m_pPacketHandler(nullptr),
                   m_pRPCHandler(nullptr),
                   m_pCharacterFlow(nullptr) {}

    ~CServerApp();

    void Startup(unsigned short nPort, int nMaxPlayers, CString const &sHostAddress);

    void Shutdown();

    void Run(un::Size nSleepTime) const;

    [[nodiscard]] CNetServerInterface *GetNetServer() const;

    [[nodiscard]] CServerPacketHandler *GetPacketHandler() const;

    [[nodiscard]] CServerRPCHandler *GetRPCHandler() const;

    [[nodiscard]] CCharacterFlow *GetCharacterFlow() const;

    static CServerApp *GetInstance();

protected:
    static void PacketHandlerWrapper(const CPacket *pPacket);
};

inline CNetServerInterface *CServerApp::GetNetServer() const {
    return m_pNetServer;
}

inline CServerPacketHandler *CServerApp::GetPacketHandler() const {
    return m_pPacketHandler;
}

inline CServerRPCHandler *CServerApp::GetRPCHandler() const {
    return m_pRPCHandler;
}

inline CCharacterFlow *CServerApp::GetCharacterFlow() const {
    return m_pCharacterFlow;
}

inline CServerApp *CServerApp::GetInstance() {
    static CServerApp Instance{};
    return &Instance;
}
