#pragma once

#include "ExampleProject/Network/RemoteCharacter.h"

#include "UECustomNet/PlayerSocket.h"
#include "UECustomNet/Containers/DataStore.h"

using TRemoteCharactersStore = CDataStore<un::EntityId, CRemotePlayer>;
