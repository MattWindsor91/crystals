#ifndef _MAIN_H
#define _MAIN_H

extern int g_running;
extern struct map_view *g_mapview;

enum
  {
    SUCCESS = 1,
    FAILURE = 0,
    TRUE = 1,
    FALSE = 0,
    NORTH = 0,
    EAST,
    SOUTH,
    WEST
  };

int
main (int argc, char **argv);

int
init (void);

void
main_loop (void);

void
cleanup (void);

#endif /* _MAIN_H */
