#ifndef PHOTON_POSIX_H
#define PHOTON_POSIX_H

#include "hooks.h"

/**
 * Returns a PhotonInitStruct populated with default POSIX implementations
 * using standard C library functions (malloc, free, fopen, etc.).
 */
PhotonInitStruct photon_get_posix_init_struct();

#endif // PHOTON_POSIX_H
