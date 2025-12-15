DME_Cold Frost Overlay Textures
================================

Place your 7 frost overlay images here with the following names:
- frost_1.edds (lightest frost - level 1)
- frost_2.edds (level 2)
- frost_3.edds (level 3)
- frost_4.edds (level 4)
- frost_5.edds (level 5)
- frost_6.edds (level 6)
- frost_7.edds (heaviest frost - level 7)

Image Requirements:
-------------------
- Format: .edds (DayZ texture format) or .paa
- Resolution: 1920x1080 recommended (will be scaled to fit screen)
- Alpha channel: Required for transparency
- The images should show progressively more frost/ice from 1 to 7

Converting Images to EDDS:
--------------------------
1. Create your PNG/TGA images with alpha channel
2. Use DayZ Tools (Texture Tool) or ImageToPAA to convert
3. For EDDS format, use the DayZ Workbench texture tools

Alternative: You can also use .paa format if preferred. Just update the 
image paths in DME_ColdFrostOverlay.c
