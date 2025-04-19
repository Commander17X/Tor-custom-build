/* Copyright (c) 2001 Matej Pfajfar.
 * Copyright (c) 2001-2004, Roger Dingledine.
 * Copyright (c) 2004-2006, Roger Dingledine, Nick Mathewson.
 * Copyright (c) 2007-2021, The Tor Project, Inc. */
/* See LICENSE for licensing information */

/**
 * @file dircollate.h
 * @brief Header for directory collation functionality
 **/

#ifndef TOR_DIRCOLLATE_H
#define TOR_DIRCOLLATE_H

#include "core/or/or.h"
#include "feature/nodelist/routerlist.h"

/* Function declarations */
int dircollate_init(void);
void dircollate_cleanup(void);

#endif /* !defined(TOR_DIRCOLLATE_H) */ 