/**
 * DME_ColdMissionClient
 * 
 * Extends MissionGameplay (CLIENT ONLY) to manage the frost overlay lifecycle.
 * MissionGameplay only runs on clients, not on the server.
 * 
 * Config is received from server via the Psyerns Core RPC router (handlers in DME_ColdRPCHandler.c)
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

	// Client RPC handlers (Psyerns Core router)
	protected ref DME_ColdRpc m_DME_ColdRpc;

	// Input action handle for the reload keybind
	protected UAInput m_DME_ReloadInput;
	protected bool m_DME_InputInitialized = false;

	// Debug log timer and last applied config sync (to re-init overlays after an admin reload)
	protected float m_DME_DebugTimer;
	protected int m_DME_LastConfigSyncCount;

	/**
	 * Called when the mission initializes
	 */
	override void OnInit()
	{
		super.OnInit();

		// Only on actual client, not server
		if (g_Game && g_Game.IsClient() && !g_Game.IsServer())
		{
			m_DME_ColdRpc = new DME_ColdRpc(false);

			// Get the input action defined in inputs.xml
			m_DME_ReloadInput = GetUApi().GetInputByName("UADMEColdReload");
			if (m_DME_ReloadInput)
			{
				m_DME_InputInitialized = true;
				PsyCore_Log.Get("DME_Cold").Info("Client: Keybind loaded - Reload Config (configurable in Options > Controls > DME Cold)");
			}
			else
			{
				PsyCore_Log.Get("DME_Cold").Warn("Client: Keybind 'UADMEColdReload' not found. Check inputs.xml");
			}
		}
	}

	/**
	 * Called when the mission starts
	 */
	override void OnMissionStart()
	{
		super.OnMissionStart();

		PsyCore_Log.Get("DME_Cold").Info("======== CLIENT MISSION START ========");
		PsyCore_Log.Get("DME_Cold").Info("Client: Waiting for config from server...");
		PsyCore_Log.Get("DME_Cold").Info("======== MISSION START COMPLETE ========");
	}

	protected void DME_InitMissionOverlay()
	{
		if (m_DME_ColdMissionOverlayInitialized)
			return;

		// Wait for config from server before initializing
		if (!DME_ColdConfig.IsConfigSynced())
		{
			return; // Silent wait for config
		}

		PlayerBase player = PlayerBase.Cast(g_Game.GetPlayer());
		if (!player)
		{
			return; // Silent wait - player will be available soon
		}

		PsyCore_Log.Get("DME_Cold").Info("======== MISSION OVERLAY INIT ========");

		// Check if frost effect is enabled
		DME_ColdConfig config = DME_ColdConfig.GetInstance();
		if (config && !config.EnableFrostEffect)
		{
			PsyCore_Log.Get("DME_Cold").Info("Frost effect is disabled in config - skipping overlay");
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
		PsyCore_Log.Get("DME_Cold").Info("Mission: Local player found: " + playerName);

		// Create overlay instance
		m_DME_ColdMissionOverlay = new DME_ColdFrostOverlay();
		if (!m_DME_ColdMissionOverlay)
		{
			PsyCore_Log.Get("DME_Cold").Error("Mission: creating overlay instance!");
			return;
		}

		// Create the actual overlay widgets
		bool success = m_DME_ColdMissionOverlay.CreateOverlay();
		if (!success)
		{
			PsyCore_Log.Get("DME_Cold").Error("Mission: creating overlay widgets!");
			m_DME_ColdMissionOverlay = null;
			return;
		}

		m_DME_ColdMissionOverlayInitialized = true;
		PsyCore_Log.Get("DME_Cold").Info("======== MISSION OVERLAY INITIALIZED ========");
	}

	protected void DME_InitToxicOverlay()
	{
		if (m_DME_ToxicOverlayInitialized)
			return;

		// Wait for config from server before initializing
		if (!DME_ColdConfig.IsConfigSynced())
		{
			return; // Silent wait for config
		}

		PlayerBase player = PlayerBase.Cast(g_Game.GetPlayer());
		if (!player)
		{
			return; // Silent wait - player will be available soon
		}

		PsyCore_Log.Get("DME_Cold").Info("======== TOXIC OVERLAY INIT ========");

		// Check if toxic effect is enabled
		DME_ColdConfig config = DME_ColdConfig.GetInstance();
		if (config && !config.EnableToxicEffect)
		{
			PsyCore_Log.Get("DME_Cold").Info("Toxic effect is disabled in config - skipping overlay");
			m_DME_ToxicOverlayInitialized = true; // Mark as "initialized" to stop trying
			return;
		}

		// Create toxic overlay instance
		m_DME_ToxicOverlay = new DME_ToxicOverlay();
		if (!m_DME_ToxicOverlay)
		{
			PsyCore_Log.Get("DME_Cold").Error("Mission: creating toxic overlay instance!");
			return;
		}

		// Create the actual overlay widgets
		bool success = m_DME_ToxicOverlay.CreateOverlay();
		if (!success)
		{
			PsyCore_Log.Get("DME_Cold").Error("Mission: creating toxic overlay widgets!");
			m_DME_ToxicOverlay = null;
			return;
		}

		m_DME_ToxicOverlayInitialized = true;
		PsyCore_Log.Get("DME_Cold").Info("======== TOXIC OVERLAY INITIALIZED ========");
	}

	protected void DME_InitBloodOverlay()
	{
		if (m_DME_BloodOverlayInitialized)
			return;

		// Wait for config from server before initializing
		if (!DME_ColdConfig.IsConfigSynced())
		{
			return; // Silent wait for config
		}

		PlayerBase player = PlayerBase.Cast(g_Game.GetPlayer());
		if (!player)
		{
			return; // Silent wait - player will be available soon
		}

		PsyCore_Log.Get("DME_Cold").Info("======== BLOOD OVERLAY INIT ========");

		// Check if blood effect is enabled
		DME_ColdConfig config = DME_ColdConfig.GetInstance();
		if (config && !config.EnableBloodEffect)
		{
			PsyCore_Log.Get("DME_Cold").Info("Blood effect is disabled in config - skipping overlay");
			m_DME_BloodOverlayInitialized = true; // Mark as "initialized" to stop trying
			return;
		}

		// Create blood overlay instance
		m_DME_BloodOverlay = new DME_BloodOverlay();
		if (!m_DME_BloodOverlay)
		{
			PsyCore_Log.Get("DME_Cold").Error("Mission: creating blood overlay instance!");
			return;
		}

		// Create the actual overlay widgets
		bool success = m_DME_BloodOverlay.CreateOverlay();
		if (!success)
		{
			PsyCore_Log.Get("DME_Cold").Error("Mission: creating blood overlay widgets!");
			m_DME_BloodOverlay = null;
			return;
		}

		m_DME_BloodOverlayInitialized = true;
		PsyCore_Log.Get("DME_Cold").Info("======== BLOOD OVERLAY INITIALIZED ========");
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
				DME_OnReloadKeyPressed();
			}
		}

		// Config was (re)synced: re-evaluate overlays that were skipped because their effect was disabled
		if (m_DME_LastConfigSyncCount != DME_ColdConfig.GetSyncCount())
		{
			m_DME_LastConfigSyncCount = DME_ColdConfig.GetSyncCount();
			if (!m_DME_ColdMissionOverlay)
				m_DME_ColdMissionOverlayInitialized = false;
			if (!m_DME_ToxicOverlay)
				m_DME_ToxicOverlayInitialized = false;
			if (!m_DME_BloodOverlay)
				m_DME_BloodOverlayInitialized = false;
		}

		// Try to initialize overlay if not done
		if (!m_DME_ColdMissionOverlayInitialized)
		{
			DME_InitMissionOverlay();
		}

		// Try to initialize toxic overlay if not done
		if (!m_DME_ToxicOverlayInitialized)
		{
			DME_InitToxicOverlay();
		}

		// Try to initialize blood overlay if not done
		if (!m_DME_BloodOverlayInitialized)
		{
			DME_InitBloodOverlay();
		}

		// Update overlays if initialized
		PlayerBase player = PlayerBase.Cast(g_Game.GetPlayer());
		if (player)
		{
			int heatStage = player.GetHeatBufferStage();
			float heatComfort = 0.0; // HeatComfort: -1.0 (freezing) .. 1.0 (hot)
			float heatBufferRaw = 0.0; // HeatBuffer: -30 to +30 (synced stat)
			float heatComfortRaw = 0.0;
			float toxicityRaw = 0.0; // Toxicity: 0-100
			float bloodRaw = 5000.0; // Blood: 0-5000 (5000 = full)
			float bloodValueDebug = 5000.0;

			// HeatBuffer is a synced stat and can be read directly
			PlayerStat<float> heatBufferStat = player.GetStatHeatBuffer();
			if (heatBufferStat)
			{
				heatBufferRaw = heatBufferStat.Get();
			}

			// HeatComfort, Toxicity and Blood are not available on the client - they are
			// sent by the server (see DME_ColdPlayerStats.c). Until the first sync arrives
			// the neutral defaults above are used.
			if (player.DME_HasSyncedStats())
			{
				heatComfortRaw = player.DME_GetSyncedHeatComfort();
				heatComfort = Math.Clamp(heatComfortRaw, -1.0, 1.0);
				toxicityRaw = player.DME_GetSyncedToxicity();
				bloodValueDebug = player.DME_GetSyncedBlood();

				// Invalid, zero or negative blood (player not fully loaded) counts as full blood
				if (bloodValueDebug > 0)
				{
					bloodRaw = Math.Clamp(bloodValueDebug, 0.0, 5000.0);
				}
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
				m_DME_DebugTimer += timeslice;
				if (m_DME_DebugTimer >= 2.0)
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

					PsyCore_Log.Get("DME_Cold").Info("============ DEBUG ============");
					PsyCore_Log.Get("DME_Cold").Info("RAW VALUES:");
					PsyCore_Log.Get("DME_Cold").Info("  HeatComfort: " + heatComfortRaw.ToString() + " | HeatBuffer: " + heatBufferRaw.ToString() + " | HeatStage: " + heatStage.ToString());
					PsyCore_Log.Get("DME_Cold").Info("  Toxicity: " + toxicityRaw.ToString() + " | Blood(GetHealth): " + bloodValueDebug.ToString() + " | Blood(Used): " + bloodRaw.ToString());
					PsyCore_Log.Get("DME_Cold").Info("OVERLAY STATES:");
					PsyCore_Log.Get("DME_Cold").Info("  Frost:  Level=" + frostLevel.ToString() + " Alpha=" + frostAlpha.ToString() + " (Enabled=" + config.EnableFrostEffect.ToString() + ")");
					PsyCore_Log.Get("DME_Cold").Info("  Toxic:  Level=" + toxicLevel.ToString() + " Alpha=" + toxicAlpha.ToString() + " (Enabled=" + config.EnableToxicEffect.ToString() + ")");
					PsyCore_Log.Get("DME_Cold").Info("  Blood:  Level=" + bloodLevel.ToString() + " Alpha=" + bloodAlpha.ToString() + " (Enabled=" + config.EnableBloodEffect.ToString() + ")");
					PsyCore_Log.Get("DME_Cold").Info("==================================");

					m_DME_DebugTimer = 0;
				}
			}
		}
	}

	/**
	 * Called when admin presses the reload keybind
	 */
	protected void DME_OnReloadKeyPressed()
	{
		PsyCore_Log.Get("DME_Cold").Info("Client: Admin reload key pressed (Numpad 8)");

		// Get player
		PlayerBase player = PlayerBase.Cast(g_Game.GetPlayer());
		if (!player)
		{
			PsyCore_Log.Get("DME_Cold").Info("Client: No player found!");
			return;
		}

		PsyCore_Log.Get("DME_Cold").Info("Client: Sending reload request to server...");

		// Send RPC to server (the server answers with DME_COLD_RPC_RELOAD_RESPONSE)
		PsyCore_RPC.SendEmpty(DME_COLD_RPC_NS, DME_COLD_RPC_RELOAD_REQUEST);
	}
}
