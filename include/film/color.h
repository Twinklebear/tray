#ifndef COLOR_H
#define COLOR_H

#include <array>
#include <cassert>
#include <vector>
#include <ostream>
#include "cie_vals.h"

//Since we fwrite this struct directly and PPM only takes RGB (24 bits)
//we can't allow any padding to be added onto the end
#pragma pack(1)
struct Color24 {
	uint8_t r, g, b;

	Color24(uint8_t r = 0, uint8_t g = 0, uint8_t b = 0);
	uint8_t& operator[](int i);
};

//32bit color we use for simplifying writing BMPs
struct Color32 {
	uint8_t r, g, b, a;

	Color32(uint8_t r = 0, uint8_t g = 0, uint8_t b = 0, uint8_t a = 0);
	uint8_t& operator[](int i);
};

/*
 * A struct representing a sample value for a spectrum at some wavelength
 */
struct SpectrumSample {
	float lambda, value;
};
bool operator<(const SpectrumSample &a, const SpectrumSample &b);

/*
 * Floating point color struct, is always kept normalized. eg all values
 * are always between [0, 1]
 */
struct Colorf {
	float r, g, b;

	/*
	 * Initialize the RGB values to the same value
	 */
	Colorf(float c = 0);
	/*
	 * Create an RGB color
	 */
	Colorf(float r, float g, float b);
	/*
	 * Create an RGB color by interpolating between the values at the
	 * given wavelengths by reading them into the XYZ colorspace then
	 * converting to RGB. This is using the method described in PBR
	 * The samples should be sorted in ascending order
	 */
	Colorf(const std::vector<SpectrumSample> &samples);
	/*
	 * Normalize the floating point color values to be clamped between 0-1
	 */
	void normalize();
	Colorf normalized() const;
	/*
	 * Compute the luminance of the  color
	 */
	float luminance() const;
	/*
	 * Check if the color is black
	 */
	bool is_black() const;
	/*
	 * Compute and return the sRGB color value for the linear RGB color value
	 */
	Colorf to_sRGB() const;
	Colorf& operator+=(const Colorf &c);
	Colorf& operator-=(const Colorf &c);
	Colorf& operator*=(const Colorf &c);
	Colorf& operator*=(float s);
	Colorf& operator/=(float s);
	float& operator[](int i);
	const float& operator[](int i) const;
	bool has_nans() const;
	/*
	 * Easily convert to the 24bpp and 32bpp color representations
	 */
	operator Color24() const;	
	operator Color32() const;	
};
Colorf operator+(const Colorf &a, const Colorf &b);
Colorf operator-(const Colorf &a, const Colorf &b);
Colorf operator*(const Colorf &a, const Colorf &b);
Colorf operator*(const Colorf &a, float s);
Colorf operator*(float s, const Colorf &a);
Colorf operator/(const Colorf &a, const Colorf &b);
Colorf operator/(const Colorf &c, float s);
bool operator==(const Colorf &a, const Colorf &b);
bool operator!=(const Colorf &a, const Colorf &b);
std::ostream& operator<<(std::ostream &os, const Colorf &c);
/*
 * Find the value of the spectrum at some wavelength by interpolating the
 * two nearest wavelengths
 */
float interpolate_spectrum(const std::vector<SpectrumSample> &samples, float lambda);

#endif

