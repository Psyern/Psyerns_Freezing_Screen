/**
 * DME_ColdMissionClient
 * 
 * Extends MissionGameplay (CLIENT ONLY) to manage the frost overlay lifecycle.
 * MissionGameplay only runs on clients, not on the server.
 * 
 * Config is received from server via RPC (handled in DME_ColdRPCHandler.c)
 */
modded class MissionGameplay
{
    protected ref DME_ColdFrostOverlay m_DME_ColdMissionOverlay;
    protected bool m_DME_ColdMissionOverlayInitialized = false;
    
    /**
     * Called when the mission starts
     */
    override void OnMissionStart()
    {
        super.OnMissionStart();
        
        Print("[DME_Cold] ======== CLIENT MISSION START ========");
        Print("[DME_Cold] Client: Waiting for config from server...");
        Print("[DME_Cold] ======== MISSION START COMPLETE ========");
    }
    
    protected void InitMissionOverlay()
    {
        if (m_DME_ColdMissionOverlayInitialized)
            return;
        
        // Wait for config from server before initializing
        if (!DME_ColdConfig.IsConfigSynced())
        {
            return; // Silent wait for config
        }
        
        PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
        if (!player)
        {
            return; // Silent wait - player will be available soon
        }
        
        Print("[DME_Cold] ======== MISSION OVERLAY INIT ========");
        
        // Check if frost effect is enabled
        DME_ColdConfig config = DME_ColdConfig.GetInstance();
        if (config && !config.EnableFrostEffect)
        {
            Print("[DME_Cold] Frost effect is disabled in config - skipping overlay");
            m_DME_ColdMissionOverlayInitialized = true; // Mark as "initialized" to stop trying
            return;
        }
        
        // Try to get player name safely
        string playerName = "Unknown";
        PlayerIdentity identity = player.GetIdentity();
        if (identity)
        {
            playerName = identity.GetName();
        }
        Print("[DME_Cold] Mission: Local player found: " + playerName);
        
        // Create overlay instance
        m_DME_ColdMissionOverlay = new DME_ColdFrostOverlay();
        if (!m_DME_ColdMissionOverlay)
        {
            Print("[DME_Cold] Mission: ERROR creating overlay instance!");
            return;
        }
        
        // Create the actual overlay widgets
        bool success = m_DME_ColdMissionOverlay.CreateOverlay();
        if (!success)
        {
            Print("[DME_Cold] Mission: ERROR creating overlay widgets!");
            m_DME_ColdMissionOverlay = null;
            return;
        }
        
        m_DME_ColdMissionOverlayInitialized = true;
        Print("[DME_Cold] ======== MISSION OVERLAY INITIALIZED ========");
    }
    
    /**
     * Called every frame on CLIENT
     */
    override void OnUpdate(float timeslice)
    {
        super.OnUpdate(timeslice);
        
        // Try to initialize overlay if not done
        if (!m_DME_ColdMissionOverlayInitialized)
        {
            InitMissionOverlay();
            return;
        }
        
        // Update overlay if initialized
        if (m_DME_ColdMissionOverlay)
        {
            PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
            if (player)
            {
                int heatStage = player.GetHeatBufferStage();
                float heatComfort = 0.0;
                float heatBufferRaw = -1.0; // For debug display
                
                // Try to get continuous heat buffer value
                PlayerStat<float> heatBufferStat = player.GetStatHeatBuffer();
                if (heatBufferStat)
                {
                    heatBufferRaw = heatBufferStat.Get();
                    // HeatBuffer ranges from 0 to ~30 (max observed)
                    // Map 0-21 to Frost Levels 7-1, above 21 = no frost
                    // 0 => -1.0 (Frost 7), 21 => 0.0 (Frost 0), 30 => ~0.43 (no frost)
                    heatComfort = (heatBufferRaw / 21.0) - 1.0;
                    heatComfort = Math.Clamp(heatComfort, -1.0, 1.0);
                }
                else
                {
                    // Fallback to discrete stages if stat not available
                    switch (heatStage)
                    {
                        case 0:
                            heatComfort = -1.0;
                            break;
                        case 1:
                            heatComfort = -0.5;
                            break;
                        case 2:
                            heatComfort = 0.0;
                            break;
                        case 3:
                            heatComfort = 0.5;
                            break;
                        default:
                            heatComfort = 1.0;
                            break;
                    }
                }
                
                // Update overlay with heat stage and raw buffer for debug display
                m_DME_ColdMissionOverlay.Update(timeslice, heatComfort, heatStage, heatBufferRaw);
            }
        }
    }
}