/**
 * DME_ColdMissionServer
 * 
 * Extends MissionServer to handle config synchronization to clients.
 * Sends config to clients when they connect.
 */
modded class MissionServer
{
    /**
     * Called when the mission starts on SERVER
     */
    override void OnMissionStart()
    {
        super.OnMissionStart();
        
        Print("[DME_Cold] ======== SERVER MISSION START ========");
        
        // Load config on server
        DME_ColdConfig config = DME_ColdConfig.GetInstance();
        if (config)
        {
            Print("[DME_Cold] Server: Config loaded successfully");
            Print("[DME_Cold] Server: EnableFrostEffect = " + config.EnableFrostEffect.ToString());
            Print("[DME_Cold] Server: DebugForceFrostLevel = " + config.DebugForceFrostLevel.ToString());
        }
        else
        {
            Print("[DME_Cold] Server: ERROR - Config is null!");
        }
        
        Print("[DME_Cold] ======== SERVER READY ========");
    }
    
    /**
     * Called when a player connects - send them the config
     */
    override void InvokeOnConnect(PlayerBase player, PlayerIdentity identity)
    {
        super.InvokeOnConnect(player, identity);
        
        if (!player || !identity)
            return;
        
        // Send config to this player
        SendConfigToClient(player, identity);
    }
    
    /**
     * Send the config to a specific client
     */
    void SendConfigToClient(PlayerBase player, PlayerIdentity identity)
    {
        if (!player || !identity)
            return;
        
        DME_ColdConfig config = DME_ColdConfig.GetInstance();
        if (!config)
        {
            Print("[DME_Cold] Server: Cannot send config - config is null!");
            return;
        }
        
        Print("[DME_Cold] Server: Sending config to player: " + identity.GetName());
        
        // Create RPC and send config to the player object
        ScriptRPC rpc = new ScriptRPC();
        config.WriteToParams(rpc);
        rpc.Send(player, DME_COLD_RPC_SYNC_CONFIG, true, identity);
        
        Print("[DME_Cold] Server: Config sent to " + identity.GetName());
    }
    
    /**
     * Send config to all connected players (useful for config reload)
     */
    void SendConfigToAllClients()
    {
        DME_ColdConfig config = DME_ColdConfig.GetInstance();
        if (!config)
            return;
        
        Print("[DME_Cold] Server: Broadcasting config to all clients...");
        
        // Iterate all players and send config to each
        array<Man> players = new array<Man>();
        GetGame().GetPlayers(players);
        
        foreach (Man man : players)
        {
            PlayerBase player = PlayerBase.Cast(man);
            if (player)
            {
                PlayerIdentity identity = player.GetIdentity();
                if (identity)
                {
                    ScriptRPC rpc = new ScriptRPC();
                    config.WriteToParams(rpc);
                    rpc.Send(player, DME_COLD_RPC_SYNC_CONFIG, true, identity);
                }
            }
        }
        
        Print("[DME_Cold] Server: Config broadcast complete - sent to " + players.Count().ToString() + " players");
    }
}
