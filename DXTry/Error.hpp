#ifndef DXTRY_ERROR_HPP
#define DXTRY_ERROR_HPP

#include "includes.hpp"

struct Error {
	std::wstring msg;

	Error(std::wstring message) : msg(message) {}

	Error(HRESULT h) :
		msg(_com_error(h).ErrorMessage())
	{ }

	Error() :
		Error(HRESULT_FROM_WIN32(GetLastError()))
	{ }
};

#endif // !DXTRY_ERROR_HPP
