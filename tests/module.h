/* tests/module.h - prototypes and global variables for the module tester */

#ifndef _TEST_MODULE_H
#define _TEST_MODULE_H

/* Not nice that I have to define this here as well */
typedef struct
{
  void *lib_handle;
  void (*init)(void);
  void (*term)(void);
} module_data;

/* modules struct - stores details about modules and functions */
typedef struct
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
} module_set;

/* function prototypes */
int
load_module_test (void);
int
load_module_foo (void);

#endif /* _TEST_MODULE_H */
