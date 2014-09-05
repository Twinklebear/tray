#ifndef COLOR_H
#define COLOR_H

#include <ostream>

//Since we fwrite this struct directly and PPM only takes RGB (24 bits)
//we can't allow any padding to be added onto the end
#pragma pack(1)
struct Color24 {
	uint8_t r, g, b;

	Color24(uint8_t r = 0, uint8_t g = 0, uint8_t b = 0);
};

/*
 * Floating point color struct, is always kept normalized. eg all values
 * are always between [0, 1]
 */
struct Colorf {
	float r, g, b;

	Colorf(float r = 0, float g = 0, float b = 0);
	/*
	 * Normalize the floating point color values to be clamped between 0-1
	 */
	void normalize();
	Colorf& operator+=(const Colorf &c);
	Colorf& operator-=(const Colorf &c);
	Colorf& operator*=(float s);
	Colorf& operator/=(float s);
	/*
	 * Easily convert to the 8-bit/channel color representation
	 */
	operator Color24() const;	
};
Colorf operator+(const Colorf &a, const Colorf &b);
Colorf operator-(const Colorf &a, const Colorf &b);
Colorf operator*(const Colorf &a, const Colorf &b);
Colorf operator*(const Colorf &a, float s);
Colorf operator*(float s, const Colorf &a);
Colorf operator/(const Colorf &c, float s);
bool operator==(const Colorf &a, const Colorf &b);
bool operator!=(const Colorf &a, const Colorf &b);
std::ostream& operator<<(std::ostream &os, const Colorf &c);

#endif

