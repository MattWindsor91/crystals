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

#include "parser.h" /**<< @todo implement a generic tree */
#include "dialog.h"

static dict_t match_names_sg = NULL; /**<< globals everywhere argh 
                                           @todo find a better solution */

struct dialog_root*
parse_dialog_file (const char *p)
{
  uint8_t counter = 0;
  
  xmlNode *xml_root = NULL;
  xmlNode *node     = NULL; 
  
  struct dialog_root *dlg_root = NULL;
  
  xml_root = parse_xml_file (p);
  
  if (xml_root == NULL)
    return NULL;
  
  dlg_root = malloc (sizeof (struct dialog_node))
  dlg_root->xml_root = xml_root;
  
  if (xml_root->name == (xmlChar) "dialog")
    node = xml_root->children;
    
  /* count amount of subcontents */
  while (node != 0)
    {
      if (node->name == "subcontent")
        ++counter;
      
      node = node->next;
    }
  
  /* allocate enough memory for all subcontents */
  dlg_root->sc_n = counter;
  dlg_root->subcontents = calloc (counter, sizeof (struct dialog_node));
  
  for (counter = 0; counter < dlg_root->sc_n; ++counter)
    dlg_root->subcontents[counter] = NULL; /* initialise all pointers */
  
  /* set node to the start again */
  node = xml_root->children;
  
  while (node != 0) 
    {
      xml_to_dlg (dlg_root, node, xml_root);
      
      node = node->next;
    }
  
  return dlg_root;
}

void
free_dialog (struct dialog_root *r)
{
}
 
static void 
xml_to_dlg (struct dialog_root *dlg_root, xmlNode *node, xmlNode *xml_root)
{
  static dict_t *match_names = NULL;
  
  int i = 0;
  struct dialog_node *tmp_sc = dlg_root->subcontents[i];
  
  switch (node->name) 
    {
      case "id":
        cfg_add (xmlNodeGetContent(node), 
                 xmlGetProp(node, "match"), &match_names);
        break;
      case "content":
        add_dialog_content (&dlg_root->content, node);
        break;
      case "subcontent":
        while (tmp_sc != NULL && i < dlg_root->sc_n)
          {
            ++i;
            tmp_sc = dlg_root->subcontents[i];
          }
        if (i < dlg_root->sc_n)
          add_dialog_content (dlg_root->subcontent[i], node)
        else
          fprintf (stderr, "DIALOG - xml_to_dlg - strange error that should not happen, occured\n");
        break;
      default:
        fprintf (stderr, "DIALOG - add_dialog_node - uknown node %s\n", node->name);
        break;
    }
}

static void 
add_dialog_content (struct dialog_node **dlg_node, xmlNode *xml_node)
{
  /** @todo finish ;-) */
}
