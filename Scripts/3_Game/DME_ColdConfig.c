/**
 * DME_ColdConfig
 * 
 * Configuration for the DME_Cold mod.
 * Configuration file is saved to: $profile:DeadmansEcho/DME_Cold_Config.json
 * 
 * IMPORTANT: Config is loaded on SERVER only, then synced to clients via RPC.
 */
class DME_ColdConfig
{
    // Config version for future compatibility
    int ConfigVersion = 1;
    
    // Enable/disable the frost effect entirely
    bool EnableFrostEffect = true;
    
    // ===== DEBUG OPTIONS =====
    // Enable debug mode (shows debug info on screen)
    bool DebugEnabled = false;
    
    // Force a specific frost level for testing (0 = disabled, 1-7 = force that level)
    int DebugForceFrostLevel = 0;
    
    // Log temperature values to server/client log
    bool DebugLogTemperature = false;
    
    // ===== TEMPERATURE THRESHOLDS =====
    // Temperature thresholds (can be customized by server admins)
    // Based on mapped heat comfort values (28-38°C range)
    float FrostLevel0Threshold = 35.0;  // Above this = no frost
    float FrostLevel1Threshold = 34.0;  // Light frost
    float FrostLevel2Threshold = 33.0;
    float FrostLevel3Threshold = 32.0;
    float FrostLevel4Threshold = 31.0;
    float FrostLevel5Threshold = 30.0;
    float FrostLevel6Threshold = 29.0;
    float FrostLevel7Threshold = 28.0;  // Maximum frost
    
    // ===== VISUAL SETTINGS =====
    // Transition speed (how fast frost appears/disappears, higher = faster)
    float TransitionSpeed = 2.0;
    
    // Minimum alpha when frost starts appearing
    float MinAlpha = 0.1;
    
    // Maximum alpha (transparency) of the frost overlay
    float MaxAlpha = 0.85;
    
    // Singleton instance
    private static ref DME_ColdConfig s_Instance;
    
    // Flag to track if config has been synced from server
    private static bool s_ConfigSynced = false;
    
    /**
     * Get the singleton instance
     * On SERVER: loads from file if not loaded
     * On CLIENT: returns instance (may be default until synced from server)
     */
    static DME_ColdConfig GetInstance()
    {
        if (!s_Instance)
        {
            s_Instance = new DME_ColdConfig();
            
            // Only load from file on server
            if (GetGame().IsServer())
            {
                s_Instance.Load();
                s_ConfigSynced = true; // Server always has valid config
            }
            else
            {
                Print("[DME_Cold] Client: Using default config until server sync");
            }
        }
        return s_Instance;
    }
    
    /**
     * Check if config has been synced from server (client-side check)
     */
    static bool IsConfigSynced()
    {
        return s_ConfigSynced;
    }
    
    /**
     * Apply synced config from server (called on client when RPC received)
     */
    static void ApplySyncedConfig(DME_ColdConfig syncedConfig)
    {
        if (!syncedConfig)
            return;
        
        s_Instance = syncedConfig;
        s_ConfigSynced = true;
        
        Print("[DME_Cold] Client: Config synced from server!");
        Print("[DME_Cold]   EnableFrostEffect: " + s_Instance.EnableFrostEffect.ToString());
        Print("[DME_Cold]   DebugEnabled: " + s_Instance.DebugEnabled.ToString());
        Print("[DME_Cold]   DebugForceFrostLevel: " + s_Instance.DebugForceFrostLevel.ToString());
        Print("[DME_Cold]   TransitionSpeed: " + s_Instance.TransitionSpeed.ToString());
        Print("[DME_Cold]   MinAlpha: " + s_Instance.MinAlpha.ToString());
        Print("[DME_Cold]   MaxAlpha: " + s_Instance.MaxAlpha.ToString());
    }
    
    /**
     * Serialize config to params for RPC
     */
    void WriteToParams(ParamsWriteContext ctx)
    {
        ctx.Write(ConfigVersion);
        ctx.Write(EnableFrostEffect);
        ctx.Write(DebugEnabled);
        ctx.Write(DebugForceFrostLevel);
        ctx.Write(DebugLogTemperature);
        ctx.Write(FrostLevel0Threshold);
        ctx.Write(FrostLevel1Threshold);
        ctx.Write(FrostLevel2Threshold);
        ctx.Write(FrostLevel3Threshold);
        ctx.Write(FrostLevel4Threshold);
        ctx.Write(FrostLevel5Threshold);
        ctx.Write(FrostLevel6Threshold);
        ctx.Write(FrostLevel7Threshold);
        ctx.Write(TransitionSpeed);
        ctx.Write(MinAlpha);
        ctx.Write(MaxAlpha);
    }
    
    /**
     * Deserialize config from params (RPC)
     */
    bool ReadFromParams(ParamsReadContext ctx)
    {
        if (!ctx.Read(ConfigVersion)) return false;
        if (!ctx.Read(EnableFrostEffect)) return false;
        if (!ctx.Read(DebugEnabled)) return false;
        if (!ctx.Read(DebugForceFrostLevel)) return false;
        if (!ctx.Read(DebugLogTemperature)) return false;
        if (!ctx.Read(FrostLevel0Threshold)) return false;
        if (!ctx.Read(FrostLevel1Threshold)) return false;
        if (!ctx.Read(FrostLevel2Threshold)) return false;
        if (!ctx.Read(FrostLevel3Threshold)) return false;
        if (!ctx.Read(FrostLevel4Threshold)) return false;
        if (!ctx.Read(FrostLevel5Threshold)) return false;
        if (!ctx.Read(FrostLevel6Threshold)) return false;
        if (!ctx.Read(FrostLevel7Threshold)) return false;
        if (!ctx.Read(TransitionSpeed)) return false;
        if (!ctx.Read(MinAlpha)) return false;
        if (!ctx.Read(MaxAlpha)) return false;
        return true;
    }
    
    /**
     * Force reload configuration from file (SERVER ONLY)
     */
    static void Reload()
    {
        if (!GetGame().IsServer())
        {
            Print("[DME_Cold] WARNING: Reload() called on client - ignored");
            return;
        }
        s_Instance = null;
        s_ConfigSynced = false;
        GetInstance();
    }
    
    /**
     * Get the config file path
     */
    static string GetConfigPath()
    {
        return "$profile:\\DeadmansEcho\\DME_Cold_Config.json";
    }
    
    /**
     * Get the config directory path
     */
    static string GetConfigDir()
    {
        return "$profile:\\DeadmansEcho";
    }
    
    /**
     * Load the configuration from file
     */
    void Load()
    {
        string configDir = GetConfigDir();
        string configPath = GetConfigPath();
        
        Print("[DME_Cold] Attempting to load config from: " + configPath);
        
        // Create directory if it doesn't exist
        if (!FileExist(configDir))
        {
            if (MakeDirectory(configDir))
            {
                Print("[DME_Cold] Created config directory: " + configDir);
            }
            else
            {
                Print("[DME_Cold] WARNING: Could not create config directory: " + configDir);
            }
        }
        
        // Check if config exists
        if (FileExist(configPath))
        {
            Print("[DME_Cold] Config file found, loading...");
            JsonFileLoader<DME_ColdConfig>.JsonLoadFile(configPath, this);
            Print("[DME_Cold] Configuration loaded from: " + configPath);
            Print("[DME_Cold] LOADED CONFIG VALUES:");
            Print("[DME_Cold]   EnableFrostEffect: " + EnableFrostEffect.ToString());
            Print("[DME_Cold]   DebugEnabled: " + DebugEnabled.ToString());
            Print("[DME_Cold]   DebugForceFrostLevel: " + DebugForceFrostLevel.ToString());
            Print("[DME_Cold]   DebugLogTemperature: " + DebugLogTemperature.ToString());
            Print("[DME_Cold]   TransitionSpeed: " + TransitionSpeed.ToString());
            Print("[DME_Cold]   MinAlpha: " + MinAlpha.ToString());
            Print("[DME_Cold]   MaxAlpha: " + MaxAlpha.ToString());
        }
        else
        {
            // Create default config
            Print("[DME_Cold] Config file not found, creating default...");
            // Values already set by class initializers above
            Save();
            Print("[DME_Cold] Default config saved");
        }
        
        // Log debug status
        if (DebugEnabled)
        {
            Print("[DME_Cold] ======== DEBUG MODE ENABLED ========");
            Print("[DME_Cold] If DebugForceFrostLevel is > 0, that level will be forced");
            Print("[DME_Cold] Current forced level: " + DebugForceFrostLevel.ToString());
        }
    }
    
    /**
     * Save the configuration to file
     */
    void Save()
    {
        string configDir = GetConfigDir();
        string configPath = GetConfigPath();
        
        // Create directory if it doesn't exist
        if (!FileExist(configDir))
        {
            if (MakeDirectory(configDir))
            {
                Print("[DME_Cold] Created config directory: " + configDir);
            }
            else
            {
                Print("[DME_Cold] WARNING: Could not create config directory: " + configDir);
            }
        }
        
        JsonFileLoader<DME_ColdConfig>.JsonSaveFile(configPath, this);
        Print("[DME_Cold] Configuration saved to: " + configPath);
    }
    
    /**
     * Debug log helper - only logs if debug is enabled
     */
    void DebugLog(string message)
    {
        if (DebugEnabled || DebugLogTemperature)
        {
            Print("[DME_Cold DEBUG] " + message);
        }
    }
}
