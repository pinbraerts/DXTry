#include "Image.hpp"
#include "Error.hpp"

ImageBuilder::ImageBuilder(std::wstring_view uri, IWICImagingFactory* imf) : imagingFactory(*imf) {
	check imagingFactory.CreateDecoderFromFilename(
		uri.data(), nullptr, GENERIC_READ,
		WICDecodeMetadataCacheOnLoad, &decoder
	);

	check decoder->GetFrame(0, &src);

	res = src;
}

ImageBuilder& ImageBuilder::convert(WICPixelFormatGUID format, WICBitmapDitherType dType, WICBitmapPaletteType pType) {
	WICPixelFormatGUID pf;
	res->GetPixelFormat(&pf);
	if (pf != format) {
		check imagingFactory.CreateFormatConverter(&converter);
		check converter->Initialize(res.Get(), format, dType, nullptr, 0.f, pType);
		res = converter;
	}

	return *this;
}

ID2D1Bitmap* ImageBuilder::to(ID2D1RenderTarget& renderTarget) {
	ID2D1Bitmap* bmp;
	check renderTarget.CreateBitmapFromWicBitmap(res.Get(), &bmp);
	return bmp;
}

std::unique_ptr<const BYTE[]> ImageBuilder::copy() {
	auto [ rowPitch, imageSize ] = pitch();

	std::unique_ptr<uint8_t[]> temp(new uint8_t[imageSize]);
	check res->CopyPixels(0, static_cast<UINT>(rowPitch), static_cast<UINT>(imageSize), temp.get());
	return temp;
}

ImageBuilder& ImageBuilder::scale(UINT dWidth, UINT dHeight, WICBitmapInterpolationMode iMode) {
	UINT w, h;
	res->GetSize(&w, &h);
	if ((dWidth != w || dHeight != h) && (dWidth != 0 || dHeight != 0)) {
		if (dWidth == 0)
			dWidth = w * dHeight / h;
		else if (dHeight == 0)
			dHeight = h * dWidth / w;
		check imagingFactory.CreateBitmapScaler(&scaler);
		check scaler->Initialize(res.Get(), dWidth, dHeight, iMode);
		res = scaler;
	}

	return *this;
}

std::pair<UINT, UINT> ImageBuilder::clamp(UINT maxsize, WICBitmapInterpolationMode iMode) {
	auto [w, h] = size();
	if (w > maxsize || h > maxsize) {
		float ar = static_cast<float>(h) / static_cast<float>(w);
		if (w > h) {
			w = static_cast<UINT>(maxsize);
			h = static_cast<UINT>(static_cast<float>(maxsize) * ar);
		}
		else {
			h = static_cast<UINT>(maxsize);
			w = static_cast<UINT>(static_cast<float>(maxsize) / ar);
		}
		scale(w, h, iMode);
		return { w, h };
	}

	return { w, h };
}

UINT ImageBuilder::bpp() const {
	return WICBPP(&imagingFactory, format());
}

std::pair<UINT, UINT> ImageBuilder::size() const {
	UINT w, h;
	check res->GetSize(&w, &h);
	return { w, h };
}

std::pair<UINT, UINT> ImageBuilder::pitch() const {
	auto [w, h] = size();
	UINT rowPitch = (w * bpp() + 7) / 8;
	UINT imageSize = rowPitch * h;
	return { rowPitch, imageSize };
}

WICPixelFormatGUID ImageBuilder::format() const {
	WICPixelFormatGUID format;
	check res->GetPixelFormat(&format);
	return format;
}

DXGI_FORMAT TRY_WIC2DXGI(WICPixelFormatGUID format) {
	for (auto& x : _arr_WICFormats) {
		if (x.first == format)
			return x.second;
	}
	return DXGI_FORMAT_UNKNOWN;
}

std::pair<DXGI_FORMAT, WICPixelFormatGUID> WIC2DXGI(WICPixelFormatGUID format) {
	DXGI_FORMAT res = TRY_WIC2DXGI(format);
	if (res != DXGI_FORMAT_UNKNOWN)
		return { res, format };
	for (auto& x : _arr_WICConvert) {
		if (x.first == format) {
			res = TRY_WIC2DXGI(x.second);
			if (res != DXGI_FORMAT_UNKNOWN)
				return { res, x.second };
		}
	}
	return { DXGI_FORMAT_UNKNOWN, GUID_WICPixelFormatUndefined };
}

UINT WICBPP(IWICImagingFactory* factory, WICPixelFormatGUID format) {
	ComPtr<IWICComponentInfo> cinfo;
	check factory->CreateComponentInfo(format, &cinfo);

	WICComponentType type;
	check cinfo->GetComponentType(&type);

	check type == WICPixelFormat;

	ComPtr<IWICPixelFormatInfo> pfinfo;
	check cinfo->QueryInterface(__uuidof(IWICPixelFormatInfo), &pfinfo);

	UINT bpp;
	check pfinfo->GetBitsPerPixel(&bpp);

	return bpp;
}
