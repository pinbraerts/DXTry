#ifndef DXTRY_INCLUDES_HPP
#define DXTRY_INCLUDES_HPP

#include <windows.h>
#include <comdef.h>
#include <d3d11.h>
// d3d12
#include <dxgi1_6.h>
#include <wrl.h>
#include <tchar.h>
#undef max

#pragma comment(lib, "d3d11.lib")

using namespace Microsoft::WRL;

#include <iostream>
#include <fstream>
#include <string>
#include <string_view>
#include <vector>
#include <bitset>
#include <chrono>
#include <algorithm>

static std::string_view read_all(std::istream& input) {
	if (!input) return std::string_view();
	input.ignore(std::numeric_limits<std::streamsize>::max());
	size_t sz = input.gcount();
	input.clear();
	input.seekg(0, std::ios::beg);
	char* buffer = new char[sz];
	input.read(buffer, sz);
	return std::string_view(buffer, sz);
}
static std::string_view read_all(std::wstring_view path) {
	std::ifstream input(path.data(), std::ios::binary);
	return read_all(input);
}

template<class T>
static void safe_release(T*& ptr) {
	if (ptr != nullptr) {
		ptr->Release();
		ptr = nullptr;
	}
}


#include "SimpleMath.hpp"
using namespace DirectX::SimpleMath;
using namespace DirectX;

struct Engine;
struct ObjectSerial;
struct Object;

#endif // !DXTRY_INCLUDES_HPP
