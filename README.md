This is a plugin loader for Crystals (working title). There currently are no plugins, so it's fairly useless.

Usage
-----

Initialise the module handler, and set the default path to load modules from. This should be user-configurable, perhaps with a command-line flag:

    init_modules (const char *path)

Close any open handles to modules:

    close_modules()

When there are modules, you will be able to load them as follows:

    load_module_NAME()

Ideas for modules
-----------------

 * Graphics:
   * graphics-sdl.so
   * graphics-opengl.so
 * Sound
   * sound-sdl.so
   * sound-null.so (for the Hurd)
 * Input
   * input-sdl.so
