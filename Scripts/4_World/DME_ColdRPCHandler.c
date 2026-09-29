/**
 * DME_ColdRPCHandler
 * 
 * Handles RPC messages for DME_Cold mod.
 * Uses modded PlayerBase to receive config sync from server.
 * 
 * Note: Server-side reload request handling is done in MissionServer (5_Mission)
 * because MissionServer type is not available in 4_World module.
 */
modded class PlayerBase
{
    /**
     * Handle RPC messages
     */
    override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
    {
        super.OnRPC(sender, rpc_type, ctx);
        
        // SERVER: Handle reload request from admin client
        // Note: We handle this by calling a static method that the MissionServer registers
        if (g_Game && g_Game.IsServer())
        {
            if (rpc_type == DME_COLD_RPC_RELOAD_REQUEST)
            {
                Print("[DME_Cold] Server: Received reload request RPC from player");
                // The actual handling is done via OnRPC in MissionServer
                // We just pass it through - MissionServer will catch this via g_Game.GetMission()
                DME_ColdReloadHandler.HandleRequest(this, sender);
            }
            return;
        }
        
        // CLIENT: Handle config sync RPC
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
        
        // CLIENT: Handle reload response from server
        if (rpc_type == DME_COLD_RPC_RELOAD_RESPONSE)
        {
            Print("[DME_Cold] Client: Received reload response RPC");
            
            Param2<bool, string> params;
            if (ctx.Read(params))
            {
                bool success = params.param1;
                string message = params.param2;
                
                if (success)
                {
                    Print("[DME_Cold] Client: Admin reload SUCCESS - " + message);
                }
                else
                {
                    Print("[DME_Cold] Client: Admin reload FAILED - " + message);
                }
            }
        }
    }
}
