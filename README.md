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

To perform the bare minimum initialisation of a module:

    module_bare_init (module_data *module)

To get the path of a module and store it in a string 'out':

    get_module_path (const char* module, char** out)

To open a module and store a handle to it, as well as pointers to any 'init' and 'term' functions, and execite a 'init' function:

    get_module (const char* modulepath, module_data *module)

To get a pointer to a function in a module and store it in 'func':

    get_module_function (module_data metadata, const char *function, void **func)

To close the handle to a module and execute a 'term' function:

    close_module (module_data *module)

Modules
=======

Modules consist of a series of functions (currently only tested with the 'void' return type, but others should work) which are to be used by the engine.

Modules may optionally contain the following functions:

    void init (void);
    void term (void);

If present, the 'init' function is executed when the module is loaded, and the 'term' function when the module is unloaded.

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

Test Suite
==========

The module loader comes with a test suite, which depends on valgrind. Build and run it with the following:

    make tests

You should get output similar to the following:

    % make tests
    Compiling test suite:
    
    Running test suite:
    Loading 'test' module
    Loading module: /home/barrucadu/projects/crystals/tests/modules/test.so
    test init: Module test has been loaded!
    Loading 'foo' module
    Loading module: /home/barrucadu/projects/crystals/tests/modules/foo.so
    (null)
    ./tests/module: undefined symbol: init
    ./tests/module: undefined symbol: term
    ./tests/module: undefined symbol: bar
    Testing 'test' module:
    Should be hello world: Hello, world.
        2 + 3 = 5
        test term: Module test has been unloaded!
    
    Running memory leak check:
    ==1113==    definitely lost: 0 bytes in 0 blocks
    ==1113==    indirectly lost: 0 bytes in 0 blocks
    ==1113==      possibly lost: 0 bytes in 0 blocks

If you get a segfault or something, you can use GDB:

    gdb -ex run -ex backtrace --args ./tests/module

If there is a memory leak, you can use valgrind:

    valgrind --tool=memcheck --leak-check=full --show-reachable=yes ./tests/module
