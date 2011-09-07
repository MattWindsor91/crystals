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

#include <glib.h>

#include "dialog.h"
#include "util.h"
#include "xml.h"

/* -- INTERNAL DECLARATIONS -- */

static void
int_ptr_array_free (gpointer data, gpointer user_data);

static void
int_add_contents (dlg_t *dlg, xml_node_t *node, bool_t is_main);

static void
int_add_content_text (dlg_t *dlg, dlg_content_t *con, xml_node_t *node);

static void
int_add_content_choices (dlg_t *dlg, dlg_content_t *con, xml_node_t *node);

static void
int_add_content_set (dlg_t *dlg, dlg_content_t *con, xml_node_t *node);

static void
int_add_content_goto (dlg_t *dlg, dlg_content_t *con, xml_nodet *node);


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

  if (xml_verify_doc (xml_root, "dialog"))
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

  dlg->path_name    = g_new (char, g_strlen(p)+1);
  g_strcpy (dlg->path_name, p);

  xml_curr = xml_root->children;
  
  /* add main contents */
  while (xml_curr != xml_root->last)
    {
      if (xml_curr->type == XML_ELEMENT_NODE &&
          xml_curr->name == "content")
        {
          int_add_contents (dlg, xml_curr, TRUE);
          break;
        }
      xml_curr = xml_curr->next;
    }

  /* add sub contents */
  xml_curr = xml_root->children;
  while (xml_curr != xml_root->last)
    {
      if (xml_curr->type == XML_ELEMENT_NODE &&
          xml_curr->name == "subcontent")
        int_add_contents (dlg, xml_curr, FALSE);
        
      xml_curr = xml_curr->next;
    }

  return dlg;
}

void
dlg_free (dlg_t *dlg)
{
  g_free (dlg->path_name);
  xml_free_doc (dlg->xml_root->doc);

  g_ptr_array_free (dlg->contents, TRUE);
  g_ptr_array_free (dlg->requirements, TRUE);

  g_free (dlg);
}


/* -- INTERNAL DEFINITIONS -- */

static void
int_ptr_array_free (gpointer data, gpointer user_data)
{
  (void) user_data;
  g_free (data);
}

static void
int_add_contents (dlg_t *dlg, xml_node_t *node, gboolean is_main)
{
  char *content_id   = NULL;
  dlg_content_t *con = NULL;
  xml_node_t *curr   = NULL;

  curr = node->children;

  while (curr != node->last)
    {
      con = g_new (dlg_content_t, 1);

      if (is_main)
        con->content_id = NULL;
      else
        con->content_id = xml_get_node_prop (node, "id");
 
      if (curr->type == XML_ELEMENT_NODE)
        {
          switch (curr->name)
            {
              case "say":
                int_add_content_text (dlg, con, curr);
                break;

              case "set":
                int_add_content_set (dlg, con,  curr);
                break;

              case "choices":
                int_add_content_choicest (dlg, con, curr);
                break;

              case "goto":
                int_add_content_goto (dlg, con, curr);
                break;

              default:
                error ("DIALOG - dlg_parse_file - Uknown node \"%s\" in %s\n",
                  curr->name, dlg->path_name);
                break;
            }

          g_ptr_array_add (dlg->contents, con);
        }
      curr = curr->next;
    }
}

static void
int_add_contents (dlg_t *dlg, xml_node_t *node, bool_t is_main)
{
}

static void
int_add_content_text (dlg_t *dlg, dlg_content_t *con, xml_node_t *node)
{
}

static void
int_add_content_choices (dlg_t *dlg, dlg_content_t *con, xml_node_t *node)
{
}

static void
int_add_content_set (dlg_t *dlg, dlg_content_t *con, xml_node_t *node)
{
}

static void
int_add_content_goto (dlg_t *dlg, dlg_content_t *con, xml_nodet *node)
{
}

/* vim: set ts=2 sw=2 softtabstop=2 cinoptions=>4,n-2,{2,^-2,:2,=2,g0,h2,p5,t0,+2,(0,u0,w1,m1: */

