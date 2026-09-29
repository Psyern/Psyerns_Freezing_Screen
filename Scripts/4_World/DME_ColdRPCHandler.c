/**
 * DME_ColdRPCHandler
 *
 * All DME_Cold network messages via the Psyerns Core RPC router (namespace DME_COLD_RPC_NS).
 * One instance lives on the server (created in MissionServer) and one on the client
 * (created in MissionGameplay); each registers only the handlers of its side.
 */
class DME_ColdRpc : Managed
{
	void DME_ColdRpc(bool isServer)
	{
		if (isServer)
		{
			PsyCore_RPC.Register(DME_COLD_RPC_NS, DME_COLD_RPC_RELOAD_REQUEST, this, "OnReloadRequest", PsyCore_RpcFlags.SERVER);
		}
		else
		{
			PsyCore_RPC.Register(DME_COLD_RPC_NS, DME_COLD_RPC_SYNC_CONFIG, this, "OnSyncConfig", PsyCore_RpcFlags.CLIENT);
			PsyCore_RPC.Register(DME_COLD_RPC_NS, DME_COLD_RPC_SYNC_STATS, this, "OnSyncStats", PsyCore_RpcFlags.CLIENT);
			PsyCore_RPC.Register(DME_COLD_RPC_NS, DME_COLD_RPC_RELOAD_RESPONSE, this, "OnReloadResponse", PsyCore_RpcFlags.CLIENT);
		}
	}

	// ---- server -------------------------------------------------------------------------

	// SERVER: admin reload request (answered with a response for admins and non-admins alike)
	void OnReloadRequest(ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		if (!sender)
			return;
		PlayerBase player = PsyCore_Players.FindByIdentity(sender);
		PsyCore_Log.Get("DME_Cold").Info("Server: Reload request from " + sender.GetName() + " (Steam64: " + sender.GetPlainId() + ")");

		if (!PsyCore_Admin.IsAdmin(sender))
		{
			PsyCore_Log.Get("DME_Cold").Warn("Server: Player " + sender.GetName() + " is NOT an admin - request denied!");
			SendReloadResponse(player, sender, false, "You are not an admin");
			return;
		}

		PsyCore_Log.Get("DME_Cold").Info("Server: Player " + sender.GetName() + " IS an admin - reloading config...");
		DME_ColdConfig.ReloadConfigFromFile();
		SendConfigToAll();
		SendReloadResponse(player, sender, true, "Config reloaded and synced to all players");
		PsyCore_Log.Get("DME_Cold").Info("Server: Config reload complete!");
	}

	// SERVER: config to one client (raw payload, target = the player's object)
	static void SendConfigTo(PlayerBase player, PlayerIdentity identity)
	{
		if (!player || !identity)
			return;
		DME_ColdConfig config = DME_ColdConfig.GetInstance();
		if (!config)
		{
			PsyCore_Log.Get("DME_Cold").Error("Server: Cannot send config - config is null!");
			return;
		}
		ScriptRPC rpc = PsyCore_RPC.Begin(DME_COLD_RPC_NS, DME_COLD_RPC_SYNC_CONFIG);
		config.WriteToParams(rpc);
		PsyCore_RPC.Send(rpc, true, identity, player);
		PsyCore_Log.Get("DME_Cold").Debug("Server: Config sent to " + identity.GetName());
	}

	// SERVER: config to every connected client (after a reload)
	static void SendConfigToAll()
	{
		array<PlayerBase> players = new array<PlayerBase>();
		PsyCore_Players.GetOnline(players);
		foreach (PlayerBase pb : players)
		{
			SendConfigTo(pb, pb.GetIdentity());
		}
		PsyCore_Log.Get("DME_Cold").Info("Server: Config broadcast complete - sent to " + players.Count().ToString() + " players");
	}

	static void SendReloadResponse(PlayerBase player, PlayerIdentity identity, bool success, string message)
	{
		if (!identity)
			return;
		PsyCore_RPC.SendParam(DME_COLD_RPC_NS, DME_COLD_RPC_RELOAD_RESPONSE, new Param2<bool, string>(success, message), true, identity, player);
	}

	// ---- client -------------------------------------------------------------------------

	void OnSyncConfig(ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		PsyCore_Log.Get("DME_Cold").Info("Client: Received config sync RPC");
		DME_ColdConfig config = new DME_ColdConfig();
		if (config.ReadFromParams(ctx))
		{
			DME_ColdConfig.ApplySyncedConfig(config);
			PsyCore_Log.Get("DME_Cold").Info("Client: Config applied successfully!");
		}
		else
		{
			PsyCore_Log.Get("DME_Cold").Error("Client: Failed to read config from RPC!");
		}
	}

	void OnSyncStats(ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		Param3<float, float, float> stats;
		if (!ctx.Read(stats) || !stats)
			return;
		PlayerBase player = PlayerBase.Cast(target);
		if (!player)
			player = PlayerBase.Cast(g_Game.GetPlayer());
		if (player)
			player.DME_ApplySyncedStats(stats.param1, stats.param2, stats.param3);
	}

	void OnReloadResponse(ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		Param2<bool, string> response;
		if (!ctx.Read(response) || !response)
			return;
		if (response.param1)
			PsyCore_Log.Get("DME_Cold").Info("Client: Admin reload SUCCESS - " + response.param2);
		else
			PsyCore_Log.Get("DME_Cold").Warn("Client: Admin reload FAILED - " + response.param2);
	}
}
