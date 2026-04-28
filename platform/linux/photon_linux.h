#ifndef PHOTON_LINUX_H
#define PHOTON_LINUX_H

#include "hooks.h"

/**
 * Returns a PhotonInitStruct populated with default Linux/POSIX implementations
 * using standard C library functions (malloc, free, fopen, etc.).
 */
PhotonInitStruct photon_get_linux_init_struct();

#endif // PHOTON_LINUX_H
