#include "photon_linux.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static void* linux_alloc(size_t size) {
    return malloc(size);
}

static void* linux_realloc(void* ptr, size_t size) {
    return realloc(ptr, size);
}

static void linux_free(void* ptr) {
    free(ptr);
}

static void linux_log(const char* msg) {
    fprintf(stderr, "[PhotonDB] %s\n", msg);
}

static void linux_memcpy(void* dst, const void* src, size_t n) {
    memcpy(dst, src, n);
}

static void linux_memmove(void* dst, const void* src, size_t n) {
    memmove(dst, src, n);
}

static void linux_memset(void* s, int c, size_t n) {
    memset(s, c, n);
}

static void* linux_fopen(const char* filename, const char* mode) {
    return fopen(filename, mode);
}

static size_t linux_fwrite(const void* ptr, size_t size, size_t nmemb, void* stream) {
    return fwrite(ptr, size, nmemb, (FILE*)stream);
}

static size_t linux_fread(void* ptr, size_t size, size_t nmemb, void* stream) {
    return fread(ptr, size, nmemb, (FILE*)stream);
}

static int linux_fclose(void* stream) {
    return fclose((FILE*)stream);
}

PhotonInitStruct photon_get_linux_init_struct() {
    PhotonInitStruct init = {
        .alloc = linux_alloc,
        .realloc = linux_realloc,
        .free = linux_free,
        .memcpy = linux_memcpy,
        .memmove = linux_memmove,
        .memset = linux_memset,
        .log = linux_log,
        .fopen = linux_fopen,
        .fwrite = linux_fwrite,
        .fread = linux_fread,
        .fclose = linux_fclose
    };
    return init;
}
