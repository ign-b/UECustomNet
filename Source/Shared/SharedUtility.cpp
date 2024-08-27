#include "SharedUtility.h"

#ifdef WIN32
#include <Windows.h>
#endif

#include "Containers/String.h"

namespace SharedUtility {
    bool StripPath(char *szString) {
        for (auto i = StringLen(szString); i > 0; --i) {
            if (szString[i] == '\\' || szString[i] == '/') {
                szString[i + 1] = '\0';
                return true;
            }
        }

        return false;
    }

    const char *GetAppPath() {
#ifdef WIN32
        static unsigned int nDummy;
#endif

        static char szAppPath[MAX_PATH];

#ifdef WIN32
        HMODULE hModuleHandle;
        GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                          reinterpret_cast<LPCSTR>(&nDummy), &hModuleHandle);
        GetModuleFileName(hModuleHandle, szAppPath, MAX_PATH);
#else
		readlink("/proc/self/exe", szAppPath, MAX_PATH);
#endif

        StripPath(szAppPath);
        return szAppPath;
    }
};
