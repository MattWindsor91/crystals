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
 * @file    src/xml.h
 * @author  Alexander Preisinger
 * @brief   Prototypes and structures for generic XML usage.
 */

#ifndef _XML_H
#define _XML_H

#include <libxml/tree.h>

#include "util.h"

/* -- TYPEDEFS -- */

/**
 * XML Node
 *
 * This structure is taken directly from libxml2 and has various pointers
 *
 * childeren        The first children
 * last             The last children
 * next             The next sibling
 * prev             The previous sibling
 * parent           The parent pointer
 *
 * Always check for NULL pointers!
 */

typedef xmlNode xml_node_t;

/**
 * XML Node Type
 *
 * A node can also have different types. At the moment we are only interested
 * content types like:
 *
 * XML_ELEMENT_NODE   An element node that has a name, content and properties
 * XML_TEXT_NODE      A node that only contains text. (Sometimes just "")
 * XML_COMMENT_NODE   Comment block.
 */

typedef xmlElementType xml_type_t;


/* -- FUNCTION PROTOTYPES -- */

/**
 * Parses a XML document
 *
 * @param  p   The path to the document
 *
 * @return     Returns a XML node to the root element
 */

xml_node_t*
xml_parse_doc (const char *p);


/**
 * Verify the document, by checking the name of the root element
 *
 * @param root       The root node.
 * @param root_name  The name the root element is supposed to have
 *
 * @return           Either TRUE if the name is the same or FALSE
 */

bool_t
xml_verify_doc (xml_node_t *root, const char *root_name);


/**
 * Return the XML Node Type
 * node->type can also be used to get the type
 *
 * @param node   A valid XML node
 *
 * @return       The type of the node.
 */

xml_type_t
xml_get_node_type (xml_node_t *node);


/**
 * Get the a specific property
 *
 * @param node   A valid XML node
 * @param prop   The name of the property
 *
 * @return       The string value of the property
 */

const char*
xml_get_node_prop (xml_node_t *node, const char *prop);


/**
 * Get the content of a node
 *
 * @param node   A valid XML node
 * 
 * @return       Returns all content inside the node
 */

const char*
xml_get_node_content (xml_node_t *node);


/**
 * Free the open docmunent
 *
 * @param root    The root node or any node
 */

void
xml_free_doc (xml_node_t *root);


/**
 * Clean up the XML praser.
 * At the moment only calling xml_free_doc is sufficient.
 */

void
cleanup_xml (void);

#endif /* _XML_H */

/* vim: set ts=2 sw=2 softtabstop=2 cinoptions=>4,n-2,{2,^-2,:2,=2,g0,h2,p5,t0,+2,(0,u0,w1,m1: */

