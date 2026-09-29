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
        
        // Register reload handler callback
        DME_ColdReloadHandler.GetOnReloadRequest().Insert(OnReloadRequestCallback);
        Print("[DME_Cold] Server: Reload handler registered");
        
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
     * Called when the mission ends on SERVER
     */
    override void OnMissionFinish()
    {
        DME_ColdReloadHandler.GetOnReloadRequest().Remove(OnReloadRequestCallback);
        
        super.OnMissionFinish();
    }
    
    /**
     * Callback when a reload request is received from a client
     */
    void OnReloadRequestCallback(PlayerBase player, PlayerIdentity identity)
    {
        HandleReloadRequest(player, identity);
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
        g_Game.GetPlayers(players);
        
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
    
    /**
     * Handle admin reload request from client
     */
    void HandleReloadRequest(PlayerBase player, PlayerIdentity identity)
    {
        if (!player || !identity)
            return;
        
        string playerName = identity.GetName();
        string playerGUID = identity.GetId();
        
        Print("[DME_Cold] Server: Reload request from " + playerName + " (GUID: " + playerGUID + ", Steam64: " + identity.GetPlainId() + ")");
        
        // Check if player is admin
        DME_ColdAdminConfig adminConfig = DME_ColdAdminConfig.Load();
        if (!adminConfig || !adminConfig.IsPlayerAdmin(identity))
        {
            Print("[DME_Cold] Server: Player " + playerName + " is NOT an admin - request denied!");
            SendReloadResponse(player, identity, false, "You are not an admin");
            return;
        }
        
        Print("[DME_Cold] Server: Player " + playerName + " IS an admin - reloading config...");
        
        // Force reload config from file
        DME_ColdConfig.ReloadConfigFromFile();
        
        // Broadcast new config to all clients
        SendConfigToAllClients();
        
        // Send success response
        SendReloadResponse(player, identity, true, "Config reloaded and synced to all players");
        
        Print("[DME_Cold] Server: Config reload complete!");
    }
    
    /**
     * Send reload response to client
     */
    void SendReloadResponse(PlayerBase player, PlayerIdentity identity, bool success, string message)
    {
        if (!player || !identity)
            return;
        
        Param2<bool, string> params = new Param2<bool, string>(success, message);
        g_Game.RPCSingleParam(player, DME_COLD_RPC_RELOAD_RESPONSE, params, true, identity);
    }
}