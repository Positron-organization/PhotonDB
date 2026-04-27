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
 * @param db The database to search.
 * @param query_vector The vector to compare against.
 * @param k The number of top results to return.
 * @param results Output array of at least k results.
 * @return The number of results found.
 */
int photon_db_search_dot_product(PhotonDB* db, const void* query_vector, size_t k, PhotonSearchResult* results);

#endif // PHOTON_SEARCH_H
