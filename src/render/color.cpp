#include <ostream>
#include "linalg/util.h"
#include "render/color.h"

Color24::Color24(uint8_t r, uint8_t g, uint8_t b) : r(r), g(g), b(b) {}

Colorf::Colorf(float r, float g, float b) : r(r), g(g), b(b) {
	normalize();
}
void Colorf::normalize(){
	r = clamp(r, 0.f, 1.f);
	g = clamp(g, 0.f, 1.f);
	b = clamp(b, 0.f, 1.f);
}
Colorf& Colorf::operator+=(const Colorf &c){
	r += c.r;
	b += c.b;
	g += c.g;
	return *this;
}
Colorf& Colorf::operator-=(const Colorf &c){
	r -= c.r;
	b -= c.b;
	g -= c.g;
	return *this;
}
Colorf& Colorf::operator*=(float s){
	r *= s;
	b *= s;
	g *= s;
	return *this;
}
Colorf& Colorf::operator/=(float s){
	return *this *= 1.f / s;
}
Colorf::operator Color24() const {
	return Color24(static_cast<uint8_t>(r * 255), static_cast<uint8_t>(g * 255),
		static_cast<uint8_t>(b * 255));
}
Colorf operator+(const Colorf &a, const Colorf &b){
	Colorf c = Colorf(a.r + b.r, a.g + b.g, a.b + b.b);
	return c;
}
Colorf operator-(const Colorf &a, const Colorf &b){
	Colorf c = Colorf(a.r - b.r, a.g - b.g, a.b - b.b);
	return c;
}
Colorf operator*(const Colorf &a, float s){
	Colorf c = Colorf(a.r * s, a.g * s, a.b * s);
	return c;
}
Colorf operator*(float s, const Colorf &a){
	Colorf c = Colorf(a.r * s, a.g * s, a.b * s);
	return c;
}
Colorf operator/(const Colorf &c, float s){
	float inv_s = 1.f / s;
	return c * inv_s;
}
bool operator==(const Colorf &a, const Colorf &b){
	return a.r == b.r && a.g == b.g && a.b == b.b;
}
bool operator!=(const Colorf &a, const Colorf &b){
	return !(a == b);
}
std::ostream& operator<<(std::ostream &os, const Colorf &c){
	os << "Colorf: [r = " << c.r << ", g = " << c.g
		<< ", b = " << c.b << "] ";
	return os;
}

