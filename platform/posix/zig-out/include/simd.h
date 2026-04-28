#ifndef PHOTON_SIMD_H
#define PHOTON_SIMD_H

#include <stddef.h>
#include <stdint.h>
#include "detect.h"

/**
 * Optimized Dot Product for F32 vectors.
 */
float photon_dot_product_f32_simd(const float* a, const float* b, size_t dim);

/**
 * Optimized L2 Distance Squared for F32 vectors.
 */
float photon_l2_distance_sq_f32_simd(const float* a, const float* b, size_t dim);

/**
 * Optimized Dot Product for I8 vectors.
 */
float photon_dot_product_i8_simd(const int8_t* a, const int8_t* b, size_t dim);

/**
 * Optimized L2 Distance Squared for I8 vectors.
 */
float photon_l2_distance_sq_i8_simd(const int8_t* a, const int8_t* b, size_t dim);

#endif // PHOTON_SIMD_H
