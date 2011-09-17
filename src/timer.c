/*
 * Crystals (working title)
 *
 * Copyright (c) 2010, 2011
 *               Matt Windsor, Michael Walker and Alexander Preisinger.
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
 * @file     src/timer.c
 * @author   Matt Windsor
 * @brief    The internal timer.
 */

#include "crystals.h"


/* -- CONSTANTS -- */

/* Number of microseconds in a second. */
const uint32_t USECONDS_PER_SECOND = 1000000;

/* Desired framerate, in frames per second. */
const uint32_t FRAMES_PER_SECOND = 60;


/* -- STATIC GLOBAL VARIABLES -- */

/**
 * Timing source.
 */
static GTimer *sg_timer = NULL;


/* Initialises and starts the timer. */
void
init_timer (void)
{
  sg_timer = g_timer_new ();
}


/* Gets the elapsed microseconds since the last call. */
uint32_t
timer_get_delta (void)
{
  static gulong previous_useconds = 0;

  gulong useconds;
  uint32_t result;

  (void) g_timer_elapsed (sg_timer, &useconds);

  /* Previous useconds will be bigger if the timer has rolled over
     a second. */
  if (useconds < previous_useconds)
    useconds += USECONDS_PER_SECOND;

  result = (uint32_t) (useconds - previous_useconds);
  previous_useconds = useconds % USECONDS_PER_SECOND;

  return result;
}


/* Resets the timer. */
void
reset_timer (void)
{
  g_timer_start (sg_timer);
}


/* Frees the timer. */
void
cleanup_timer (void)
{
  g_timer_destroy (sg_timer);
}
