/* ----------------------------------------------------------------------------- 
 * return.c
 *
 *     This file manages the set of return-points for the WAD signal handler.
 * 
 * Author(s) : David Beazley (beazley@cs.uchicago.edu)
 *
 * Copyright (C) 2000.  The University of Chicago
 * See the file LICENSE for information on usage and redistribution.	
 * ----------------------------------------------------------------------------- */

#include "wad.h"

/* Maximum number of return points */
#define WAD_NUMBER_RETURN  128

static WadReturnFunc return_points[WAD_NUMBER_RETURN];
static int           num_return = 0;

void wad_set_return(const char *name, long value) {
  WadReturnFunc *rp;
  rp = &return_points[num_return];
  strcpy(rp->name,name);
  rp->value = value;
  num_return++;
  if (wad_debug_mode & DEBUG_RETURN) {
    printf("wad: Setting return ('%s', %d)\n", name,value);
  }
}

void wad_set_returns(WadReturnFunc *rf) {
  int i = 0;
  while (strlen(rf[i].name)) {
    wad_set_return(rf[i].name, rf[i].value);
    i++;
  }
}

WadReturnFunc *wad_check_return(const char *name) {
  int i;
  for (i = 0; i < num_return; i++) {
    if (strcmp(name,return_points[i].name) == 0) {
      if (wad_debug_mode & DEBUG_RETURN) {
	printf("wad: Found return ('%s', %d)\n", return_points[i].name, return_points[i].value);
      }
      return &return_points[i];
    }
  }
  return 0;
}

