#pragma once

#include <functional>

#include "Common.h"
#include "Containers/List.h"

class CBitStream;
class CPlayerSocket;
class CPacket;

namespace un {
    using PacketIdentifier = unsigned char;
    using TPacketCallback = std::function<void(CBitStream *, CPlayerSocket const*)>;
};

struct SPacketData {
    un::PacketIdentifier nPacket;
    un::TPacketCallback fnCallback;
};

class UNET_API CPacketHandler {
protected:
    CList<SPacketData *> m_aPacketDatas;

public:
    ~CPacketHandler();

    void Add(un::PacketIdentifier nPacket, un::TPacketCallback const &fnCallback);

    void Remove(un::PacketIdentifier nPacket);

    SPacketData *GetPacketData(un::PacketIdentifier nPacket);

    bool HandlePacket(const CPacket *pPacket);
};
