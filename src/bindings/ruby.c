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

/** 
 * @file    src/bindings/ruby.c
 * @author  Alexander Preisinger
 * @brief   Ruby module for scripting.
 */

#include <stdio.h>

#include "bindings.h"
#include "../util.h"

#include "ruby.h"

/* -- INTERNAL DEFINITIONS -- */

bool_t
init_bindings (void)
{
  ruby_init ();
  ruby_init_loadpath ();
  ruby_script ("crystals");
  return SUCCESS;
}

void
cleanup_bindings (void)
{
  ruby_finalize (); 
}

bool_t
run_script (const char* path)
{
  rb_load_file (path);
  rb_eval_string ("puts 'Embed the ruby interpret properly'");
  return SUCCESS;
}

