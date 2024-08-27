#include "PlayerSocket.h"

#ifdef WIN32
#include <WinSock2.h>
#endif

#ifdef SetPort
#undef SetPort
#endif

CString CPlayerSocket::GetAddress() const {
    in_addr in{};
    memset(&in, 0, sizeof(in));
    in.s_addr = m_nAddress;
    return CString{inet_ntoa(in)};
}

void CPlayerSocket::Clear() {
    SetPlayerId(un::INVALID_ENTITY_ID);
    SetBinaryAddress(-1);
    SetPort(-1);
    SetSerial("0000000000000000");
}
