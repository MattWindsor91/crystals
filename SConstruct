import os

# -- CONFIGURE BOOLS --
#
# This booleans are used to decide if optional modules can be built.
# If a optional module is forced with an option the SConstruct will 
# exit if the librarys are not installed.
HAS_SDL = False
HAS_SDL_IMAGE = False
HAS_PYTHON2 = False

# -- OPTIONS --
#
AddOption('--prefix',
    dest='prefix',
    type='string',
    nargs=1,
    action='store',
    metavar='PREFIX',
    help='Installation prefix for architecture-independent files [/usr/local]'
)

AddOption('--exec-prefix',
    dest='exec-prefix',
    type='string',
    nargs=1,
    action='store',
    metavar='EPREFIX',
    help='Installation exec-prefix for architecture-dependent files [PREFIX]'
)
AddOption('--bindir',
    dest='bindir',
    type='string',
    nargs=1,
    action='store',
    metavar='DIR',
    help='User executables [EPREFIX/bin]'
)

AddOption('--install',
    dest='install',
    type='string',
    nargs=1,
    action='store',
    metavar='DIR',
    help='Destination directory for packaging []'
)

# -- SETUP ENVIRONMENT --
#
# The environment depends on the user envirenment variables and
# on the option given with scons.
#
# The environment can still be changed latter by the configure 
# routines describe below.

# Change Module Path
MODPATH = os.environ['PWD']+'/modules/'

# NOTE: Do not use this if changes within 
# a second after building is possible
Decider('MD5-timestamp')

# Executeable name
prog = 'maprender-test'

# Files to build
objs = Split("""
    bindings.c
    events.c
    graphics.c
    main.c
    map.c
    mapview.c
    module.c
    optionparser.c
    util.c
             """)

# Add flags for warnings
warn_flags = Split(""" 
    -Wall -Wextra -Wshadow -Wpointer-arith -Wcast-align -Wwrite-strings 
    -Wmissing-prototypes -Wmissing-declarations -Wredundant-decls 
    -Wnested-externs -Winline -Wno-long-long -Wconversion 
    -Wstrict-prototypes
                   """)

# Configure Main Program
crystals = Environment(
    ENV=os.environ,
    CC=[os.environ['CC'] if 'CC' in os.environ else 'cc'],
# Silent Strings
    CCCOMSTR='Compiling $TARGET',
    LINKCOMSTR='Linking $TARGET',
    SHCCCOMSTR='Compiling shared object $TARGET',
    SHLINKCOMSTR='Linking shared $TARGET',
# Options
    PREFIX=GetOption('prefix'),
    EPREFIX=GetOption('exec-prefix'),
    BINDIR=GetOption('bindir'),
    DESTDIR=GetOption('install')
)
crystals.MergeFlags([os.environ['CFLAGS'] if 'CFLAGS' in os.environ else ''])
crystals.MergeFlags([os.environ['LDFLAGS'] if 'LDFLAGS' in os.environ else ''])
crystals.Replace(SHLIBPREFIX='') # Build modules without 'lib' prefix
crystals.Append(
    CCFLAGS = Split('-ansi -pedantic -O2 -ggdb -DDEFMODPATH=\\"{mp}\\"'
    .format(mp=MODPATH))
)
crystals.Append(CCFLAGS=warn_flags)
crystals.Append(CPPPATH=[''])
crystals.Append(LIBS=['dl'])
crystals.Append(LDFLAGS=['-g'])


# -- CONFIGURE --
#
config = Configure(crystals)

if not config.CheckLibWithHeader('python2.6', 'python2.6/Python.h', 'c'):
    print("Python 2.6 not found, skip python bindings")
else:
    HAS_PYTHON2 = True

if not config.CheckLibWithHeader('SDL', 'SDL/SDL.h', 'c'):
    print("SDL not found, skip sdl modules")
else:
    HAS_SDL = True
    
    if not config.CheckLib('SDL_image'):
        print("SDL_image not found, skip gfx sdl module")
    else:
        HAS_SDL_IMAGE = True

crystals = config.Finish()


# -- MAIN BUILD --
#
# Build Main Programm
crystals.Program(prog, objs)


# -- BUILD MODULES --
# 
# Build crystals modules
if HAS_SDL:
    event_sdl = crystals.Clone()
    event_sdl.ParseConfig('sdl-config --cflags')
    event_sdl.ParseConfig('sdl-config --libs')
    event_sdl.SharedLibrary('modules/event-sdl.c')

    if HAS_SDL_IMAGE:
        gfx_sdl = event_sdl.Clone()
        gfx_sdl.Append(LIBS = ['SDL_image'])
        gfx_sdl.SharedLibrary('modules/gfx-sdl.c',)

if HAS_PYTHON2:
    python = crystals.Clone()
    python.ParseConfig('python-config --cflags')
    python.ParseConfig('python-config --ldflags')
    python.SharedLibrary('modules/bindings-python.c')

# -- INSTALL --
#
Alias('install', crystals.Install('$DESTDIR/$PREFIX/bin/', prog))

# vim: set filetype=python:
