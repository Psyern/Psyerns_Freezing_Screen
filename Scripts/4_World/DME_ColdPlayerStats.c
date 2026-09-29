/**
 * DME_ColdPlayerStats
 * 
 * HeatComfort and Toxicity are NOT synced player stats (only HeatBuffer is, see
 * PlayerStatsPCO_v130 / EPSstatsFlags.SYNCED), and the exact blood value is server-side.
 * On a multiplayer client these values stay at their defaults, so the server sends
 * them to the owning client periodically via RPC.
 */
modded class PlayerBase
{
	protected float m_DME_StatSyncTimer;
	protected float m_DME_StatSyncForceTimer;
	protected bool m_DME_HasSyncedStats;
	protected float m_DME_SyncedHeatComfort;
	protected float m_DME_SyncedToxicity;
	protected float m_DME_SyncedBlood = 5000.0;

	override void OnScheduledTick(float deltaTime)
	{
		super.OnScheduledTick(deltaTime);

		if (!g_Game || !g_Game.IsServer())
			return;

		m_DME_StatSyncTimer += deltaTime;
		m_DME_StatSyncForceTimer += deltaTime;
		if (m_DME_StatSyncTimer < DME_COLD_STAT_SYNC_INTERVAL)
			return;

		m_DME_StatSyncTimer = 0;
		DME_UpdateSyncedStats();
	}

	/**
	 * SERVER: read the current stats and send them to the owning client if they changed
	 */
	protected void DME_UpdateSyncedStats()
	{
		float heatComfort = 0;
		PlayerStat<float> heatComfortStat = GetStatHeatComfort();
		if (heatComfortStat)
			heatComfort = heatComfortStat.Get();

		float toxicity = 0;
		PlayerStat<float> toxicityStat = GetStatToxicity();
		if (toxicityStat)
			toxicity = toxicityStat.Get();

		float blood = GetHealth("", "Blood");

		bool changed = !m_DME_HasSyncedStats;
		if (Math.AbsFloat(heatComfort - m_DME_SyncedHeatComfort) > 0.01)
			changed = true;
		if (Math.AbsFloat(toxicity - m_DME_SyncedToxicity) > 0.5)
			changed = true;
		if (Math.AbsFloat(blood - m_DME_SyncedBlood) > 5.0)
			changed = true;

		// Values are stored locally as well - offline/listen server reads them directly
		m_DME_SyncedHeatComfort = heatComfort;
		m_DME_SyncedToxicity = toxicity;
		m_DME_SyncedBlood = blood;
		m_DME_HasSyncedStats = true;

		if (!g_Game.IsMultiplayer())
			return;

		if (!changed && m_DME_StatSyncForceTimer < DME_COLD_STAT_SYNC_FORCE_INTERVAL)
			return;

		PlayerIdentity identity = GetIdentity();
		if (!identity)
			return;

		m_DME_StatSyncForceTimer = 0;
		Param3<float, float, float> data = new Param3<float, float, float>(heatComfort, toxicity, blood);
		g_Game.RPCSingleParam(this, DME_COLD_RPC_SYNC_STATS, data, false, identity);
	}

	/**
	 * CLIENT: apply stats received from the server
	 */
	void DME_ApplySyncedStats(float heatComfort, float toxicity, float blood)
	{
		m_DME_SyncedHeatComfort = heatComfort;
		m_DME_SyncedToxicity = toxicity;
		m_DME_SyncedBlood = blood;
		m_DME_HasSyncedStats = true;
	}

	bool DME_HasSyncedStats()
	{
		return m_DME_HasSyncedStats;
	}

	// HeatComfort: -1.0 (freezing) .. 1.0 (hot)
	float DME_GetSyncedHeatComfort()
	{
		return m_DME_SyncedHeatComfort;
	}

	// Toxicity: 0 .. 100
	float DME_GetSyncedToxicity()
	{
		return m_DME_SyncedToxicity;
	}

	// Blood: 0 .. 5000
	float DME_GetSyncedBlood()
	{
		return m_DME_SyncedBlood;
	}
}
