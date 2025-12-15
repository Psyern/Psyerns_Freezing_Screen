// DME_Cold - 3_Game Entry Point
// This file initializes the game-level components

/**
 * DME_ColdSettings
 * 
 * Static settings class that reads values from DME_ColdConfig.
 * Provides easy access to configuration values throughout the mod.
 */
class DME_ColdSettings
{
    // Get frost level thresholds from config
    static float FROST_LEVEL_0_THRESHOLD()
    {
        return DME_ColdConfig.GetInstance().FrostLevel0Threshold;
    }
    
    static float FROST_LEVEL_1_THRESHOLD()
    {
        return DME_ColdConfig.GetInstance().FrostLevel1Threshold;
    }
    
    static float FROST_LEVEL_2_THRESHOLD()
    {
        return DME_ColdConfig.GetInstance().FrostLevel2Threshold;
    }
    
    static float FROST_LEVEL_3_THRESHOLD()
    {
        return DME_ColdConfig.GetInstance().FrostLevel3Threshold;
    }
    
    static float FROST_LEVEL_4_THRESHOLD()
    {
        return DME_ColdConfig.GetInstance().FrostLevel4Threshold;
    }
    
    static float FROST_LEVEL_5_THRESHOLD()
    {
        return DME_ColdConfig.GetInstance().FrostLevel5Threshold;
    }
    
    static float FROST_LEVEL_6_THRESHOLD()
    {
        return DME_ColdConfig.GetInstance().FrostLevel6Threshold;
    }
    
    static float FROST_LEVEL_7_THRESHOLD()
    {
        return DME_ColdConfig.GetInstance().FrostLevel7Threshold;
    }
    
    // Visual settings
    static float FROST_TRANSITION_SPEED()
    {
        return DME_ColdConfig.GetInstance().TransitionSpeed;
    }
    
    static float FROST_MIN_ALPHA()
    {
        return DME_ColdConfig.GetInstance().MinAlpha;
    }
    
    static float FROST_MAX_ALPHA()
    {
        return DME_ColdConfig.GetInstance().MaxAlpha;
    }
    
    // Debug settings
    static bool IsDebugEnabled()
    {
        return DME_ColdConfig.GetInstance().DebugEnabled;
    }
    
    static int GetForcedFrostLevel()
    {
        return DME_ColdConfig.GetInstance().DebugForceFrostLevel;
    }
    
    static bool IsEnabled()
    {
        return DME_ColdConfig.GetInstance().EnableFrostEffect;
    }
}
