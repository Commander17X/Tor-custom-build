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
#include "feature/nodelist/routerlist.h"

/* Constants for bandwidth measurement */
#define MAX_MEASUREMENT_AGE (24*60*60)  /* 24 hours */
#define MAX_BW_FILE_HEADER_COUNT_IN_VOTE 10
#define BW_FILE_HEADERS_TERMINATOR "-----END BW MEASUREMENT-----"

/* Structure for measured bandwidth line */
typedef struct measured_bw_line_t {
  char node_id[DIGEST_LEN];
  uint32_t bw_kb;
  time_t as_of;
} measured_bw_line_t;

/* Function declarations */
void bwauth_init(void);
void bwauth_cleanup(void);

/* Bandwidth measurement functions */
int dirserv_count_measured_bws(const smartlist_t *routers);
int dirserv_get_last_n_measured_bws(void);
void dirserv_cache_measured_bw(const measured_bw_line_t *parsed_line,
                              time_t as_of);
void dirserv_clear_measured_bw_cache(void);
void dirserv_expire_measured_bw_cache(time_t now);
int dirserv_query_measured_bw_cache_kb(const char *node_id, long *bw_kb_out,
                                     time_t *as_of_out);
int dirserv_has_measured_bw(const char *node_id);
int dirserv_get_measured_bw_cache_size(void);
uint32_t dirserv_get_credible_bandwidth_kb(const routerinfo_t *ri);
int dirserv_read_measured_bandwidths(const char *from_file,
                                   smartlist_t *routerstatuses);
int measured_bw_line_parse(measured_bw_line_t *out, const char *orig_line,
                          time_t as_of);
int measured_bw_line_apply(measured_bw_line_t *parsed_line,
                          smartlist_t *routerstatuses);

#endif /* !defined(TOR_BWAUTH_H) */ 