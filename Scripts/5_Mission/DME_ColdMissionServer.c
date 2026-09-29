/**
 * DME_ColdMissionServer
 *
 * Server side: loads the config, imports the legacy admin list into the Psyerns Core admin list,
 * registers the RPC handlers and sends the config to every connecting client.
 */
modded class MissionServer
{
	protected ref DME_ColdRpc m_DME_ColdRpc;

	override void OnMissionStart()
	{
		super.OnMissionStart();

		PsyCore_Log.Get("DME_Cold").Info("======== SERVER MISSION START ========");

		DME_ColdAdminConfig.ImportIntoCore();
		m_DME_ColdRpc = new DME_ColdRpc(true);

		DME_ColdConfig config = DME_ColdConfig.GetInstance();
		if (config)
		{
			PsyCore_Log.Get("DME_Cold").Info("Server: Config loaded successfully");
			PsyCore_Log.Get("DME_Cold").Info("Server: EnableFrostEffect = " + config.EnableFrostEffect.ToString());
			PsyCore_Log.Get("DME_Cold").Info("Server: DebugForceFrostLevel = " + config.DebugForceFrostLevel.ToString());
		}
		else
		{
			PsyCore_Log.Get("DME_Cold").Error("Server: Config is null!");
		}

		PsyCore_Log.Get("DME_Cold").Info("======== SERVER READY ========");
	}

	override void InvokeOnConnect(PlayerBase player, PlayerIdentity identity)
	{
		super.InvokeOnConnect(player, identity);

		if (!player || !identity)
			return;

		DME_ColdRpc.SendConfigTo(player, identity);
	}
}
