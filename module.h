/* module.h - prototypes and global variables for the module loader. */

#ifndef _MODULE_H
#define _MODULE_H

/* The file suffix of modules */
#define MODULESUFFIX ".so"

/* This is the struct used for individual modules */
typedef struct
{
  void *lib_handle;
  void (*init)(void);
  void (*term)(void);
} module_data;

/* If making tests, we have another test module struct elsewhere. */
#ifndef TESTSUITE
/* Modules are stored in a struct of structs - each substruct containing function pointers, and a library pointer */
struct
{
  char* path;

  /* No modules yet - but use this:
  struct
  {
    module_data metadata;
    void (*FUNCTION)(PARAMETERS);
  } MODULE;
  */
} g_modules;
#endif /* TESTSUITE */

/* Function prototypes */
#ifndef TESTSUITE
void
init_modules (const char *path);
#endif /* TESTSUITE */
void
module_bare_init (module_data *module);
void
get_module_path (const char *module, char **out);
void
get_module (const char* modulepath, module_data *module);
void
get_module_function (module_data metadata, const char *function, void **func);
void
close_module (module_data *module);
#ifndef TESTSUITE
void
close_modules (void);
#endif /* TESTSUITE */

#endif /* _MODULE_H */
