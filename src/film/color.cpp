#include <ostream>
#include "linalg/util.h"
#include "film/color.h"

Color24::Color24(uint8_t r, uint8_t g, uint8_t b) : r(r), g(g), b(b) {}
uint8_t& Color24::operator[](int i){
	switch (i){
		case 0:
			return r;
		case 1:
			return g;
		default:
			return b;
	}
}

Colorf::Colorf(float c) : r(c), g(c), b(c){}
Colorf::Colorf(float r, float g, float b) : r(r), g(g), b(b){}
void Colorf::normalize(){
	r = clamp(r, 0.f, 1.f);
	g = clamp(g, 0.f, 1.f);
	b = clamp(b, 0.f, 1.f);
}
Colorf Colorf::normalized() const {
	return Colorf{clamp(r, 0.f, 1.f), clamp(g, 0.f, 1.f), clamp(b, 0.f, 1.f)};
}
float Colorf::luminance() const {
	return 0.2126f * r + 0.7152f * g + 0.0722f * b;
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
float& Colorf::operator[](int i){
	switch (i){
		case 0:
			return r;
		case 1:
			return g;
		default:
			return b;
	}
}
Colorf::operator Color24() const {
	return Color24(static_cast<uint8_t>(r * 255), static_cast<uint8_t>(g * 255),
		static_cast<uint8_t>(b * 255));
}
Colorf operator+(const Colorf &a, const Colorf &b){
	return Colorf{a.r + b.r, a.g + b.g, a.b + b.b};
}
Colorf operator-(const Colorf &a, const Colorf &b){
	return Colorf{a.r - b.r, a.g - b.g, a.b - b.b};
}
Colorf operator*(const Colorf &a, const Colorf &b){
	return Colorf{a.r * b.r, a.g * b.g, a.b * b.b};
}
Colorf operator*(const Colorf &a, float s){
	return Colorf{a.r * s, a.g * s, a.b * s};
}
Colorf operator*(float s, const Colorf &a){
	return Colorf{a.r * s, a.g * s, a.b * s};
}
Colorf operator/(const Colorf &a, const Colorf &b){
	return Colorf{a.r / b.r, a.g / b.g, a.b / b.b};
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

