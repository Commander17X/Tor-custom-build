/* Copyright (c) 2001 Matej Pfajfar.
 * Copyright (c) 2001-2004, Roger Dingledine.
 * Copyright (c) 2004-2006, Roger Dingledine, Nick Mathewson.
 * Copyright (c) 2007-2021, The Tor Project, Inc. */
/* See LICENSE for licensing information */

/**
 * @file bwauth.h
 * @brief Header for bandwidth authority functionality
 **/

#ifndef TOR_BWAUTH_H
#define TOR_BWAUTH_H

#include "core/or/or.h"

void bwauth_init(void);
void bwauth_cleanup(void);

#endif /* !defined(TOR_BWAUTH_H) */ 