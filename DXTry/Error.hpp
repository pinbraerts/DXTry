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

struct _Checker {
	template<class T>
	void operator=(const T& t) {
		if constexpr (std::is_same_v<T, HRESULT>) {
			if (FAILED(t))
				throw Error(t);
		}
		else {
			if (!t)
				throw Error();
		}
	}
	template<class T>
	void operator=(T&& t) {
		if constexpr (std::is_same_v<T, HRESULT>) {
			if (FAILED(t))
				throw Error(t);
		}
		else {
			if (!t)
				throw Error();
		}
	}
};
#define check _Checker() = 

#endif // !DXTRY_ERROR_HPP
