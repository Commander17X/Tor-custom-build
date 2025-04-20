/* Copyright (c) 2025, Neon Secure Network */
/* See LICENSE for licensing information */

#include "config.h"
#include "core/or/or.h"
#include "feature/sn/websites/neon-secure/config.h"

static int
neon_secure_init(void)
{
  log_info(LD_GENERAL, "Initializing %s website", NEON_SECURE_WEBSITE_NAME);
  return 0;
}

static void
neon_secure_free_all(void)
{
  log_info(LD_GENERAL, "Freeing %s website resources", NEON_SECURE_WEBSITE_NAME);
}

const struct subsys_fns_t sys_neon_secure = {
  .name = NEON_SECURE_WEBSITE_NAME,
  .supported = true,
  .level = 80,
  .initialize = neon_secure_init,
  .shutdown = neon_secure_free_all,
}; 