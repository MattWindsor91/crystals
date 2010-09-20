/* module.h - prototypes and global variables for the module loader. */

#ifndef _MODULE_H
#define _MODULE_H

/* The file suffix of modules */
#define MODULESUFFIX ".so"

/* If making tests, we have another test module struct elsewhere. */
#ifndef TESTSUITE
/* Modules are stored in a struct of structs - each substruct containing function pointers, and a library pointer */
struct
{
  char* path;

  /* No modules yet - but use structs like this:
  struct {
    void *lib_handle;
    void (*FUNCTION)(ARGS);
  } MODULE;*/
} modules;
#endif /* TESTSUITE */

/* Function prototypes */
#ifndef TESTSUITE
void
init_modules (const char *path);
#endif /* TESTSUITE */
void
get_module_path (const char *module, char **out);
void
get_module_handle (const char *modulepath, void **lib_handle);
void
get_module_function (void *lib_handle, const char *function, void **func);
#ifndef TESTSUITE
void
close_modules (void);
#endif /* TESTSUITE */

#endif /* _MODULE_H */
