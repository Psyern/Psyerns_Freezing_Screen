/**
 * DME_ColdManager
 * Singleton manager class that handles the frost overlay system.
 * Located in 4_World module so it can access PlayerBase.
 * Does NOT store overlay reference to avoid Object destructor issues.
 */
class DME_ColdManager
{
    private static ref DME_ColdManager s_Instance;
    protected bool m_IsActive;
    
    static DME_ColdManager GetInstance()
    {
        if (!s_Instance)
        {
            s_Instance = new DME_ColdManager();
        }
        return s_Instance;
    }
    
    void DME_ColdManager()
    {
        m_IsActive = false;
        Print("[DME_Cold] Manager initialized (native DayZ version, 4_World)");
    }
    
    void Activate()
    {
        if (m_IsActive)
        {
            Print("[DME_Cold] Already active, skipping activation");
            return;
        }
        
        m_IsActive = true;
        Print("[DME_Cold] ======== Frost overlay activated successfully (manager) ========");
    }
    
    void Deactivate()
    {
        if (!m_IsActive)
            return;
        
        m_IsActive = false;
        Print("[DME_Cold] Frost overlay deactivated");
    }
    
    void Update(float deltaTime)
    {
        // This method is simplified - mission will call overlay.Update directly
        // This just ensures manager is active
    }
    
    bool IsActive()
    {
        return m_IsActive;
    }
}
