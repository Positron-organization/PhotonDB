#include "photon_posix.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static void* posix_alloc(size_t size) {
    return malloc(size);
}

static void* posix_realloc(void* ptr, size_t size) {
    return realloc(ptr, size);
}

static void posix_free(void* ptr) {
    free(ptr);
}

static void posix_log(const char* msg) {
    printf("[PhotonDB] %s\n", msg);
}

static void posix_memcpy(void* dst, const void* src, size_t n) {
    memcpy(dst, src, n);
}

static void posix_memmove(void* dst, const void* src, size_t n) {
    memmove(dst, src, n);
}

static void posix_memset(void* s, int c, size_t n) {
    memset(s, c, n);
}

static void* posix_fopen(const char* filename, const char* mode) {
    return (void*)fopen(filename, mode);
}

static size_t posix_fwrite(const void* ptr, size_t size, size_t nmemb, void* stream) {
    return fwrite(ptr, size, nmemb, (FILE*)stream);
}

static size_t posix_fread(void* ptr, size_t size, size_t nmemb, void* stream) {
    return fread(ptr, size, nmemb, (FILE*)stream);
}

static int posix_fclose(void* stream) {
    return fclose((FILE*)stream);
}

PhotonInitStruct photon_get_posix_init_struct() {
    return (PhotonInitStruct){
        .alloc = posix_alloc,
        .realloc = posix_realloc,
        .free = posix_free,
        .memcpy = posix_memcpy,
        .memmove = posix_memmove,
        .memset = posix_memset,
        .log = posix_log,
        .fopen = posix_fopen,
        .fwrite = posix_fwrite,
        .fread = posix_fread,
        .fclose = posix_fclose
    };
}
