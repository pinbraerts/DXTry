#ifndef DXTRY_ENGINE_HPP
#define DXTRY_ENGINE_HPP

#include "includes.hpp"
#include "Error.hpp"
#include "Object.hpp"
#include "Input.hpp"
#include "Camera.hpp"
#include "Scene.hpp"
#include "Image.hpp"

struct Engine {
	static LRESULT CALLBACK WndProc(
		HWND hWnd,
		UINT message,
		WPARAM wParam,
		LPARAM lParam);
	std::pair<std::unique_ptr<const BYTE[]>, UINT> load_vertex(
		ComPtr<ID3D11VertexShader>& vertex_shader,
		std::wstring_view path
	);
	ComPtr<ID3D11PixelShader> load_pixel(std::wstring_view path);
	ComPtr<ID3D11GeometryShader> load_geometry(
		std::wstring_view path,
		const D3D11_SO_DECLARATION_ENTRY* entrys, UINT num,
		const UINT* strides, UINT num_strides
	);
	ComPtr<ID3D11GeometryShader> load_geometry(
		std::wstring_view path,
		std::vector<D3D11_SO_DECLARATION_ENTRY> entrys
	) {
		return load_geometry(
			path,
			entrys.data(), (UINT)entrys.size(),
			nullptr, 0
		);
	}
	ComPtr<ID3D11GeometryShader> load_geometry(
		std::wstring_view path,
		std::vector<D3D11_SO_DECLARATION_ENTRY> entrys,
		std::vector<UINT> strides
	) {
		return load_geometry(
			path,
			entrys.data(), (UINT)entrys.size(),
			strides.data(), (UINT)strides.size()
		);
	}
	ComPtr<ID3D11GeometryShader> load_geometry(std::wstring_view path);
	ComPtr<ID3D11Buffer> create_buffer(
		D3D11_BIND_FLAG flag,
		const void* data, UINT size,
		UINT mem_pitch = 0,
		UINT mem_slice_pitch = 0
	);
	ComPtr<ID3D11Buffer> create_buffer(
		D3D11_BIND_FLAG flag,
		UINT size
	);
	template<class T>
	ComPtr<ID3D11Buffer> create_buffer(
		D3D11_BIND_FLAG flag,
		const std::vector<T>& vec,
		UINT mem_pitch = 0,
		UINT mem_slice_pitch = 0
	) {
		return create_buffer(flag, vec.data(), (UINT)vec.size() * sizeof(T), mem_pitch, mem_slice_pitch);
	}
	std::pair<ComPtr<ID3D11Texture2D>, ComPtr<ID3D11ShaderResourceView>> create_texture(std::wstring_view path, UINT maxsize = 0);

	std::wstring_view class_name = L"Application";
	std::wstring_view window_title = L"Game";
	D3D_FEATURE_LEVEL levels[2] {
		//D3D_FEATURE_LEVEL_9_1,
		//D3D_FEATURE_LEVEL_9_2,
		//D3D_FEATURE_LEVEL_9_3,
		//D3D_FEATURE_LEVEL_10_0,
		//D3D_FEATURE_LEVEL_10_1,
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
	D3D_FEATURE_LEVEL feature_level;
	D3D11_TEXTURE2D_DESC buffer_desc;
	D3D11_VIEWPORT viewport;
	ComPtr<ID3D11Device> device;
	ComPtr<IWICImagingFactory> imaging;
	ComPtr<ID3D11DeviceContext> context;
	ComPtr<IDXGISwapChain> swap_chain;
	ComPtr<ID3D11Texture2D> buffer;
	ComPtr<ID3D11RenderTargetView> target;
	ComPtr<ID3D11Texture2D> depth_stencil;
	ComPtr<ID3D11DepthStencilView> depth_stencil_view;
	ComPtr<ID3D11Debug> debug;
	Input input;
	std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
	size_t frames = 0;
	float delta_time = 0.1f;
	float sensivity = 0.001f;
	Scene scene;

	Engine(HINSTANCE hInst);

	void register_class();
	void register_raw_input();
	void create_window();
	void create_d3dcontext();
	void create_imaging();
	void create_swap_chain();
	void create_buffer();
	void create_depth_stencil();
	void create_viewport();
	void create_window_resources();

	HICON get_icon() {
		TCHAR path[MAX_PATH];
		GetModuleFileName(nullptr, path, MAX_PATH);

		return ExtractIcon(window_class.hInstance, path, 0);
	}

	void run();

	void init();
	void update();
	void render();
	// void destroy();

	void present() {
		swap_chain->Present(1, 0);
	}

	~Engine();
};

#endif // !DXTRY_ENGINE_HPP
