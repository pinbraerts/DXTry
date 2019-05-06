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
	app.register_class();
	app.create_window();
	app.create_d3dcontext();
	app.create_window_resources();
	app.create_layout();
	app.create_constant_buffer();
	app.create_vertex_buffer();
	app.create_index_buffer();
	app.create_view();
	app.run();
#ifndef _DEBUG
	}
	catch (const Engine::Error& e) {
		std::wcerr << e.msg << std::endl;
	}
#endif // !_DEBUG

	return 0;
}
