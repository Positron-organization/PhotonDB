#include "simd.h"

#if defined(PHOTON_HAVE_AVX) || defined(PHOTON_HAVE_AVX2)
#include <immintrin.h>
#endif

#if defined(PHOTON_HAVE_SSE2)
#include <emmintrin.h>
#endif

#if defined(PHOTON_HAVE_NEON)
#include <arm_neon.h>
#endif

#if defined(PHOTON_HAVE_RVV)
#include <riscv_vector.h>
#endif

/* --- F32 Dot Product --- */

float photon_dot_product_f32_simd(const float* a, const float* b, size_t dim) {
    size_t i = 0;
    float sum = 0.0f;

#if defined(PHOTON_HAVE_AVX)
    __m256 vsum = _mm256_setzero_ps();
    for (; i + 7 < dim; i += 8) {
        __m256 va = _mm256_loadu_ps(a + i);
        __m256 vb = _mm256_loadu_ps(b + i);
        vsum = _mm256_add_ps(vsum, _mm256_mul_ps(va, vb));
    }
    float tmp[8];
    _mm256_storeu_ps(tmp, vsum);
    for (int j = 0; j < 8; j++) sum += tmp[j];
#elif defined(PHOTON_HAVE_SSE2)
    __m128 vsum = _mm_setzero_ps();
    for (; i + 3 < dim; i += 4) {
        __m128 va = _mm_loadu_ps(a + i);
        __m128 vb = _mm_loadu_ps(b + i);
        vsum = _mm_add_ps(vsum, _mm_mul_ps(va, vb));
    }
    float tmp[4];
    _mm_storeu_ps(tmp, vsum);
    for (int j = 0; j < 4; j++) sum += tmp[j];
#elif defined(PHOTON_HAVE_NEON)
    float32x4_t vsum = vdupq_n_f32(0.0f);
    for (; i + 3 < dim; i += 4) {
        float32x4_t va = vld1q_f32(a + i);
        float32x4_t vb = vld1q_f32(b + i);
        vsum = vaddq_f32(vsum, vmulq_f32(va, vb));
    }
    sum = vgetq_lane_f32(vsum, 0) + vgetq_lane_f32(vsum, 1) + vgetq_lane_f32(vsum, 2) + vgetq_lane_f32(vsum, 3);
#elif defined(PHOTON_HAVE_RVV)
    size_t n = dim;
    const float *pa = a;
    const float *pb = b;
    vfloat32m1_t v_acc = __riscv_vfmv_v_f_f32m1(0.0f, __riscv_vsetvlmax_e32m1());
    for (size_t vl; n > 0; n -= vl, pa += vl, pb += vl) {
        vl = __riscv_vsetvl_e32m1(n);
        vfloat32m1_t va = __riscv_vle32_v_f32m1(pa, vl);
        vfloat32m1_t vb = __riscv_vle32_v_f32m1(pb, vl);
        v_acc = __riscv_vfmacc_vv_f32m1(v_acc, va, vb, vl);
    }
    vfloat32m1_t v_res = __riscv_vfmv_v_f_f32m1(0.0f, 1);
    v_res = __riscv_vfredusum_vs_f32m1_f32m1(v_acc, v_res, __riscv_vsetvlmax_e32m1());
    sum = __riscv_vfmv_f_s_f32m1_f(v_res);
    i = dim; // Mark as done
#endif

    for (; i < dim; i++) {
        sum += a[i] * b[i];
    }
    return sum;
}

/* --- F32 L2 Distance Squared --- */

float photon_l2_distance_sq_f32_simd(const float* a, const float* b, size_t dim) {
    size_t i = 0;
    float sum = 0.0f;

#if defined(PHOTON_HAVE_AVX)
    __m256 vsum = _mm256_setzero_ps();
    for (; i + 7 < dim; i += 8) {
        __m256 va = _mm256_loadu_ps(a + i);
        __m256 vb = _mm256_loadu_ps(b + i);
        __m256 diff = _mm256_sub_ps(va, vb);
        vsum = _mm256_add_ps(vsum, _mm256_mul_ps(diff, diff));
    }
    float tmp[8];
    _mm256_storeu_ps(tmp, vsum);
    for (int j = 0; j < 8; j++) sum += tmp[j];
#elif defined(PHOTON_HAVE_SSE2)
    __m128 vsum = _mm_setzero_ps();
    for (; i + 3 < dim; i += 4) {
        __m128 va = _mm_loadu_ps(a + i);
        __m128 vb = _mm_loadu_ps(b + i);
        __m128 diff = _mm_sub_ps(va, vb);
        vsum = _mm_add_ps(vsum, _mm_mul_ps(diff, diff));
    }
    float tmp[4];
    _mm_storeu_ps(tmp, vsum);
    for (int j = 0; j < 4; j++) sum += tmp[j];
#elif defined(PHOTON_HAVE_NEON)
    float32x4_t vsum = vdupq_n_f32(0.0f);
    for (; i + 3 < dim; i += 4) {
        float32x4_t va = vld1q_f32(a + i);
        float32x4_t vb = vld1q_f32(b + i);
        float32x4_t diff = vsubq_f32(va, vb);
        vsum = vaddq_f32(vsum, vmulq_f32(diff, diff));
    }
    sum = vgetq_lane_f32(vsum, 0) + vgetq_lane_f32(vsum, 1) + vgetq_lane_f32(vsum, 2) + vgetq_lane_f32(vsum, 3);
#elif defined(PHOTON_HAVE_RVV)
    size_t n = dim;
    const float *pa = a;
    const float *pb = b;
    vfloat32m1_t v_acc = __riscv_vfmv_v_f_f32m1(0.0f, __riscv_vsetvlmax_e32m1());
    for (size_t vl; n > 0; n -= vl, pa += vl, pb += vl) {
        vl = __riscv_vsetvl_e32m1(n);
        vfloat32m1_t va = __riscv_vle32_v_f32m1(pa, vl);
        vfloat32m1_t vb = __riscv_vle32_v_f32m1(pb, vl);
        vfloat32m1_t vd = __riscv_vfsub_vv_f32m1(va, vb, vl);
        v_acc = __riscv_vfmacc_vv_f32m1(v_acc, vd, vd, vl);
    }
    vfloat32m1_t v_res = __riscv_vfmv_v_f_f32m1(0.0f, 1);
    v_res = __riscv_vfredusum_vs_f32m1_f32m1(v_acc, v_res, __riscv_vsetvlmax_e32m1());
    sum = __riscv_vfmv_f_s_f32m1_f(v_res);
    i = dim;
#endif

    for (; i < dim; i++) {
        float diff = a[i] - b[i];
        sum += diff * diff;
    }
    return sum;
}

/* --- I8 Dot Product --- */

float photon_dot_product_i8_simd(const int8_t* a, const int8_t* b, size_t dim) {
    size_t i = 0;
    int32_t sum = 0;

#if defined(PHOTON_HAVE_AVX2)
    __m256i vsum = _mm256_setzero_si256();
    for (; i + 31 < dim; i += 32) {
        __m256i va = _mm256_loadu_si256((const __m256i*)(a + i));
        __m256i vb = _mm256_loadu_si256((const __m256i*)(b + i));
        
        // This is a common trick for i8 dot product with AVX2
        // vpmaddubsw multiplies unsigned i8 and signed i8.
        // But we have signed i8 and signed i8.
        // For signed i8, we can use _mm256_maddubs_epi16 if we adjust one of them, 
        // but it's easier to just sign-extend to i16.
        
        __m256i va_lo = _mm256_cvtepi8_epi16(_mm256_extracti128_si256(va, 0));
        __m256i va_hi = _mm256_cvtepi8_epi16(_mm256_extracti128_si256(va, 1));
        __m256i vb_lo = _mm256_cvtepi8_epi16(_mm256_extracti128_si256(vb, 0));
        __m256i vb_hi = _mm256_cvtepi8_epi16(_mm256_extracti128_si256(vb, 1));
        
        vsum = _mm256_add_epi32(vsum, _mm256_madd_epi16(va_lo, vb_lo));
        vsum = _mm256_add_epi32(vsum, _mm256_madd_epi16(va_hi, vb_hi));
    }
    int32_t tmp[8];
    _mm256_storeu_si256((__m256i*)tmp, vsum);
    for (int j = 0; j < 8; j++) sum += tmp[j];
#elif defined(PHOTON_HAVE_SSE2)
    __m128i vsum = _mm_setzero_si128();
    for (; i + 15 < dim; i += 16) {
        __m128i va = _mm_loadu_si128((const __m128i*)(a + i));
        __m128i vb = _mm_loadu_si128((const __m128i*)(b + i));
        
        __m128i va_lo = _mm_cvtepi8_epi16(va);
        __m128i va_hi = _mm_cvtepi8_epi16(_mm_srli_si128(va, 8));
        __m128i vb_lo = _mm_cvtepi8_epi16(vb);
        __m128i vb_hi = _mm_cvtepi8_epi16(_mm_srli_si128(vb, 8));

        vsum = _mm_add_epi32(vsum, _mm_madd_epi16(va_lo, vb_lo));
        vsum = _mm_add_epi32(vsum, _mm_madd_epi16(va_hi, vb_hi));
    }
    int32_t tmp[4];
    _mm_storeu_si128((__m128i*)tmp, vsum);
    for (int j = 0; j < 4; j++) sum += tmp[j];
#elif defined(PHOTON_HAVE_NEON)
    int32x4_t vsum = vdupq_n_s32(0);
    for (; i + 7 < dim; i += 8) {
        int8x8_t va = vld1_s8(a + i);
        int8x8_t vb = vld1_s8(b + i);
        int16x8_t prod = vmull_s8(va, vb);
        vsum = vaddw_s16(vsum, vget_low_s16(prod));
        vsum = vaddw_s16(vsum, vget_high_s16(prod));
    }
    sum = vgetq_lane_s32(vsum, 0) + vgetq_lane_s32(vsum, 1) + vgetq_lane_s32(vsum, 2) + vgetq_lane_s32(vsum, 3);
#elif defined(PHOTON_HAVE_RVV)
    size_t n = dim;
    const int8_t *pa = a;
    const int8_t *pb = b;
    vint32m1_t v_acc = __riscv_vmv_v_x_i32m1(0, __riscv_vsetvlmax_e32m1());
    for (size_t vl; n > 0; n -= vl, pa += vl, pb += vl) {
        vl = __riscv_vsetvl_e8mf2(n);
        vint8mf2_t va8 = __riscv_vle8_v_i8mf2(pa, vl);
        vint8mf2_t vb8 = __riscv_vle8_v_i8mf2(pb, vl);
        vint16m1_t va16 = __riscv_vsext_vf2_i16m1(va8, vl);
        vint16m1_t vb16 = __riscv_vsext_vf2_i16m1(vb8, vl);
        v_acc = __riscv_vwmacc_vv_i32m1(v_acc, va16, vb16, vl);
    }
    vint32m1_t v_res = __riscv_vmv_v_x_i32m1(0, 1);
    v_res = __riscv_vfredsum_vs_i32m1_i32m1(v_acc, v_res, __riscv_vsetvlmax_e32m1());
    sum = (float)__riscv_vmv_x_s_i32m1_i32(v_res);
    i = dim;
#endif

    for (; i < dim; i++) {
        sum += (int32_t)a[i] * (int32_t)b[i];
    }
    return (float)sum;
}

/* --- I8 L2 Distance Squared --- */

float photon_l2_distance_sq_i8_simd(const int8_t* a, const int8_t* b, size_t dim) {
    size_t i = 0;
    uint32_t sum = 0;

#if defined(PHOTON_HAVE_AVX2)
    __m256i vsum = _mm256_setzero_si256();
    for (; i + 31 < dim; i += 32) {
        __m256i va = _mm256_loadu_si256((const __m256i*)(a + i));
        __m256i vb = _mm256_loadu_si256((const __m256i*)(b + i));
        
        __m256i va_lo = _mm256_cvtepi8_epi16(_mm256_extracti128_si256(va, 0));
        __m256i va_hi = _mm256_cvtepi8_epi16(_mm256_extracti128_si256(va, 1));
        __m256i vb_lo = _mm256_cvtepi8_epi16(_mm256_extracti128_si256(vb, 0));
        __m256i vb_hi = _mm256_cvtepi8_epi16(_mm256_extracti128_si256(vb, 1));
        
        __m256i diff_lo = _mm256_sub_epi16(va_lo, vb_lo);
        __m256i diff_hi = _mm256_sub_epi16(va_hi, vb_hi);
        
        vsum = _mm256_add_epi32(vsum, _mm256_madd_epi16(diff_lo, diff_lo));
        vsum = _mm256_add_epi32(vsum, _mm256_madd_epi16(diff_hi, diff_hi));
    }
    uint32_t tmp[8];
    _mm256_storeu_si256((__m256i*)tmp, vsum);
    for (int j = 0; j < 8; j++) sum += tmp[j];
#elif defined(PHOTON_HAVE_SSE2)
    __m128i vsum = _mm_setzero_si128();
    for (; i + 15 < dim; i += 16) {
        __m128i va = _mm_loadu_si128((const __m128i*)(a + i));
        __m128i vb = _mm_loadu_si128((const __m128i*)(b + i));
        
        __m128i va_lo = _mm_cvtepi8_epi16(va);
        __m128i va_hi = _mm_cvtepi8_epi16(_mm_srli_si128(va, 8));
        __m128i vb_lo = _mm_cvtepi8_epi16(vb);
        __m128i vb_hi = _mm_cvtepi8_epi16(_mm_srli_si128(vb, 8));

        __m128i diff_lo = _mm_sub_epi16(va_lo, vb_lo);
        __m128i diff_hi = _mm_sub_epi16(va_hi, vb_hi);

        vsum = _mm_add_epi32(vsum, _mm_madd_epi16(diff_lo, diff_lo));
        vsum = _mm_add_epi32(vsum, _mm_madd_epi16(diff_hi, diff_hi));
    }
    uint32_t tmp[4];
    _mm_storeu_si128((__m128i*)tmp, vsum);
    for (int j = 0; j < 4; j++) sum += tmp[j];
#elif defined(PHOTON_HAVE_NEON)
    uint32x4_t vsum = vdupq_n_u32(0);
    for (; i + 7 < dim; i += 8) {
        int8x8_t va = vld1_s8(a + i);
        int8x8_t vb = vld1_s8(b + i);
        int16x8_t diff = vsubl_s8(va, vb);
        int32x4_t sq_lo = vmull_s16(vget_low_s16(diff), vget_low_s16(diff));
        int32x4_t sq_hi = vmull_s16(vget_high_s16(diff), vget_high_s16(diff));
        vsum = vaddq_u32(vsum, vreinterpretq_u32_s32(sq_lo));
        vsum = vaddq_u32(vsum, vreinterpretq_u32_s32(sq_hi));
    }
    sum = vgetq_lane_u32(vsum, 0) + vgetq_lane_u32(vsum, 1) + vgetq_lane_u32(vsum, 2) + vgetq_lane_u32(vsum, 3);
#elif defined(PHOTON_HAVE_RVV)
    size_t n = dim;
    const int8_t *pa = a;
    const int8_t *pb = b;
    vint32m1_t v_acc = __riscv_vmv_v_x_i32m1(0, __riscv_vsetvlmax_e32m1());
    for (size_t vl; n > 0; n -= vl, pa += vl, pb += vl) {
        vl = __riscv_vsetvl_e8mf2(n);
        vint8mf2_t va8 = __riscv_vle8_v_i8mf2(pa, vl);
        vint8mf2_t vb8 = __riscv_vle8_v_i8mf2(pb, vl);
        vint16m1_t va16 = __riscv_vsext_vf2_i16m1(va8, vl);
        vint16m1_t vb16 = __riscv_vsext_vf2_i16m1(vb8, vl);
        vint16m1_t vd16 = __riscv_vsub_vv_i16m1(va16, vb16, vl);
        v_acc = __riscv_vwmacc_vv_i32m1(v_acc, vd16, vd16, vl);
    }
    vint32m1_t v_res = __riscv_vmv_v_x_i32m1(0, 1);
    v_res = __riscv_vfredsum_vs_i32m1_i32m1(v_acc, v_res, __riscv_vsetvlmax_e32m1());
    sum = (float)__riscv_vmv_x_s_i32m1_i32(v_res);
    i = dim;
#endif

    for (; i < dim; i++) {
        int32_t diff = (int32_t)a[i] - (int32_t)b[i];
        sum += (uint32_t)(diff * diff);
    }
    return (float)sum;
}
