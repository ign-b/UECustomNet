#pragma once

enum eInternalPacketIdentfiers {
    INTERNAL_PACKET_CONNECTION_REQUEST = 134 /*ID_USER_PACKET_ENUM*/,
    INTERNAL_PACKET_CONNECTION_REQUEST_ACCEPTED,
    INTERNAL_PACKET_END
};

enum ePacketIdentfiers {
    PACKET_CONNECTION_REJECTED = INTERNAL_PACKET_END,
    PACKET_CONNECTION_SUCCEEDED,
    PACKET_CONNECTION_FAILED,
    PACKET_ALREADY_CONNECTED,
    PACKET_SERVER_FULL,
    PACKET_NEW_CONNECTION,
    PACKET_DISCONNECTED,
    PACKET_LOST_CONNECTION,
    PACKET_BANNED,
    PACKET_PASSWORD_INVALID,
    PACKET_RPC,

    PACKET_CHARACTER_ONFOOT_DATA
};
