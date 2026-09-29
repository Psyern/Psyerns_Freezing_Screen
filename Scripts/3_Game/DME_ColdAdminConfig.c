// DME_Cold Admin Configuration
// Handles admin authentication for in-game config reload

class DME_ColdAdminConfig
{
	ref array<string> AdminGUIDs;
	
	void DME_ColdAdminConfig()
	{
		AdminGUIDs = new array<string>();
	}
	
	static string GetConfigPath()
	{
		return "$profile:DeadmansEcho/dme_cold_admins.json";
	}
	
	static string GetFolderPath()
	{
		return "$profile:DeadmansEcho";
	}
	
	// Load admin config from JSON file
	static DME_ColdAdminConfig Load()
	{
		DME_ColdAdminConfig config = new DME_ColdAdminConfig();
		string path = GetConfigPath();
		
		if (FileExist(path))
		{
			Print("[DME_Cold] Loading admin config from: " + path);
			string loadError;
			if (JsonFileLoader<DME_ColdAdminConfig>.LoadFile(path, config, loadError) && config)
			{
				if (!config.AdminGUIDs)
					config.AdminGUIDs = new array<string>();
				
				Print("[DME_Cold] Admin config loaded with " + config.AdminGUIDs.Count().ToString() + " admins");
				return config;
			}
			
			// Keep the broken file untouched so the admin can fix it - nobody is admin until then
			Print("[DME_Cold] ERROR: Failed to load admin config: " + loadError);
			return new DME_ColdAdminConfig();
		}
		
		// Create default config if not exists
		Print("[DME_Cold] Admin config not found, creating default...");
		config = CreateDefaultConfig();
		config.Save();
		
		return config;
	}
	
	void Save()
	{
		// Ensure folder exists
		if (!FileExist(GetFolderPath()))
		{
			MakeDirectory(GetFolderPath());
		}
		
		string saveError;
		if (!JsonFileLoader<DME_ColdAdminConfig>.SaveFile(GetConfigPath(), this, saveError))
		{
			Print("[DME_Cold] ERROR: Failed to save admin config: " + saveError);
			return;
		}
		Print("[DME_Cold] Admin config saved to: " + GetConfigPath());
	}
	
	static DME_ColdAdminConfig CreateDefaultConfig()
	{
		DME_ColdAdminConfig config = new DME_ColdAdminConfig();
		
		// Add example GUID (commented format)
		config.AdminGUIDs.Insert("REPLACE_WITH_YOUR_STEAM64_ID");
		
		return config;
	}
	
	// Check if a player GUID is an admin
	bool IsAdmin(string guid)
	{
		if (!AdminGUIDs || AdminGUIDs.Count() == 0)
			return false;
		
		for (int i = 0; i < AdminGUIDs.Count(); i++)
		{
			if (AdminGUIDs.Get(i) == guid)
				return true;
		}
		
		return false;
	}
	
	// Check if a player is an admin by PlayerIdentity
	bool IsPlayerAdmin(PlayerIdentity identity)
	{
		if (!identity)
			return false;
		
		string guid = identity.GetId();
		return IsAdmin(guid);
	}
};
