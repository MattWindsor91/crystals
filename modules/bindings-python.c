/*
* Crystals (working title)
*
* Copyright (c) 2010 Matt Windsor, Michael Walker and Alexander
* Preisinger.
*
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
*
* * Redistributions of source code must retain the above copyright
* notice, this list of conditions and the following disclaimer.
*
* * Redistributions in binary form must reproduce the above
* copyright notice, this list of conditions and the following
* disclaimer in the documentation and/or other materials provided
* with the distribution.
*
* * The names of contributors may not be used to endorse or promote
* products derived from this software without specific prior
* written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
* FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
* AFOREMENTIONED COPYRIGHT HOLDERS BE LIABLE FOR ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
* OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/** @file bindings-python.c
 * @author Alexander Preisinger
 * @brief Python module for scripting
 */

/* the python header should always be first */
#include "Python.h"

#include <stdio.h>


/** Initialise the python module. */

void
init (void);


/** Terminate the python module. */

void
term (void);


/** Execute the file
 *  The file has to be a valid python source
 *
 *  @param path Path to the file.
 *
 *  @return Returns SUCCESS if success else FAILURE
 * */

int
run_file (const char* path);

static PyObject*
crystals_test (PyObject *self, PyObject *args);

/* -- DEFINITIONS -- */

static PyMethodDef
CrystalsMethods[] =
{
/* name, C func, flags, docstring */
  {"test", crystals_test, METH_VARARGS, "Test the Crystal Module."},
  {NULL, NULL, 0, NULL}
};

void
init (void)
{
  Py_Initialize();
}

void
term (void)
{
  Py_Finalize();
}

int
run_file (const char* path)
{
  FILE *stream;

  short EXIT;

  stream = fopen(path, "r");

  if (stream)
    {
      if (PyRun_SimpleFile(stream, path) == 0)
        EXIT = 1;
      else
        EXIT = 0;
    }
  else
    EXIT = 0;

  fclose(stream);
  return EXIT;
}

static PyObject*
crystals_test (PyObject *self, PyObject *args)
{
  char *s;
  PyObject *py_return;
  /* parse string and store it into the given pointers */
  if (!PyArg_ParseTuple(args, "s:test", &s))
    return 0;

  printf("%s\n", s);
  return py_return;
}

/* vim: set st=2 sw=2 softtabstop=2: */
