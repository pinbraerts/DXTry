#include "Engine.hpp"

LRESULT CALLBACK Engine::WndProc(
	HWND hWnd,
	UINT message,
	WPARAM wParam,
	LPARAM lParam) {

	Engine* sys = (Engine*)GetWindowLongPtr(hWnd, GWLP_USERDATA);

	switch (message) {
	case WM_CREATE:
	{
		CREATESTRUCT* cs = (CREATESTRUCT*)lParam;
		if (cs != nullptr) {
			sys = (Engine*)cs->lpCreateParams;
			SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)sys);
		}
		else return DefWindowProc(hWnd, message, wParam, lParam);
		return S_OK;
	}
	case WM_DESTROY:
		PostQuitMessage(0);
		return S_OK;
	case WM_INPUT:
		sys->input.update(lParam);
		return S_OK;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
}

Engine::Engine(HINSTANCE hInst) :
	window_class {
		sizeof(WNDCLASSEX), // size
		CS_DBLCLKS, // style
		Engine::WndProc, // window proc
		0, // cbClsExtra
		0, // cbWndExtra
		hInst ? hInst : (HINSTANCE)GetModuleHandle(nullptr),
		get_icon(), // icon
		LoadCursor(nullptr, IDC_ARROW), // cursor
		(HBRUSH)GetStockObject(BLACK_BRUSH), // backgound
		nullptr, // menu name
		class_name.data(),
		0 // small icon
	} { }

void Engine::register_class() {
	check RegisterClassEx(&window_class);
}

void Engine::register_raw_input() {
	RAWINPUTDEVICE rid[2]{
		{
			0x01, // HID usage page generic
			0x02, // mouse
			RIDEV_NOLEGACY,
			0
		},
		{
			0x01, // HID usage page generic
			0x06, // keyboard
			RIDEV_NOLEGACY,
			0
		}
	};

	check RegisterRawInputDevices(rid, (UINT)std::size(rid), sizeof(RAWINPUTDEVICE));
}

void Engine::create_window() {
	check window = CreateWindow(
		class_name.data(),
		window_title.data(),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, // x, y
		CW_USEDEFAULT, CW_USEDEFAULT, // width, height
		nullptr, // parent window
		nullptr, // menu
		window_class.hInstance,
		this // lpParam
	);

	RECT rect;
	check GetWindowRect(window, &rect);
	UINT cx = (rect.left + rect.right) / 2,
		cy = (rect.top + rect.bottom) / 2;
	check SetCursorPos(cx, cy);
	SetCursor(nullptr);
	check ClipCursor(nullptr);
}

void Engine::create_d3dcontext() {
	check D3D11CreateDevice(
		nullptr, // adapter
		D3D_DRIVER_TYPE_HARDWARE,
		0, // Should be 0 unless the driver is D3D_DRIVER_TYPE_SOFTWARE
		deviceFlags,
		levels,
		(UINT)std::size(levels),
		D3D11_SDK_VERSION,
		&device,
		&feature_level,
		&context
	);

	check device.As(&debug);
}

void Engine::create_imaging() {
	check CoCreateInstance(
		CLSID_WICImagingFactory,
		nullptr,
		CLSCTX_INPROC_SERVER,
		__uuidof(IWICImagingFactory),
		(LPVOID*)&imaging
	);
}

void Engine::create_swap_chain() {
	DXGI_SWAP_CHAIN_DESC desc;
	ZeroMemory(&desc, sizeof(DXGI_SWAP_CHAIN_DESC));
	desc.Windowed = TRUE; // Sets the initial state of full-screen mode.
	desc.BufferCount = 2;
	desc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	desc.SampleDesc.Count = 1; // multisampling setting
	desc.SampleDesc.Quality = 0; // vendor-specific flag
	desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
	desc.OutputWindow = window;

	ComPtr<IDXGIDevice> dxgi_device;
	device.As(&dxgi_device);

	ComPtr<IDXGIAdapter> adapter;
	ComPtr<IDXGIFactory> factory;

	check dxgi_device->GetAdapter(&adapter);
	check adapter->GetParent(IID_PPV_ARGS(&factory));
	check factory->CreateSwapChain(
		device.Get(),
		&desc,
		&swap_chain
	);
}

void Engine::create_buffer() {
	check swap_chain->GetBuffer(
		0,
		__uuidof(ID3D11Texture2D),
		(void**)&buffer
	);

	check device->CreateRenderTargetView(
		buffer.Get(),
		nullptr,
		target.GetAddressOf()
	);

	buffer->GetDesc(&buffer_desc);
}

void Engine::create_depth_stencil() {
	CD3D11_TEXTURE2D_DESC depthStencilDesc(
		DXGI_FORMAT_D24_UNORM_S8_UINT,
		static_cast<UINT> (buffer_desc.Width),
		static_cast<UINT> (buffer_desc.Height),
		1, // This depth stencil view has only one texture.
		1, // Use a single mipmap level.
		D3D11_BIND_DEPTH_STENCIL
	);

	check device->CreateTexture2D(
		&depthStencilDesc,
		nullptr,
		&depth_stencil
	);

	CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2D);

	check device->CreateDepthStencilView(
		depth_stencil.Get(),
		&depthStencilViewDesc,
		depth_stencil_view.GetAddressOf()
	);
}

void Engine::create_viewport() {
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
	viewport.Height = (float)buffer_desc.Height;
	viewport.Width = (float)buffer_desc.Width;
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1;

	context->RSSetViewports(
		1,
		&viewport
	);
}

void Engine::create_window_resources() {
	create_swap_chain();
	create_buffer();
	create_depth_stencil();
	create_viewport();
}

std::pair<std::unique_ptr<const BYTE[]>, UINT> Engine::load_vertex(
	ComPtr<ID3D11VertexShader>& vertex_shader,
	std::wstring_view path) {
	std::string_view shb = read_all(path);
	if (shb.empty())
		throw Error(L"Could not read vertex shader");
	std::unique_ptr<const BYTE[]> ptr;
	ptr.reset((const BYTE*)shb.data());
	check device->CreateVertexShader(
		shb.data(),
		shb.size(),
		nullptr,
		&vertex_shader
	);

	return { std::move(ptr), (UINT)shb.size() };
}

ComPtr<ID3D11PixelShader> Engine::load_pixel(
	std::wstring_view path) {
	ComPtr<ID3D11PixelShader> res;
	std::string_view shb = read_all(path);
	if (shb.empty())
		throw Error(L"Could not read pixel shader");
	std::unique_ptr<const BYTE[]> ptr;
	ptr.reset((const BYTE*)shb.data());
	check device->CreatePixelShader(
		shb.data(),
		shb.size(),
		nullptr,
		&res
	);
	return res;
}

ComPtr<ID3D11GeometryShader> Engine::load_geometry(
	std::wstring_view path,
	const D3D11_SO_DECLARATION_ENTRY* entrys, UINT num,
	const UINT* strides, UINT num_strides) {
	ComPtr<ID3D11GeometryShader> res;
	std::string_view shb = read_all(path);
	if (shb.empty())
		throw Error(L"Could not read gometry shader");
	std::unique_ptr<const BYTE[]> ptr;
	ptr.reset((const BYTE*)shb.data());
	check device->CreateGeometryShaderWithStreamOutput(
		shb.data(), shb.size(),
		entrys, num,
		strides, num_strides,
		D3D11_SO_NO_RASTERIZED_STREAM,
		nullptr,
		&res
	);
	return res;
}

ComPtr<ID3D11GeometryShader> Engine::load_geometry(std::wstring_view path) {
	ComPtr<ID3D11GeometryShader> res;
	std::string_view shb = read_all(path);
	if (shb.empty())
		throw Error(L"Could not read geometry shader");
	std::unique_ptr<const BYTE[]> ptr;
	ptr.reset((const BYTE*)shb.data());
	check device->CreateGeometryShader(
		shb.data(),
		shb.size(),
		nullptr,
		&res
	);
	return res;
}

ComPtr<ID3D11Buffer> Engine::create_buffer(
	D3D11_BIND_FLAG flag,
	const void* data, UINT size,
	UINT mem_pitch, UINT mem_slice_pitch) {
	ComPtr<ID3D11Buffer> res;

	CD3D11_BUFFER_DESC iDesc(size, flag);

	D3D11_SUBRESOURCE_DATA iData {
		data,
		mem_pitch,
		mem_slice_pitch
	};

	check device->CreateBuffer(
		&iDesc,
		&iData,
		&res
	);
	return res;
}

ComPtr<ID3D11Buffer> Engine::create_buffer(D3D11_BIND_FLAG flag, UINT size) {
	ComPtr<ID3D11Buffer> res;

	CD3D11_BUFFER_DESC iDesc(size, flag);

	check device->CreateBuffer(
		&iDesc,
		nullptr,
		&res
	);
	return res;
}

std::pair<ComPtr<ID3D11Texture2D>, ComPtr<ID3D11ShaderResourceView>> Engine::create_texture(std::wstring_view path, UINT maxsize) {
	ImageBuilder builder(path, imaging.Get());

	if (maxsize == 0) {
		// This is a bit conservative because the hardware could support larger textures than
		// the Feature Level defined minimums, but doing it this way is much easier and more
		// performant for WIC than the 'fail and retry' model used by DDSTextureLoader

		switch (feature_level)
		{
		case D3D_FEATURE_LEVEL_9_1:
		case D3D_FEATURE_LEVEL_9_2:
			maxsize = D3D_FL9_1_REQ_TEXTURE2D_U_OR_V_DIMENSION;
			break;

		case D3D_FEATURE_LEVEL_9_3:
			maxsize = D3D_FL9_3_REQ_TEXTURE2D_U_OR_V_DIMENSION;
			break;

		case D3D_FEATURE_LEVEL_10_0:
		case D3D_FEATURE_LEVEL_10_1:
			maxsize = D3D10_REQ_TEXTURE2D_U_OR_V_DIMENSION;
			break;

		default:
			maxsize = D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION;
			break;
		}
	}

	auto [ format, used_wic ] = WIC2DXGI(builder.format());
	if (format == DXGI_FORMAT_UNKNOWN)
		throw Error(HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED));

	// Verify our target format is supported by the current device
	// (handles WDDM 1.0 or WDDM 1.1 device driver cases as well as DirectX 11.0 Runtime without 16bpp format support)
	UINT support = 0;
	HRESULT hr = device->CheckFormatSupport(format, &support);
	if (FAILED(hr) || !(support & D3D11_FORMAT_SUPPORT_TEXTURE2D)) {
		// Fallback to RGBA 32-bit format which is supported by all devices
		used_wic = GUID_WICPixelFormat32bppRGBA;
		format = DXGI_FORMAT_R8G8B8A8_UNORM;
	}
	// Allocate temporary memory for image
	auto [ width, height ] = builder.clamp(maxsize);
	auto [ rowPitch, imageSize ] = builder.pitch();

	builder.convert(used_wic);
	std::unique_ptr<const BYTE[]> temp = builder.copy();

	// See if format is supported for auto-gen mipmaps (varies by feature level)
	bool autogen = false;
	UINT fmtSupport = 0;
	hr = device->CheckFormatSupport(format, &fmtSupport);
	if (SUCCEEDED(hr) && (fmtSupport & D3D11_FORMAT_SUPPORT_MIP_AUTOGEN))
		autogen = true;

	// Create texture
	D3D11_TEXTURE2D_DESC desc;
	desc.Width = width;
	desc.Height = height;
	desc.MipLevels = (autogen) ? 0 : 1;
	desc.ArraySize = 1;
	desc.Format = format;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = (autogen) ? (D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET) : (D3D11_BIND_SHADER_RESOURCE);
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = (autogen) ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0;

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = temp.get();
	initData.SysMemPitch = static_cast<UINT>(rowPitch);
	initData.SysMemSlicePitch = static_cast<UINT>(imageSize);


	ComPtr<ID3D11Texture2D> texture;
	ComPtr<ID3D11ShaderResourceView> view;
	
	check device->CreateTexture2D(&desc, (autogen) ? nullptr : &initData, &texture);
	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
	memset(&SRVDesc, 0, sizeof(SRVDesc));
	SRVDesc.Format = format;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	SRVDesc.Texture2D.MipLevels = (autogen) ? -1 : 1;

	check device->CreateShaderResourceView(texture.Get(), &SRVDesc, &view);

	if (autogen) {
		context->UpdateSubresource(texture.Get(), 0, nullptr, temp.get(), static_cast<UINT>(rowPitch), static_cast<UINT>(imageSize));
		context->GenerateMips(view.Get());
	}

	return { texture, view };
}

void Engine::run() {
	if (!IsWindowVisible(window))
		ShowWindow(window, SW_SHOW);

	bool bGotMsg;
	MSG  msg;
	msg.message = WM_NULL;
	PeekMessage(&msg, NULL, 0U, 0U, PM_NOREMOVE);

	while (WM_QUIT != msg.message)
	{
		// Process window events.
		// Use PeekMessage() so we can use idle time to render the scene. 
		bGotMsg = (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE) != 0);

		if (bGotMsg)
		{
			// Translate and dispatch the message
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			// Update the scene.
			update();

			// Render frames during idle time (when no messages are waiting).
			render();

			// Present the frame to the screen.
			present();
		}
	}
}

void Engine::init() {
	// winapi settings
	register_class();
	create_window();
	register_raw_input();
	CoInitialize(nullptr);

	// directx settings
	create_d3dcontext();
	create_window_resources();

	// other factories
	create_imaging();

	// init scene
	scene.init(*this);
}

void Engine::update() {
	start = std::chrono::high_resolution_clock::now();
	
	if (input.keyboard.just_pressed(VK_ESCAPE)) {
		PostMessage(window, WM_CLOSE, 0, 0);
		return;
	}

	scene.update(*this);
}

void Engine::render() {
	// Clear the render target and the z-buffer.
	const float teal[] = { 0.098f, 0.439f, 0.439f, 1.000f };
	context->ClearRenderTargetView(
		target.Get(),
		teal
	);
	context->ClearDepthStencilView(
		depth_stencil_view.Get(),
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1,
		0
	);

	ID3D11RenderTargetView* targets[] {
		target.Get()
	};
	// Set the render target.
	context->OMSetRenderTargets(
		(UINT)std::size(targets),
		targets,
		depth_stencil_view.Get()
	);

	scene.render(*this);

	end = std::chrono::high_resolution_clock::now();
	delta_time = std::chrono::duration<float>(end - start).count();
}

Engine::~Engine() {
	imaging.Reset();
	CoUninitialize();
}
