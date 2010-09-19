/* loader.h - prototypes and global variables for the module loader. */

#ifndef _LOADER_H
#define _LOADER_H 1

/* The path to search for modules. This should be user-configurable (command-line parameter?) and be a sensible
   location by default. */
#define MODULEPATH "/home/barrucadu/projects/crystals/modules/"

/* The file suffix of modules */
#define MODULESUFFIX ".so"

/* Modules are stored in a struct of structs - each substruct containing function pointers, and a library pointer */
struct
{
  struct {
    /* This is the lib pointer */
    void *lib_handle;

    /* These are the function pointers */
    void (*hello)(void);
    void (*sum_numbers)(int a, int b, int *ans);
  } test;
} modules;

/* Function prototypes */
void
init_modules (void);
void
get_module_path (const char *module, char **out);
void
get_module_handle (const char *modulepath, void *lib_handle);
void
get_module_function (void *lib_handle, const char *function, void **func);
void
load_module_test (void);
void
close_modules(void);
int
main (int argc, char* argv[]);


#endif
