there should be located data in .bin format. This data is frequently used and
it's makes sense to link it to the source code.
Examples:
window icon;
cursor;
hud data;
fonts;
main character.

Utility for such conversion is not ready yet

for icon, cursor it should be (due to https://www.glfw.org/docs/3.3/group__window.html#gadd7ccd39fe7a7d1f0904666ae5932dc5):


The pixels are 32-bit, little-endian, non-premultiplied RGBA, 
i.e. eight bits per channel with the red channel first. They are arranged 
canonically as packed sequential rows, starting from the top-left corner.

The desired image sizes varies depending on platform and system settings.
The selected images will be rescaled as needed. Good sizes include 16x16, 
32x32 and 48x48.