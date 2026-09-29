/**
 * DME_ColdConfig
 * 
 * Configuration for the DME_Cold mod.
 * Configuration file is saved to: $profile:DeadmansEcho/DME_Cold_Config.json
 * 
 * IMPORTANT: Config is loaded on SERVER only, then synced to clients via RPC.
 */
class DME_ColdConfig : PsyCore_ConfigBase
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

	// PsyCore_ConfigBase: the version field keeps its existing JSON name "ConfigVersion"
	override int GetVersion()
	{
		return ConfigVersion;
	}

	override void SetVersion(int version)
	{
		ConfigVersion = version;
	}

	override int GetCurrentVersion()
	{
		return 1;
	}

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
				PsyCore_Log.Get("DME_Cold").Info("Client: Using default config until server sync");
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

		PsyCore_Log.Get("DME_Cold").Info("Client: Config synced from server!");
		PsyCore_Log.Get("DME_Cold").Info("  EnableFrostEffect: " + s_Instance.EnableFrostEffect.ToString());
		PsyCore_Log.Get("DME_Cold").Info("  EnableToxicEffect: " + s_Instance.EnableToxicEffect.ToString());
		PsyCore_Log.Get("DME_Cold").Info("  EnableBloodEffect: " + s_Instance.EnableBloodEffect.ToString());
		PsyCore_Log.Get("DME_Cold").Info("  DebugEnabled: " + s_Instance.DebugEnabled.ToString());
		PsyCore_Log.Get("DME_Cold").Info("  DebugForceFrostLevel: " + s_Instance.DebugForceFrostLevel.ToString());
		PsyCore_Log.Get("DME_Cold").Info("  DebugForceToxicLevel: " + s_Instance.DebugForceToxicLevel.ToString());
		PsyCore_Log.Get("DME_Cold").Info("  DebugForceBloodLevel: " + s_Instance.DebugForceBloodLevel.ToString());
		PsyCore_Log.Get("DME_Cold").Info("  TransitionSpeed: " + s_Instance.TransitionSpeed.ToString());
		PsyCore_Log.Get("DME_Cold").Info("  MinAlpha: " + s_Instance.MinAlpha.ToString());
		PsyCore_Log.Get("DME_Cold").Info("  MaxAlpha: " + s_Instance.MaxAlpha.ToString());
	}

	/**
	 * Force reload config from file (SERVER ONLY)
	 * Used by admin reload command
	 */
	static void ReloadConfigFromFile()
	{
		if (!g_Game || !g_Game.IsServer())
		{
			PsyCore_Log.Get("DME_Cold").Info("ReloadConfigFromFile can only be called on server!");
			return;
		}

		PsyCore_Log.Get("DME_Cold").Info("Server: Reloading config from file...");

		// Create new instance and load from file
		s_Instance = LoadFromFile();
		s_ConfigSynced = true;

		PsyCore_Log.Get("DME_Cold").Info("Server: Config reloaded successfully!");
		PsyCore_Log.Get("DME_Cold").Info("  EnableFrostEffect: " + s_Instance.EnableFrostEffect.ToString());
		PsyCore_Log.Get("DME_Cold").Info("  EnableToxicEffect: " + s_Instance.EnableToxicEffect.ToString());
		PsyCore_Log.Get("DME_Cold").Info("  EnableBloodEffect: " + s_Instance.EnableBloodEffect.ToString());
		PsyCore_Log.Get("DME_Cold").Info("  DebugEnabled: " + s_Instance.DebugEnabled.ToString());
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
			PsyCore_Log.Get("DME_Cold").Warn("Reload() called on client - ignored");
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
		bool existed = FileExist(configPath);
		PsyCore_Log.Get("DME_Cold").Info("Attempting to load config from: " + configPath);

		// Existing files are never rewritten (a broken file stays untouched so the admin can fix it)
		DME_ColdConfig config;
		bool loaded = PsyCore_ConfigLoader<DME_ColdConfig>.Load(configPath, config, "", "DME_Cold", false);
		if (loaded)
		{
			PsyCore_Log.Get("DME_Cold").Info("Configuration loaded from: " + configPath);
			config.PrintValues();
		}
		else if (!existed)
		{
			PsyCore_Log.Get("DME_Cold").Info("Config file not found, creating default...");
			config.Save();
		}

		// Log debug status
		if (config.DebugEnabled)
		{
			PsyCore_Log.Get("DME_Cold").Info("======== DEBUG MODE ENABLED ========");
			PsyCore_Log.Get("DME_Cold").Info("If DebugForceFrostLevel is > 0, that level will be forced");
			PsyCore_Log.Get("DME_Cold").Info("Current forced level: " + config.DebugForceFrostLevel.ToString());
		}

		return config;
	}

	/**
	 * Print the loaded config values to the log
	 */
	void PrintValues()
	{
		PsyCore_Log.Get("DME_Cold").Info("LOADED CONFIG VALUES:");
		PsyCore_Log.Get("DME_Cold").Info("  EnableFrostEffect: " + EnableFrostEffect.ToString());
		PsyCore_Log.Get("DME_Cold").Info("  EnableToxicEffect: " + EnableToxicEffect.ToString());
		PsyCore_Log.Get("DME_Cold").Info("  EnableBloodEffect: " + EnableBloodEffect.ToString());
		PsyCore_Log.Get("DME_Cold").Info("  DebugEnabled: " + DebugEnabled.ToString());
		PsyCore_Log.Get("DME_Cold").Info("  DebugForceFrostLevel: " + DebugForceFrostLevel.ToString());
		PsyCore_Log.Get("DME_Cold").Info("  DebugForceToxicLevel: " + DebugForceToxicLevel.ToString());
		PsyCore_Log.Get("DME_Cold").Info("  DebugForceBloodLevel: " + DebugForceBloodLevel.ToString());
		PsyCore_Log.Get("DME_Cold").Info("  DebugLogTemperature: " + DebugLogTemperature.ToString());
		PsyCore_Log.Get("DME_Cold").Info("  TransitionSpeed: " + TransitionSpeed.ToString());
		PsyCore_Log.Get("DME_Cold").Info("  MinAlpha: " + MinAlpha.ToString());
		PsyCore_Log.Get("DME_Cold").Info("  MaxAlpha: " + MaxAlpha.ToString());
		PsyCore_Log.Get("DME_Cold").Info("  ToxicTransitionSpeed: " + ToxicTransitionSpeed.ToString());
		PsyCore_Log.Get("DME_Cold").Info("  ToxicMinAlpha: " + ToxicMinAlpha.ToString());
		PsyCore_Log.Get("DME_Cold").Info("  ToxicMaxAlpha: " + ToxicMaxAlpha.ToString());
		PsyCore_Log.Get("DME_Cold").Info("  BloodTransitionSpeed: " + BloodTransitionSpeed.ToString());
		PsyCore_Log.Get("DME_Cold").Info("  BloodMinAlpha: " + BloodMinAlpha.ToString());
		PsyCore_Log.Get("DME_Cold").Info("  BloodMaxAlpha: " + BloodMaxAlpha.ToString());
	}

	/**
	 * Save the configuration to file
	 */
	void Save()
	{
		if (PsyCore_ConfigLoader<DME_ColdConfig>.Save(GetConfigPath(), this, "DME_Cold"))
			PsyCore_Log.Get("DME_Cold").Info("Configuration saved to: " + GetConfigPath());
	}

	/**
	 * Debug log helper - only logs if debug is enabled
	 */
	void DebugLog(string message)
	{
		if (DebugEnabled || DebugLogTemperature)
		{
			PsyCore_Log.Get("DME_Cold").Force("DEBUG " + message);
		}
	}
}
