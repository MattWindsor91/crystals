Overview
========

Crystals is a project to create a Free Software monster-battling game, similar to a certain handheld game of the mid-90s.

Current Status
--------------

Early planning.  Several devteam idea branches have been made, and hopefully the design will be a blend of the brainstorming going on in each. Additionally, some basic functionality has been implemented.

Main decisions made so far:
--------------------------

 * Will be programmed in C with scripting in Python.
 * Will use a modular system for input/graphics/sound/etc.

Contributing
============

Crystals is licensed under the three-clause BSD license, so feel free to dive in and start hacking. However, by doing so you agree that you surrender copyright over all that you contribute which is merged into the developers' own branches and/or the official distribution to the copyright holders mentioned in the license text.

Test Suite
==========

Some functions have a test suite, which can be run as follows:

    make tests

You should get output similar to the following:

    % make tests
    Running tests...

If any test fails, there will be a notice, in which case you can do further debugging with gdb and valgrind:

    gdb -ex run -ex backtrace --args ./tests/module
    valgrind --tool=memcheck --leak-check=full --show-reachable=yes ./tests/module
