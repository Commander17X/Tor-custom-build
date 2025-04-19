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

/* Structure definitions */
struct double_digest_map {
  struct ddmap_entry_t **hth_table;
  unsigned hth_table_length;
  unsigned hth_n_entries;
  unsigned hth_load_limit;
  int hth_prime_idx;
};

struct ddmap_entry_t {
  struct ddmap_entry_t *node;
  char rsa_sha1[DIGEST_LEN];
  char ed25519[DIGEST_LEN];
  int n_votes;
  networkstatus_t **votes;
};

struct dircollator_t {
  int n_votes;
  int n_authorities;
  digestmap_t *by_rsa_sha1;
  struct double_digest_map by_both_ids;
};

/* Function declarations */
int dircollate_init(void);
void dircollate_cleanup(void);

#endif /* !defined(TOR_DIRCOLLATE_H) */ 