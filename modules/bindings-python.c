
#include <Python.h>

/** Initialise the python module. */

int
init (void);

/** De-initialise the python module. */

void
term (void);

/** Test the python module. */

void
test (void);

void
init (void)
{
    Py_Initialize();

    /* set the patch in which the python files are */
    PySys_SetPath(".");
}

void
term (void)
{
    PyFinalize();
}

void
test (void)
{
    PySys_SetPath("./tests");

    char* test =    "import sys"
                    "print sys.argv"
                    "print sys.path";

    /* a simple test */
    PyRun_SimpleString(test);
}