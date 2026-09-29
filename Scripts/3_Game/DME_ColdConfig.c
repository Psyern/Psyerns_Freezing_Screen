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
	float TransitionSpeed = 0.65;

	// Minimum alpha when frost starts appearing
	float MinAlpha = 0.1;

	// Maximum alpha (transparency) of the frost overlay
	float MaxAlpha = 0.85;

	// ===== TOXICITY SETTINGS =====
	// Enable/disable the toxic overlay effect
	bool EnableToxicEffect = true;

	// Force a specific toxic level for testing (0 = disabled, 1-5 = force that level)
	int DebugForceToxicLevel = 0;

	// Toxicity thresholds (percentage 0-100) - 5 levels
	float ToxicLevel0Threshold = 0.0;   // Below this = no toxic overlay
	float ToxicLevel1Threshold = 20.0;  // Light toxic
	float ToxicLevel2Threshold = 40.0;
	float ToxicLevel3Threshold = 60.0;
	float ToxicLevel4Threshold = 80.0;
	float ToxicLevel5Threshold = 100.0; // Maximum toxic

	// Toxic overlay visual settings
	float ToxicTransitionSpeed = 0.65;
	float ToxicMinAlpha = 0.1;
	float ToxicMaxAlpha = 0.75;

	// ===== BLOOD LOSS SETTINGS =====
	// Enable/disable the blood loss overlay effect
	bool EnableBloodEffect = true;

	// Force a specific blood level for testing (0 = disabled, 1-7 = force that level)
	int DebugForceBloodLevel = 0;

	// Blood thresholds (based on DayZ blood values: 0-5000)
	// Level 7-4: Severe blood loss (smaller ranges)
	// Level 3-1: Moderate blood loss (larger ranges)
	float BloodLevel0Threshold = 5000.0; // Full blood = no overlay
	float BloodLevel1Threshold = 4200.0; // Light blood loss
	float BloodLevel2Threshold = 3400.0; // Moderate
	float BloodLevel3Threshold = 2500.0; // Noticeable
	float BloodLevel4Threshold = 1700.0; // Serious
	float BloodLevel5Threshold = 1000.0; // Severe
	float BloodLevel6Threshold = 750.0;  // Critical
	float BloodLevel7Threshold = 500.0;  // Near death

	// Blood overlay visual settings
	float BloodTransitionSpeed = 0.65;
	float BloodMinAlpha = 0.1;
	float BloodMaxAlpha = 0.8;

	// Singleton instance
	private static ref DME_ColdConfig s_Instance;

	// Flag to track if config has been synced from server
	private static bool s_ConfigSynced = false;

	// Incremented every time a config from the server is applied (client)
	private static int s_SyncCount = 0;

	/**
	 * Get the singleton instance
	 * On SERVER: loads from file if not loaded
	 * On CLIENT: returns instance (may be default until synced from server)
	 */
	static DME_ColdConfig GetInstance()
	{
		if (!s_Instance)
		{
			// Only load from file on server
			if (g_Game && g_Game.IsServer())
			{
				s_Instance = LoadFromFile();
				s_ConfigSynced = true; // Server always has valid config
			}
			else
			{
				s_Instance = new DME_ColdConfig();
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
	 * Number of configs applied from the server (client-side), changes after an admin reload
	 */
	static int GetSyncCount()
	{
		return s_SyncCount;
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
		s_SyncCount++;

		Print("[DME_Cold] Client: Config synced from server!");
		Print("[DME_Cold]   EnableFrostEffect: " + s_Instance.EnableFrostEffect.ToString());
		Print("[DME_Cold]   EnableToxicEffect: " + s_Instance.EnableToxicEffect.ToString());
		Print("[DME_Cold]   EnableBloodEffect: " + s_Instance.EnableBloodEffect.ToString());
		Print("[DME_Cold]   DebugEnabled: " + s_Instance.DebugEnabled.ToString());
		Print("[DME_Cold]   DebugForceFrostLevel: " + s_Instance.DebugForceFrostLevel.ToString());
		Print("[DME_Cold]   DebugForceToxicLevel: " + s_Instance.DebugForceToxicLevel.ToString());
		Print("[DME_Cold]   DebugForceBloodLevel: " + s_Instance.DebugForceBloodLevel.ToString());
		Print("[DME_Cold]   TransitionSpeed: " + s_Instance.TransitionSpeed.ToString());
		Print("[DME_Cold]   MinAlpha: " + s_Instance.MinAlpha.ToString());
		Print("[DME_Cold]   MaxAlpha: " + s_Instance.MaxAlpha.ToString());
	}

	/**
	 * Force reload config from file (SERVER ONLY)
	 * Used by admin reload command
	 */
	static void ReloadConfigFromFile()
	{
		if (!g_Game || !g_Game.IsServer())
		{
			Print("[DME_Cold] ReloadConfigFromFile can only be called on server!");
			return;
		}

		Print("[DME_Cold] Server: Reloading config from file...");

		// Create new instance and load from file
		s_Instance = LoadFromFile();
		s_ConfigSynced = true;

		Print("[DME_Cold] Server: Config reloaded successfully!");
		Print("[DME_Cold]   EnableFrostEffect: " + s_Instance.EnableFrostEffect.ToString());
		Print("[DME_Cold]   EnableToxicEffect: " + s_Instance.EnableToxicEffect.ToString());
		Print("[DME_Cold]   EnableBloodEffect: " + s_Instance.EnableBloodEffect.ToString());
		Print("[DME_Cold]   DebugEnabled: " + s_Instance.DebugEnabled.ToString());
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
		// Toxicity settings (5 levels)
		ctx.Write(EnableToxicEffect);
		ctx.Write(DebugForceToxicLevel);
		ctx.Write(ToxicLevel0Threshold);
		ctx.Write(ToxicLevel1Threshold);
		ctx.Write(ToxicLevel2Threshold);
		ctx.Write(ToxicLevel3Threshold);
		ctx.Write(ToxicLevel4Threshold);
		ctx.Write(ToxicLevel5Threshold);
		ctx.Write(ToxicTransitionSpeed);
		ctx.Write(ToxicMinAlpha);
		ctx.Write(ToxicMaxAlpha);
		// Blood settings
		ctx.Write(EnableBloodEffect);
		ctx.Write(DebugForceBloodLevel);
		ctx.Write(BloodLevel0Threshold);
		ctx.Write(BloodLevel1Threshold);
		ctx.Write(BloodLevel2Threshold);
		ctx.Write(BloodLevel3Threshold);
		ctx.Write(BloodLevel4Threshold);
		ctx.Write(BloodLevel5Threshold);
		ctx.Write(BloodLevel6Threshold);
		ctx.Write(BloodLevel7Threshold);
		ctx.Write(BloodTransitionSpeed);
		ctx.Write(BloodMinAlpha);
		ctx.Write(BloodMaxAlpha);
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
		// Toxicity settings (5 levels)
		if (!ctx.Read(EnableToxicEffect)) return false;
		if (!ctx.Read(DebugForceToxicLevel)) return false;
		if (!ctx.Read(ToxicLevel0Threshold)) return false;
		if (!ctx.Read(ToxicLevel1Threshold)) return false;
		if (!ctx.Read(ToxicLevel2Threshold)) return false;
		if (!ctx.Read(ToxicLevel3Threshold)) return false;
		if (!ctx.Read(ToxicLevel4Threshold)) return false;
		if (!ctx.Read(ToxicLevel5Threshold)) return false;
		if (!ctx.Read(ToxicTransitionSpeed)) return false;
		if (!ctx.Read(ToxicMinAlpha)) return false;
		if (!ctx.Read(ToxicMaxAlpha)) return false;
		// Blood settings
		if (!ctx.Read(EnableBloodEffect)) return false;
		if (!ctx.Read(DebugForceBloodLevel)) return false;
		if (!ctx.Read(BloodLevel0Threshold)) return false;
		if (!ctx.Read(BloodLevel1Threshold)) return false;
		if (!ctx.Read(BloodLevel2Threshold)) return false;
		if (!ctx.Read(BloodLevel3Threshold)) return false;
		if (!ctx.Read(BloodLevel4Threshold)) return false;
		if (!ctx.Read(BloodLevel5Threshold)) return false;
		if (!ctx.Read(BloodLevel6Threshold)) return false;
		if (!ctx.Read(BloodLevel7Threshold)) return false;
		if (!ctx.Read(BloodTransitionSpeed)) return false;
		if (!ctx.Read(BloodMinAlpha)) return false;
		if (!ctx.Read(BloodMaxAlpha)) return false;
		return true;
	}

	/**
	 * Force reload configuration from file (SERVER ONLY)
	 */
	static void Reload()
	{
		if (!g_Game || !g_Game.IsServer())
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
		return "$profile:\\DeadmansEcho\\DME_Overlay.json";
	}

	/**
	 * Get the config directory path
	 */
	static string GetConfigDir()
	{
		return "$profile:\\DeadmansEcho";
	}

	/**
	 * Load the configuration from file (creates the default file if missing)
	 * JsonFileLoader.LoadFile uses an out parameter, so the loaded object is returned
	 * instead of filling "this".
	 */
	static DME_ColdConfig LoadFromFile()
	{
		string configPath = GetConfigPath();
		DME_ColdConfig config = new DME_ColdConfig();

		Print("[DME_Cold] Attempting to load config from: " + configPath);

		if (FileExist(configPath))
		{
			Print("[DME_Cold] Config file found, loading...");
			string loadError;
			if (!JsonFileLoader<DME_ColdConfig>.LoadFile(configPath, config, loadError) || !config)
			{
				// Keep the broken file untouched so the admin can fix it
				Print("[DME_Cold] ERROR: Failed to load config, using defaults: " + loadError);
				config = new DME_ColdConfig();
			}
			else
			{
				Print("[DME_Cold] Configuration loaded from: " + configPath);
				config.PrintValues();
			}
		}
		else
		{
			// Create default config - values already set by class initializers above
			Print("[DME_Cold] Config file not found, creating default...");
			config.Save();
			Print("[DME_Cold] Default config saved");
		}

		// Log debug status
		if (config.DebugEnabled)
		{
			Print("[DME_Cold] ======== DEBUG MODE ENABLED ========");
			Print("[DME_Cold] If DebugForceFrostLevel is > 0, that level will be forced");
			Print("[DME_Cold] Current forced level: " + config.DebugForceFrostLevel.ToString());
		}

		return config;
	}

	/**
	 * Print the loaded config values to the log
	 */
	void PrintValues()
	{
		Print("[DME_Cold] LOADED CONFIG VALUES:");
		Print("[DME_Cold]   EnableFrostEffect: " + EnableFrostEffect.ToString());
		Print("[DME_Cold]   EnableToxicEffect: " + EnableToxicEffect.ToString());
		Print("[DME_Cold]   EnableBloodEffect: " + EnableBloodEffect.ToString());
		Print("[DME_Cold]   DebugEnabled: " + DebugEnabled.ToString());
		Print("[DME_Cold]   DebugForceFrostLevel: " + DebugForceFrostLevel.ToString());
		Print("[DME_Cold]   DebugForceToxicLevel: " + DebugForceToxicLevel.ToString());
		Print("[DME_Cold]   DebugForceBloodLevel: " + DebugForceBloodLevel.ToString());
		Print("[DME_Cold]   DebugLogTemperature: " + DebugLogTemperature.ToString());
		Print("[DME_Cold]   TransitionSpeed: " + TransitionSpeed.ToString());
		Print("[DME_Cold]   MinAlpha: " + MinAlpha.ToString());
		Print("[DME_Cold]   MaxAlpha: " + MaxAlpha.ToString());
		Print("[DME_Cold]   ToxicTransitionSpeed: " + ToxicTransitionSpeed.ToString());
		Print("[DME_Cold]   ToxicMinAlpha: " + ToxicMinAlpha.ToString());
		Print("[DME_Cold]   ToxicMaxAlpha: " + ToxicMaxAlpha.ToString());
		Print("[DME_Cold]   BloodTransitionSpeed: " + BloodTransitionSpeed.ToString());
		Print("[DME_Cold]   BloodMinAlpha: " + BloodMinAlpha.ToString());
		Print("[DME_Cold]   BloodMaxAlpha: " + BloodMaxAlpha.ToString());
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

		string saveError;
		if (!JsonFileLoader<DME_ColdConfig>.SaveFile(configPath, this, saveError))
		{
			Print("[DME_Cold] ERROR: Failed to save config: " + saveError);
			return;
		}
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
