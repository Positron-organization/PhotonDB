#include <stdint.h>
#include "search.h"
#include "hooks.h"

/* --- Internal Math Helpers --- */

static float photon_sqrtf(float x) {
    if (x <= 0) return 0;
    float z = x;
    /* 5 iterations of Newton-Raphson is usually enough for float precision */
    for (int i = 0; i < 5; i++) {
        z = 0.5f * (z + x / z);
    }
    return z;
}

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

static float l2_distance_sq_f32(const float* a, const float* b, size_t dim) {
    float sum = 0.0f;
    for (size_t i = 0; i < dim; i++) {
        float diff = a[i] - b[i];
        sum += diff * diff;
    }
    return sum;
}

static float l2_distance_sq_i8(const int8_t* a, const int8_t* b, size_t dim) {
    uint32_t sum = 0;
    for (size_t i = 0; i < dim; i++) {
        int32_t diff = (int32_t)a[i] - (int32_t)b[i];
        sum += (uint32_t)(diff * diff);
    }
    return (float)sum;
}

/* --- Top-K Management --- */

static void insert_result(PhotonSearchResult* results, size_t* found, size_t k, size_t id, float score, bool lower_is_better) {
    if (*found < k) {
        size_t pos = *found;
        while (pos > 0) {
            bool condition = lower_is_better ? (results[pos - 1].score > score) : (results[pos - 1].score < score);
            if (!condition) break;
            results[pos] = results[pos - 1];
            pos--;
        }
        results[pos].id = id;
        results[pos].score = score;
        (*found)++;
    } else {
        bool condition = lower_is_better ? (score < results[k - 1].score) : (score > results[k - 1].score);
        if (condition) {
            size_t pos = k - 1;
            while (pos > 0) {
                bool sub_condition = lower_is_better ? (results[pos - 1].score > score) : (results[pos - 1].score < score);
                if (!sub_condition) break;
                results[pos] = results[pos - 1];
                pos--;
            }
            results[pos].id = id;
            results[pos].score = score;
        }
    }
}

/* --- Public Search API --- */

int photon_db_search_dot_product(PhotonDB* db, const void* query_vector, size_t k, PhotonSearchResult* results) {
    if (db == NULL || query_vector == NULL || results == NULL || k == 0) return 0;
    
    size_t found = 0;
    size_t element_size = (db->dtype == PHOTON_VECTOR_F32) ? sizeof(float) : sizeof(int8_t);
    size_t vector_bytes = db->dim * element_size;

    for (size_t i = 0; i < db->count; i++) {
        float score = 0.0f;
        void* current_vector = (uint8_t*)db->data + (i * vector_bytes);
        
        if (db->dtype == PHOTON_VECTOR_F32) {
            score = dot_product_f32((const float*)query_vector, (const float*)current_vector, db->dim);
        } else {
            score = dot_product_i8((const int8_t*)query_vector, (const int8_t*)current_vector, db->dim);
        }

        insert_result(results, &found, k, db->list_of_id[i], score, false);
    }

    return (int)found;
}

int photon_db_search_l2_distance(PhotonDB* db, const void* query_vector, size_t k, PhotonSearchResult* results) {
    if (db == NULL || query_vector == NULL || results == NULL || k == 0) return 0;
    
    size_t found = 0;
    size_t element_size = (db->dtype == PHOTON_VECTOR_F32) ? sizeof(float) : sizeof(int8_t);
    size_t vector_bytes = db->dim * element_size;

    for (size_t i = 0; i < db->count; i++) {
        float score = 0.0f;
        void* current_vector = (uint8_t*)db->data + (i * vector_bytes);
        
        if (db->dtype == PHOTON_VECTOR_F32) {
            score = l2_distance_sq_f32((const float*)query_vector, (const float*)current_vector, db->dim);
        } else {
            score = l2_distance_sq_i8((const int8_t*)query_vector, (const int8_t*)current_vector, db->dim);
        }
        
        /* We return actual distance, not squared, for clarity if possible, but squared is fine too */
        score = photon_sqrtf(score);

        insert_result(results, &found, k, db->list_of_id[i], score, true);
    }

    return (int)found;
}

int photon_db_search_cosine_similarity(PhotonDB* db, const void* query_vector, size_t k, PhotonSearchResult* results) {
    if (db == NULL || query_vector == NULL || results == NULL || k == 0) return 0;
    
    size_t found = 0;
    size_t element_size = (db->dtype == PHOTON_VECTOR_F32) ? sizeof(float) : sizeof(int8_t);
    size_t vector_bytes = db->dim * element_size;

    float query_norm_sq = 0.0f;
    if (db->dtype == PHOTON_VECTOR_F32) {
        query_norm_sq = dot_product_f32((const float*)query_vector, (const float*)query_vector, db->dim);
    } else {
        query_norm_sq = dot_product_i8((const int8_t*)query_vector, (const int8_t*)query_vector, db->dim);
    }
    float query_norm = photon_sqrtf(query_norm_sq);

    if (query_norm <= 0.000001f) return 0;

    for (size_t i = 0; i < db->count; i++) {
        void* current_vector = (uint8_t*)db->data + (i * vector_bytes);
        float dot = 0.0f;
        float current_norm_sq = 0.0f;

        if (db->dtype == PHOTON_VECTOR_F32) {
            dot = dot_product_f32((const float*)query_vector, (const float*)current_vector, db->dim);
            current_norm_sq = dot_product_f32((const float*)current_vector, (const float*)current_vector, db->dim);
        } else {
            dot = dot_product_i8((const int8_t*)query_vector, (const int8_t*)current_vector, db->dim);
            current_norm_sq = dot_product_i8((const int8_t*)current_vector, (const int8_t*)current_vector, db->dim);
        }

        float current_norm = photon_sqrtf(current_norm_sq);
        float score = 0.0f;
        if (current_norm > 0.000001f) {
            score = dot / (query_norm * current_norm);
        }

        insert_result(results, &found, k, db->list_of_id[i], score, false);
    }

    return (int)found;
}
