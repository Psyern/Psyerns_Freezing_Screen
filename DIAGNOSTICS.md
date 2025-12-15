# DME Cold Frost Overlay - Comprehensive Diagnostics Guide

## Overview
The frost overlay system now includes extensive logging at every stage of initialization and operation. This document explains what to look for in the server/client logs.

## Expected Log Output Sequence

### 1. Mission Start Phase
```
[DME_Cold] ======== MISSION START ========
[DME_Cold] IsClient: true/false
[DME_Cold] IsMultiplayer: true/false
[DME_Cold] Initializing configuration...
[DME_Cold] ======== LOADING CONFIG ========
[DME_Cold] Attempting to load config from: [path to config file]
[DME_Cold] Config file not found, creating default... OR Configuration loaded from: [path]
[DME_Cold] Config values:
[DME_Cold]   EnableFrostEffect: 1 (should be 1 for true)
[DME_Cold]   DebugEnabled: 1 (should be 1 for true)
[DME_Cold]   DebugForceFrostLevel: 5 (should be 5 for testing)
[DME_Cold]   DebugLogTemperature: 1 (should be 1 for true)
[DME_Cold] DEBUG MODE ENABLED
[DME_Cold] Config instance created successfully
[DME_Cold] Config debug enabled: true
[DME_Cold] Config frost effect enabled: true
[DME_Cold] Running on client side
[DME_Cold] Manager instance created successfully
[DME_Cold] Waiting for player to spawn...
[DME_Cold] ======== MISSION START COMPLETE ========
```

### 2. Player Spawn Detection
Once the player spawns and enters the game world:
```
[DME_Cold] Player found: [player name] - Activating frost overlay
[DME_Cold] Activation call complete
```

### 3. Overlay Activation
```
[DME_Cold] ======== ACTIVATING FROST OVERLAY ========
[DME_Cold] Creating frost overlay instance...
[DME_Cold] Frost overlay instance created successfully
[DME_Cold] Overlay object reference: [memory address]
[DME_Cold] Getting SCameraOverlaysManager instance...
[DME_Cold] SCameraOverlaysManager found successfully
[DME_Cold] Calling activate() method...
[DME_Cold] activate() method completed
[DME_Cold] ======== Frost overlay activated successfully ========
```

### 4. Overlay Initialization (onInit)
```
[DME_Cold] ======== SCameraOverlay::onInit() CALLED ========
[DME_Cold] Overlay object: [memory address]
[DME_Cold] Loaded 7 frost images
[DME_Cold] Set initial image: DME_Cold/GUI/textures/Freeze1.paa
[DME_Cold] Set fullscreen position (0, 0) and size (1, 1)
[DME_Cold] Set priority to 100
[DME_Cold] ======== Frost overlay initialized successfully ========
```

### 5. Frame Animation Loop (onAnimate)
With debug enabled, every 2 seconds you should see:
```
[DME_Cold DEBUG] =====================
[DME_Cold DEBUG] Heat Comfort: [value between -1 and 1]
[DME_Cold DEBUG] Effective Temp: [value between 28 and 38]C
[DME_Cold DEBUG] Frost Level: [0-7]/7
[DME_Cold DEBUG] Target Level: [0-7]
[DME_Cold DEBUG] Alpha: [0.0 to 0.85]
[DME_Cold DEBUG] Target Alpha: [0.0 to 0.85]
[DME_Cold DEBUG] Forced Level: 5
[DME_Cold DEBUG] =====================
```

## Troubleshooting Guide

### Issue: No [DME_Cold] messages appear at all
**Root Cause:** Mod scripts not loading or mission hook not being called

**Check:**
1. Config file should be at: `profiles/DeadmansEcho/DME_Cold_Config.json`
2. Look for any script compilation errors in the server startup
3. Verify sFramework and sVisual are active mods

### Issue: Mission Start logs appear, but not "Player found"
**Root Cause:** Player is not spawning or method never detects player

**Check:**
1. Verify player actually spawned in-game
2. GetGame().GetPlayer() might be returning null
3. IsActive() check might be preventing second activation attempt

### Issue: Player found but no Overlay Activation logs
**Root Cause:** m_Manager is null or Activate() is not being called

**Check:**
1. Verify "Manager instance created successfully" appears in Mission Start phase
2. OnUpdate might not be running
3. IsActive() might already be true

### Issue: Overlay Activation logs appear but no onInit logs
**Root Cause:** SCameraOverlaysManager.activate() might not be calling onInit

**Check:**
1. Verify "SCameraOverlaysManager found successfully" appears
2. sFramework's manager might have different method name or behavior
3. overlay object creation succeeded (check "Frost overlay instance created successfully")

### Issue: onInit logs appear but overlay not visible on screen
**Root Cause:** Image files not found, priority issue, or alpha always 0

**Check:**
1. Verify texture files exist: `DME_Cold/GUI/textures/Freeze1.paa` through `Freeze7.paa`
2. Check onAnimate debug output - if Target Alpha stays at 0.0, frost level calculation is wrong
3. If Frost Level stays at 0, either heat comfort is high or thresholds are wrong
4. Try forcing a different frost level - Forced Level should show 5 in debug output

## Config File Location
**Path:** `C:\Users\Administrator\AppData\Local\DayZ\profiles\DeadmansEcho\DME_Cold_Config.json`

**Expected Content:**
```json
{
  "EnableFrostEffect": true,
  "DebugEnabled": true,
  "DebugForceFrostLevel": 5,
  "DebugLogTemperature": true,
  "TransitionSpeed": 2.0,
  "MinAlpha": 0.1,
  "MaxAlpha": 0.85,
  "FrostLevelThresholds": [35.0, 34.0, 33.0, 32.0, 31.0, 30.0, 29.0, 28.0]
}
```

## Where to Find Logs

**DayZ Server Console:**
- Typically shown in server startup output
- May need to enable logging if running in background

**DayZ Client Console:**
- In-game: Press "`" (backtick) key to open console
- Look for [DME_Cold] messages in the console output

**Log Files:**
- Server: `[DayZ Server Directory]/logs/` or similar
- Client: `%AppData%\Local\DayZ\logs\` or game directory

## Testing with Forced Frost Level

With `DebugForceFrostLevel: 5`, the overlay should:
1. Always show Frost Level 5 in debug output
2. Target Alpha should be around 0.475 (midway between min and max)
3. Overlay should display Freeze5.paa texture
4. Visible effect should be obvious after 2-3 seconds of fading in

To change the forced level, edit the config JSON and set `DebugForceFrostLevel` to a value between 1 and 7.

## Performance Notes
- Debug logging every 2 seconds is minimal overhead
- Set `DebugEnabled: false` in production for no console spam
- Overlay rendering should be negligible performance impact (single fullscreen image with alpha blend)
