# DME_Cold - Frost, Toxic & Blood Overlay Mod for DayZ

A DayZ mod that adds visual overlay effects based on the player's body temperature, toxicity level, and blood loss.

## Features

- **7 Progressive Frost Levels**: As the player gets colder, the screen progressively shows more frost
- **5 Progressive Toxic Levels**: Toxic zone contamination shows green overlay effect
- **7 Progressive Blood Levels**: Blood loss shows red vignette overlay effect
- **Smooth Transitions**: Effects fade in and out smoothly based on stat changes
- **Temperature-Based Frost**: Uses player's HeatComfort stat for realistic cold feeling
- **Toxicity-Based Poison**: Uses player's Toxicity stat for gas zone effects
- **Blood Loss Detection**: Uses player's Blood stat for injury visualization
- **Native DayZ Widgets**: Built using native DayZ widget system (no external dependencies!)
- **Server-Controlled Config**: Configuration is stored on server and synced to clients automatically
- **Admin Hotkey Reload**: Admins can reload config in-game with Numpad 8 (configurable)
- **Configurable**: Server admins can customize thresholds for both effects

## Requirements

- DayZ 1.29 or 1.30 (uses `g_Game`, `JsonFileLoader.LoadFile/SaveFile`)
- No external mod dependencies!

## Installation

1. Copy the `DME_Cold` folder to your DayZ server's mod directory
2. Add `@DME_Cold` to your server's mod list (both server AND client mods!)
3. Restart the server

## Admin Hotkey Reload

Admins can reload the config in-game without restarting the server:

1. **Setup Admin List**: Copy `dme_cold_admins.json.example` to `profiles/DeadmansEcho/dme_cold_admins.json`
2. **Add your Steam64 ID** to the `AdminGUIDs` array (the BI GUID from `PlayerIdentity.GetId()` is accepted as well)
3. **Press Numpad 8** (default) in-game to reload config
4. The keybind can be changed in **Options > Controls > DME Cold** (registered via `Data/Inputs.xml` + `inputs=` in `config.cpp`, labels in `stringtable.csv`)

Example `dme_cold_admins.json`:
```json
{
    "AdminGUIDs": [
        "76561198012345678",
        "76561198087654321"
    ]
}
```

## Configuration

The configuration file is stored **on the server only** at:
```
profiles/DeadmansEcho/DME_Overlay.json
```

**Important:** The config is automatically synced from server to all clients when they connect. Clients do NOT need their own config file - they receive all settings from the server.

### Configuration Options

| Option | Default | Description |
|--------|---------|-------------|
| **EnableFrostEffect** | true | **Enable/disable the frost overlay** |
| **EnableToxicEffect** | true | **Enable/disable the toxic overlay** |
| **EnableBloodEffect** | true | **Enable/disable the blood loss overlay** |
| **DebugEnabled** | false | **Enable debug logging (shows values in log)** |
| **DebugForceFrostLevel** | 0 | **Force a specific frost level (0=off, 1-7=force)** |
| **DebugForceToxicLevel** | 0 | **Force a specific toxic level (0=off, 1-5=force)** |
| **DebugForceBloodLevel** | 0 | **Force a specific blood level (0=off, 1-7=force)** |
| **DebugLogTemperature** | false | **Log temperature values to console** |
| FrostLevel0Threshold | 35.0 | Temperature above which no frost appears |
| FrostLevel1Threshold | 34.0 | Temperature for light frost |
| FrostLevel2Threshold | 33.0 | Temperature for level 2 frost |
| FrostLevel3Threshold | 32.0 | Temperature for level 3 frost |
| FrostLevel4Threshold | 31.0 | Temperature for level 4 frost |
| FrostLevel5Threshold | 30.0 | Temperature for level 5 frost |
| FrostLevel6Threshold | 29.0 | Temperature for level 6 frost |
| FrostLevel7Threshold | 28.0 | Temperature for maximum frost |
| TransitionSpeed | 0.65 | How fast frost fades in/out |
| MinAlpha | 0.1 | Minimum opacity when frost starts |
| MaxAlpha | 0.85 | Maximum opacity of frost overlay (0.0-1.0) |
| ToxicLevel0-5Threshold | 0-100 | Toxicity percentage thresholds (5 levels) |
| ToxicTransitionSpeed | 0.65 | How fast toxic fades in/out |
| ToxicMinAlpha | 0.1 | Minimum opacity when toxic starts |
| ToxicMaxAlpha | 0.75 | Maximum opacity of toxic overlay |
| BloodLevel0-7Threshold | 5000-500 | Blood thresholds (see below) |
| BloodTransitionSpeed | 0.65 | How fast blood fades in/out |
| BloodMinAlpha | 0.1 | Minimum opacity when blood loss starts |
| BloodMaxAlpha | 0.8 | Maximum opacity of blood overlay |

## Debug Mode

Set `DebugEnabled` to `true` in the config to see debug output in the log every 2 seconds:
```
[DME_Cold]   HeatComfort: -0.52 | HeatBuffer: 0.0 | HeatStage: 0
[DME_Cold] HeatComfort(raw): -0.52 HeatBuffer: 0.0 Used: -0.52 Temp: 31.36 FrostFloat: 3.6 Alpha: 0.42
```

**Debug values explained:**
- `HeatComfort`: The HeatComfort stat (-1.0 to +1.0), sent by the server
- `HeatBuffer`: The raw HeatBuffer stat (changes quickly near fire)
- `HeatStage`: The heat buffer stage (informational only)
- `Temp`: Calculated effective temperature in °C
- `FrostFloat`: Current frost level as float (0.0-7.0)
- `Alpha`: Current overlay opacity (0.0-1.0)
- `Toxicity`: Contamination level (0-100)
- `Blood`: Current blood level (0-5000)

Set `DebugForceFrostLevel` to 1-7 to force a specific frost level for testing.
Set `DebugForceToxicLevel` to 1-5 to force a specific toxic level for testing.
Set `DebugForceBloodLevel` to 1-7 to force a specific blood level for testing.

## How It Works

### Architecture

The mod uses a modular architecture across DayZ script modules:

- **3_Game Module**: Core logic (loads first on both server and client)
  - `A_DME_ColdFrostOverlay.c`: Overlay widget, frost level calculation, alpha transitions
  - `DME_ColdConfig.c`: Configuration loading (server) and RPC serialization
  - `DME_ColdRPC.c`: RPC constants for server→client communication
  - `DME_ColdAdminConfig.c`: Admin list for the in-game reload

- **4_World Module**: Entity integration
  - `DME_ColdRPCHandler.c`: Modded PlayerBase that receives config/stat sync RPCs
  - `DME_ColdPlayerStats.c`: Server sends HeatComfort, Toxicity and Blood to the owning client
  - `DME_ColdReloadHandler.c`: Bridges the reload RPC (4_World) to MissionServer (5_Mission)
  
- **5_Mission Module**: Lifecycle management
  - `DME_ColdMissionServer.c`: Loads config, sends to clients on connect
  - `DME_ColdMissionClient.c`: Waits for config, then initializes overlay

### Server → Client Config Sync

```
┌─────────────────────────┐                    ┌─────────────────────────┐
│        SERVER           │                    │         CLIENT          │
├─────────────────────────┤                    ├─────────────────────────┤
│ 1. OnMissionStart()     │                    │ 1. OnMissionStart()     │
│    └─ Load config.json  │                    │    └─ Wait for config...│
│                         │                    │                         │
│ 2. InvokeOnConnect()    │                    │                         │
│    └─ Player joins      │                    │                         │
│                         │                    │                         │
│ 3. SendConfigToClient() │                    │                         │
│    └─ ScriptRPC.Send()  │ ───── RPC ──────▶  │ 2. PlayerBase.OnRPC()   │
│                         │                    │    └─ ApplySyncedConfig()│
│                         │                    │                         │
│                         │                    │ 3. OnUpdate()           │
│                         │                    │    └─ InitMissionOverlay│
│                         │                    │    └─ Overlay starts!   │
└─────────────────────────┘                    └─────────────────────────┘
```

### Server → Client Stat Sync

HeatComfort and Toxicity are **not** synced player stats in vanilla DayZ (only HeatBuffer is,
see `PlayerStatsPCO_v130`), and the exact blood value is server-side. On a multiplayer client
these values stay at their defaults. The server therefore checks them every second in
`PlayerBase.OnScheduledTick` and sends them to the owning client (`DME_COLD_RPC_SYNC_STATS`)
when they changed, at least every 10 seconds. In offline mode the values are read directly.

### Heat Detection

The mod monitors the player's temperature using DayZ's native stat system:

**HeatComfort** (Primary - used for frost level):
- Range: -1.0 to +1.0 (vanilla warning at -0.15, critical at -0.45)
- **Negative values = Cold** → Frost appears
- **Zero = Neutral** → No frost
- **Positive values = Warm** → No frost
- Changes **slowly** (realistic body temperature)
- This is the "felt" temperature that determines frost level

**HeatBuffer** (Secondary - informational):
- Range: -30 to +30
- Changes **quickly** near heat sources (fire, warm buildings)
- Affects how fast HeatComfort changes
- Only shown in debug display

```
HeatComfort → Effective Temp → Frost Level
   -1.0     →    28°C        →    7 (max)
   -0.5     →    31.5°C      →    ~3-4
    0.0     →    35°C        →    0 (none)
   +0.5     →    38.5°C      →    0 (none)
   +1.0     →    42°C        →    0 (none)
```

### Toxicity Detection (Toxic Zones)

The mod monitors the player's toxicity level for gas zone effects:

**Toxicity** (Primary - used for toxic overlay):
- Range: 0 to 100 (percentage)
- **0 = No contamination** → No overlay
- **1-100 = Contaminated** → Green toxic overlay appears
- Increases when in toxic zones without proper protection
- Decreases slowly when leaving toxic zones

```
Toxicity (raw) → Toxic Level → Overlay
     0         →    0        → None
    20         →    1        → Light green tint
    40         →    2        → Moderate
    60         →    3        → Noticeable
    80         →    4        → Strong
   100         →    5        → Maximum
```

### Blood Loss Detection

The mod monitors the player's blood level for injury visualization:

**Blood** (Primary - used for blood overlay):
- Range: 0 to 5000 (5000 = full blood)
- **5000 = Full blood** → No overlay
- **Below 5000 = Blood loss** → Red overlay appears
- Decreases when injured/bleeding
- Increases when eating, resting, or using blood bags

**Blood Thresholds (non-linear distribution):**
- Level 1-3: Larger ranges (~800 each) - moderate blood loss
- Level 4-7: Smaller ranges (~250-700 each) - severe blood loss

```
Blood (raw)  → Blood Level → Overlay
   5000      →    0        → None (full health)
   4200      →    1        → Light red tint
   3400      →    2        → Moderate
   2500      →    3        → Noticeable
   1700      →    4        → Serious
   1000      →    5        → Severe
    750      →    6        → Critical
    500      →    7        → Maximum (near death)
```

### Widget System

- Creates one fullscreen ImageWidget per effect directly on the workspace (negative z-order, ignorepointer)
- Sets alpha transparency (0.0 = invisible, 1.0 = opaque)
- Native DayZ widget system (no sFramework needed)

## File Structure

```
DME_Cold/
├── config.cpp                              # DayZ mod configuration
├── mod.cpp                                 # Mod metadata
├── stringtable.csv                         # Keybind labels
├── dme_cold_admins.json.example            # Admin list template
├── README.md                               # This file
│
├── Data/
│   └── Inputs.xml                          # Admin reload keybind (Numpad 8)
│
├── GUI/
│   ├── layouts/
│   │   └── debug_overlay.layout            # Debug text panel (frost overlay debug)
│   └── textures/
│       ├── Freeze1.paa                     # Frost overlay level 1 (lightest)
│       ├── Freeze2.paa                     # Frost overlay level 2
│       ├── Freeze3.paa                     # Frost overlay level 3
│       ├── Freeze4.paa                     # Frost overlay level 4
│       ├── Freeze5.paa                     # Frost overlay level 5
│       ├── Freeze6.paa                     # Frost overlay level 6
│       ├── Freeze7.paa                     # Frost overlay level 7 (heaviest)
│       ├── toxic1.paa                      # Toxic overlay level 1 (lightest green)
│       ├── toxic2.paa                      # Toxic overlay level 2
│       ├── toxic3.paa                      # Toxic overlay level 3
│       ├── toxic4.paa                      # Toxic overlay level 4
│       ├── toxic5.paa                      # Toxic overlay level 5 (heaviest)
│       ├── blood1.paa                      # Blood overlay level 1 (lightest red)
│       ├── blood2.paa                      # Blood overlay level 2
│       ├── blood3.paa                      # Blood overlay level 3
│       ├── blood4.paa                      # Blood overlay level 4
│       ├── blood5.paa                      # Blood overlay level 5
│       ├── blood6.paa                      # Blood overlay level 6
│       ├── blood7.paa                      # Blood overlay level 7 (heaviest)
│       └── README.txt                      # Texture info
│
└── Scripts/
    ├── 3_Game/                             # Game-level scripts (loads first)
    │   ├── A_DME_BloodOverlay.c            # Blood loss overlay widget logic
    │   ├── A_DME_ColdFrostOverlay.c        # Frost overlay widget logic
    │   ├── A_DME_ToxicOverlay.c            # Toxic overlay widget logic
    │   ├── DME_Cold.c                      # Module entry comment
    │   ├── DME_ColdAdminConfig.c           # Admin list (reload permission)
    │   ├── DME_ColdConfig.c                # Configuration management & RPC serialization
    │   └── DME_ColdRPC.c                   # RPC constants for server-client sync
    │
    ├── 4_World/                            # World-level scripts
    │   ├── DME_Cold.c                      # Module entry comment
    │   ├── DME_ColdPlayerStats.c           # Server -> client stat sync (HeatComfort, Toxicity, Blood)
    │   ├── DME_ColdReloadHandler.c         # Reload request bridge to MissionServer
    │   └── DME_ColdRPCHandler.c            # Receives config/stat RPCs
    │
    └── 5_Mission/                          # Mission-level scripts (loads last)
        ├── DME_Cold.c                      # Module registration
        ├── DME_ColdMissionClient.c         # Client overlay lifecycle (MissionGameplay)
        └── DME_ColdMissionServer.c         # Server config sync (MissionServer)
```

## Performance

- **Minimal Server Load**: Config once per connect; 3 floats per player only when stats change (checked every 1 s, resent at least every 10 s)
- **Client-Side Rendering**: All overlay logic runs on client
- **Efficient Updates**: Only updates visible overlay, no tick when alpha=0
- **Minimal Dependencies**: Pure DayZ native code, no external frameworks
- **Low Memory**: Single overlay widget instance per client

## Troubleshooting

### Overlay Not Visible?

1. Check **server** logs for config loading:
   ```
   [DME_Cold] ======== SERVER MISSION START ========
   [DME_Cold] Server: Config loaded successfully
   [DME_Cold] Server: Sending config to player: PlayerName
   ```

2. Check **client** logs for config sync and overlay init:
   ```
   [DME_Cold] ======== CLIENT MISSION START ========
   [DME_Cold] Client: Waiting for config from server...
   [DME_Cold] Client: Received config sync RPC
   [DME_Cold] Client: Config applied successfully!
   [DME_Cold] ======== MISSION OVERLAY INIT ========
   [DME_Cold] ImageWidget created - Screen size: 1920x1080
   ```

3. Enable debug mode in server config: `"DebugEnabled": true`

4. Force a frost level for testing: `"DebugForceFrostLevel": 5`

### Common Issues

| Issue | Solution |
|-------|----------|
| "Waiting for config" forever | Check server is sending RPC, check mod is in both server AND client mods |
| No frost at normal temp | Working as intended! Frost only shows when HeatComfort < 0 |
| Frost shows when warm | Check HeatComfort values in debug log, may need to adjust thresholds |
| Always same frost image | Make sure Freeze1-7.paa exist in GUI/textures/ |
| Overlay blocks UI | Check Z-order in CreateOverlay(), should be negative |
| Config changes not applied | Admins press Numpad 8 (reload + broadcast), otherwise restart the server |
| Frost/toxic only with DebugForce | Server and client must run the same mod version (stat sync RPC) |
| Transitions too fast/slow | Adjust TransitionSpeed in config (lower = slower) |

## Credits

- **Author**: DeadmansEcho
- **Original Concept**: sFramework-based prototype
- **Rewrite**: Native DayZ widget system (2025)

## License

MIT License - See LICENSE file for details

## Version History

- **v2.5** (2026-09-29): DayZ 1.29/1.30 update (`g_Game`, `JsonFileLoader.LoadFile/SaveFile`), server→client stat sync (frost/toxic never reacted in multiplayer, HeatComfort range fixed to -1..1), keybind registered via `inputs=`, valid debug layout, admin check accepts Steam64
- **v2.4** (2025-12-15): Added Blood loss overlay (7 levels, non-linear thresholds)
- **v2.3** (2025-12-15): Added Toxicity overlay for toxic zones (7 levels)
- **v2.2** (2025-12-15): Fixed HeatComfort/HeatBuffer system, smooth float-based frost transitions
- **v2.1** (2025-12-11): Added server→client config sync via RPC, 7 frost image levels
- **v2.0** (2025-12-10): Complete rewrite using native DayZ widgets, no sFramework dependency
- **v1.0** (original): sFramework-based version (archived)
