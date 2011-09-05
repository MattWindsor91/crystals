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
 * @file    src/xml.c
 * @author  Alexander Preisinger
 * @brief   XML module.
 * @todo    Test!
 */
 
#include "xml.h"

xmlNode* 
xml_parse_doc (const char *p)
{    
  xmlDoc  *doc  = NULL;
  xmlNode *root = NULL;

  /*
   * this initialize the library and check potential ABI mismatches
   * between the version it was compiled for and the actual shared
   * library used.
   */
  LIBXML_TEST_VERSION

  doc = xmlParseFile (p);

  if (doc == NULL) 
    {
      error ("XML - parse_xml_file - Could not parse file %s", p);
      xmlFreeDoc(doc);
      return NULL;
    }
  else
    {
      /*Get the root element node */
      root = xmlDocGetRootElement(doc);

      if (root == NULL)
        {
          error ("XML - parse_xml_file - Empty document: %s", p);
          xmlFreeDoc(doc);
          return NULL;
        }
      else
        return root;

    }
}

bool_t
xml_verify_doc (xmlNode *root, const char *root_name)
{
  if (!xmlStrcmp(root->name, (const xmlChar *) root_name))
    return TRUE;
  else
    return FALSE;
}

void
xml_free_doc (xmlNode *root)
{
  xmlFreeDoc (root->doc);
}

/* isn't really needed */
void
cleanup_xml (void) 
{
  xmlCleanupParser ();
}

/* vim: set ts=2 sw=2 softtabstop=2 cinoptions=>4,n-2,{2,^-2,:2,=2,g0,h2,p5,t0,+2,(0,u0,w1,m1: */

