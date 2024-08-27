#pragma once

#include <string>

#include "ExampleProject/Network/RemoteCharactersStore.h"

class CNetClientInterface;
class CClientPacketHandler;
class CClientRPCHandler;
class CPacket;
class CString;

class CClientApp {
protected:
    CNetClientInterface* m_pNetClient;
    CClientPacketHandler* m_pPacketHandler;
    CClientRPCHandler* m_pRPCHandler;
    TRemoteCharactersStore* m_pCharactersStore;

public:
    CClientApp() : m_pNetClient(nullptr),
                   m_pPacketHandler(nullptr),
                   m_pRPCHandler(nullptr),
                   m_pCharactersStore(nullptr)
    {}

    ~CClientApp();

    void Startup(CString const& sHost, int nPort, CString const& sPassword);
    void Shutdown();
    void Tick() const;

    [[nodiscard]] CNetClientInterface* GetNetClient() const;
    [[nodiscard]] CClientPacketHandler* GetPacketHandler() const;
    [[nodiscard]] CClientRPCHandler* GetRPCHandler() const;
    [[nodiscard]] TRemoteCharactersStore* GetCharactersStore() const;

    static CClientApp* GetInstance();

protected:
    static void PacketHandlerWrapper(CPacket* pPacket);
};

inline CNetClientInterface* CClientApp::GetNetClient() const {
    return m_pNetClient;
}

inline CClientPacketHandler* CClientApp::GetPacketHandler() const {
    return m_pPacketHandler;
}

inline CClientRPCHandler* CClientApp::GetRPCHandler() const {
    return m_pRPCHandler;
}

inline TRemoteCharactersStore* CClientApp::GetCharactersStore() const {
    return m_pCharactersStore;
}

inline CClientApp* CClientApp::GetInstance() {
    static CClientApp Instance{};
    return &Instance;
}
