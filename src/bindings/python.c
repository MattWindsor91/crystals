/*
 * Crystals (working title)
 *
 * Copyright (c) 2010 Matt Windsor, Michael Walker and Alexander
 *                    Preisinger.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *
 *   * The names of contributors may not be used to endorse or promote
 *     products derived from this software without specific prior
 *     written permission.
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

/** @file   src/bindings/python.c
 *  @author Alexander Preisinger
 *  @brief  Python module for scripting.
 */

/* the python header should always be first */
#include "Python.h"

#include <stdio.h>
#include "bindings.h"
#include "../util.h"

/* -- INTERAL PROTOTYPES -- */
/* bindings.h defines what functions to use externally */

/* -- PYTHON PROTOTYPES AND STRUCTS*/

/** Run a test */

static PyObject*
crystals_init (void);

static PyObject*
crystals_test (PyObject *self, PyObject *args);

static PyMethodDef
crystals_meth[] = {
  /* name, C func,        flags,        docstring */
  {"test", crystals_test, METH_VARARGS, "Test the Crystal Module."},
  {NULL, NULL, 0, NULL}
};

static PyModuleDef 
crystals_mod = {
    /* a quick and dirty hack to prevent the PyModuleDef_HEAD_INIT warning */
    {PyObject_HEAD_INIT(NULL) NULL, 0, NULL}, "crystals", NULL, -1, crystals_meth,
    NULL, NULL, NULL, NULL
};


/* -- INTERNAL DEFINITIONS -- */

int
init_bindings (void)
{
  Py_Initialize(); 
  return SUCCESS;
}

void
cleanup_bindings (void)
{
  Py_Finalize();
}

int
run_script (const char* path)
{
  FILE *file_stream;
  short EXIT;

  PyImport_AppendInittab("crystals", &crystals_init);

  file_stream = fopen(path, "r");

  if (file_stream)
    {
      if (PyRun_SimpleFile(file_stream, path) == 0)
        EXIT = SUCCESS;
      else
        EXIT = FAILURE;
    }
  else
    EXIT = FAILURE;

  fclose(file_stream);
  return EXIT;
}


/* PYTHON DEFINITIONS */

static PyObject*
crystals_init (void) {
    return PyModule_Create (&crystals_mod);
}

static PyObject*
crystals_test (PyObject *self, PyObject *args)
{
  char *s;

  self = self; /* to prevent unused error */

  /* parse string and store it into the given pointers */
  if (!PyArg_ParseTuple(args, "s:test", &s))
    return NULL;

  printf("%s\n", s);
  return Py_None;
}

/* vim: set st=2 sw=2 softtabstop=2: */
