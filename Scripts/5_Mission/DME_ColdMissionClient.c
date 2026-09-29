/**
 * DME_ColdMissionClient
 * 
 * Extends MissionGameplay (CLIENT ONLY) to manage the frost overlay lifecycle.
 * MissionGameplay only runs on clients, not on the server.
 * 
 * Config is received from server via RPC (handled in DME_ColdRPCHandler.c)
 * Includes admin keybind for config reload (Numpad 8 by default, configurable in Options > Controls)
 */
modded class MissionGameplay
{
    protected ref DME_ColdFrostOverlay m_DME_ColdMissionOverlay;
    protected ref DME_ToxicOverlay m_DME_ToxicOverlay;
    protected ref DME_BloodOverlay m_DME_BloodOverlay;
    protected bool m_DME_ColdMissionOverlayInitialized = false;
    protected bool m_DME_ToxicOverlayInitialized = false;
    protected bool m_DME_BloodOverlayInitialized = false;
    
    // Input action handle for the reload keybind
    protected UAInput m_DME_ReloadInput;
    protected bool m_DME_InputInitialized = false;
    
    /**
     * Called when the mission initializes
     */
    override void OnInit()
    {
        super.OnInit();
        
        // Only on actual client, not server
        if (GetGame().IsClient() && !GetGame().IsServer())
        {
            // Get the input action defined in inputs.xml
            m_DME_ReloadInput = GetUApi().GetInputByName("UADMEColdReload");
            if (m_DME_ReloadInput)
            {
                m_DME_InputInitialized = true;
                Print("[DME_Cold] Client: Keybind loaded - Reload Config (configurable in Options > Controls > DME Cold)");
            }
            else
            {
                Print("[DME_Cold] Client: WARNING - Keybind 'UADMEColdReload' not found. Check inputs.xml");
            }
        }
    }
    
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
    
    protected void InitToxicOverlay()
    {
        if (m_DME_ToxicOverlayInitialized)
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
        
        Print("[DME_Cold] ======== TOXIC OVERLAY INIT ========");
        
        // Check if toxic effect is enabled
        DME_ColdConfig config = DME_ColdConfig.GetInstance();
        if (config && !config.EnableToxicEffect)
        {
            Print("[DME_Cold] Toxic effect is disabled in config - skipping overlay");
            m_DME_ToxicOverlayInitialized = true; // Mark as "initialized" to stop trying
            return;
        }
        
        // Create toxic overlay instance
        m_DME_ToxicOverlay = new DME_ToxicOverlay();
        if (!m_DME_ToxicOverlay)
        {
            Print("[DME_Cold] Mission: ERROR creating toxic overlay instance!");
            return;
        }
        
        // Create the actual overlay widgets
        bool success = m_DME_ToxicOverlay.CreateOverlay();
        if (!success)
        {
            Print("[DME_Cold] Mission: ERROR creating toxic overlay widgets!");
            m_DME_ToxicOverlay = null;
            return;
        }
        
        m_DME_ToxicOverlayInitialized = true;
        Print("[DME_Cold] ======== TOXIC OVERLAY INITIALIZED ========");
    }
    
    protected void InitBloodOverlay()
    {
        if (m_DME_BloodOverlayInitialized)
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
        
        Print("[DME_Cold] ======== BLOOD OVERLAY INIT ========");
        
        // Check if blood effect is enabled
        DME_ColdConfig config = DME_ColdConfig.GetInstance();
        if (config && !config.EnableBloodEffect)
        {
            Print("[DME_Cold] Blood effect is disabled in config - skipping overlay");
            m_DME_BloodOverlayInitialized = true; // Mark as "initialized" to stop trying
            return;
        }
        
        // Create blood overlay instance
        m_DME_BloodOverlay = new DME_BloodOverlay();
        if (!m_DME_BloodOverlay)
        {
            Print("[DME_Cold] Mission: ERROR creating blood overlay instance!");
            return;
        }
        
        // Create the actual overlay widgets
        bool success = m_DME_BloodOverlay.CreateOverlay();
        if (!success)
        {
            Print("[DME_Cold] Mission: ERROR creating blood overlay widgets!");
            m_DME_BloodOverlay = null;
            return;
        }
        
        m_DME_BloodOverlayInitialized = true;
        Print("[DME_Cold] ======== BLOOD OVERLAY INITIALIZED ========");
    }
    
    /**
     * Called every frame on CLIENT
     */
    override void OnUpdate(float timeslice)
    {
        super.OnUpdate(timeslice);
        
        // Check for admin reload keybind (only if input was successfully initialized)
        if (m_DME_InputInitialized && m_DME_ReloadInput)
        {
            if (m_DME_ReloadInput.LocalPress())
            {
                OnDME_ReloadKeyPressed();
            }
        }
        
        // Try to initialize overlay if not done
        if (!m_DME_ColdMissionOverlayInitialized)
        {
            InitMissionOverlay();
        }
        
        // Try to initialize toxic overlay if not done
        if (!m_DME_ToxicOverlayInitialized)
        {
            InitToxicOverlay();
        }
        
        // Try to initialize blood overlay if not done
        if (!m_DME_BloodOverlayInitialized)
        {
            InitBloodOverlay();
        }
        
        // Update overlays if initialized
        PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
        if (player)
        {
            int heatStage = player.GetHeatBufferStage();
            float heatComfort = 0.0;
            float heatBufferRaw = 0.0; // HeatBuffer: -30 to +30
            float heatComfortRaw = -999.0; // HeatComfort: probably similar range
            float toxicityRaw = 0.0; // Toxicity: 0-100
            float bloodRaw = 5000.0; // Blood: 0-5000 (5000 = full)
            float bloodValueDebug = 0.0; // Debug: original GetHealth value
            
            // Get HeatBuffer (-30 to +30, changes quickly near heat sources)
            PlayerStat<float> heatBufferStat = player.GetStatHeatBuffer();
            if (heatBufferStat)
            {
                heatBufferRaw = heatBufferStat.Get();
            }
            
            // Get HeatComfort (changes slowly, the "felt" temperature)
            // This is the value we should use for the frost effect!
            PlayerStat<float> heatComfortStat = player.GetStatHeatComfort();
            if (heatComfortStat)
            {
                heatComfortRaw = heatComfortStat.Get();
                
                // HeatComfort range appears to be negative (cold) to positive (warm)
                // We need to normalize this to our expected -1.0 to +1.0 range
                // Based on DayZ code: values are likely similar to HeatBuffer (-30 to +30)
                // Normalize: divide by 30 to get -1 to +1 range
                // Negative = cold (frost), 0 = neutral, Positive = warm (no frost)
                heatComfort = heatComfortRaw / 30.0;
                heatComfort = Math.Clamp(heatComfort, -1.0, 1.0);
            }
            else
            {
                // Fallback to HeatStage if HeatComfort stat not available
                // HeatStage: 0=freezing, 1=cold, 2=neutral, 3=warm, 4=hot
                switch (heatStage)
                {
                    case 0: heatComfort = -1.0; break;  // Freezing
                    case 1: heatComfort = -0.5; break;  // Cold
                    case 2: heatComfort = 0.0; break;   // Neutral
                    case 3: heatComfort = 0.5; break;   // Warm
                    default: heatComfort = 1.0; break;  // Hot
                }
            }
            
            // Get Toxicity stat (0-100, contamination level in toxic zones)
            PlayerStat<float> toxicityStat = player.GetStatToxicity();
            if (toxicityStat)
            {
                toxicityRaw = toxicityStat.Get();
            }
            
            // Get Blood stat (0-5000, 5000 = full blood)
            // DayZ uses GetHealth("", "Blood") or GetHealth("GlobalHealth", "Blood")
            // Try both methods to ensure we get a valid value
            float bloodValue = player.GetHealth("", "Blood");
            bloodValueDebug = bloodValue; // Store original for debug
            
            // Validate the blood value
            // DayZ blood range is 0-5000, default is 5000 (full)
            if (bloodValue > 0 && bloodValue <= 5000)
            {
                // Valid blood value in expected range
                bloodRaw = bloodValue;
            }
            else if (bloodValue > 5000)
            {
                // Value exceeds max, clamp it
                bloodRaw = 5000.0;
            }
            else
            {
                // Invalid, zero, or negative - assume full blood
                // This handles cases where player hasn't fully loaded yet
                bloodRaw = 5000.0;
            }
            
            // Update frost overlay with heat stage and raw values for debug display
            if (m_DME_ColdMissionOverlay)
            {
                m_DME_ColdMissionOverlay.Update(timeslice, heatComfort, heatStage, heatBufferRaw, heatComfortRaw);
            }
            
            // Update toxic overlay with toxicity value
            if (m_DME_ToxicOverlay)
            {
                m_DME_ToxicOverlay.Update(timeslice, toxicityRaw);
            }
            
            // Update blood overlay with blood value
            if (m_DME_BloodOverlay)
            {
                m_DME_BloodOverlay.Update(timeslice, bloodRaw);
            }
            
            // DEBUG: Log all values every 2 seconds (only if debug enabled)
            DME_ColdConfig config = DME_ColdConfig.GetInstance();
            if (config && config.DebugEnabled)
            {
                static float debugTimer = 0;
                debugTimer += timeslice;
                if (debugTimer >= 2.0)
                {
                    // Get current overlay states
                    float frostLevel = 0;
                    float frostAlpha = 0;
                    float toxicLevel = 0;
                    float toxicAlpha = 0;
                    float bloodLevel = 0;
                    float bloodAlpha = 0;
                    
                    if (m_DME_ColdMissionOverlay)
                    {
                        frostLevel = m_DME_ColdMissionOverlay.GetCurrentFrostLevelFloat();
                        frostAlpha = m_DME_ColdMissionOverlay.GetCurrentAlpha();
                    }
                    if (m_DME_ToxicOverlay)
                    {
                        toxicLevel = m_DME_ToxicOverlay.GetCurrentToxicLevelFloat();
                        toxicAlpha = m_DME_ToxicOverlay.GetCurrentAlpha();
                    }
                    if (m_DME_BloodOverlay)
                    {
                        bloodLevel = m_DME_BloodOverlay.GetCurrentBloodLevelFloat();
                        bloodAlpha = m_DME_BloodOverlay.GetCurrentAlpha();
                    }
                    
                    Print("[DME_Cold] ============ DEBUG ============");
                    Print("[DME_Cold] RAW VALUES:");
                    Print("[DME_Cold]   HeatComfort: " + heatComfortRaw.ToString() + " | HeatBuffer: " + heatBufferRaw.ToString() + " | HeatStage: " + heatStage.ToString());
                    Print("[DME_Cold]   Toxicity: " + toxicityRaw.ToString() + " | Blood(GetHealth): " + bloodValueDebug.ToString() + " | Blood(Used): " + bloodRaw.ToString());
                    Print("[DME_Cold] OVERLAY STATES:");
                    Print("[DME_Cold]   Frost:  Level=" + frostLevel.ToString() + " Alpha=" + frostAlpha.ToString() + " (Enabled=" + config.EnableFrostEffect.ToString() + ")");
                    Print("[DME_Cold]   Toxic:  Level=" + toxicLevel.ToString() + " Alpha=" + toxicAlpha.ToString() + " (Enabled=" + config.EnableToxicEffect.ToString() + ")");
                    Print("[DME_Cold]   Blood:  Level=" + bloodLevel.ToString() + " Alpha=" + bloodAlpha.ToString() + " (Enabled=" + config.EnableBloodEffect.ToString() + ")");
                    Print("[DME_Cold] ==================================");
                    
                    debugTimer = 0;
                }
            }
        }
    }
    
    /**
     * Called when admin presses the reload keybind
     */
    protected void OnDME_ReloadKeyPressed()
    {
        Print("[DME_Cold] Client: Admin reload key pressed (Numpad 8)");
        
        // Get player
        PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
        if (!player)
        {
            Print("[DME_Cold] Client: No player found!");
            return;
        }
        
        Print("[DME_Cold] Client: Sending reload request to server...");
        
        // Send RPC to server
        auto params = new Param1<bool>(true);
        GetGame().RPCSingleParam(player, DME_COLD_RPC_RELOAD_REQUEST, params, true);
    }
    
    /**
     * Called when server responds to reload request
     */
    void OnReloadResponse(bool success, string message)
    {
        if (success)
        {
            Print("[DME_Cold] Client: Config reloaded successfully - " + message);
        }
        else
        {
            Print("[DME_Cold] Client: Config reload failed - " + message);
        }
    }
}