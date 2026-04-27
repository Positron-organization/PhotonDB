#include "hooks.h"
#include <stddef.h>

static PhotonInitStruct h = {
    .alloc = NULL,
    .realloc = NULL,
    .free = NULL,
    .memcpy = NULL,
    .memmove = NULL,
    .memset = NULL,
    .log = NULL,
    .fopen = NULL,
    .fwrite = NULL,
    .fread = NULL,
    .fclose = NULL
};

void photonInit(PhotonInitStruct init) {
  h = init;
}

PhotonInitStruct* photonGetInitStruct() {
  return &h;
}
