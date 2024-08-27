#include "Common.h"
#include "NetClient.h"
#include "NetClientInterface.h"
#include "NetServer.h"
#include "NetServerInterface.h"

UNET_API CNetServerInterface *GetNetServerInterface() {
    CNetServerInterface *pNetServer = new CNetServer();
    return pNetServer;
}

UNET_API void DestroyNetServerInterface(CNetServerInterface const *pNetServer) {
    SAFE_DELETE(pNetServer);
}

UNET_API CNetClientInterface *GetNetClientInterface() {
    CNetClientInterface *pNetClient = new CNetClient();
    return pNetClient;
}

UNET_API void DestroyNetClientInterface(CNetClientInterface const *pNetClient) {
    SAFE_DELETE(pNetClient);
}

BOOL WINAPI DllMain(HMODULE hModule, DWORD dwReason, void *) {
    if (dwReason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hModule);
    }

    return TRUE;
}
