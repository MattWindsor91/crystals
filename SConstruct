import os

# -- CONFIGURE BOOLS --
#
# This booleans are used to decide if optional modules can be built.
# If a optional module is forced with an option the SConstruct will 
# exit if the librarys are not installed.
HAS_SDL = False
HAS_SDL_IMAGE = False
HAS_PYTHON2 = False
HAS_LUA = False

# -- OBJECTS AND FLASG --
#

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
    parser.c
    util.c
             """)

# Add flags for warnings
warn_flags = Split(""" 
    -Wall -Wextra -Wshadow -Wpointer-arith -Wcast-align -Wwrite-strings 
    -Wmissing-prototypes -Wmissing-declarations -Wredundant-decls 
    -Wnested-externs -Winline -Wno-long-long -Wconversion 
    -Wstrict-prototypes
                   """)

# -- OPTIONS --
#
AddOption('--prefix',
    dest='prefix',
    type='string',
    nargs=1,
    action='store',
    metavar='PREFIX',
    default='/usr/local/',
    help='Installation prefix for architecture-independent files [/usr/local]'
)

AddOption('--exec-prefix',
    dest='exec-prefix',
    type='string',
    nargs=1,
    action='store',
    metavar='EPREFIX',
    default='$PREFIX',
    help='Installation exec-prefix for architecture-dependent files [PREFIX]'
)

AddOption('--bindir',
    dest='bindir',
    type='string',
    nargs=1,
    action='store',
    metavar='DIR',
    default='$EPREFIX/bin',
    help='User executables [EPREFIX/bin]'
)

AddOption('--libdir',
    dest='libdir',
    type='string',
    nargs=1,
    action='store',
    metavar='DIR',
    default='$EPREFIX/lib',
    help='Program modules [EPREFIX/lib]'
)

AddOption('--datadir',
    dest='datadir',
    type='string',
    nargs=1,
    action='store',
    metavar='DIR',
    default='$PREFIX/share/',
    help='Program data and scripts [PREFIX/share/]'
)

# For packager
AddOption('--install',
    dest='install',
    type='string',
    nargs=1,
    action='store',
    metavar='DIR',
    default='',
    help='Destination directory for packaging []'
)

AddOption('--with-sdl',
    dest='with_sdl',
    action='store_true',
    help='Force to build with SDL, exits build if not possible'
)

AddOption('--with-lua',
    dest='with_lua',
    action='store_true',
    help='Force to build with Lua, exits build if not possible'
)

AddOption('--with-python',
    dest='with_python',
    action='store_true',
    help='Force to build with python, exits build if not possible'
)

AddOption('--python-version',
    dest='py_ver',
    type='string',
    nargs=1,
    action='store',
    metavar='X.Y',
    default='2.6',
    help='Python to build with [2.6]'
)

# -- SETUP ENVIRONMENT --
#
# The environment depends on the user envirenment variables and
# on the option given with scons.
#
# The environment can still be changed latter by the configure 
# routines describe below.

# Change Module Path
# FIXME: only temporary 
MODPATH = os.environ['PWD']+'/modules/'

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
    LIBDIR=GetOption('libdir')+'/crystals',
    DATADIR=GetOption('datadir'),
    DESTDIR=GetOption('install')
)
crystals.MergeFlags([os.environ['CFLAGS'] if 'CFLAGS' in os.environ else ''])
crystals.MergeFlags([os.environ['LDFLAGS'] if 'LDFLAGS' in os.environ else ''])
crystals.Replace(SHLIBPREFIX='') # Build modules without 'lib' prefix
crystals.Append(
    CCFLAGS = Split('-ansi -pedantic -O2 -ggb -DDEFMODPATH=\\"{mp}\\"'
    .format(mp=MODPATH))
)
crystals.Append(CCFLAGS=warn_flags)
crystals.Append(CPPPATH=[''])
crystals.Append(LIBS=[''])
crystals.Append(LINKFLAGS=['-g'])

event_sdl = crystals.Clone()
gfx_sdl = event_sdl.Clone()
python = crystals.Clone()
lua = crystals.Clone()

# -- CONFIGURE --
#
# - MAIN - 
config = Configure(crystals)

if not config.CheckCC():
    print "The %s compiler does not work!" % crystals['CC'][0]
    Exit()

if not config.CheckCHeader('stdio.h'):
    Exit()
if not config.CheckCHeader('stdlib.h'):
    Exit()
if not config.CheckCHeader('string.h'):
    Exit()
if not config.CheckLibWithHeader('dl', 'dlfcn.h', 'c'):
    Exit()

crystals = config.Finish()

# - MODULES - 
# Python
config = Configure(python)
py_lib = "python%s" % GetOption('py_ver')
py_head = py_lib + '/Python.h'

if not config.CheckLibWithHeader(py_lib, py_head, 'c'):
    if GetOption('with_python'):
        print "Python %s not found, exit build." % GetOption('py_ver')
        Exit()
    
    else:
        print "Python %s not found, skip python bindings." % GetOption('py_ver')

else:
    py_conf = FindFile('python%s-config' % GetOption('py_ver'),
        crystals['ENV']['PATH'].split(':'))
    
    if py_conf is None:
        py_conf = FindFile('python-config-%s' % GetOption('py_ver'),
            crystals['ENV']['PATH'].split(':'))
        
        if py_conf is None:
            print "Wether python%s-config nor python-config-%s found." % (
                GetOption('py_ver'), GetOption('py_ver'))
            print """
    If your distribution does not provide a versioned python-config
    then create a symbolic link to ~/bin/python%s-config and add that
    path to the PATH variable (e.g. export PATH=$PATH:~/bin)
                  """ % GetOption('py_ver')
            Exit()
    
    python = config.Finish()
    python.ParseConfig('%s --cflags' % py_conf)
    python.ParseConfig('%s --ldflags' % py_conf)

    HAS_PYTHON2 = True


# Lua
config = Configure(lua)
if not config.CheckLibWithHeader('lua', 'lua.h', ''):
    if GetOption("with-lua"):
        print "Lua not found, exit build"
        Exit()
    else:
        print "Lua not found, skip lua module"
else:
    lua = config.Finish()
    lua.ParseConfig('pkg-config lua --libs')
    HAS_LUA = True

# SDL
config = Configure(event_sdl)
if not config.CheckLibWithHeader('SDL', 'SDL/SDL.h', 'c'):
    if GetOption("with-sdl"):
        print "SDL not found, exit build"
        Exit()
    else:
        print "SDL not found, skip sdl modules"
else:
    HAS_SDL = True
    event_sdl = config.Finish()
    
    event_sdl.ParseConfig('sdl-config --cflags')
    event_sdl.ParseConfig('sdl-config --libs')
    
    gfx_sdl = event_sdl.Clone()
    config = Configure(gfx_sdl)

    if not config.CheckLib('SDL_image'):
        print "SDL_image not found, skip gfx sdl module"
    else:
        gfx_sdl = config.Finish()
        HAS_SDL_IMAGE = True

# -- MAIN BUILD --
#
# Build Main Programm
crystals.Program(prog, objs)


# -- BUILD MODULES --
# 
# SDL
if HAS_SDL:
    evt_sdl_so = event_sdl.SharedLibrary('modules/event-sdl.c')

    if HAS_SDL_IMAGE:
        gfx_sdl_so = gfx_sdl.SharedLibrary('modules/gfx-sdl.c',)

# Python
if HAS_PYTHON2:
    py_so = python.SharedLibrary('modules/bindings-python.c')

# Lua
if HAS_LUA:
    lua_so = lua.SharedLibrary('modules/bindings-lua.c')

# -- INSTALL --
#
MODULEDIR='$DESTDIR/$LIBDIR'
Alias('install', crystals.Install('$DESTDIR/$BINDIR', prog))

if HAS_SDL:
    Alias('install', event_sdl.Install(MODULEDIR, evt_sdl_so))
    
    if HAS_SDL_IMAGE:
        Alias('install', gfx_sdl.Install(MODULEDIR, gfx_sdl_so))

if HAS_PYTHON2:
    Alias('install', python.Install(MODULEDIR, py_so))

if HAS_LUA:
    Alias('install', lua.Install(MODULEDIR, lua_so))

# vim: set filetype=python:
