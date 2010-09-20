/* tests/module.h - prototypes and global variables for the module tester */

#ifndef _TEST_MODULE_H
#define _TEST_MODULE_H

/* modules struct - stores details about modules and functions */
struct
{
  char* path;

  /* This module exists */
  struct
  {
    module_data metadata;
    void (*hello)(void);
    void (*sum_numbers)(int a, int b, int *ans);
  } test;

  /* This one doesn't */
  struct
  {
    module_data metadata;
    void (*bar)(void);
  } foo;
} modules;

/* function prototypes */
void
init_modules (const char *path);
void
close_modules (void);
void
load_module_test (void);
void
load_module_foo (void);


#endif /* _TEST_MODULE_H */
