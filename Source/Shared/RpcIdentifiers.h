#pragma once

namespace un {
    using RpcIdentifier = unsigned char;
};

enum eRPCIdentfiers {
    RPC_INVALID = 0,

    // NOTE: server -> client
    RPC_CHARACTER_WORLD_JOIN,
    RPC_CHARACTER_WORLD_LEAVE,
    RPC_CHARACTER_WORLD_ADD,
    RPC_CHARACTER_WORLD_REMOVE,

    // NOTE: client <-> server
    RPC_CHARACTER_REQUEST_SPAWN
};
