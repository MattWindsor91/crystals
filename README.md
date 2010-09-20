Overview
========

Crystals is a project to create a Free Software monster-battling game, similar to a certain handheld game of the mid-90s.

Current Status
--------------

Early planning.  Several devteam idea branches have been made, and hopefully the design will be a blend of the brainstorming going on in each.

Main decisions made so far:
--------------------------

 * Will be programmed in C with scripting in Python.
 * Will use a modular system for input/graphics/sound/etc.

Contributing
============

Crystals is licensed under the three-clause BSD license, so feel free to dive in and start hacking. However, by doing so you agree that you surrender copyright over all that you contribute which is merged into the developers' own branches and/or the official distribution to the copyright holders mentioned in the license text.

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
