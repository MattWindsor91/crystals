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

/**
 * @file   src/main.h
 * @author Matt Windsor
 * @brief  Prototypes and declarations for main functions.
 */


#ifndef _MAIN_H
#define _MAIN_H

/* -- CONSTANTS -- */

extern const char *DEFAULT_CONFIG_PATH;  /**< Default config file path. */


/* -- GLOBAL VARIABLES -- */

extern dict_t *g_config;    /**< Configuration dictionary. */


/* -- DECLARATIONS -- */

#ifndef BUILD_TEST
/**
 * The main function.
 *
 * @param argc  Argument count.
 * @param argv  Argument vector.
 *
 * @return      the exit status of the program.
 */

int
main (int argc, char **argv);
#endif

/**
 * Initialise all engine subsystems.
 */

void
init (void);


/** Execute the main loop of the program. */

void
main_loop (void);


/** Clean up all initialised subsystems. */

void
cleanup (void);


#endif /* not _MAIN_H */
