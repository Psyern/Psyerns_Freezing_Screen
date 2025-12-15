class CfgPatches
{
    class DME_Cold
    {
        units[] = {};
        weapons[] = {};
        requiredVersion = 0.1;
        requiredAddons[] = {"DZ_Data"};
    };
};

class CfgMods
{
    class DME_Cold
    {
        dir = "DME_Cold";
        picture = "";
        action = "";
        hideName = 1;
        hidePicture = 1;
        name = "DME_Cold";
        credits = "DeadmansEcho";
        author = "DeadmansEcho";
        authorID = "0";
        version = "1.0.0";
        extra = 0;
        type = "mod";
        
        dependencies[] = {"Game", "World", "Mission"};
        
        class defs
        {
            class gameScriptModule
            {
                value = "";
                files[] = {"DME_Cold/Scripts/3_Game"};
            };
            class worldScriptModule
            {
                value = "";
                files[] = {"DME_Cold/Scripts/4_World"};
            };
            class missionScriptModule
            {
                value = "";
                files[] = {"DME_Cold/Scripts/5_Mission"};
            };
        };
    };
};
