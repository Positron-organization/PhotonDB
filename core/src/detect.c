#include "detect.h"
#include "hooks.h"

static PhotonInitStruct* hooks = NULL;

void photonDetectInit() {
    hooks = photonGetInitStruct();
}

void photon_get_env_info(PhotonEnvInfo* info) {
    if (!info) return;

    /* OS Name */
#if defined(PHOTON_OS_WINDOWS)
    info->os_name = "Windows";
#elif defined(PHOTON_OS_LINUX)
    info->os_name = "Linux";
#elif defined(PHOTON_OS_MACOS)
    info->os_name = "macOS";
#elif defined(PHOTON_OS_FREERTOS)
    info->os_name = "FreeRTOS";
#else
    info->os_name = "Unknown/Bare-metal";
#endif

    /* Arch Name */
#if defined(PHOTON_ARCH_X86_64)
    info->arch_name = "x86_64";
#elif defined(PHOTON_ARCH_X86)
    info->arch_name = "x86";
#elif defined(PHOTON_ARCH_ARM64)
    info->arch_name = "ARM64";
#elif defined(PHOTON_ARCH_ARM)
    info->arch_name = "ARM";
#elif defined(PHOTON_ARCH_RISCV)
    info->arch_name = "RISC-V";
#else
    info->arch_name = "Unknown";
#endif

    info->is_mcu = PHOTON_IS_MCU;

    /* SIMD Support */
#if defined(PHOTON_HAVE_SSE2)
    info->simd.has_sse2 = true;
#else
    info->simd.has_sse2 = false;
#endif

#if defined(PHOTON_HAVE_AVX)
    info->simd.has_avx = true;
#else
    info->simd.has_avx = false;
#endif

#if defined(PHOTON_HAVE_AVX2)
    info->simd.has_avx2 = true;
#else
    info->simd.has_avx2 = false;
#endif

#if defined(PHOTON_HAVE_NEON)
    info->simd.has_neon = true;
#else
    info->simd.has_neon = false;
#endif
}

void photon_log_env_info(void) {
    if (hooks == NULL || hooks->log == NULL) return;

    PhotonEnvInfo info;
    photon_get_env_info(&info);

    hooks->log("--- PhotonDB Environment Info ---");
    hooks->log(info.os_name);
    hooks->log(info.arch_name);
    hooks->log(info.is_mcu ? "Target: MCU" : "Target: Desktop/Server");
    
    if (info.simd.has_sse2) hooks->log("SIMD: SSE2 detected");
    if (info.simd.has_avx)  hooks->log("SIMD: AVX detected");
    if (info.simd.has_avx2) hooks->log("SIMD: AVX2 detected");
    if (info.simd.has_neon) hooks->log("SIMD: NEON detected");
    hooks->log("---------------------------------");
}
