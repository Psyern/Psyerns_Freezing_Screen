/**
 * DME_BloodOverlay
 * A native DayZ blood loss overlay that displays based on player blood level.
 * Creates a fullscreen red overlay with variable alpha based on blood loss.
 * 
 * Blood thresholds (DayZ blood values: 0-5000):
 * - Level 1-3: Larger ranges (~800 each) - moderate blood loss
 * - Level 4-7: Smaller ranges (~250-700 each) - severe blood loss
 */

class DME_BloodOverlay
{
	protected Widget m_LayoutRoot;
	protected ImageWidget m_BloodImage;
	protected float m_CurrentAlpha;
	protected float m_CurrentBloodLevelFloat;  // Smooth float value for transitions
	protected int m_CurrentImageLevel;  // Track which image is currently loaded
	protected float m_DebugLogTimer;
	protected const float DEBUG_LOG_INTERVAL = 2.0;
	protected bool m_IsInitialized;

	void DME_BloodOverlay()
	{
		PsyCore_Log.Get("DME_Cold").Info("DME_BloodOverlay constructor called");
		m_IsInitialized = false;
		m_CurrentAlpha = 0.0;
		m_CurrentBloodLevelFloat = 0.0;
		m_CurrentImageLevel = 0;
		m_DebugLogTimer = 0.0;
	}

	void ~DME_BloodOverlay()
	{
		PsyCore_Log.Get("DME_Cold").Info("Cleaning up blood overlay");
		DestroyOverlay();
	}

	void DestroyOverlay()
	{
		if (m_LayoutRoot)
		{
			m_LayoutRoot.Unlink();
			m_LayoutRoot = null;
		}
		m_BloodImage = null;
		m_IsInitialized = false;
	}

	bool CreateOverlay()
	{
		PsyCore_Log.Get("DME_Cold").Info("BloodOverlay CreateOverlay called");

		if (m_IsInitialized)
		{
			PsyCore_Log.Get("DME_Cold").Info("Blood overlay already initialized");
			return true;
		}

		// Get the workspace
		if (!g_Game)
			return false;

		WorkspaceWidget workspace = g_Game.GetWorkspace();
		if (!workspace)
		{
			PsyCore_Log.Get("DME_Cold").Error("No workspace available for blood overlay");
			return false;
		}

		PsyCore_Log.Get("DME_Cold").Info("Creating blood overlay widget...");

		// Create image widget directly on the workspace
		// Use z-order -8 to be above toxic overlay (-9) but still behind UI
		int flags = WidgetFlags.VISIBLE | WidgetFlags.IGNOREPOINTER | WidgetFlags.BLEND;

		Widget imgWidget = workspace.CreateWidget(ImageWidgetTypeID, 0, 0, 1.0, 1.0, flags, ARGB(255, 255, 255, 255), -8);

		if (!imgWidget)
		{
			PsyCore_Log.Get("DME_Cold").Error("Failed to create blood image widget");
			return false;
		}

		m_LayoutRoot = imgWidget;
		m_BloodImage = ImageWidget.Cast(imgWidget);

		if (!m_BloodImage)
		{
			PsyCore_Log.Get("DME_Cold").Error("Failed to cast to ImageWidget for blood");
			DestroyOverlay();
			return false;
		}

		// Set screen size
		int screenW;
		int screenH;
		GetScreenSize(screenW, screenH);

		m_BloodImage.SetScreenSize(screenW, screenH);
		m_BloodImage.SetScreenPos(0, 0);

		PsyCore_Log.Get("DME_Cold").Info("Blood ImageWidget created - Screen size: " + screenW.ToString() + "x" + screenH.ToString());

		// Load the blood texture
		bool textureLoaded = m_BloodImage.LoadImageFile(0, "DME_Cold/GUI/textures/blood1.paa");
		if (!textureLoaded)
		{
			PsyCore_Log.Get("DME_Cold").Warn("Could not load blood1.paa");
		}
		else
		{
			PsyCore_Log.Get("DME_Cold").Info("Blood texture loaded successfully");
			m_BloodImage.SetFlags(m_BloodImage.GetFlags() | WidgetFlags.STRETCH);
		}

		// Start with invisible
		m_BloodImage.SetColor(ARGB(0, 255, 255, 255));
		m_CurrentAlpha = 0.0;
		m_BloodImage.Show(true);

		m_IsInitialized = true;
		PsyCore_Log.Get("DME_Cold").Info("Blood overlay initialization complete!");

		return true;
	}

	void Update(float deltaTime, float bloodRaw)
	{
		if (!m_IsInitialized || !m_BloodImage)
			return;

		// Get config
		DME_ColdConfig config = DME_ColdConfig.GetInstance();
		if (!config)
			return;

		// Check if effect is enabled
		if (!config.EnableBloodEffect)
		{
			if (m_CurrentAlpha > 0)
			{
				m_CurrentAlpha = 0;
				m_CurrentBloodLevelFloat = 0;
				ApplyAlpha();
			}
			return;
		}

		// Calculate target blood level (as float for smooth transitions)
		float targetBloodLevel = CalculateTargetBloodLevel(bloodRaw, config);

		// If blood is full (level 0), ensure overlay is hidden
		if (targetBloodLevel <= 0.0)
		{
			m_CurrentBloodLevelFloat = 0.0;
			m_CurrentAlpha = 0.0;
			ApplyAlpha();
			return;
		}

		// Smoothly interpolate blood level using TransitionSpeed
		float diff = targetBloodLevel - m_CurrentBloodLevelFloat;
		m_CurrentBloodLevelFloat += diff * deltaTime * config.BloodTransitionSpeed;

		// Clamp to valid range
		m_CurrentBloodLevelFloat = Math.Clamp(m_CurrentBloodLevelFloat, 0.0, 7.0);

		// Update blood image based on rounded level
		int roundedBloodLevel = Math.Round(m_CurrentBloodLevelFloat);
		UpdateBloodImage(roundedBloodLevel);

		// Calculate alpha directly from the float blood level (smooth!)
		UpdateAlphaFromBloodLevel(config);

		// Apply alpha to widget
		ApplyAlpha();

		// Debug logging to file
		if (config.DebugEnabled && config.DebugLogTemperature)
		{
			m_DebugLogTimer += deltaTime;
			if (m_DebugLogTimer >= DEBUG_LOG_INTERVAL)
			{
				PsyCore_Log.Get("DME_Cold").Info("Blood: " + bloodRaw.ToString() + " BloodFloat: " + m_CurrentBloodLevelFloat.ToString() + " Image: " + m_CurrentImageLevel.ToString() + " Alpha: " + m_CurrentAlpha.ToString());
				m_DebugLogTimer = 0.0;
			}
		}
	}

	protected float CalculateTargetBloodLevel(float bloodRaw, DME_ColdConfig config)
	{
		// Debug forced blood level
		if (config && config.DebugEnabled && config.DebugForceBloodLevel > 0)
		{
			return config.DebugForceBloodLevel;
		}

		// Calculate blood level as float based on blood value
		return CalculateBloodLevelFloat(bloodRaw, config);
	}

	protected float CalculateBloodLevelFloat(float blood, DME_ColdConfig config)
	{
		// Return float blood level for smooth interpolation
		// Blood loss is INVERTED: High blood (5000) = level 0, Low blood (0) = level 7
		// Level 0: blood >= 5000 (full health)
		// Level 7: blood <= 500 (near death)

		if (blood >= config.BloodLevel0Threshold) return 0.0;  // Full blood, no overlay
		if (blood <= config.BloodLevel7Threshold) return 7.0;  // Critical blood loss

		// Declare variables once at the start
		float range;
		float offset;

		// Calculate level based on thresholds
		// We go backwards: higher blood = lower level
		if (blood >= config.BloodLevel1Threshold) 
		{
			// Between level 0 and 1
			range = config.BloodLevel0Threshold - config.BloodLevel1Threshold;
			offset = config.BloodLevel0Threshold - blood;
			return offset / range;
		}
		else if (blood >= config.BloodLevel2Threshold)
		{
			// Between level 1 and 2
			range = config.BloodLevel1Threshold - config.BloodLevel2Threshold;
			offset = config.BloodLevel1Threshold - blood;
			return 1.0 + (offset / range);
		}
		else if (blood >= config.BloodLevel3Threshold)
		{
			// Between level 2 and 3
			range = config.BloodLevel2Threshold - config.BloodLevel3Threshold;
			offset = config.BloodLevel2Threshold - blood;
			return 2.0 + (offset / range);
		}
		else if (blood >= config.BloodLevel4Threshold)
		{
			// Between level 3 and 4
			range = config.BloodLevel3Threshold - config.BloodLevel4Threshold;
			offset = config.BloodLevel3Threshold - blood;
			return 3.0 + (offset / range);
		}
		else if (blood >= config.BloodLevel5Threshold)
		{
			// Between level 4 and 5
			range = config.BloodLevel4Threshold - config.BloodLevel5Threshold;
			offset = config.BloodLevel4Threshold - blood;
			return 4.0 + (offset / range);
		}
		else if (blood >= config.BloodLevel6Threshold)
		{
			// Between level 5 and 6
			range = config.BloodLevel5Threshold - config.BloodLevel6Threshold;
			offset = config.BloodLevel5Threshold - blood;
			return 5.0 + (offset / range);
		}
		else
		{
			// Between level 6 and 7
			range = config.BloodLevel6Threshold - config.BloodLevel7Threshold;
			offset = config.BloodLevel6Threshold - blood;
			return 6.0 + (offset / range);
		}
	}

	protected void UpdateBloodImage(int targetImageLevel)
	{
		// Only update if image level changed and is valid
		if (targetImageLevel == m_CurrentImageLevel)
			return;

		if (targetImageLevel <= 0)
		{
			// No blood loss, keep image but set alpha to 0 (handled elsewhere)
			m_CurrentImageLevel = 0;
			return;
		}

		// Clamp to valid image range (1-7)
		int imageLevel = Math.Clamp(targetImageLevel, 1, 7);

		// Load the appropriate blood image
		string imagePath = "DME_Cold/GUI/textures/blood" + imageLevel.ToString() + ".paa";
		bool loaded = m_BloodImage.LoadImageFile(0, imagePath);

		if (loaded)
		{
			m_CurrentImageLevel = targetImageLevel;
			PsyCore_Log.Get("DME_Cold").Info("Switched to blood image: " + imagePath);
		}
		else
		{
			PsyCore_Log.Get("DME_Cold").Warn("Failed to load " + imagePath);
		}
	}

	protected void UpdateAlphaFromBloodLevel(DME_ColdConfig config)
	{
		if (m_CurrentBloodLevelFloat <= 0.0)
		{
			m_CurrentAlpha = 0.0;
		}
		else
		{
			// Map blood level (0-7) directly to alpha range
			float alphaRange = config.BloodMaxAlpha - config.BloodMinAlpha;
			float levelFactor = (m_CurrentBloodLevelFloat - 1.0) / 6.0; // 0.0 to 1.0 for levels 1-7
			levelFactor = Math.Clamp(levelFactor, 0.0, 1.0);
			m_CurrentAlpha = config.BloodMinAlpha + (alphaRange * levelFactor);

			// Ensure minimum alpha when there's any blood loss
			if (m_CurrentBloodLevelFloat > 0.0 && m_CurrentAlpha < config.BloodMinAlpha)
				m_CurrentAlpha = config.BloodMinAlpha;
		}
	}

	protected void ApplyAlpha()
	{
		if (!m_BloodImage)
			return;

		// Convert float alpha (0.0-1.0) to int (0-255)
		int alphaInt = Math.Round(m_CurrentAlpha * 255.0);

		// Use SetColor with ARGB
		m_BloodImage.SetColor(ARGB(alphaInt, 255, 255, 255));
	}

	int GetCurrentBloodLevel()
	{
		return Math.Round(m_CurrentBloodLevelFloat);
	}

	float GetCurrentBloodLevelFloat()
	{
		return m_CurrentBloodLevelFloat;
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
