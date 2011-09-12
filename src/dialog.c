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

static void
int_add_req_event (req_t *req, xml_node_t *node);

static void
int_add_req_quest (req_t *req, xml_node_t *node);

static void
int_add_req_item (req_t *req, xml_node_t *node);

static void
int_add_req_attr (req_t *req, xml_node_t *node);

static void
int_add_req_level (req_t *req, xml_node_t *node);


/* -- DEFINITIONS -- */

/* Parses the XML dialog file */

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


/* Iterates trough all contents */

dlg_content_t*
dlg_content_next (dlg_t *dlg, uint8_t choice)
{
  uint32_t i = 0;
  const char *choice_id = NULL;
  dlg_content_t *prev = NULL;
  dlg_content_t *con = NULL;

  if (dlg->dlg_next < dlg->contents->len)
    {
      if (dlg->dlg_next > 0)
        {
          prev = (dlg_content_t *) g_ptr_array_index (dlg->contents,
            dlg->dlg_next - 1);

          if (prev->type == CHOICES)
            {
              choice_id =
                (const char *) g_ptr_array_index
                (prev->action.choices.content_ids, choice);
             
              g_assert (choice_id != NULL);

              while (i < dlg->contents->len &&
                  strcmp (choice_id, ((dlg_content_t *)
                  g_ptr_array_index (dlg->contents, i))->content_id))
                i++;

              dlg->dlg_next = i;
            }
        }
      con = (dlg_content_t *) g_ptr_array_index
        (dlg->contents, dlg->dlg_next);
    }

  if (prev != NULL && con != NULL &&
      prev->type != CHOICES &&
      strcmp (prev->content_id, con->content_id))
    {
      con = NULL;
      dlg->dlg_next = dlg->contents->len;
    }

  dlg->dlg_next += 1;
  return con;
}


/* Iterates trough all requirements*/

req_t*
dlg_requirement_next (dlg_t *dlg)
{
  req_t *req = NULL;

  if (dlg->req_next < dlg->requirements->len)
    req = g_ptr_array_index (dlg->requirements, dlg->req_next);

  dlg->req_next += 1;
  return req;
}


/* Free all allocated memory */

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

/* Add content from the XML tree */

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
      con->content_id = "";

      if (is_main)
        con->content_id = "main";
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


/* Add plain text */

static void
int_add_content_text (dlg_content_t *con, xml_node_t *node)
{
  con->type = TEXT;
  con->action.text.actor_id = xml_node_get_prop (node, "who");
  con->action.text.text = xml_node_get_content (node);
  g_assert (con->action.text.actor_id != NULL);
  g_assert (con->action.text.text != NULL);
}


/* Add multiple choices as content */

static void
int_add_content_choices (dlg_content_t *con, xml_node_t *node)
{
  xml_node_t *curr = node->children;

  con->type = CHOICES;
  con->action.choices.actor_id = xml_node_get_prop (node, "who");
  con->action.choices.content_ids = g_ptr_array_new ();
  con->action.choices.descrptions = g_ptr_array_new ();

  g_assert (con->action.choices.actor_id != NULL);

  while (curr != node->last)
    {
      if (curr->type == XML_ELEMENT_NODE &&
          !strcmp ((const char *) curr->name, "choice"))
        {
          g_assert (xml_node_get_prop (curr, "id") != NULL);
          g_ptr_array_add (con->action.choices.content_ids,
            (gpointer) xml_node_get_prop (curr, "id"));

          g_assert (xml_node_get_content (curr) != NULL);
          g_ptr_array_add (con->action.choices.descrptions,
            (gpointer) xml_node_get_content (curr));
        }

      curr = curr->next;
    }
}


/* Add setter content */

static void
int_add_content_set (dlg_content_t *con, xml_node_t *node)
{
  const char *node_type = xml_node_get_prop (node, "type");
 
  if (!strcmp (node_type, "event"))
    {
      con->type = SET_EVENT;
      con->action.event = xml_node_get_content (node);
      g_assert (con->action.event != NULL);
    }
  else if (!strcmp (node_type, "item"))
    {
      con->type = SET_ITEM;
      con->action.item.name = xml_node_get_content (node);

      if (xml_node_get_prop (node, "amount") != NULL)
        con->action.item.amount =
          atoi (xml_node_get_prop (node, "amount"));

      else
        {
          error ("DIALOG - dlg_parse_file - ");
          error ("Property is not an integer: \"amount\"; using 0\n");
          con->action.item.amount = 0;
        }

      g_assert (con->action.item.name != NULL);
    }
  else if (!strcmp (node_type, "attr"))
    {
      con->type = SET_ATTR;
      con->action.attr.name = xml_node_get_content (node);

      if (xml_node_get_prop (node, "value") != NULL)
        con->action.attr.value =
          atoi (xml_node_get_prop (node, "value"));

      else
        {
          error ("DIALOG - dlg_parse_file - ");
          error ("Property is not an integer: \"value\"; using 0\n");
          con->action.attr.value = 0;
        }

      g_assert (con->action.attr.name != NULL);
    }
  else if (!strcmp (node_type, "quest"))
    {
      con->type = SET_QUEST;
      con->action.quest.name = xml_node_get_content (node);
      con->action.quest.state = xml_node_get_prop (node, "state");
      g_assert (con->action.quest.name != NULL);
      g_assert (con->action.quest.state != NULL);
    }
  else if (!strcmp (node_type, "exp"))
    {
      con->type = SET_EXP;

      if (xml_node_get_content (node) != NULL)
        con->action.exp = atoi (xml_node_get_content (node));

      else
        {
          error ("DIALOG - dlg_parse_file - ");
          error ("Content is not an integer; using 0\n");
          con->action.exp = 0;
        }
    }
  else
    {
      error ("DIALOG - dlg_parse_file - ");
      error ("Unknown set type: \"%s\"\n",node_type);
    }
}


/* Add internal goto content for multiple choices */

static void
int_add_content_goto (dlg_content_t *con, xml_node_t *node)
{
  con->type = INTERNAL;
  con->action.goto_id = xml_node_get_content (node);
  g_assert (con->action.goto_id != NULL);
}


/* Add requirements */

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
            int_add_req_event (req, curr);
           
          else if (!strcmp (node_name, "quest"))
            int_add_req_quest (req, curr);

          else if (!strcmp (node_name, "item"))
            int_add_req_item (req, curr);

          else if (!strcmp (node_name, "attr"))
            int_add_req_attr (req, curr);

          else if (!strcmp (node_name, "level"))
            int_add_req_level (req, curr);

          else
            {
              error ("DIALOG - dlg_parse_file - ");
              error ("Uknown node \"%s\"\n", node_name);
            }

          g_ptr_array_add (dlg->requirements, (gpointer) req);
        }
      curr = curr->next;
    }
}


/* Add requirement for a specific event */

static void
int_add_req_event (req_t *req, xml_node_t *node)
{
  req->type = EVENT;
  req->action.event = xml_node_get_content (node);
  g_assert (req->action.event != NULL);
}


/* Add quest state requirment */

static void
int_add_req_quest (req_t *req, xml_node_t *node)
{
  req->type = QUEST;
  req->action.quest.name = xml_node_get_content (node);
  req->action.quest.state = xml_node_get_prop (node, "state");
  g_assert (req->action.quest.name != NULL);
  g_assert (req->action.quest.state != NULL);
}


/* Add item requiremnt */

static void
int_add_req_item (req_t *req, xml_node_t *node)
{
  req->type = ITEM;
  req->action.item.name = xml_node_get_content (node);

  if (xml_node_get_prop (node, "amount") != NULL)
    req->action.item.amount = atoi (xml_node_get_prop (node, "amount"));
 
  else
    {
      error ("DIALOG - dlg_parse_file - ");
      error ("Property is not an integer: \"amount\"; using 0\n");
      req->action.item.amount = 0;
    }

  g_assert (req->action.item.name != NULL);
}


/* Add attribute or skill requirement */

static void
int_add_req_attr (req_t *req, xml_node_t *node)
{
  req->type = ATTR;
  req->action.attr.name = xml_node_get_content (node);
 
  if (xml_node_get_prop (node, "value") != NULL)
    req->action.attr.value = atoi (xml_node_get_prop (node, "value"));

  else
    {
      error ("DIALOG - dlg_parse_file - ");
      error ("Property is not an integer: \"value\"; using 0\n");
      req->action.attr.value = 0;
    }

  g_assert (req->action.attr.name != NULL);
}


/* Add level requirement */

static void
int_add_req_level (req_t *req, xml_node_t *node)
{
  req->type = LEVEL;
 
  if (xml_node_get_content (node) != NULL)
    req->action.lvl = atoi (xml_node_get_content (node));

  else
    {
      error ("DIALOG - dlg_parse_file - ");
      error ("Requirement is not an integer; using 0\n");
      req->action.lvl = 0;
    }
}


/* Free the choices content types */

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

