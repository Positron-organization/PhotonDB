#include <stddef.h>

typedef struct {
  void* (*alloc)(size_t size);
  void* (*realloc)(void* ptr, size_t size);
  void  (*free)(void* ptr);
  void  (*memcpy)(void* dst, const void* src, size_t n);
  void  (*memmove)(void* dst, const void* src, size_t n);
  void  (*memset)(void* s, int c, size_t n);
  void  (*log)(const char* msg);
} PhotonInitStruct;

void photonInit(PhotonInitStruct init);

PhotonInitStruct* photonGetInitStruct();
