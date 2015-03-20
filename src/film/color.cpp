#include <array>
#include <algorithm>
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

bool operator<(const SpectrumSample &a, const SpectrumSample &b){
	return a.lambda < b.lambda;
}

Colorf::Colorf(float c) : r(c), g(c), b(c){}
Colorf::Colorf(float r, float g, float b) : r(r), g(g), b(b){}
Colorf::Colorf(const std::vector<SpectrumSample> &samples){
	assert(std::is_sorted(samples.begin(), samples.end()));
	std::array<float, 3> xyz{0};
	float y_integral = 0;
	for (size_t i = 0; i < NUM_CIE_SAMPLES; ++i){
		y_integral += CIE_Y[i];
		float val = interpolate_spectrum(samples, CIE_LAMBDA[i]);
		xyz[0] += val * CIE_X[i];
		xyz[1] += val * CIE_Y[i];
		xyz[2] += val * CIE_Z[i];
	}
	for (int i = 0; i < 3; ++i){
		xyz[i] /= y_integral;
	}
	//Convert to RGB using the standard spectra from PBRT
	r = 3.240479f * xyz[0] - 1.537150f * xyz[1] - 0.498535f * xyz[2];
	g = -0.969256f * xyz[0] + 1.875991f * xyz[1] + 0.041556f * xyz[2];
	b = 0.055648f * xyz[0] - 0.204043f * xyz[1] + 1.057311f * xyz[2];
}
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
bool Colorf::is_black() const {
	return r == 0 && g == 0 && b == 0;
}
Colorf Colorf::to_sRGB() const {
	//Transformation from wikipedia: http://en.wikipedia.org/wiki/SRGB
	const float a = 0.055;
	const float b = 1.f / 2.4f;
	Colorf srgb;
	for (int i = 0; i < 3; ++i){
		if ((*this)[i] <= 0.0031308){
			srgb[i] = 12.92 * (*this)[i];
		}
		else {
			srgb[i] = (1 + a) * std::pow((*this)[i], b) - a;
		}
	}
	return srgb;
}
Colorf& Colorf::operator+=(const Colorf &c){
	r += c.r;
	g += c.g;
	b += c.b;
	return *this;
}
Colorf& Colorf::operator-=(const Colorf &c){
	r -= c.r;
	g -= c.g;
	b -= c.b;
	return *this;
}
Colorf& Colorf::operator*=(const Colorf &c){
	r *= c.r;
	g *= c.g;
	b *= c.b;
	return *this;
}
Colorf& Colorf::operator*=(float s){
	r *= s;
	g *= s;
	b *= s;
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
const float& Colorf::operator[](int i) const {
	switch (i){
		case 0:
			return r;
		case 1:
			return g;
		default:
			return b;
	}
}
bool Colorf::has_nans() const {
	return std::isnan(r) || std::isnan(g) || std::isnan(b);
}
Colorf Colorf::exp() const {
	return Colorf{std::exp(r), std::exp(g), std::exp(b)};
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
Colorf operator-(const Colorf &c){
	return Colorf{-c.r, -c.g, -c.b};
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
float interpolate_spectrum(const std::vector<SpectrumSample> &samples, float lambda){
	if (lambda <= samples.front().lambda){
		return samples.front().value;
	}
	if (lambda >= samples.back().lambda){
		return samples.back().value;
	}
	//Find the first sample at wavelength higher than lambda and lerp it with the
	//previous one (eg. the sample less than lambda)
	auto grt = std::lower_bound(samples.begin(), samples.end(), lambda,
		[](const auto &s, const auto &l){
			return s.lambda < l;
		});
	auto less = grt - 1;
	float t = (lambda - less->lambda) / (grt->lambda - less->lambda);
	return lerp(t, less->value, grt->value);
}

