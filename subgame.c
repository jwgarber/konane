#include <stddef.h>
#include <stdint.h>

#include "konane.h"
#if 0

// Max number of connected components is board size^2 / 2.
// We should technically perhaps add 1, since the CCs are
// 1-indexed, but I don't care.
#define MAXCC (SIZE * SIZE / 2)

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define MAX(X, Y) (((X) < (Y)) ? (Y) : (X))

static void calculate_distances(const uint32_t cc[SIZE][SIZE]) {

    // All distances are strictly greater than 0
    uint32_t dist[MAXCC][MAXCC] = {};

    // Iterate over each square of the first board
    for (size_t i = 0; i < SIZE; ++i) {
        for (size_t j = 0; j < SIZE; ++j) {

            // First connected component
            const uint32_t a = cc[i][j];

            // Skip the component if it is empty
            if (a == 0) continue;

            // Now iterate over each square of the second board
            for (size_t k = 0; k < SIZE; ++k) {
                for (size_t l = 0; l < SIZE; ++l) {

                    // Second connected component
                    const uint32_t b = cc[k][l];

                    // Skip this square if it is empty, or if it is in the same component as above
                    if (b == 0 || b == a) continue;

                    // Now we find the Manhattan distance between the two coordinate points
                    const uint32_t x_d = MAX(i, k) - MIN(i, k);
                    const uint32_t y_d = MAX(j, l) - MIN(j, l);
                    const uint32_t d = x_d + y_d;

                    const uint32_t min = MIN(a, b);
                    const uint32_t max = MAX(a, b);

                    if (dist[min][max] == 0) {
                        // This is the first distance for these two CCs
                        dist[min][max] = d;
                    } else {
                        // If the new distance is smaller, use that instead
                        if (d < dist[min][max]) {
                            dist[min][max] = d;
                        }
                    }
                }
            }
        }
    }
}

static void search_cc(uint32_t cc[SIZE][SIZE], const State board[SIZE][SIZE], const size_t i, const size_t j, const uint32_t ccnum) {

    cc[i][j] = ccnum;

    // Search up
    if (i != 0 && board[i - 1][j] != EMPTY) {
        search_cc(cc, board, i - 1, j, ccnum);
    }

    // Search left
    if (j != 0 && board[i][j - 1] != EMPTY) {
        search_cc(cc, board, i, j - 1, ccnum);
    }

    // Search down
    if (i < SIZE - 1 && board[i + 1][j] != EMPTY) {
        search_cc(cc, board, i + 1, j, ccnum);
    }

    // Search right
    if (j < SIZE - 1 && board[i][j + 1] != EMPTY) {
        search_cc(cc, board, i, j + 1, ccnum);
    }
}


static uint32_t connected_components(const State board[SIZE][SIZE]) {

    // We use 0 to indicate an empty spot
    uint32_t cc[SIZE][SIZE] = {};

    uint32_t numccs = 0;

    for (size_t i = 0; i < SIZE; ++i) {
        for (size_t j = 0; j < SIZE; ++j) {
            if (board[i][j] != EMPTY && cc[i][j] == 0) {
                // We haven't searched this one yet. By construction, it must be in a new
                // CC, so perform a search from this node with the new cc
                ++numccs;
                search_cc(cc, board, i, j, numccs);
            }
        }
    }

    return numccs;
}
static void merge_cc(uint32_t cc[SIZE][SIZE], uint32_t dist[MAXCC][MAXCC], const uint32_t numccs) {
    // now we iterate over all pairs of connected components.
    // If the distance between them is less than 4, we merge them together. Actually, is merging necessary?
    // Thing is, if two things actually interfere with each other, that is a fair number of stones that
    // have to be moved. If this is the case, it is unlikely that they would then be able to interfere
    // with another group. So perhaps we just treat
    
    // We do merge connected components together, but

}
#endif
