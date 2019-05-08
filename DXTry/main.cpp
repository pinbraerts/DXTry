#include "includes.hpp"
#include "Engine.hpp"

int APIENTRY _tWinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPTSTR    lpCmdLine,
	int       nCmdShow) {
#ifndef _DEBUG
	try {
#endif // !_DEBUG
	Engine app(hInstance);
	app.init();
	app.run();
#ifndef _DEBUG
	}
	catch (const Engine::Error& e) {
		std::wcerr << e.msg << std::endl;
	}
#endif // !_DEBUG

	return 0;
}
