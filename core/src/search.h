#ifndef PHOTON_SEARCH_H
#define PHOTON_SEARCH_H

#include <stddef.h>
#include "vector.h"

typedef struct {
  size_t id;
  float score;
} PhotonSearchResult;

/**
 * Perform a brute-force search using dot product similarity.
 * @return The number of results found.
 */
int photon_db_search_dot_product(PhotonDB* db, const void* query_vector, size_t k, PhotonSearchResult* results);

/**
 * Perform a brute-force search using Euclidean (L2) distance.
 * Note: Lower scores (distances) are better.
 * @return The number of results found.
 */
int photon_db_search_l2_distance(PhotonDB* db, const void* query_vector, size_t k, PhotonSearchResult* results);

/**
 * Perform a brute-force search using Cosine similarity.
 * @return The number of results found.
 */
int photon_db_search_cosine_similarity(PhotonDB* db, const void* query_vector, size_t k, PhotonSearchResult* results);

#endif // PHOTON_SEARCH_H
