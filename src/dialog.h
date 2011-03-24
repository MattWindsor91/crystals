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

#include "types.h"
#include "xml.h"

/* -- ENUMS -- */

enum dialog_type 
{
  SIMPLE_TEXT,
  CHOICES,
  SET_EVENT, /**<< changes the status of a specific event */
  SET_ITEM, /**<< Player gets an item */
  SET_ATTR, /**<< increases Player attributes or skills */
  GOTO /**<< simple dialog specific goto */
};


/* -- UNIONS -- */

union dialog_action {
  struct dialog_event *event;
  struct item_node    *item;
  struct choices_node *choices;
  struct dialog_node  *content;
  char                *goto_id;
};

/* -- STRUCTURES -- */ 

struct dialog_root 
{
  struct dialog_node  *content;
  struct dialog_node  **subcontents;
  uint8_t             sc_n; /**<< amount of subcontents */
  struct dialog_requirements *requirements;
  xmlNode             xml_root;
};

/**
 * choices_node
 * 
 * @todo either use the first bits of text as description and use dialog_node
 *       instead of choices_node, or find a better way of solving this.
 */
struct choices_node 
{
  char                *description;
  char                *text;
  struct choices_node *next_node; 
};

struct dialog_node 
{
  void                speaker; /**<< @todo find a way to map to a speaker, 
                                           maybe some sort of ID? */
  char                *text;
  char                *sc_id; /**<< is only set for subcontents, otherwise NULL */
  enum  dialog_type   type;
  union dialog_action *action;
};

struct dialog_requirements
{
  enum event_type *events;
  enum attr_type  *attr;
};

/* -- DECLARAITONS -- */

struct dialog_root*
parse_dialog_file (const char *p);

void
free_dialog (struct dialog_root *r);

static void 
xml_to_dlg (struct dialog_root *dlg_root, xmlNode *node, xmlNode *xml_root);

static void 
add_dialog_content (struct dialog_node **xml_node, xmlNode *xml_node);

#endif /* _DIALOG_H */
