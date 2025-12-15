# DME Cold Frost Overlay - Installation & Setup Guide

## Overview
DME_Cold is a DayZ mod that displays a progressive frost overlay effect on the player's screen based on their body temperature. The overlay consists of 7 levels of frost images that appear as the player gets colder.

**Version: 2.0 - Native DayZ Implementation**
- Removed sFramework dependency
- Uses native DayZ widget system for rendering
- No external mod dependencies required

## Features
- **7-Level Frost Progression**: From subtle frost (Level 1) to heavy frost (Level 7)
- **Temperature-Based**: Responds to player heat comfort stat
- **Smooth Transitions**: Alpha blending for natural fade-in/fade-out
- **Configurable**: JSON-based configuration system
- **Debug Mode**: Test frost levels without getting cold
- **No Dependencies**: Works standalone without sFramework or sVisual

## Installation

### 1. File Structure
Place the mod in your DayZ server mods directory:
```
@DME_Cold/
├── addons/
│   └── DME_Cold/
│       ├── config.cpp
│       ├── mod.cpp
│       └── Scripts/
│           ├── 3_Game/
│           ├── 4_World/
│           └── 5_Mission/
└── keys/
    └── DME.bikey (if using signed version)
```

### 2. Add to Server Mods
In your server startup command or CFG file:
```
-mod=@DME_Cold;
```

### 3. Configuration File
The mod automatically creates a configuration file at:
```
profiles/DeadmansEcho/DME_Cold_Config.json
```

**Default Configuration:**
```json
{
    "ConfigVersion": 1,
    "EnableFrostEffect": true,
    "DebugEnabled": true,
    "DebugForceFrostLevel": 7,
    "DebugLogTemperature": true,
    "FrostLevel0Threshold": 35.0,
    "FrostLevel1Threshold": 34.0,
    "FrostLevel2Threshold": 33.0,
    "FrostLevel3Threshold": 32.0,
    "FrostLevel4Threshold": 31.0,
    "FrostLevel5Threshold": 30.0,
    "FrostLevel6Threshold": 29.0,
    "FrostLevel7Threshold": 28.0,
    "TransitionSpeed": 2.0,
    "MinAlpha": 0.1,
    "MaxAlpha": 0.85
}
```

## Configuration Options

### Enable/Disable
- `EnableFrostEffect`: Set to `true` to enable, `false` to disable entirely

### Debug Mode
- `DebugEnabled`: Enable debug logging to console
- `DebugForceFrostLevel`: Force a specific frost level (1-7) for testing. Set to 0 to disable forcing
- `DebugLogTemperature`: Log temperature values every 2 seconds

### Visual Settings
- `TransitionSpeed`: How fast frost appears/disappears (higher = faster, default 2.0)
- `MinAlpha`: Minimum transparency when frost starts appearing (0.1 = 10%)
- `MaxAlpha`: Maximum transparency of the overlay (0.85 = 85%)

### Temperature Thresholds
- `FrostLevelXThreshold`: Temperature threshold for each frost level (in °C)
- Higher thresholds = frost appears at warmer temperatures
- Lower thresholds = frost only appears when very cold
- Temperatures mapped from -1 (freezing) to +1 (overheating) heat comfort stat, scaled to 28-38°C range

**Default Thresholds:**
- Level 0 (None): > 35°C
- Level 1: ≥ 34°C
- Level 2: ≥ 33°C
- Level 3: ≥ 32°C
- Level 4: ≥ 31°C
- Level 5: ≥ 30°C
- Level 6: ≥ 29°C
- Level 7 (Max): < 28°C

## Testing

### With Debug Mode Enabled
1. Ensure `DebugEnabled: true` in config
2. Set `DebugForceFrostLevel` to test value (1-7)
3. Start the server and join as client
4. Frost overlay should appear at full opacity showing that level

### Without Debug Mode
1. Set `EnableFrostEffect: true`
2. Play normally - frost overlay appears automatically when you get cold
3. Wear clothing, light fires, or seek shelter to warm up and see it fade away

### Monitoring
- Check server console for `[DME_Cold]` messages
- With debug enabled, you'll see temperature and frost level every 2 seconds

## Texture Assets

The mod uses 7 frost overlay images:
```
DME_Cold/GUI/textures/
├── Freeze1.paa  (Level 1 - Light frost)
├── Freeze2.paa  (Level 2)
├── Freeze3.paa  (Level 3)
├── Freeze4.paa  (Level 4)
├── Freeze5.paa  (Level 5)
├── Freeze6.paa  (Level 6)
└── Freeze7.paa  (Level 7 - Heavy frost)
```

All images should be in `.paa` format with alpha channel for transparency.

## Troubleshooting

### Overlay Not Appearing
1. **Check config file exists**: 
   - `profiles/DeadmansEcho/DME_Cold_Config.json`
   - If missing, server should create it on startup

2. **Check config values**:
   - `EnableFrostEffect` must be `true`
   - `DebugForceFrostLevel` can be set 1-7 to test

3. **Check debug output**:
   - Set `DebugEnabled: true`
   - Look for `[DME_Cold]` messages in server console
   - Should see initialization and temperature updates

4. **Verify texture files**:
   - Check `DME_Cold/GUI/textures/Freeze1.paa` through `Freeze7.paa` exist
   - Files must be in `.paa` format

### Overlay Disappears Too Quickly
- Increase `TransitionSpeed` value (default 2.0)
- Higher values = slower fade out

### Overlay Too Transparent/Opaque
- Adjust `MinAlpha` (when frost starts) or `MaxAlpha` (maximum frost)
- Valid range: 0.0 (invisible) to 1.0 (fully opaque)

### Frost Appears at Wrong Temperatures
- Adjust the `FrostLevelXThreshold` values
- Lower thresholds = only appears when colder
- Higher thresholds = appears at warmer temperatures

## Console Commands

### Via Chat (if admin console enabled)
```
evaluateScript DME_ColdSettings.IsEnabled()
evaluateScript DME_ColdSettings.GetForcedFrostLevel()
```

## Performance Impact
- Minimal CPU usage (simple alpha blending)
- Single fullscreen texture rendering
- Negligible impact on frame rate

## Known Limitations
- Requires client-side mod installation if clientside mods allowed
- Configuration changes require server restart to take effect
- Frost level updates every frame for smooth transitions

## Version History

### v2.0 - Native DayZ Implementation
- Removed sFramework dependency
- Now uses native DayZ widget system
- Simplified architecture
- Better compatibility with standalone servers

### v1.0 - Initial Release
- Based on sFramework SCameraOverlay
- 7-level frost progression
- JSON configuration system

## Support
For issues or questions about this mod, ensure:
1. Config file is properly formatted JSON
2. Texture files exist and are valid `.paa` files
3. DayZ server is running latest version
4. All required texture assets are included

## License
Mod developed by DeadmansEcho for DayZ.
