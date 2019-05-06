#ifndef DXTRY_ENGINE_HPP
#define DXTRY_ENGINE_HPP

#include "includes.hpp"

struct Engine {
	struct Error {
		std::wstring msg;

		Error(std::wstring message): msg(message) {}

		Error(HRESULT h) :
			msg(_com_error(h).ErrorMessage())
		{ }

		Error() :
			Error(HRESULT_FROM_WIN32(GetLastError()))
		{ }
	};

	static LRESULT CALLBACK WndProc(
		HWND hWnd,
		UINT message,
		WPARAM wParam,
		LPARAM lParam);

	std::wstring_view class_name = L"Application";
	std::wstring_view window_title = L"Game";
	D3D_FEATURE_LEVEL levels[7]{
		D3D_FEATURE_LEVEL_9_1,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_11_1,
	};
#ifdef _DEBUG
	UINT deviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT | D3D11_CREATE_DEVICE_DEBUG;
#else
	UINT deviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#endif // _DEBUG

	WNDCLASSEX window_class;
	HWND window;
	HRESULT hr;
	D3D_FEATURE_LEVEL feature_level;
	D3D11_TEXTURE2D_DESC buffer_desc;
	D3D11_VIEWPORT viewport;
	ComPtr<ID3D11Device> device;
	ComPtr<ID3D11DeviceContext> context;
	ComPtr<IDXGISwapChain> swap_chain;
	ComPtr<ID3D11Texture2D> buffer;
	ComPtr<ID3D11RenderTargetView> target;
	ComPtr<ID3D11Texture2D> depth_stencil;
	ComPtr<ID3D11DepthStencilView> depth_stencil_view;

	ComPtr<ID3D11InputLayout> input_layout;
	ComPtr<ID3D11VertexShader> vertex_shader;
	ComPtr<ID3D11PixelShader> pixel_shader;
	ComPtr<ID3D11Buffer> constant_buffer;
	ComPtr<ID3D11Buffer> vertex_buffer;
	ComPtr<ID3D11Buffer> index_buffer;
	size_t n_indices;
	size_t frames = 0;

	struct ConstantBufferStruct {
		mat4 model;
		mat4 view;
		mat4 projection;
	} matrices;

	Engine(HINSTANCE hInst);

	void register_class();
	void create_window();
	void create_d3dcontext();
	void create_swap_chain();
	void create_buffer();
	void create_depth_stencil();
	void create_viewport();
	void create_window_resources();
	void release_buffer();
	
	std::string_view load_vertex();
	std::string_view load_pixel();
	void create_layout();
	void create_constant_buffer();
	void create_vertex_buffer();
	void create_index_buffer();
	void create_view();

	HICON get_icon() {
		TCHAR path[MAX_PATH];
		GetModuleFileName(nullptr, path, MAX_PATH);

		return ExtractIcon(window_class.hInstance, path, 0);
	}

	void run();

	void update();

	void render();

	void present() {
		swap_chain->Present(1, 0);
	}
};

#endif // !DXTRY_ENGINE_HPP
