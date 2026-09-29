/**
 * DME_ColdFrostOverlay
 * A native DayZ frost overlay that displays based on player temperature.
 * Creates a fullscreen white overlay with variable alpha based on cold level.
 */

class DME_ColdFrostOverlay
{
    protected Widget m_LayoutRoot;
    protected ImageWidget m_FrostImage;
    protected float m_CurrentAlpha;
    protected float m_TargetAlpha;
    protected float m_CurrentFrostLevelFloat;  // Smooth float value for transitions
    protected int m_CurrentImageLevel;  // Track which image is currently loaded
    protected float m_DebugLogTimer;
    protected const float DEBUG_LOG_INTERVAL = 2.0;
    protected bool m_IsInitialized;
    
    // Debug UI elements
    protected Widget m_DebugWidget;
    protected TextWidget m_DebugText;
    protected float m_LastHeatComfort;
    protected float m_LastEffectiveTemp;
    protected int m_LastHeatStage;
    protected float m_LastHeatBufferRaw;
    
    void DME_ColdFrostOverlay()
    {
        Print("[DME_Cold] DME_ColdFrostOverlay constructor called");
        m_IsInitialized = false;
        m_CurrentAlpha = 0.0;
        m_TargetAlpha = 0.0;
        m_CurrentFrostLevelFloat = 0.0;
        m_CurrentImageLevel = 0;
        m_DebugLogTimer = 0.0;
        m_LastHeatComfort = 0.0;
        m_LastEffectiveTemp = 35.0;
        m_LastHeatStage = 2;
        m_LastHeatBufferRaw = -1.0;
    }
    
    void ~DME_ColdFrostOverlay()
    {
        Print("[DME_Cold] Cleaning up frost overlay");
        DestroyDebugWidget();
        DestroyOverlay();
    }
    
    void DestroyDebugWidget()
    {
        if (m_DebugWidget)
        {
            m_DebugWidget.Unlink();
            m_DebugWidget = null;
        }
        m_DebugText = null;
    }
    
    void DestroyOverlay()
    {
        if (m_LayoutRoot)
        {
            m_LayoutRoot.Unlink();
            m_LayoutRoot = null;
        }
        m_FrostImage = null;
        m_IsInitialized = false;
    }
    
    void CreateDebugWidget()
    {
        DME_ColdConfig config = DME_ColdConfig.GetInstance();
        if (!config || !config.DebugEnabled)
            return;
        
        if (m_DebugWidget)
            return; // Already created
        
        if (!g_Game)
            return;
        
        WorkspaceWidget workspace = g_Game.GetWorkspace();
        if (!workspace)
            return;
        
        // Try to load the debug layout
        m_DebugWidget = workspace.CreateWidgets("DME_Cold/GUI/layouts/debug_overlay.layout");
        
        if (m_DebugWidget)
        {
            m_DebugText = RichTextWidget.Cast(m_DebugWidget.FindAnyWidget("DebugText"));
            if (m_DebugText)
            {
                m_DebugText.SetText("DME_Cold Debug Loading...");
                Print("[DME_Cold] Debug widget created from layout successfully");
                return;
            }
        }
        
        // Fallback: Create simple text widget directly
        Print("[DME_Cold] Layout failed, creating simple TextWidget");
        int flags = WidgetFlags.VISIBLE | WidgetFlags.IGNOREPOINTER;
        Widget textWidget = workspace.CreateWidget(TextWidgetTypeID, 0.01, 0.01, 0.5, 0.03, flags, ARGB(255, 0, 255, 0), 100);
        
        if (textWidget)
        {
            m_DebugWidget = textWidget;
            m_DebugText = TextWidget.Cast(textWidget);
            if (m_DebugText)
            {
                m_DebugText.SetColor(ARGB(255, 0, 255, 0));
                m_DebugText.SetText("DME_Cold Debug");
                Print("[DME_Cold] Fallback debug widget created");
            }
        }
        else
        {
            Print("[DME_Cold] ERROR: Failed to create any debug widget");
        }
    }
    
    void UpdateDebugDisplay(float heatComfort, int heatStage, float heatBufferRaw = -1.0, float heatComfortRaw = -999.0)
    {
        if (!m_DebugText)
        {
            // Try to create debug widget if debug is enabled
            CreateDebugWidget();
            if (!m_DebugText)
                return;
        }
        
        DME_ColdConfig config = DME_ColdConfig.GetInstance();
        if (!config || !config.DebugEnabled)
        {
            // Hide debug if disabled
            if (m_DebugWidget)
                m_DebugWidget.Show(false);
            return;
        }
        
        m_DebugWidget.Show(true);
        
        // Store values
        m_LastHeatComfort = heatComfort;
        m_LastHeatStage = heatStage;
        m_LastEffectiveTemp = CalculateEffectiveTemperature(heatComfort);
        m_LastHeatBufferRaw = heatBufferRaw;
        
        // Build compact single-line debug text showing BOTH stats
        // HC = HeatComfort (the real one!), HB = HeatBuffer
        string debugText = "[DME] HC:" + FormatFloat(heatComfortRaw, 2);
        debugText += " HB:" + FormatFloat(m_LastHeatBufferRaw, 1);
        debugText += " Tmp:" + FormatFloat(m_LastEffectiveTemp, 1);
        debugText += " Fst:" + FormatFloat(m_CurrentFrostLevelFloat, 1);
        debugText += " A:" + FormatFloat(m_CurrentAlpha, 2);
        
        m_DebugText.SetText(debugText);
    }

    bool CreateOverlay()
    {
        Print("[DME_Cold] CreateOverlay called");
        
        if (m_IsInitialized)
        {
            Print("[DME_Cold] Overlay already initialized");
            return true;
        }
        
        // Get the workspace
        if (!g_Game)
            return false;
        
        WorkspaceWidget workspace = g_Game.GetWorkspace();
        if (!workspace)
        {
            Print("[DME_Cold] ERROR: No workspace available");
            return false;
        }
        
        Print("[DME_Cold] Workspace found, creating widgets manually...");
        
        // Create image widget directly on the workspace
        // IGNOREPOINTER = don't block mouse clicks
        // BLEND = enable alpha blending  
        // Use negative z-order to be behind menus but in front of game
        int flags = WidgetFlags.VISIBLE | WidgetFlags.IGNOREPOINTER | WidgetFlags.BLEND;
        
        Widget imgWidget = workspace.CreateWidget(ImageWidgetTypeID, 0, 0, 1.0, 1.0, flags, ARGB(255, 255, 255, 255), -10);
        
        if (!imgWidget)
        {
            Print("[DME_Cold] ERROR: Failed to create image widget");
            return false;
        }
        
        m_LayoutRoot = imgWidget;
        m_FrostImage = ImageWidget.Cast(imgWidget);
        
        if (!m_FrostImage)
        {
            Print("[DME_Cold] ERROR: Failed to cast to ImageWidget");
            DestroyOverlay();
            return false;
        }
        
        // Use SetScreenSize and SetScreenPos for proper screen coverage
        // This should stretch the image to fill the screen
        int screenW;
        int screenH;
        GetScreenSize(screenW, screenH);
        
        m_FrostImage.SetScreenSize(screenW, screenH);
        m_FrostImage.SetScreenPos(0, 0);
        
        Print("[DME_Cold] ImageWidget created - Screen size: " + screenW.ToString() + "x" + screenH.ToString());
        
        // Load the frost texture
        bool textureLoaded = m_FrostImage.LoadImageFile(0, "DME_Cold/GUI/textures/Freeze1.paa");
        if (!textureLoaded)
        {
            Print("[DME_Cold] Could not load Freeze1.paa, trying built-in texture...");
            // Try a different approach - use built-in texture
            textureLoaded = m_FrostImage.LoadImageFile(0, "set:dayz_gui image:white");
        }
        
        if (textureLoaded)
        {
            Print("[DME_Cold] Texture loaded successfully");
            // Set image to STRETCH mode so it fills the entire widget
            m_FrostImage.SetFlags(m_FrostImage.GetFlags() | WidgetFlags.STRETCH);
        }
        else
        {
            Print("[DME_Cold] WARNING: No texture loaded, overlay may not be visible");
        }
        
        // Set color with alpha for initial transparency
        // Use SetColor to control the alpha - ARGB format
        DME_ColdConfig config = DME_ColdConfig.GetInstance();
        if (config && config.DebugEnabled && config.DebugForceFrostLevel > 0)
        {
            // Start with visible alpha for debugging (128 = 50%)
            m_FrostImage.SetColor(ARGB(128, 255, 255, 255));
            m_CurrentAlpha = 0.5;
            Print("[DME_Cold] Debug mode: Starting with alpha 0.5");
        }
        else
        {
            m_FrostImage.SetColor(ARGB(0, 255, 255, 255));
            m_CurrentAlpha = 0.0;
        }
        m_FrostImage.Show(true);
        
        m_IsInitialized = true;
        Print("[DME_Cold] Overlay initialization complete!");
        
        return true;
    }
    
    // Legacy method - now uses CreateOverlay internally
    void SetFrostImageWidget(Widget frostWidget)
    {
        CreateOverlay();
    }
    
    void Update(float deltaTime, float playerHeatComfort, int heatStage = 2, float heatBufferRaw = -1.0, float heatComfortRaw = -999.0)
    {
        if (!m_IsInitialized || !m_FrostImage)
            return;
        
        // Get config
        DME_ColdConfig config = DME_ColdConfig.GetInstance();
        if (!config)
            return;
        
        // Hide the debug widget when debug was disabled (e.g. by an admin config reload)
        if (!config.DebugEnabled && m_DebugWidget)
            m_DebugWidget.Show(false);
        
        // Check if effect is enabled
        if (!config.EnableFrostEffect)
        {
            if (m_CurrentAlpha > 0)
            {
                m_CurrentAlpha = 0;
                m_CurrentFrostLevelFloat = 0;
                ApplyAlpha();
            }
            return;
        }
        
        // Calculate target frost level (as float for smooth transitions)
        float targetFrostLevel = CalculateTargetFrostLevel(playerHeatComfort, config);
        
        // Smoothly interpolate frost level using TransitionSpeed
        float diff = targetFrostLevel - m_CurrentFrostLevelFloat;
        m_CurrentFrostLevelFloat += diff * deltaTime * config.TransitionSpeed;
        
        // Clamp to valid range
        m_CurrentFrostLevelFloat = Math.Clamp(m_CurrentFrostLevelFloat, 0.0, 7.0);
        
        // Update frost image based on rounded level
        int roundedFrostLevel = Math.Round(m_CurrentFrostLevelFloat);
        UpdateFrostImage(roundedFrostLevel);
        
        // Calculate alpha directly from the float frost level (smooth!)
        UpdateAlphaFromFrostLevel(config);
        
        // Apply alpha to widget
        ApplyAlpha();
        
        // Update debug display (if enabled)
        if (config.DebugEnabled)
        {
            UpdateDebugDisplay(playerHeatComfort, heatStage, heatBufferRaw, heatComfortRaw);
        }
        
        // Debug logging to file
        if (config.DebugEnabled && config.DebugLogTemperature)
        {
            m_DebugLogTimer += deltaTime;
            if (m_DebugLogTimer >= DEBUG_LOG_INTERVAL)
            {
                float effectiveTemp = CalculateEffectiveTemperature(playerHeatComfort);
                Print("[DME_Cold] HeatComfort(raw): " + heatComfortRaw.ToString() + " HeatBuffer: " + heatBufferRaw.ToString() + " Used: " + playerHeatComfort.ToString() + " Temp: " + effectiveTemp.ToString() + " FrostFloat: " + m_CurrentFrostLevelFloat.ToString() + " Alpha: " + m_CurrentAlpha.ToString());
                m_DebugLogTimer = 0.0;
            }
        }
    }
    
    protected float CalculateTargetFrostLevel(float heatComfort, DME_ColdConfig config)
    {
        // Debug forced frost level
        if (config && config.DebugEnabled && config.DebugForceFrostLevel > 0)
        {
            return config.DebugForceFrostLevel;
        }
        
        // Calculate effective temperature from heat comfort
        float effectiveTemp = CalculateEffectiveTemperature(heatComfort);
        
        // Calculate frost level as float based on temperature
        // This allows smooth transitions between levels
        return CalculateFrostLevelFloat(effectiveTemp, config);
    }
    
    protected float CalculateFrostLevelFloat(float temperature, DME_ColdConfig config)
    {
        // Return float frost level for smooth interpolation
        // Temperature 35+ = 0, Temperature 28- = 7
        if (temperature >= config.FrostLevel0Threshold) return 0.0;
        if (temperature <= config.FrostLevel7Threshold) return 7.0;
        
        // Linear interpolation between thresholds
        // Map temperature range (28-35) to frost level (7-0)
        float tempRange = config.FrostLevel0Threshold - config.FrostLevel7Threshold; // 35 - 28 = 7
        float tempOffset = config.FrostLevel0Threshold - temperature; // How far below 35
        float frostLevel = (tempOffset / tempRange) * 7.0;
        
        return Math.Clamp(frostLevel, 0.0, 7.0);
    }
    
    protected void UpdateFrostImage(int targetImageLevel)
    {
        // Only update if image level changed and is valid
        if (targetImageLevel == m_CurrentImageLevel)
            return;
        
        if (targetImageLevel <= 0)
        {
            // No frost, keep image but set alpha to 0 (handled elsewhere)
            m_CurrentImageLevel = 0;
            return;
        }
        
        // Clamp to valid image range (1-7)
        int imageLevel = Math.Clamp(targetImageLevel, 1, 7);
        
        // Load the appropriate frost image
        string imagePath = "DME_Cold/GUI/textures/Freeze" + imageLevel.ToString() + ".paa";
        bool loaded = m_FrostImage.LoadImageFile(0, imagePath);
        
        if (loaded)
        {
            m_CurrentImageLevel = targetImageLevel;
            Print("[DME_Cold] Switched to frost image: " + imagePath);
        }
        else
        {
            Print("[DME_Cold] WARNING: Failed to load " + imagePath);
        }
    }
    
    protected void UpdateAlphaFromFrostLevel(DME_ColdConfig config)
    {
        if (m_CurrentFrostLevelFloat <= 0.0)
        {
            m_CurrentAlpha = 0.0;
        }
        else
        {
            // Map frost level (0-7) directly to alpha range
            // This gives smooth alpha transitions as frost level changes
            float alphaRange = config.MaxAlpha - config.MinAlpha;
            float levelFactor = (m_CurrentFrostLevelFloat - 1.0) / 6.0; // 0.0 to 1.0 for levels 1-7
            levelFactor = Math.Clamp(levelFactor, 0.0, 1.0);
            m_CurrentAlpha = config.MinAlpha + (alphaRange * levelFactor);
            
            // Ensure minimum alpha when there's any frost
            if (m_CurrentFrostLevelFloat > 0.0 && m_CurrentAlpha < config.MinAlpha)
                m_CurrentAlpha = config.MinAlpha;
        }
    }
    
    protected void ApplyAlpha()
    {
        if (!m_FrostImage)
            return;
        
        // Convert float alpha (0.0-1.0) to int (0-255)
        int alphaInt = Math.Round(m_CurrentAlpha * 255.0);
        
        // Use SetColor with ARGB - this is more reliable than SetAlpha
        m_FrostImage.SetColor(ARGB(alphaInt, 255, 255, 255));
    }
    
    float CalculateEffectiveTemperature(float heatComfort)
    {
        // heatComfort ranges from -1.0 (very cold) to 1.0 (very hot)
        // DayZ has 5 Heat Stages (0-4), mapped to heatComfort:
        //   Stage 0 = -1.0 (freezing)
        //   Stage 1 = -0.5 (cold)
        //   Stage 2 =  0.0 (normal)
        //   Stage 3 =  0.5 (warm)
        //   Stage 4 =  1.0 (hot)
        //
        // We need to map the COLD range (-1.0 to 0.0) to all 7 frost levels
        // Map: -1.0 to 0.0 => 28°C to 35°C (7 degree range for 7 levels)
        //       0.0 to 1.0 => 35°C to 42°C (no frost, warm)
        //
        // This gives us:
        // -1.000 (Stage 0)     = 28.0°C = Frost Level 7
        // -0.857               = 29.0°C = Frost Level 6
        // -0.714               = 30.0°C = Frost Level 5
        // -0.571               = 31.0°C = Frost Level 4
        // -0.429               = 32.0°C = Frost Level 3
        // -0.286               = 33.0°C = Frost Level 2
        // -0.143               = 34.0°C = Frost Level 1
        //  0.000 (Stage 2)     = 35.0°C = Frost Level 0 (no frost)
        return 35.0 + (heatComfort * 7.0);
    }
    
    int CalculateFrostLevel(float temperature)
    {
        // Use config thresholds
        DME_ColdConfig config = DME_ColdConfig.GetInstance();
        if (!config)
        {
            // Fallback if no config
            if (temperature >= 35.0) return 0;
            if (temperature >= 34.0) return 1;
            if (temperature >= 33.0) return 2;
            if (temperature >= 32.0) return 3;
            if (temperature >= 31.0) return 4;
            if (temperature >= 30.0) return 5;
            if (temperature >= 29.0) return 6;
            return 7;
        }
        
        // Use configured thresholds
        if (temperature >= config.FrostLevel0Threshold) return 0;
        if (temperature >= config.FrostLevel1Threshold) return 1;
        if (temperature >= config.FrostLevel2Threshold) return 2;
        if (temperature >= config.FrostLevel3Threshold) return 3;
        if (temperature >= config.FrostLevel4Threshold) return 4;
        if (temperature >= config.FrostLevel5Threshold) return 5;
        if (temperature >= config.FrostLevel6Threshold) return 6;
        return 7;
    }
    
    int GetCurrentFrostLevel()
    {
        return Math.Round(m_CurrentFrostLevelFloat);
    }
    
    float GetCurrentFrostLevelFloat()
    {
        return m_CurrentFrostLevelFloat;
    }
    
    float GetCurrentAlpha()
    {
        return m_CurrentAlpha;
    }
    
    bool IsInitialized()
    {
        return m_IsInitialized;
    }
    
    // Helper function to format float with specific decimal places
    protected string FormatFloat(float value, int decimals)
    {
        string result = value.ToString();
        int dotPos = result.IndexOf(".");
        
        if (dotPos == -1)
        {
            // No decimal point, add one
            if (decimals > 0)
            {
                result += ".";
                for (int i = 0; i < decimals; i++)
                    result += "0";
            }
        }
        else
        {
            // Has decimal point, trim or pad
            int currentDecimals = result.Length() - dotPos - 1;
            if (currentDecimals > decimals)
            {
                result = result.Substring(0, dotPos + 1 + decimals);
            }
        }
        return result;
    }
}
