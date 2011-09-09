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
 * @file    src/dialog.c
 * @author  Alexander Preisinger
 * @brief   Dialog module for a conversation system.
 * @todo    Test!
 */

#include <ctype.h>
#include <string.h>

#include <glib.h>

#include "dialog.h"
#include "util.h"
#include "xml.h"

/* -- INTERNAL DECLARATIONS -- */

static void
int_add_contents (dlg_t *dlg, xml_node_t *node, bool_t is_main);

static void
int_add_content_text (dlg_content_t *con, xml_node_t *node);

static void
int_add_content_choices (dlg_content_t *con, xml_node_t *node);

static void
int_add_content_set (dlg_content_t *con, xml_node_t *node);

static void
int_add_content_goto (dlg_content_t *con, xml_node_t *node);

static void
int_add_requirements (dlg_t *dlg, xml_node_t *node);

static void
int_free_content (gpointer data, gpointer user_data);

/* -- DEFINITIONS -- */

dlg_t*
dlg_parse_file (const char *p)
{
  xml_node_t *xml_root = NULL;
  xml_node_t *xml_curr = NULL;

  dlg_t *dlg = NULL;

  xml_root = xml_parse_doc (p);

  if (xml_root == NULL)
    return NULL;

  if (!xml_verify_doc (xml_root, "dialog"))
    {
      error (
        "DIALOG - dlg_parse_file - File \"%s\" is not a crystals dialog file.",
        p);
      xml_free_doc (xml_root);
      return NULL;
    }

  dlg               = g_new (dlg_t, 1);
  dlg->contents     = g_ptr_array_new ();
  dlg->requirements = g_ptr_array_new ();
  dlg->xml_root     = xml_root;
  dlg->dlg_next     = 0;
  dlg->req_next     = 0;

  dlg->path_name    = g_new (char, strlen(p)+1);
  strcpy (dlg->path_name, p);

  xml_curr = xml_root->children;

  while (xml_curr != xml_root->last)
    {
      if (xml_curr->type == XML_ELEMENT_NODE)
        {
          if (!strcmp ((const char *) xml_curr->name, "requirements"))
            int_add_requirements (dlg, xml_curr);

          /* add main content */
          else if (!strcmp ((const char *) xml_curr->name, "content"))
            int_add_contents (dlg, xml_curr, TRUE);

          /* add sub contents */
          else if (!strcmp ((const char *) xml_curr->name, "subcontent"))
            int_add_contents (dlg, xml_curr, FALSE);

          else
            error ("DIALOG - dlg_parse_file - Uknown node \"%s\" in %s\n",
              xml_curr->name, p);
        }

      xml_curr = xml_curr->next;
    }

  return dlg;
}

dlg_content_t*
dlg_content_next (dlg_t *dlg, uint8_t choice)
{
  uint32_t i;
  const char *choice_id = NULL;
  dlg_content_t *con = NULL;
  
  if (dlg->dlg_next > 0)
    {
      con = (dlg_content_t *) g_ptr_array_index (dlg->contents,
        dlg->dlg_next - 1);

      if (con->type == CHOICES) 
        {
          choice_id = 
            (const char *) g_ptr_array_index 
            (con->action.choices.content_ids, choice);

          for (i = 0; i < dlg->contents->len; ++i)
            {
              if (choice_id == (const char *) 
                  g_ptr_array_index (dlg->contents, i)) 
                break;
            }
          
          dlg->dlg_next = i;
        }
    }


  if (dlg->dlg_next < dlg->contents->len)  
    con = (dlg_content_t *) g_ptr_array_index (dlg->contents, dlg->dlg_next);
  else 
    return NULL;
    
  dlg->dlg_next += 1;
  return con;
}

req_t*
dlg_requirement_next (dlg_t *dlg)
{
  if (dlg->req_next < dlg->requirements->len)
    req_t *req = g_ptr_array_index (dlg->requirements, dlg->req_next);
  else 
    return NULL;

  dlg->req_next += 1;
  return req;
}

void
dlg_free (dlg_t *dlg)
{
  xml_free_doc (dlg->xml_root);

  g_ptr_array_foreach (dlg->contents, int_free_content, NULL);
  g_ptr_array_free (dlg->contents, FALSE);
  g_ptr_array_free (dlg->requirements, TRUE);

  g_free (dlg->path_name);
  g_free (dlg);
}


/* -- INTERNAL DEFINITIONS -- */

static void
int_add_contents (dlg_t *dlg, xml_node_t *node, bool_t is_main)
{
  const char *node_name = NULL;
  dlg_content_t *con    = NULL;
  xml_node_t *curr      = NULL;

  curr = node->children;

  while (curr != node->last)
    {
      con = g_new (dlg_content_t, 1);

      if (is_main)
        con->content_id = NULL;
      else
        con->content_id = xml_node_get_prop (node, "id");

      if (curr->type == XML_ELEMENT_NODE)
        {
          node_name = (const char *) curr->name;

          if (!strcmp (node_name, "say"))
            int_add_content_text (con, curr);

          else if (!strcmp (node_name, "set"))
            int_add_content_set (con,  curr);

          else if (!strcmp (node_name, "choices"))
            int_add_content_choices (con, curr);

          else if (!strcmp (node_name, "goto"))
            int_add_content_goto (con, curr);

          else
            error ("DIALOG - dlg_parse_file - Uknown node \"%s\" in %s\n",
              node_name, dlg->path_name);

          g_ptr_array_add ((dlg->contents), (gpointer) con);
        }
      curr = curr->next;
    }
}

/* the following needs more error checking */

static void
int_add_content_text (dlg_content_t *con, xml_node_t *node)
{
  con->type = TEXT;
  con->action.text.actor_id = xml_node_get_prop (node, "who");
  con->action.text.text = xml_node_get_content (node);
}

static void
int_add_content_choices (dlg_content_t *con, xml_node_t *node)
{
  xml_node_t *curr = node->children;

  con->type = CHOICES;
  con->action.choices.actor_id = xml_node_get_prop (node, "who");
  con->action.choices.content_ids = g_ptr_array_new ();
  con->action.choices.descrptions = g_ptr_array_new ();

  while (curr != node->last)
    {
      if (curr->type == XML_ELEMENT_NODE &&
          !strcmp((const char *) curr->name, "choice"))
        {
          g_ptr_array_add (con->action.choices.content_ids,
            (gpointer) xml_node_get_prop (curr, "id"));
          g_ptr_array_add (con->action.choices.descrptions,
            (gpointer) xml_node_get_content (curr));
        }

      curr = curr->next;
    }
}

static void
int_add_content_set (dlg_content_t *con, xml_node_t *node)
{
  const char *node_type = xml_node_get_prop (node, "type");

  if (!strcmp (node_type, "event"))
    {
      con->type = SET_EVENT;
      con->action.event = xml_node_get_content (node);
    }
  else if (!strcmp (node_type, "item"))
    {
      con->type = SET_ITEM;
      con->action.item.name = xml_node_get_content (node);
      con->action.item.amount = atoi (xml_node_get_prop (node, "amount"));
    }
  else if (!strcmp (node_type, "attr"))
    {
      con->type = SET_ATTR;
      con->action.attr.name = xml_node_get_content (node);
      con->action.attr.value = atoi (xml_node_get_prop (node, "value"));
    }
  else if (!strcmp (node_type, "quest"))
    {
      con->type = SET_QUEST;
      con->action.quest.name = xml_node_get_content (node);
      con->action.quest.state = xml_node_get_prop (node, "state");
    }
  else if (!strcmp (node_type, "exp"))
    {
      con->type = SET_EXP;
      con->action.exp = atoi (xml_node_get_content (node));
    }
  else
    error ("DIALOG - dlg_parse_file - Unknown set type: \"%s\"",
      node_type);
}

static void
int_add_content_goto (dlg_content_t *con, xml_node_t *node)
{
  con->type = INTERNAL;
  con->action.goto_id = xml_node_get_content (node);
}

static void
int_add_requirements (dlg_t *dlg, xml_node_t *node)
{
  xml_node_t *curr = node->children;
  const char *node_name = NULL;
  req_t *req;

  while (curr != node->last)
    {
      if (curr->type == XML_ELEMENT_NODE)
        {
          req = g_new (req_t, 1);
          node_name = (const char *) curr->name;

          if (!strcmp (node_name, "event"))
            {
              req->type = EVENT;
              req->action.event = xml_node_get_content (curr);
            }
          else if (!strcmp (node_name, "quest"))
            {
              req->type = QUEST;
              req->action.quest.name = xml_node_get_content (curr);
              req->action.quest.state = xml_node_get_prop (curr, "state");
            }
          else if (!strcmp (node_name, "item"))
            {
              req->type = ITEM;
              req->action.item.name = xml_node_get_content (curr);
              req->action.item.amount =
                atoi (xml_node_get_prop (curr, "amount"));
            }
          else if (!strcmp (node_name, "attr"))
            {
              req->type = ATTR;
              req->action.attr.name = xml_node_get_content (curr);
              req->action.attr.value =
                atoi (xml_node_get_prop (curr, "value"));
            }
          else if (!strcmp (node_name, "level"))
            {
              req->type = LEVEL;
              req->action.lvl = atoi (xml_node_get_content (curr));
            }
          else
            error ("DIALOG - dlg_parse_file - Uknown node \"%s\"\n",
              node_name);

          g_ptr_array_add (dlg->requirements, (gpointer) req);
        }
      curr = curr->next;
    }
}

static void
int_free_content (gpointer data, gpointer user_data)
{
  dlg_content_t *con = (dlg_content_t *) data;

  (void) user_data;

  if (con->type == CHOICES)
    {
      g_ptr_array_free (con->action.choices.content_ids, FALSE);
      g_ptr_array_free (con->action.choices.descrptions, FALSE);
    }

  g_free (con);
}

/* vim: set ts=2 sw=2 softtabstop=2 cinoptions=>4,n-2,{2,^-2,:2,=2,g0,h2,p5,t0,+2,(0,u0,w1,m1: */

