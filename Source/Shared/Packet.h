#pragma once

class CPlayerSocket;

namespace un {
    using PacketId = unsigned char;
    PacketId constexpr INVALID_PACKET_ID = -1;
};

class CPacket {
public:
    CPlayerSocket *pPlayerSocket;
    un::PacketId nPacket;
    unsigned int nLength;
    unsigned char *pData;
};
