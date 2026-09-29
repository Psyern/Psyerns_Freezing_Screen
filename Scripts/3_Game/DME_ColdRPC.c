/**
 * DME_ColdRPC
 * 
 * RPC constants and helper for DME_Cold mod.
 * Used for Server -> Client config synchronization.
 */

// RPC IDs for DME_Cold
// Using high numbers to avoid conflicts with other mods
const int DME_COLD_RPC_SYNC_CONFIG = 0x444D4530; // "DME0" in hex

// RPC ID for admin reload request (client -> server)
const int DME_COLD_RPC_RELOAD_REQUEST = 0x444D4531; // "DME1" in hex

// RPC ID for admin reload response (server -> client)
const int DME_COLD_RPC_RELOAD_RESPONSE = 0x444D4532; // "DME2" in hex

class DME_ColdRPC
{
    /**
     * Register RPC handlers - call this on both client and server
     */
    static void RegisterRPCs()
    {
        // RPC handlers are registered automatically via GetRPCMin/GetRPCMax
        Print("[DME_Cold] RPC system initialized");
    }
}
