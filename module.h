/* module.h - prototypes and global variables for the module loader. */

#ifndef _MODULE_H
#define _MODULE_H

/* The file suffix of modules */
#define MODULESUFFIX ".so"

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

/* Function prototypes */
void
init_modules (const char *path);
void
get_module_path (const char *module, char **out);
void
get_module_handle (const char *modulepath, void **lib_handle);
void
get_module_function (void *lib_handle, const char *function, void **func);
void
close_modules (void);

#endif /* _MODULE_H */
