#pragma once

#include "PacketPriorities.h"

namespace un {
    using TNetStat = unsigned long long int;
};

enum eNetStatMetrics {
    USER_MESSAGE_BYTES_PUSHED,
    USER_MESSAGE_BYTES_SENT,
    USER_MESSAGE_BYTES_RESENT,
    USER_MESSAGE_BYTES_RECEIVED_PROCESSED,
    USER_MESSAGE_BYTES_RECEIVED_IGNORED,
    ACTUAL_BYTES_SENT,
    ACTUAL_BYTES_RECEIVED,

    NET_STAT_METRICS_COUNT
};

struct SNetStats {
    un::TNetStat nValueOverLastSecond[NET_STAT_METRICS_COUNT];
    un::TNetStat nRunningTotal[NET_STAT_METRICS_COUNT];
    un::TNetStat nConnectionStartTime;
    bool bIsLimitedByCongestionControl;
    un::TNetStat nBPSLimitByCongestionControl;
    bool bIsLimitedByOutgoingBandwidthLimit;
    un::TNetStat nBPSLimitByOutgoingBandwidthLimit;
    unsigned int nMessageInSendBuffer[PRIORITY_COUNT];
    double dBytesInSendBuffer[PRIORITY_COUNT];
    unsigned int nMessagesInResendBuffer;
    un::TNetStat nBytesInResendBuffer;
    float fPacketlossLastSecond;
    float fPacketlossTotal;
};
