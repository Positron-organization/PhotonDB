#ifndef PHOTON_DETECT_H
#define PHOTON_DETECT_H

#include <stdbool.h>

/* --- OS Detection --- */
#if defined(_WIN32) || defined(_WIN64)
    #define PHOTON_OS_WINDOWS
#elif defined(__linux__)
    #define PHOTON_OS_LINUX
#elif defined(__APPLE__) && defined(__MACH__)
    #define PHOTON_OS_MACOS
#elif defined(__FreeRTOS__)
    #define PHOTON_OS_FREERTOS
#else
    #define PHOTON_OS_UNKNOWN
#endif

/* --- Architecture Detection --- */
#if defined(__x86_64__) || defined(_M_X64)
    #define PHOTON_ARCH_X86_64
#elif defined(__i386__) || defined(_M_IX86)
    #define PHOTON_ARCH_X86
#elif defined(__aarch64__) || defined(_M_ARM64)
    #define PHOTON_ARCH_ARM64
#elif defined(__arm__) || defined(_M_ARM)
    #define PHOTON_ARCH_ARM
#elif defined(__riscv)
    #define PHOTON_ARCH_RISCV
#else
    #define PHOTON_ARCH_UNKNOWN
#endif

/* --- MCU Detection --- */
/* Simple heuristic: if we are not on a major desktop OS, assume MCU/Bare-metal */
#if !defined(PHOTON_OS_WINDOWS) && !defined(PHOTON_OS_LINUX) && !defined(PHOTON_OS_MACOS)
    #define PHOTON_IS_MCU 1
#else
    #define PHOTON_IS_MCU 0
#endif

/* --- SIMD Support (Compile-time) --- */
#if defined(__SSE2__) || (defined(_M_IX86_FP) && _M_IX86_FP >= 2) || defined(_M_X64)
    #define PHOTON_HAVE_SSE2
#endif

#if defined(__AVX__)
    #define PHOTON_HAVE_AVX
#endif

#if defined(__AVX2__)
    #define PHOTON_HAVE_AVX2
#endif

#if defined(__ARM_NEON) || defined(__ARM_NEON__)
    #define PHOTON_HAVE_NEON
#endif

#if defined(__riscv_vector)
    #define PHOTON_HAVE_RVV
#endif

/* --- Runtime Detection API --- */

typedef struct {
    const char* os_name;
    const char* arch_name;
    bool is_mcu;
    struct {
        bool has_sse2;
        bool has_avx;
        bool has_avx2;
        bool has_neon;
        bool has_rvv;
    } simd;
} PhotonEnvInfo;

/**
 * Initialize detection module with hooks.
 */
void photonDetectInit();

/**
 * Get information about the current environment.
 */
void photon_get_env_info(PhotonEnvInfo* info);

/**
 * Print environment information to the configured log hook.
 */
void photon_log_env_info(void);

#endif // PHOTON_DETECT_H
