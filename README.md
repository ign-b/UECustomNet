# Unreal Engine Custom Server

![Custom Network in Action](./ue_custom_net.gif)

This is a simple proof-of-concept project implementing a custom network for Unreal Engine. It presents a basic example of an authoritative server that handles incoming connections, player positions, and movement data (based on the `AnimInstance` of a `Character`). The server then broadcasts this data to all players within a 3x3 grid of specified size around the player. The client receives this data, creates/deletes specified `Character` instances, moves them around the map, and applies the corresponding animation parameters.

This project explores an alternative approach to server-side logic management in Unreal Engine, offering insights into optimizing performance for MMO games.

Basically it refers a discussion on the official Unreal Engine forum ([source](https://forums.unrealengine.com/t/atlas-mmo-framework/2301/9)), where Tim Sweeney, the head of Epic Games, shares insights from game studios that developed MMO games using Unreal Engine, despite the server not being able to offer the necessary level of performance. It is suggested to move all game server logic outside of Unreal Engine and use an approach where the game instance runs only on the client.

## What are the advantages of this approach?

1. **Reduced server performance costs**: The server no longer calculates player positions but simply relays data from one player to all nearby players.
2. **Reduced network costs for both client and server**: Only the minimal necessary data is transmitted, allowing it to be replayed on proxy players.
3. **Significantly increased player capacity**: Due to the reduced load on the server and client, the number of players that can join the server increases by an order of magnitude.

## What are the disadvantages of this approach?

1. **Cheating and unauthorized gameplay interference**: This approach requires additional server logic to handle such issues.
2. **Complete encapsulation of server and client game worlds**: Integration of deterministic physics libraries or similar systems may be necessary to ensure correct, predictable, and identical calculations on both the server and client sides.
