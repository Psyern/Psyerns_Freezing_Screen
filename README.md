# DME_Cold - Frost Overlay Mod for DayZ

A DayZ mod that adds a visual frost/ice overlay effect based on the player's body temperature.

## Features

- **7 Progressive Frost Levels**: As the player gets colder, the screen progressively shows more frost
- **Smooth Transitions**: Frost fades in and out smoothly based on temperature changes
- **Temperature-Based**: Uses player's heat comfort combined with environment temperature
- **Native DayZ Widgets**: Built using native DayZ widget system (no external dependencies!)
- **Server-Controlled Config**: Configuration is stored on server and synced to clients automatically
- **Configurable**: Server admins can customize temperature thresholds

## Requirements

- DayZ Server (1.24+)
- No external mod dependencies!

## Installation

1. Copy the `DME_Cold` folder to your DayZ server's mod directory
2. Add `@DME_Cold` to your server's mod list (both server AND client mods!)
3. Restart the server

## Configuration

The configuration file is stored **on the server only** at:
```
profiles/DeadmansEcho/DME_Cold_Config.json
```

**Important:** The config is automatically synced from server to all clients when they connect. Clients do NOT need their own config file - they receive all settings from the server.

### Configuration Options

| Option | Default | Description |
|--------|---------|-------------|
| EnableFrostEffect | true | Enable/disable the frost effect |
| **DebugEnabled** | false | **Enable debug logging (shows values in log)** |
| **DebugForceFrostLevel** | 0 | **Force a specific frost level (0=off, 1-7=force)** |
| **DebugLogTemperature** | false | **Log temperature values to console** |
| FrostLevel0Threshold | 35.0 | Temperature above which no frost appears |
| FrostLevel1Threshold | 34.0 | Temperature for light frost |
| FrostLevel2Threshold | 33.0 | Temperature for level 2 frost |
| FrostLevel3Threshold | 32.0 | Temperature for level 3 frost |
| FrostLevel4Threshold | 31.0 | Temperature for level 4 frost |
| FrostLevel5Threshold | 30.0 | Temperature for level 5 frost |
| FrostLevel6Threshold | 29.0 | Temperature for level 6 frost |
| FrostLevel7Threshold | 28.0 | Temperature for maximum frost |
| TransitionSpeed | 2.0 | How fast frost appears/disappears (higher = faster) |
| MinAlpha | 0.1 | Minimum opacity when frost starts |
| MaxAlpha | 0.85 | Maximum opacity of frost overlay (0.0-1.0) |

## Debug Mode

Set `DebugEnabled` to `true` in the config to see debug output in the log every 2 seconds:
```
[DME_Cold DEBUG] =====================
[DME_Cold DEBUG] Heat Comfort: -0.5
[DME_Cold DEBUG] Effective Temp: 31.5C
[DME_Cold DEBUG] Frost Level: 4/7
[DME_Cold DEBUG] Alpha: 0.65
[DME_Cold DEBUG] Forced Level: 0
```

Set `DebugForceFrostLevel` to 1-7 to force a specific frost level for testing.

## How It Works

### Architecture

The mod uses a modular architecture across DayZ script modules:

- **3_Game Module**: Core logic (loads first on both server and client)
  - `A_DME_ColdFrostOverlay.c`: Overlay widget, frost level calculation, alpha transitions
  - `DME_ColdConfig.c`: Configuration loading (server) and RPC serialization
  - `DME_ColdRPC.c`: RPC constants for server→client communication
  
- **4_World Module**: Entity integration
  - `DME_ColdRPCHandler.c`: Modded PlayerBase that receives config sync RPC
  - `B_DME_ColdManager.c`: Singleton manager
  
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

### Heat Detection

The mod monitors the player's heat state using:
```
PlayerBase.GetHeatBufferStage() returns:
  0 = Very Cold  (-1.0 comfort)
  1 = Cold       (-0.5 comfort)
  2 = Normal     (0.0 comfort)
  3 = Hot        (0.5 comfort)
  4+ = Very Hot  (1.0 comfort)
```

### Widget System

- Searches for `ingame_hud` widget in workspace
- Falls back to workspace if HUD not found
- Sets alpha transparency (0.0 = invisible, 1.0 = opaque)
- Native DayZ widget system (no sFramework needed)

## File Structure

```
DME_Cold/
├── config.cpp                              # DayZ mod configuration
├── mod.cpp                                 # Mod metadata
├── README.md                               # This file
│
├── GUI/
│   ├── layouts/
│   │   └── frost_overlay.layout            # UI layout (unused - manual widget creation)
│   └── textures/
│       ├── Freeze1.paa                     # Frost overlay level 1 (lightest)
│       ├── Freeze2.paa                     # Frost overlay level 2
│       ├── Freeze3.paa                     # Frost overlay level 3
│       ├── Freeze4.paa                     # Frost overlay level 4
│       ├── Freeze5.paa                     # Frost overlay level 5
│       ├── Freeze6.paa                     # Frost overlay level 6
│       ├── Freeze7.paa                     # Frost overlay level 7 (heaviest)
│       └── README.txt                      # Texture info
│
└── Scripts/
    ├── 3_Game/                             # Game-level scripts (loads first)
    │   ├── A_DME_ColdFrostOverlay.c        # Core overlay widget logic
    │   ├── DME_Cold.c                      # Module registration
    │   ├── DME_ColdConfig.c                # Configuration management & RPC serialization
    │   └── DME_ColdRPC.c                   # RPC constants for server-client sync
    │
    ├── 4_World/                            # World-level scripts
    │   ├── B_DME_ColdManager.c             # Manager singleton
    │   ├── DME_Cold.c                      # Module registration
    │   ├── DME_ColdPlayerClient.c          # Player integration (stub)
    │   └── DME_ColdRPCHandler.c            # Receives config RPC from server
    │
    └── 5_Mission/                          # Mission-level scripts (loads last)
        ├── DME_Cold.c                      # Module registration
        ├── DME_ColdMissionClient.c         # Client overlay lifecycle (MissionGameplay)
        └── DME_ColdMissionServer.c         # Server config sync (MissionServer)
```

## Performance

- **Minimal Server Load**: Only sends config once per player connect
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
| No frost at normal temp | Working as intended! Frost only shows when Heat Buffer Stage < 2 |
| Always same frost image | Make sure Freeze1-7.paa exist in GUI/textures/ |
| Overlay blocks UI | Check Z-order in CreateOverlay(), should be negative |
| Config changes not applied | Restart server (config syncs on player connect) |

## Credits

- **Author**: DeadmansEcho
- **Original Concept**: sFramework-based prototype
- **Rewrite**: Native DayZ widget system (2025)

## License

MIT License - See LICENSE file for details

## Version History

- **v2.1** (2025-12-11): Added server→client config sync via RPC, 7 frost image levels
- **v2.0** (2025-12-10): Complete rewrite using native DayZ widgets, no sFramework dependency
- **v1.0** (original): sFramework-based version (archived)
