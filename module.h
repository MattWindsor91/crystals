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

/* Modules are stored in a struct of structs - each substruct containing function pointers, and a library pointer */
#ifndef TESTSUITE
struct ModuleSet
{
  char *path;

  struct
  {
    module_data metadata;

    int
    (*init_screen) (unsigned short width,
                    unsigned short height, unsigned char depth);

    void
    (*draw_rect) (short x, 
                  short y, 
                  unsigned short width,
                  unsigned short height, 
                  unsigned char red,
                  unsigned char green,
                  unsigned char blue);

    void *
    (*load_image_data) (const char filename[]);

    void 
    (*free_image_data) (void *data);

    int
    (*draw_image) (void *image, 
                   short image_x,
                   short image_y,
                   short screen_x,
                   short screen_y,
                   unsigned short width,
                   unsigned short height);

    void
    (*update_screen) (void);

    void
    (*scroll_screen) (unsigned int direction);

  } gfx;
 
};
#endif /* TESTSUITE */

extern struct ModuleSet g_modules;

/* Function prototypes */
#ifndef TESTSUITE
int
init_modules (const char *path);
#endif /* TESTSUITE */

void
module_bare_init (module_data *module);

void
get_module_path (const char *module, char **out);

int
get_module_by_name (const char* name, module_data *module);

int
get_module (const char* modulepath, module_data *module);

int
get_module_function (module_data metadata, const char *function, void **func);

#ifndef TESTSUITE
int
load_module_gfx (void);
#endif /* TESTSUITE */

void
close_module (module_data *module);

#ifndef TESTSUITE
void
cleanup_modules (void);
#endif /* TESTSUITE */

#endif /* _MODULE_H */
