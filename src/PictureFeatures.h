#pragma once

#include <algorithm>
#include <cmath>

namespace rigkit {
namespace picture {

/** @brief One-frame color / temperature snapshot for the picture bus. */
struct PictureFeatureFrame {
	float colorR = 0.f;
	float colorG = 0.f;
	float colorB = 0.f;
	float colorA = 1.f;
	float temperature = 0.5f; ///< 0 = cold, 1 = warm
};

/**
 * @brief Average rgba and warm/cold from a packed RGBA8 or float buffer.
 * @details `rgba` is interleaved r,g,b,a. Values in `asFloat` are already
 * 0-1; otherwise bytes (0-255) are scaled. Temperature is 0.5 + 0.5*(r-b)/sum.
 */
inline PictureFeatureFrame analyzePictureRgba(const float* rgba, int pixels, bool asFloat = true) {
	PictureFeatureFrame out;
	if (!rgba || pixels <= 0) {
		return out;
	}
	double r = 0, g = 0, b = 0, a = 0;
	const float scale = asFloat ? 1.f : (1.f / 255.f);
	for (int i = 0; i < pixels; ++i) {
		const int o = i * 4;
		r += static_cast<double>(rgba[o] * scale);
		g += static_cast<double>(rgba[o + 1] * scale);
		b += static_cast<double>(rgba[o + 2] * scale);
		a += static_cast<double>(rgba[o + 3] * scale);
	}
	const double n = static_cast<double>(pixels);
	out.colorR = static_cast<float>(r / n);
	out.colorG = static_cast<float>(g / n);
	out.colorB = static_cast<float>(b / n);
	out.colorA = static_cast<float>(a / n);
	const double sum = r + g + b;
	if (sum > 1e-6) {
		out.temperature = static_cast<float>(std::clamp(0.5 + 0.5 * (r - b) / sum, 0.0, 1.0));
	}
	return out;
}

inline PictureFeatureFrame analyzePictureRgba8(const unsigned char* rgba, int pixels) {
	PictureFeatureFrame out;
	if (!rgba || pixels <= 0) {
		return out;
	}
	double r = 0, g = 0, b = 0, a = 0;
	for (int i = 0; i < pixels; ++i) {
		const int o = i * 4;
		r += rgba[o];
		g += rgba[o + 1];
		b += rgba[o + 2];
		a += rgba[o + 3];
	}
	const double n = static_cast<double>(pixels) * 255.0;
	out.colorR = static_cast<float>(r / n);
	out.colorG = static_cast<float>(g / n);
	out.colorB = static_cast<float>(b / n);
	out.colorA = static_cast<float>(a / n);
	const double sum = r + g + b;
	if (sum > 1e-6) {
		out.temperature = static_cast<float>(std::clamp(0.5 + 0.5 * (r - b) / sum, 0.0, 1.0));
	}
	return out;
}

} // namespace picture
} // namespace rigkit
