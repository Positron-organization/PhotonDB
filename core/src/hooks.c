#include "hooks.h"
#include <stddef.h>

static PhotonInitStruct h = {
    .alloc = NULL,
    .realloc = NULL,
    .free = NULL,
    .memcpy = NULL,
    .memmove = NULL,
    .memset = NULL,
    .log = NULL
};

void photonInit(PhotonInitStruct init) {
  h = init;
}

PhotonInitStruct* photonGetInitStruct() {
  return &h;
}
