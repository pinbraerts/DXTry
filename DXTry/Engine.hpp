#ifndef DXTRY_ENGINE_HPP
#define DXTRY_ENGINE_HPP

#include "includes.hpp"
#include "Error.hpp"
#include "Object.hpp"
#include "Input.hpp"
#include "Camera.hpp"

struct Engine {
	static LRESULT CALLBACK WndProc(
		HWND hWnd,
		UINT message,
		WPARAM wParam,
		LPARAM lParam);
	static std::string_view load_vertex(
		ID3D11Device* device,
		ComPtr<ID3D11VertexShader>& vertex_shader,
		std::wstring_view path
	);
	static std::string_view load_pixel(
		ID3D11Device* device,
		ComPtr<ID3D11PixelShader>& pixel_shader,
		std::wstring_view path
	);
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
	ComPtr<ID3D11Buffer> constant_buffer;
	ComPtr<ID3D11Debug> debug;
	Input input;
	Camera camera;
	std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
	size_t frames = 0;
	float delta_time = 0.1f;
	float sensivity = 0.001f;

	struct ConstantBufferStruct {
		//mat4 model;
		Matrix world;
		Matrix view;
		Matrix projection;
	} matrices;

	Engine(HINSTANCE hInst);

	void register_class();
	void register_raw_input();
	void create_window();
	void create_d3dcontext();
	void create_swap_chain();
	void create_buffer();
	void create_depth_stencil();
	void create_viewport();
	void create_window_resources();
	void release_buffer();
	void create_constant_buffer();
	void create_projection();
	void update_camera() {
		camera.update(matrices.view);
	}

	Object cube;
	void create_cube();

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
