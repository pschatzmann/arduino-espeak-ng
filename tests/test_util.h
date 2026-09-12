#pragma once
#include <stdio.h>
#include <stdlib.h>

// Minimal, dependency-free test harness: no external framework is used
// anywhere else in this project, so we keep this consistent with that.
static int g_test_failures = 0;

#define CHECK(cond, msg)                                                     \
  do {                                                                       \
    if (!(cond)) {                                                           \
      fprintf(stderr, "FAIL %s:%d: %s (%s)\n", __FILE__, __LINE__, #cond,    \
              msg);                                                          \
      g_test_failures++;                                                     \
    } else {                                                                 \
      printf("PASS %s:%d: %s\n", __FILE__, __LINE__, #cond);                 \
    }                                                                        \
  } while (0)

#define TEST_EXIT() return g_test_failures == 0 ? 0 : 1
