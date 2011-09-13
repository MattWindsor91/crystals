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
 * @file   src/crystals.h
 * @author Michael Walker
 * @brief  Header to include all other required headers.
 */

#ifndef _CRYSTALS_H
#define _CRYSTALS_H

/* External headers */
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>

#include <glib.h>
#include <glib-2.0/glib/gslist.h>
#include <glib-2.0/gmodule.h>

/* Crystals headers */
#include "types.h"
#include "file.h"
#include "events.h"
#include "graphics.h"
#include "parser.h"
#include "main.h"
#include "module.h"
#include "optionparser.h"
#include "state.h"
#include "util.h"

#include "bindings/bindings.h"

#include "field/map.h"
#include "field/mapview.h"
#include "field/field.h"
#include "field/mapload.h"
#include "field/object-image.h"
#include "field/object.h"
#include "field/object-api.h"

#endif /* not _CRYSTALS_H */
