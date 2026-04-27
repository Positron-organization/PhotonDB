#include <stddef.h>

typedef struct {
  void* (*alloc)(size_t size);
  void* (*realloc)(void* ptr, size_t size);
  void  (*free)(void* ptr);
  void  (*memcpy)(void* dst, const void* src, size_t n);
  void  (*memmove)(void* dst, const void* src, size_t n);
  void  (*memset)(void* s, int c, size_t n);
  void  (*log)(const char* msg);

  // File I/O
  void* (*fopen)(const char* filename, const char* mode);
  size_t (*fwrite)(const void* ptr, size_t size, size_t nmemb, void* stream);
  size_t (*fread)(void* ptr, size_t size, size_t nmemb, void* stream);
  int    (*fclose)(void* stream);
} PhotonInitStruct;

void photonInit(PhotonInitStruct init);

PhotonInitStruct* photonGetInitStruct();
