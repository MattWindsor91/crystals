This is a plugin loader for Crystals (working title). There currently are no plugins, so it's fairly useless.

Usage
=====

Initialise the module handler, and set the default path to load modules from. This should be user-configurable, perhaps with a command-line flag:

    init_modules (const char *path)

Close any open handles to modules:

    close_modules()

When there are modules, you will be able to load them as follows:

    load_module_NAME()

That will populate the modules.NAME global struct with function pointers.

Internal Functions
------------------

These are used by the above functions, and so shouldn't really be used anywhere else.

To get the path of a module and store it in a string 'out':

    get_module_path (const char* module, char** out)

To open a module and store a handle to it for use with the dl* functions in 'lib_handle':

    get_module_handle (const char* modulepath, void **lib_handle)

To get a pointer to a function in a module and store it in 'func':

    get_module_function (void *lib_handle, const char *function, void **func)

Ideas for modules
=================

 * Graphics:
   * graphics-sdl.so
   * graphics-opengl.so
 * Sound
   * sound-sdl.so
   * sound-null.so (for the Hurd)
 * Input
   * input-sdl.so
