import os.path

from subprocess import Popen, PIPE

# Change Module Path
MODPATH = os.environ['PWD']+'/modules/'

# NOTE: Do not use this if changes within 
# a second after building is possible
Decider('MD5-timestamp')

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
crystals = Environment(ENV=os.environ)
crystals.SetDefault(CC='clang') # Set default compiler to clang
crystals.Replace(SHLIBPREFIX='') # Build modules without 'lib' prefix
crystals.Append(
    CCFLAGS = Split('-ansi -pedantic -O2 -ggdb -DDEFMODPATH=\\"{mp}\\"'
    .format(mp=MODPATH))
)
crystals.Append(CCFLAGS=warn_flags)
crystals.Append(CPPPATH=[''])
crystals.Append(LIBS=['dl'])
crystals.Append(LDFLAGS=['-g'])

# Build Main Programm
crystals.Program('maprender-test', objs)
# -- MODULES --
# 
# Build crystals modules

event_sdl = crystals.Clone()
event_sdl.ParseConfig('sdl-config --cflags')
event_sdl.ParseConfig('sdl-config --libs')
event_sdl.SharedLibrary('modules/event-sdl.c')

gfx_sdl = event_sdl.Clone()
gfx_sdl.Append(LIBS = ['SDL_image'])
gfx_sdl.SharedLibrary('modules/gfx-sdl.c',)

python = crystals.Clone()
python.ParseConfig('python-config --cflags')
python.ParseConfig('python-config --ldflags')
python.SharedLibrary('modules/bindings-python.c')

# vim: set filetype=python:
