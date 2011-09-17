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
 * @file     src/timer.h
 * @author   Matt Windsor
 * @brief    Public interface for the internal timer.
 */


#ifndef _TIMER_H
#define _TIMER_H


/* -- CONSTANTS -- */

/**
 * Number of microseconds in a second.
 */
extern const uint32_t USECONDS_PER_SECOND;


/**
 * Desired framerate, in frames per second.
 */
extern const uint32_t FRAMES_PER_SECOND;


/**
 * Number of microseconds in a frame.
 */
#define USECONDS_PER_FRAME (USECONDS_PER_SECOND / FRAMES_PER_SECOND)


/* -- DECLARATIONS -- */

/**
 * Initialises and starts the timer.
 */
void init_timer (void);


/**
 * Gets the number of microseconds elapsed since the previous call.
 *
 * @return  the total number of microseconds elapsed, modulo 1000000.
 */
uint32_t timer_get_delta (void);


/**
 * Resets the timer.
 */
void reset_timer (void);


/**
 * Frees the timer.
 */
void cleanup_timer (void);


#endif /* not _TIMER_H */
