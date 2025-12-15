/**
 * DME_ColdRPCHandler
 * 
 * Handles RPC messages for DME_Cold mod.
 * Uses modded PlayerBase to receive config sync from server.
 */
modded class PlayerBase
{
    /**
     * Handle RPC messages
     */
    override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
    {
        super.OnRPC(sender, rpc_type, ctx);
        
        // Only process on client
        if (GetGame().IsServer())
            return;
        
        // Handle config sync RPC
        if (rpc_type == DME_COLD_RPC_SYNC_CONFIG)
        {
            Print("[DME_Cold] Client: Received config sync RPC");
            
            // Create new config and read from RPC
            DME_ColdConfig config = new DME_ColdConfig();
            if (config.ReadFromParams(ctx))
            {
                // Apply the synced config
                DME_ColdConfig.ApplySyncedConfig(config);
                Print("[DME_Cold] Client: Config applied successfully!");
            }
            else
            {
                Print("[DME_Cold] Client: ERROR - Failed to read config from RPC!");
            }
        }
    }
}
