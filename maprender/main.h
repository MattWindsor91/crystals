#ifndef __MAIN_H__
#define __MAIN_H__

extern int g_running;

int
main (int argc, char **argv);

int
init (void);

void
main_loop (void);

void
cleanup (void);

#endif /* __MAIN_H__ */
