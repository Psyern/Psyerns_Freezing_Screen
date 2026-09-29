// DME_Cold legacy admin list ($profile:DeadmansEcho/dme_cold_admins.json).
// Admin checks now go through PsyCore_Admin ($profile:DeadmansEcho/Core/admins.json).
// The old file is still read on server start and its ids stay valid (PsyCore_Admin.ImportLegacy).
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

	// Reads the legacy file (if present) and registers its ids with the core admin list.
	static void ImportIntoCore()
	{
		string path = GetConfigPath();
		if (!FileExist(path))
			return;

		DME_ColdAdminConfig config = new DME_ColdAdminConfig();
		string loadError;
		if (!PsyCore_Json<DME_ColdAdminConfig>.Load(path, config, loadError) || !config)
		{
			PsyCore_Log.Get("DME_Cold").Error("Failed to read legacy admin file " + path + ": " + loadError);
			return;
		}
		PsyCore_Admin.ImportLegacy("DME_Cold:" + path, config.AdminGUIDs);
		PsyCore_Log.Get("DME_Cold").Info("Legacy admin list imported into Psyerns Core (" + config.AdminGUIDs.Count().ToString() + " entries)");
	}
};
