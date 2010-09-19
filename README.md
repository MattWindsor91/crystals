This is a plugin loader for Crystals (working title). There currently are no plugins, so it's fairly useless.

Usage
-----

init_modules (const char *path): Initialise the module handler, and set the default path to load modules from. This should be user-configurable, perhaps with a command-line flag.
close_modules(): Close any open handles to modules.

When there are modules, you can load them as follows:

load_module_NAME(): Loads the functions specified in NAME and fills the struct modules.NAME with function pointers.

Ideas for modules:

 * Graphics:
   * graphics-sdl.so
   * graphics-opengl.so
 * Sound
   * sound-sdl.so
   * sound-null.so (for the Hurd)
 * Input
   * input-sdl.so
