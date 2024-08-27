#pragma once

#include "Containers/String.h"

#ifdef SetPort
#undef SetPort
#endif

namespace un {
    using EntityId = unsigned short;
    EntityId constexpr INVALID_ENTITY_ID = -1;
};

class CPlayerSocket {
protected:
    un::EntityId m_nPlayerId;
    unsigned long m_nAddress;
    unsigned short m_nPort;
    CString m_sSerial;

public:
    CPlayerSocket(
        un::EntityId const nInPlayerId,
        unsigned long const nInAddress,
        unsigned short const nInPort,
        CString const &sInSerial
    ) : m_nPlayerId(nInPlayerId),
        m_nAddress(nInAddress),
        m_nPort(nInPort),
        m_sSerial(sInSerial) {
    }

    CPlayerSocket() : m_nPlayerId(un::INVALID_ENTITY_ID),
                      m_nAddress(-1),
                      m_nPort(-1),
                      m_sSerial("0000000000000000") {
    }

    void SetPlayerId(un::EntityId nId) { m_nPlayerId = nId; }
    void SetBinaryAddress(unsigned long nAddress) { m_nAddress = nAddress; }
    void SetPort(unsigned short nPort) { m_nPort = nPort; }
    void SetSerial(CString const &sSerial) { m_sSerial.Assign(sSerial); }

    [[nodiscard]] un::EntityId GetPlayerId() const { return m_nPlayerId; }
    [[nodiscard]] unsigned long GetBinaryAddress() const { return m_nAddress; }
    [[nodiscard]] unsigned short GetPort() const { return m_nPort; }
    [[nodiscard]] CString const &GetSerial() const { return m_sSerial; }

    [[nodiscard]] CString GetAddress() const;

    void Clear();
};
