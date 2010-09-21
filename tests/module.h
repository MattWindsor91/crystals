/* tests/module.h - prototypes and global variables for the module tester */

#ifndef _TEST_MODULE_H
#define _TEST_MODULE_H

/* modules struct - stores details about modules and functions */
struct ModuleSet
{
  char *path;

  /* This module exists */
  struct
  {
    module_data metadata;
    void (*sum_numbers)(int a, int b, int *ans);
    int  (*mul_numbers)(int a, int b);
  } test;

  /* This one doesn't */
  struct
  {
    module_data metadata;
    void (*bar)(void);
  } foo;
};

/* function prototypes */
int
init_modules (const char *path);
void
cleanup_modules (void);
int
load_module_test (void);
int
load_module_foo (void);

#endif /* _TEST_MODULE_H */
