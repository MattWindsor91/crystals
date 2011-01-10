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

/** @file    src/tests/module.h
 *  @author  Michael Walker
 *  @brief   Prototypes and definitions for the module loader sanity
 *           test.
 */

#ifndef _TEST_MODULE_H
#define _TEST_MODULE_H

/* Annoying I have to define this here, too... */

/** Structure for module data. */
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

/** modules struct - stores details about modules and functions */
typedef struct
{
  char *path; /**< Path to the module directory. */

  struct
  {
    module_data metadata; /**< Metadata for the test module. */

    void (*sum_numbers)(int a, int b, int *ans);
    int  (*mul_numbers)(int a, int b);
  } test; /**< This module exists. */

  struct
  {
    module_data metadata; /**< Metadata for the foo module. */
    void (*bar)(void);    /**< Nonexistent function. */
  } foo; /**< This module doesn't. */
} test_module_set;

/* -- PROTOTYPES -- */


/** This initialises the struct of modules to NULL and sets the load
 *  path.
 *
 *  @param path  The path to the module files.
 *
 *  @return  SUCCESS if successful; FAILURE otherwise.
 */

int
init_test_modules (const char *path);


/** This closes any loaded modules, run before program termination. */

void
cleanup_test_modules (void);


/** Function to load 'test' module.
 *
 *  @param modules  Set of test modules.
 *
 *  @return  SUCCESS if successful, FAILURE otherwise.
 */

int
load_module_test (test_module_set* modules);


/** Function to load 'foo' module.
 *
 *  @param modules  Set of test modules.
 *
 *  @return  SUCCESS if successful, FAILURE otherwise.
 */

int
load_module_foo (test_module_set* modules);


#endif /* _TEST_MODULE_H */
