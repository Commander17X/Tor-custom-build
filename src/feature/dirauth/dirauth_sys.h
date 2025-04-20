/* Copyright (c) 2001-2004, Roger Dingledine.
 * Copyright (c) 2004-2006, Roger Dingledine, Nick Mathewson.
 * Copyright (c) 2007-2021, The Tor Project, Inc. */
/* See LICENSE for licensing information */

/**
 * \file dirauth_sys.h
 * \brief Headers for dirauth_sys.c
 **/

#ifndef TOR_DIRAUTH_SYS_H
#define TOR_DIRAUTH_SYS_H

#include "core/or/or.h"
#include "feature/dirauth/dirauth_periodic.h"
#include "feature/dirauth/dirvote.h"
#include "feature/dirauth/shared_random.h"
#include "feature/dirauth/voting_schedule.h"

int dirauth_init(void);
void dirauth_free_all(void);

struct dirauth_options_t;
const struct dirauth_options_t *dirauth_get_options(void);

extern const struct subsys_fns_t sys_dirauth;

/**
 * Subsystem level for the directory-authority system.
 *
 * Defined here so that it can be shared between the real and stub
 * definitions.
 **/
#define DIRAUTH_SUBSYS_LEVEL 70

#ifdef DIRAUTH_SYS_PRIVATE
STATIC int dirauth_set_options(void *arg);
#endif

#endif /* !defined(TOR_DIRAUTH_SYS_H) */
