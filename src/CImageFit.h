#pragma once

#include <algorithm>
#include <cmath>
#include <cstring>
#include <string>
#include <utility>
#include <vector>

#include "ecs/PropertyReflection.h"

namespace rigkit {
namespace ecs {

/**
 * @brief Fit an image into the entity's shape. Speaks `rig.render.image_fit`.
 * @details CSS object-fit / object-position. Absent component = fill (stretch),
 * matching every sprite and PDF emit today. Compose on the image frame entity
 * (geometry + CAssetRef kind image).
 */
struct CImageFit {
	enum class Fit : int { Fill = 0, Contain = 1, Cover = 2, None = 3, ScaleDown = 4 };

	Fit fit = Fit::Fill;
	float positionX = 0.5f;
	float positionY = 0.5f;
	float offsetX = 0.f;
	float offsetY = 0.f;
	float scale = 1.f;
	float insetTop = 0.f;
	float insetRight = 0.f;
	float insetBottom = 0.f;
	float insetLeft = 0.f;

	std::vector<sProp> GetProperties() {
		static const char* const kFit[] = {"fill", "contain", "cover", "none", "scale-down"};
		return {{0, "Fit", EPT_ENUM, reinterpret_cast<int*>(&fit), kFit, 5},
				{1, "Position X", EPT_FLOAT, &positionX},
				{2, "Position Y", EPT_FLOAT, &positionY},
				{3, "Offset X", EPT_FLOAT, &offsetX},
				{4, "Offset Y", EPT_FLOAT, &offsetY},
				{5, "Scale", EPT_FLOAT, &scale},
				{6, "Inset Top", EPT_FLOAT, &insetTop},
				{7, "Inset Right", EPT_FLOAT, &insetRight},
				{8, "Inset Bottom", EPT_FLOAT, &insetBottom},
				{9, "Inset Left", EPT_FLOAT, &insetLeft}};
	}
};

inline const char* imageFitToken(CImageFit::Fit fit) {
	switch (fit) {
	case CImageFit::Fit::Contain:
		return "contain";
	case CImageFit::Fit::Cover:
		return "cover";
	case CImageFit::Fit::None:
		return "none";
	case CImageFit::Fit::ScaleDown:
		return "scale-down";
	case CImageFit::Fit::Fill:
	default:
		return "fill";
	}
}

inline CImageFit::Fit imageFitFromToken(const std::string& token) {
	if (token == "contain") {
		return CImageFit::Fit::Contain;
	}
	if (token == "cover") {
		return CImageFit::Fit::Cover;
	}
	if (token == "none") {
		return CImageFit::Fit::None;
	}
	if (token == "scale-down") {
		return CImageFit::Fit::ScaleDown;
	}
	return CImageFit::Fit::Fill;
}

/** Placed content rectangle in the frame's local units (top-left origin, +Y down). */
struct ImageContentPlacement {
	float x = 0.f;
	float y = 0.f;
	float width = 0.f;
	float height = 0.f;
	float boxX = 0.f;
	float boxY = 0.f;
	float boxW = 0.f;
	float boxH = 0.f;
};

/**
 * @brief Resolve where the texture sits inside a frame rectangle.
 * @param frameW Frame width in local units (CRectangle.width).
 * @param frameH Frame height.
 * @param imagePxW Natural width in pixels.
 * @param imagePxH Natural height in pixels.
 * @param pxPerLocal Pixels per local unit for natural size (e.g. 96/25.4 for mm at 96 ppi).
 *        Pass 0 to treat imagePx as already in local units.
 */
inline ImageContentPlacement resolveImageContent(float frameW, float frameH, float imagePxW,
												 float imagePxH, const CImageFit& fit,
												 float pxPerLocal = 0.f) {
	ImageContentPlacement out;
	const float insetL = std::max(0.f, fit.insetLeft);
	const float insetR = std::max(0.f, fit.insetRight);
	const float insetT = std::max(0.f, fit.insetTop);
	const float insetB = std::max(0.f, fit.insetBottom);
	out.boxX = insetL;
	out.boxY = insetT;
	out.boxW = std::max(0.f, frameW - insetL - insetR);
	out.boxH = std::max(0.f, frameH - insetT - insetB);
	if (out.boxW <= 0.f || out.boxH <= 0.f || imagePxW <= 0.f || imagePxH <= 0.f) {
		return out;
	}

	float natW = imagePxW;
	float natH = imagePxH;
	if (pxPerLocal > 0.f) {
		natW = imagePxW / pxPerLocal;
		natH = imagePxH / pxPerLocal;
	}

	auto containSize = [&]() {
		const float s = std::min(out.boxW / natW, out.boxH / natH);
		return std::make_pair(natW * s, natH * s);
	};
	auto coverSize = [&]() {
		const float s = std::max(out.boxW / natW, out.boxH / natH);
		return std::make_pair(natW * s, natH * s);
	};

	float cw = out.boxW;
	float ch = out.boxH;
	switch (fit.fit) {
	case CImageFit::Fit::Contain: {
		const auto s = containSize();
		cw = s.first;
		ch = s.second;
		break;
	}
	case CImageFit::Fit::Cover: {
		const auto s = coverSize();
		cw = s.first;
		ch = s.second;
		break;
	}
	case CImageFit::Fit::None:
		cw = natW;
		ch = natH;
		break;
	case CImageFit::Fit::ScaleDown: {
		const auto s = containSize();
		if (natW <= out.boxW && natH <= out.boxH) {
			cw = natW;
			ch = natH;
		} else {
			cw = s.first;
			ch = s.second;
		}
		break;
	}
	case CImageFit::Fit::Fill:
	default:
		cw = out.boxW;
		ch = out.boxH;
		break;
	}

	const float sc = fit.scale > 0.f ? fit.scale : 1.f;
	cw *= sc;
	ch *= sc;

	const float freeX = out.boxW - cw;
	const float freeY = out.boxH - ch;
	out.x = out.boxX + fit.positionX * freeX + fit.offsetX;
	out.y = out.boxY + fit.positionY * freeY + fit.offsetY;
	out.width = cw;
	out.height = ch;
	return out;
}

/** Effective pixels per inch when the placed content covers `placedW` local units. */
inline float effectiveImagePpi(float imagePxW, float placedW, float unitsPerInch) {
	if (imagePxW <= 0.f || placedW <= 0.f || unitsPerInch <= 0.f) {
		return 0.f;
	}
	return imagePxW / (placedW / unitsPerInch);
}

} // namespace ecs
} // namespace rigkit
