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
	static ref DME_ColdAdminConfig Load()
	{
		ref DME_ColdAdminConfig config;
		string path = GetConfigPath();
		
		if (FileExist(path))
		{
			Print("[DME_Cold] Loading admin config from: " + path);
			JsonFileLoader<DME_ColdAdminConfig>.JsonLoadFile(path, config);
			
			if (config)
			{
				Print("[DME_Cold] Admin config loaded with " + config.AdminGUIDs.Count() + " admins");
				return config;
			}
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
		
		JsonFileLoader<DME_ColdAdminConfig>.JsonSaveFile(GetConfigPath(), this);
		Print("[DME_Cold] Admin config saved to: " + GetConfigPath());
	}
	
	static ref DME_ColdAdminConfig CreateDefaultConfig()
	{
		ref DME_ColdAdminConfig config = new DME_ColdAdminConfig();
		
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
