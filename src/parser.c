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
 * @file    src/parser.c
 * @author  Alexander Preisinger
 * @brief   Configuration parser.
 */

#include "util.h"
#include "parser.h"

/* Initialise the config system. */

dict_t *
init_config (const char *config_path)
{
  GKeyFile *config = g_key_file_new(); 
  
  /* FIXME: GError handling */
  g_key_file_load_from_file(config, config_path, G_KEY_FILE_NONE, NULL);

  return config;
}


/* Function for freeing allocated memory in cfgs. */

void
cfg_free (dict_t *cfg)
{
  if (cfg != NULL)
    {
      g_key_file_free(cfg);
    }
}


/* Get the string value of the appropriate key. */

char *
cfg_get_str (const char *group, const char *key, dict_t *cfg)
{ 
  /* FIXME: GError handling */
  return g_key_file_get_string(cfg, group, key, NULL);
}

/* Get the integer value of the appropriate key. */

int32_t
cfg_get_int (const char *group, const char *key, dict_t *cfg)
{
  /* FIXME: GError handling */
  return g_key_file_get_integer(cfg, group, key, NULL);
}



/* vim: set ts=2 sw=2 softtabstop=2: */

