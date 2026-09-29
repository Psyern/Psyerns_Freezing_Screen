/**
 * DME_ToxicOverlay
 * A native DayZ toxic overlay that displays based on player toxicity level.
 * Creates a fullscreen green overlay with variable alpha based on toxicity.
 */

class DME_ToxicOverlay
{
    protected Widget m_LayoutRoot;
    protected ImageWidget m_ToxicImage;
    protected float m_CurrentAlpha;
    protected float m_CurrentToxicLevelFloat;  // Smooth float value for transitions
    protected int m_CurrentImageLevel;  // Track which image is currently loaded
    protected float m_DebugLogTimer;
    protected const float DEBUG_LOG_INTERVAL = 2.0;
    protected bool m_IsInitialized;
    
    void DME_ToxicOverlay()
    {
        Print("[DME_Cold] DME_ToxicOverlay constructor called");
        m_IsInitialized = false;
        m_CurrentAlpha = 0.0;
        m_CurrentToxicLevelFloat = 0.0;
        m_CurrentImageLevel = 0;
        m_DebugLogTimer = 0.0;
    }
    
    void ~DME_ToxicOverlay()
    {
        Print("[DME_Cold] Cleaning up toxic overlay");
        DestroyOverlay();
    }
    
    void DestroyOverlay()
    {
        if (m_LayoutRoot)
        {
            m_LayoutRoot.Unlink();
            m_LayoutRoot = null;
        }
        m_ToxicImage = null;
        m_IsInitialized = false;
    }
    
    bool CreateOverlay()
    {
        Print("[DME_Cold] ToxicOverlay CreateOverlay called");
        
        if (m_IsInitialized)
        {
            Print("[DME_Cold] Toxic overlay already initialized");
            return true;
        }
        
        // Get the workspace
        if (!g_Game)
            return false;
        
        WorkspaceWidget workspace = g_Game.GetWorkspace();
        if (!workspace)
        {
            Print("[DME_Cold] ERROR: No workspace available for toxic overlay");
            return false;
        }
        
        Print("[DME_Cold] Creating toxic overlay widget...");
        
        // Create image widget directly on the workspace
        // Use z-order -9 to be slightly above frost overlay (-10) but still behind UI
        int flags = WidgetFlags.VISIBLE | WidgetFlags.IGNOREPOINTER | WidgetFlags.BLEND;
        
        Widget imgWidget = workspace.CreateWidget(ImageWidgetTypeID, 0, 0, 1.0, 1.0, flags, ARGB(255, 255, 255, 255), -9);
        
        if (!imgWidget)
        {
            Print("[DME_Cold] ERROR: Failed to create toxic image widget");
            return false;
        }
        
        m_LayoutRoot = imgWidget;
        m_ToxicImage = ImageWidget.Cast(imgWidget);
        
        if (!m_ToxicImage)
        {
            Print("[DME_Cold] ERROR: Failed to cast to ImageWidget for toxic");
            DestroyOverlay();
            return false;
        }
        
        // Set screen size
        int screenW;
        int screenH;
        GetScreenSize(screenW, screenH);
        
        m_ToxicImage.SetScreenSize(screenW, screenH);
        m_ToxicImage.SetScreenPos(0, 0);
        
        Print("[DME_Cold] Toxic ImageWidget created - Screen size: " + screenW.ToString() + "x" + screenH.ToString());
        
        // Load the toxic texture
        bool textureLoaded = m_ToxicImage.LoadImageFile(0, "DME_Cold/GUI/textures/toxic1.paa");
        if (!textureLoaded)
        {
            Print("[DME_Cold] WARNING: Could not load toxic1.paa");
        }
        else
        {
            Print("[DME_Cold] Toxic texture loaded successfully");
            m_ToxicImage.SetFlags(m_ToxicImage.GetFlags() | WidgetFlags.STRETCH);
        }
        
        // Start with invisible
        m_ToxicImage.SetColor(ARGB(0, 255, 255, 255));
        m_CurrentAlpha = 0.0;
        m_ToxicImage.Show(true);
        
        m_IsInitialized = true;
        Print("[DME_Cold] Toxic overlay initialization complete!");
        
        return true;
    }
    
    void Update(float deltaTime, float toxicityRaw)
    {
        if (!m_IsInitialized || !m_ToxicImage)
            return;
        
        // Get config
        DME_ColdConfig config = DME_ColdConfig.GetInstance();
        if (!config)
            return;
        
        // Check if effect is enabled
        if (!config.EnableToxicEffect)
        {
            if (m_CurrentAlpha > 0)
            {
                m_CurrentAlpha = 0;
                m_CurrentToxicLevelFloat = 0;
                ApplyAlpha();
            }
            return;
        }
        
        // Calculate target toxic level (as float for smooth transitions)
        float targetToxicLevel = CalculateTargetToxicLevel(toxicityRaw, config);
        
        // Smoothly interpolate toxic level using TransitionSpeed
        float diff = targetToxicLevel - m_CurrentToxicLevelFloat;
        m_CurrentToxicLevelFloat += diff * deltaTime * config.ToxicTransitionSpeed;
        
        // Clamp to valid range (0-5 for 5 toxic images)
        m_CurrentToxicLevelFloat = Math.Clamp(m_CurrentToxicLevelFloat, 0.0, 5.0);
        
        // Update toxic image based on rounded level
        int roundedToxicLevel = Math.Round(m_CurrentToxicLevelFloat);
        UpdateToxicImage(roundedToxicLevel);
        
        // Calculate alpha directly from the float toxic level (smooth!)
        UpdateAlphaFromToxicLevel(config);
        
        // Apply alpha to widget
        ApplyAlpha();
        
        // Debug logging to file
        if (config.DebugEnabled && config.DebugLogTemperature)
        {
            m_DebugLogTimer += deltaTime;
            if (m_DebugLogTimer >= DEBUG_LOG_INTERVAL)
            {
                Print("[DME_Cold] Toxicity: " + toxicityRaw.ToString() + " ToxicFloat: " + m_CurrentToxicLevelFloat.ToString() + " Image: " + m_CurrentImageLevel.ToString() + " Alpha: " + m_CurrentAlpha.ToString());
                m_DebugLogTimer = 0.0;
            }
        }
    }
    
    protected float CalculateTargetToxicLevel(float toxicityRaw, DME_ColdConfig config)
    {
        // Debug forced toxic level
        if (config && config.DebugEnabled && config.DebugForceToxicLevel > 0)
        {
            return config.DebugForceToxicLevel;
        }
        
        // Calculate toxic level as float based on toxicity percentage
        return CalculateToxicLevelFloat(toxicityRaw, config);
    }
    
    protected float CalculateToxicLevelFloat(float toxicity, DME_ColdConfig config)
    {
        // Return float toxic level for smooth interpolation
        // Toxicity 0 = 0, Toxicity 100 = 5 (5 images now)
        if (toxicity <= config.ToxicLevel0Threshold) return 0.0;
        if (toxicity >= config.ToxicLevel5Threshold) return 5.0;
        
        // Linear interpolation between thresholds
        // Map toxicity range (0-100) to toxic level (0-5)
        float toxicRange = config.ToxicLevel5Threshold - config.ToxicLevel0Threshold;
        float toxicOffset = toxicity - config.ToxicLevel0Threshold;
        float toxicLevel = (toxicOffset / toxicRange) * 5.0;
        
        return Math.Clamp(toxicLevel, 0.0, 5.0);
    }
    
    protected void UpdateToxicImage(int targetImageLevel)
    {
        // Only update if image level changed and is valid
        if (targetImageLevel == m_CurrentImageLevel)
            return;
        
        if (targetImageLevel <= 0)
        {
            // No toxic, keep image but set alpha to 0 (handled elsewhere)
            m_CurrentImageLevel = 0;
            return;
        }
        
        // Clamp to valid image range (1-5 for 5 toxic images)
        int imageLevel = Math.Clamp(targetImageLevel, 1, 5);
        
        // Load the appropriate toxic image
        string imagePath = "DME_Cold/GUI/textures/toxic" + imageLevel.ToString() + ".paa";
        bool loaded = m_ToxicImage.LoadImageFile(0, imagePath);
        
        if (loaded)
        {
            m_CurrentImageLevel = targetImageLevel;
            Print("[DME_Cold] Switched to toxic image: " + imagePath);
        }
        else
        {
            Print("[DME_Cold] WARNING: Failed to load " + imagePath);
        }
    }
    
    protected void UpdateAlphaFromToxicLevel(DME_ColdConfig config)
    {
        if (m_CurrentToxicLevelFloat <= 0.0)
        {
            m_CurrentAlpha = 0.0;
        }
        else
        {
            // Map toxic level (0-5) directly to alpha range
            float alphaRange = config.ToxicMaxAlpha - config.ToxicMinAlpha;
            float levelFactor = (m_CurrentToxicLevelFloat - 1.0) / 4.0; // 0.0 to 1.0 for levels 1-5
            levelFactor = Math.Clamp(levelFactor, 0.0, 1.0);
            m_CurrentAlpha = config.ToxicMinAlpha + (alphaRange * levelFactor);
            
            // Ensure minimum alpha when there's any toxic
            if (m_CurrentToxicLevelFloat > 0.0 && m_CurrentAlpha < config.ToxicMinAlpha)
                m_CurrentAlpha = config.ToxicMinAlpha;
        }
    }
    
    protected void ApplyAlpha()
    {
        if (!m_ToxicImage)
            return;
        
        // Convert float alpha (0.0-1.0) to int (0-255)
        int alphaInt = Math.Round(m_CurrentAlpha * 255.0);
        
        // Use SetColor with ARGB
        m_ToxicImage.SetColor(ARGB(alphaInt, 255, 255, 255));
    }
    
    int GetCurrentToxicLevel()
    {
        return Math.Round(m_CurrentToxicLevelFloat);
    }
    
    float GetCurrentToxicLevelFloat()
    {
        return m_CurrentToxicLevelFloat;
    }
    
    float GetCurrentAlpha()
    {
        return m_CurrentAlpha;
    }
    
    bool IsInitialized()
    {
        return m_IsInitialized;
    }
}
