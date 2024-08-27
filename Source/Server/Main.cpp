#include "Main.h"

CServerApp *g_pServerApp = nullptr;

int main(int argC, char *argV[]) {
    g_pServerApp = CServerApp::GetInstance();
    g_pServerApp->Startup(17778, 8, {});
    g_pServerApp->Run(10);
    return 0;
}
