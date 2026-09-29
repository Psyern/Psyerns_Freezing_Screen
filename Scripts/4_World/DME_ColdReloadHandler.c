/**
 * DME_ColdReloadHandler
 * 
 * Static handler for admin reload requests.
 * This class bridges 4_World (where RPC is received) to 5_Mission (where config reload happens).
 */
class DME_ColdReloadHandler
{
    // Static reference to callback - set by MissionServer
    protected static ref ScriptInvoker s_OnReloadRequest = new ScriptInvoker();
    
    /**
     * Get the invoker for reload requests
     */
    static ScriptInvoker GetOnReloadRequest()
    {
        return s_OnReloadRequest;
    }
    
    /**
     * Handle a reload request from a player
     * Called from PlayerBase.OnRPC in 4_World
     */
    static void HandleRequest(PlayerBase player, PlayerIdentity sender)
    {
        if (!player || !sender)
        {
            Print("[DME_Cold] ReloadHandler: Invalid player or sender");
            return;
        }
        
        Print("[DME_Cold] ReloadHandler: Processing request from " + sender.GetName());
        
        // Invoke the callback - MissionServer will handle it
        s_OnReloadRequest.Invoke(player, sender);
    }
}
