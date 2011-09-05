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
 * @file    src/parser.h
 * @author  Alexander Preisinger
 * @brief   Prototypes and declarations for the configuration parser.
 */

#ifndef _PARSER_H
#define _PARSER_H

#include <glib.h>
#include "types.h"

/* -- TYPEDEFS -- */

typedef GKeyFile dict_t; /**< Node type for the key-value tree. */

/* -- FUNCTION PROTOTYPES -- */

/** 
 * Initialise the config system. 
 *
 * @param   config_path    Path of the configuration file. 
 *
 * @return  A pointer to the master configuration dictionary.
 */

dict_t *
init_config (const char *config_path);


/** 
 * Get the string value of the appropriate key.
 *
 * @param   key     Pass a string, which will be searched in the tree.
 * @param   cfg     The root node which contains the information.
 *
 * @return  Return the value to the appropriate key or NULL if none is found.
 */

char *
cfg_get_str (const char *group, const char *key, dict_t *cfg);

/** 
 * Get the integer value of the appropriate key.
 *
 * @param   key     Pass a string, which will be searched in the tree.
 * @param   cfg     The root node which contains the information.
 *
 * @return  Return the value to the appropriate key or NULL if none is found.
 */

int32_t
cfg_get_int (const char *group, const char *key, dict_t *cfg);

/** 
 * Function for freeing allocated memory in cfgs.
 *
 * @param   cfg     Node type.
 */

void
cfg_free (dict_t *cfg);


#endif /* not _PARSER_H */

/* vim: set ts=2 sw=2 softtabstop=2 cinoptions=>4,n-2,{2,^-2,:2,=2,g0,h2,p5,t0,+2,(0,u0,w1,m1: */

