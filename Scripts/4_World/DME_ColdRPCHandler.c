/**
 * DME_ColdRPCHandler
 *
 * Handles RPC messages for DME_Cold mod.
 * Uses modded PlayerBase to receive config/stat sync from server.
 *
 * Note: Server-side reload request handling is done in MissionServer (5_Mission)
 * because MissionServer type is not available in 4_World module.
 */
modded class PlayerBase
{
	/**
	 * Handle RPC messages
	 */
	override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
	{
		super.OnRPC(sender, rpc_type, ctx);

		if (!g_Game)
			return;

		// SERVER: Handle reload request from admin client
		if (g_Game.IsServer())
		{
			if (rpc_type == DME_COLD_RPC_RELOAD_REQUEST)
			{
				// Only accept the request for the sender's own player
				if (!sender || !GetIdentity() || sender.GetId() != GetIdentity().GetId())
				{
					Print("[DME_Cold] Server: Ignoring reload request - sender does not own this player");
					return;
				}

				Print("[DME_Cold] Server: Received reload request RPC from player");
				// The actual handling is done by MissionServer via DME_ColdReloadHandler's invoker
				DME_ColdReloadHandler.HandleRequest(this, sender);
			}
			return;
		}

		// CLIENT: Handle config sync RPC
		if (rpc_type == DME_COLD_RPC_SYNC_CONFIG)
		{
			Print("[DME_Cold] Client: Received config sync RPC");

			// Create new config and read from RPC
			DME_ColdConfig config = new DME_ColdConfig();
			if (config.ReadFromParams(ctx))
			{
				// Apply the synced config
				DME_ColdConfig.ApplySyncedConfig(config);
				Print("[DME_Cold] Client: Config applied successfully!");
			}
			else
			{
				Print("[DME_Cold] Client: ERROR - Failed to read config from RPC!");
			}
		}
		// CLIENT: Handle player stat sync (HeatComfort, Toxicity, Blood)
		else if (rpc_type == DME_COLD_RPC_SYNC_STATS)
		{
			Param3<float, float, float> stats = new Param3<float, float, float>(0, 0, 5000);
			if (ctx.Read(stats))
			{
				DME_ApplySyncedStats(stats.param1, stats.param2, stats.param3);
			}
		}
		// CLIENT: Handle reload response from server
		else if (rpc_type == DME_COLD_RPC_RELOAD_RESPONSE)
		{
			Print("[DME_Cold] Client: Received reload response RPC");

			Param2<bool, string> response = new Param2<bool, string>(false, "");
			if (ctx.Read(response))
			{
				if (response.param1)
				{
					Print("[DME_Cold] Client: Admin reload SUCCESS - " + response.param2);
				}
				else
				{
					Print("[DME_Cold] Client: Admin reload FAILED - " + response.param2);
				}
			}
		}
	}
}
