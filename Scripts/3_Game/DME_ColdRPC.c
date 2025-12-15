/**
 * DME_ColdRPC
 * 
 * RPC constants and helper for DME_Cold mod.
 * Used for Server -> Client config synchronization.
 */

// RPC IDs for DME_Cold
// Using high numbers to avoid conflicts with other mods
const int DME_COLD_RPC_SYNC_CONFIG = 0x444D4530; // "DME0" in hex

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
