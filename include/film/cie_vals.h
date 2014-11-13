#ifndef CIE_VALS_H
#define CIE_VALS_H

#include <array>

//We use PBRT's values for the CIE function values
static const size_t NUM_CIE_SAMPLES = 471;
extern const std::array<float, NUM_CIE_SAMPLES> CIE_X;
extern const std::array<float, NUM_CIE_SAMPLES> CIE_Y;
extern const std::array<float, NUM_CIE_SAMPLES> CIE_Z;
extern const std::array<float, NUM_CIE_SAMPLES> CIE_LAMBDA;

#endif

