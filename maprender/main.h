#ifndef _MAIN_H
#define _MAIN_H

extern int g_running;

enum
  {
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
