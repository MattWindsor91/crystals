#ifndef _MAIN_H
#define _MAIN_H

extern int g_running;
extern struct map_view *g_mapview;

int
main (int argc, char **argv);

int
init (void);

void
main_loop (void);

void
cleanup (void);

#endif /* _MAIN_H */
