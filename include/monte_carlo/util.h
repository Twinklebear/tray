#ifndef MONTE_CARLO_UTIL_H
#define MONTE_CARLO_UTIL_H

#include <array>

/*
 * Compute concentric sample positions on a unit disk mapping input from range [0, 1]
 * to sample positions on a disk
 * See: https://mediatech.aalto.fi/~jaakko/T111-5310/K2013/JGT-97.pdf
 */
std::array<float, 2> concentric_sample_disk(const std::array<float, 2> &u);

#endif

