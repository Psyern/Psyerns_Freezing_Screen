/**
 * DME_ColdRPC
 *
 * RPC names for DME_Cold. All messages go through the Psyerns Core RPC router
 * (PsyCore_RPC, namespace DME_COLD_RPC_NS) - no raw RPC ids any more.
 */

const string DME_COLD_RPC_NS = "DME_Cold";

// server -> client: full config (raw payload written by DME_ColdConfig.WriteToParams)
const string DME_COLD_RPC_SYNC_CONFIG = "SyncConfig";

// client -> server: admin reload request
const string DME_COLD_RPC_RELOAD_REQUEST = "ReloadRequest";

// server -> client: reload result Param2<bool, string>
const string DME_COLD_RPC_RELOAD_RESPONSE = "ReloadResponse";

// server -> owning client: Param3<float, float, float> HeatComfort, Toxicity, Blood
const string DME_COLD_RPC_SYNC_STATS = "SyncStats";

// Stat sync timing (seconds): check interval and forced resend interval
const float DME_COLD_STAT_SYNC_INTERVAL = 1.0;
const float DME_COLD_STAT_SYNC_FORCE_INTERVAL = 10.0;
