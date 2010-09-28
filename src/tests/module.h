/* tests/module.h - prototypes and global variables for the module tester */

#ifndef _TEST_MODULE_H
#define _TEST_MODULE_H

/* Annoying I have to define this here, too... */
typedef struct
{
  void *lib_handle; /**< The dynamic loading handle for the module. */

  void
  (*init) (void); /**< Pointer to the module's initialisation
                     function. */
  void
  (*term) (void); /**< Pointer to the module's termination
                     function. */
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
} test_module_set;

/* function prototypes */
int
init_test_modules (const char *path);
void
cleanup_test_modules (void);
int
load_module_test (test_module_set* modules);
int
load_module_foo (test_module_set* modules);

#endif /* _TEST_MODULE_H */
