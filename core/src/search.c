#include <stdint.h>
#include "search.h"
#include "hooks.h"

static float dot_product_f32(const float* a, const float* b, size_t dim) {
  float sum = 0.0f;
  for (size_t i = 0; i < dim; i++) {
    sum += a[i] * b[i];
  }
  return sum;
}

static float dot_product_i8(const int8_t* a, const int8_t* b, size_t dim) {
  int32_t sum = 0;
  for (size_t i = 0; i < dim; i++) {
    sum += (int32_t)a[i] * (int32_t)b[i];
  }
  return (float)sum;
}

int photon_db_search_dot_product(PhotonDB* db, const void* query_vector, size_t k, PhotonSearchResult* results) {
  if (db == NULL || query_vector == NULL || results == NULL || k == 0) return 0;
  
  size_t found = 0;
  size_t element_size;
  if (db->dtype == PHOTON_VECTOR_F32) {
    element_size = sizeof(float);
  } else if (db->dtype == PHOTON_VECTOR_I8) {
    element_size = sizeof(int8_t);
  } else {
    return 0;
  }

  size_t vector_bytes = db->dim * element_size;

  for (size_t i = 0; i < db->count; i++) {
    float score = 0.0f;
    void* current_vector = (uint8_t*)db->data + (i * vector_bytes);
    
    if (db->dtype == PHOTON_VECTOR_F32) {
      score = dot_product_f32((const float*)query_vector, (const float*)current_vector, db->dim);
    } else {
      score = dot_product_i8((const int8_t*)query_vector, (const int8_t*)current_vector, db->dim);
    }

    size_t id = db->list_of_id[i];

    // Maintain top-k results
    if (found < k) {
      // Find insertion point
      size_t pos = found;
      while (pos > 0 && results[pos - 1].score < score) {
        results[pos] = results[pos - 1];
        pos--;
      }
      results[pos].id = id;
      results[pos].score = score;
      found++;
    } else if (score > results[k - 1].score) {
      // Find insertion point
      size_t pos = k - 1;
      while (pos > 0 && results[pos - 1].score < score) {
        results[pos] = results[pos - 1];
        pos--;
      }
      results[pos].id = id;
      results[pos].score = score;
    }
  }

  return (int)found;
}
