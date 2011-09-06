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
 * @file    src/dialog.h
 * @author  Alexander Preisinger
 * @brief   Declarations and types for the dialog system.
 */

#ifndef _DIALOG_H
#define _DIALOG_H

#include <glib.h>

#include "types.h"
#include "xml.h"

/* -- TYPEDEFS -- */

/* dialog types */
typedef enum dialog_type 
{
  TEXT,
  CHOICES,
  SET_EVENT, /**<< changes the status of a specific event */
  SET_ITEM,  /**<< player gets an item */
  SET_ATTR,  /**<< increases player attributes or skills */
  SET_EXP    /**<< set experience points */
} dlg_type_t;

typedef struct dialog_text
{
  char *actor_id;
  char *text;
} dlg_text_id;

typedef struct dialog_choises
{
  char      *actor_id;
  GPtrArray *descrptions; /**<< */
  GPtrArray *content_ids; /**<< */
  /* Those two arrays have to have the same length */
} dlg_choices_t;

typedef struct dialog_event
{
  char *name;
  char *state;
} dlg_event_t;

typedef struct dialog_item
{
  char    *name;
  int32_t amount;
} dlg_item_t;

typedef struct dialog_attribute
{
  char    *name;
  int32_t *value; /**<< value to add to the chosen attribute */
} dlg_attr_t;

typedef union dialog_action 
{
  dlg_event_t   *event;
  dlg_item_t    *item;
  dlg_choices_t *choices;
  dlg_text_t    *text;
  dlg_attr_t    *attr;
  int32_t       *exp;
  char          *goto_id;
} dlg_action_t;

typedef struct dialog_contents
{
  char         *content_id;
  dlg_type_t   type;
  dlg_action_t *action;
} dlg_content_t;  

typedef struct dialog_root 
{
  GPtrArray  *requirements;
  GPtrArray  *contents;
  xml_node_t *xml_root;
} dlg_t;


/* requirements types */
typedef enum requirement_type
{
  EVENT, /**<< all sorts of ingame events from triggers or other dialogs */
  ATTR,  /**<< is a special attribute or skill required for the conversation */
  ITEM,  /**<< an item, you found your the stolen flowers, yay */
  LEVEL  /**<< a specified level, maybe to enter a dangerous cave :O */
} req_type_t;

typedef union requirement_action
{
  dlg_event_t *event;
  dlg_item_t  *item;
  dlg_attr_t  *attr;
  int32_t     *lvl;
} req_action_t;

typedef struct requirement
{
  req_type_t   type;
  req_action_t *action;
} req_t;

/* -- DECLARAITONS -- */

dlg_t*
dlg_parse_file (const char *p);

/* fun api stuff to come*/

void
dlg_free (dlg_t *dlg);

#endif /* _DIALOG_H */

/* vim: set ts=2 sw=2 softtabstop=2 cinoptions=>4,n-2,{2,^-2,:2,=2,g0,h2,p5,t0,+2,(0,u0,w1,m1: */

