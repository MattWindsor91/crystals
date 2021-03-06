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
 * @file    src/field/file.c
 * @author  Matt Windsor
 * @brief   Convenience functions for file manipulation.
 */

#include "crystals.h"

/* Reads an unsigned 16-bit integer from two bytes in 
 * big-endian format.
 */
uint16_t
read_uint16 (FILE *file)
{
  uint16_t ushort = 0;

  ushort |= (uint16_t) (fgetc (file) << 8); /* Most significant byte  */
  ushort |= (uint16_t) (fgetc (file));      /* Least significant byte */

  return ushort;
}


/* Reads an unsigned 32-bit integer from four bytes in 
 * big-endian format.
 */
uint32_t
read_uint32 (FILE *file)
{
  uint32_t ulong = 0;

  ulong |= (uint32_t) (fgetc (file) << 24); /* Most significant byte  */
  ulong |= (uint32_t) (fgetc (file) << 16);
  ulong |= (uint32_t) (fgetc (file) << 8);
  ulong |= (uint32_t) (fgetc (file));       /* Least significant byte */

  return ulong;
}
