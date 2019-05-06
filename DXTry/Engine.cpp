#include "Engine.hpp"

LRESULT CALLBACK Engine::WndProc(
	HWND hWnd,
	UINT message,
	WPARAM wParam,
	LPARAM lParam) {

	Engine* sys = (Engine*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
	if (sys == nullptr) {
		CREATESTRUCT* cs = (CREATESTRUCT*)lParam;
		if (cs != nullptr) {
			sys = (Engine*)cs->lpCreateParams;
			SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)sys);
		}
		else return DefWindowProc(hWnd, message, wParam, lParam);
	}

	switch (message) {
	case WM_DESTROY:
		PostQuitMessage(0);
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
	if (!RegisterClassEx(&window_class))
		throw Error();
}

void Engine::create_window() {
	window = CreateWindow(
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

	if (window == nullptr)
		throw Error();
}

void Engine::create_d3dcontext() {
	hr = D3D11CreateDevice(
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

	if (FAILED(hr))
		throw Error(hr);
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

	hr = dxgi_device->GetAdapter(&adapter);
	if (FAILED(hr))
		throw Error(hr);

	hr = adapter->GetParent(IID_PPV_ARGS(&factory));
	if (FAILED(hr))
		throw Error(hr);

	hr = factory->CreateSwapChain(
		device.Get(),
		&desc,
		&swap_chain
	);
	if (FAILED(hr))
		throw Error(hr);
}

void Engine::create_buffer() {
	hr = swap_chain->GetBuffer(
		0,
		__uuidof(ID3D11Texture2D),
		(void**)&buffer
	);
	if (FAILED(hr))
		throw Error(hr);

	hr = device->CreateRenderTargetView(
		buffer.Get(),
		nullptr,
		target.GetAddressOf()
	);
	if (FAILED(hr))
		throw Error(hr);

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

	hr = device->CreateTexture2D(
		&depthStencilDesc,
		nullptr,
		&depth_stencil
	);
	if (FAILED(hr))
		throw Error(hr);

	CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2D);

	hr = device->CreateDepthStencilView(
		depth_stencil.Get(),
		&depthStencilViewDesc,
		depth_stencil_view.GetAddressOf()
	);
	if (FAILED(hr))
		throw Error(hr);
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

std::string_view Engine::load_vertex() {
	std::string_view shb = read_all(L"base_vertex.cso");
	if (shb.empty())
		throw Error(L"Could not read vertex shader");
	hr = device->CreateVertexShader(
		shb.data(),
		shb.size(),
		nullptr,
		vertex_shader.GetAddressOf()
	);
	if (FAILED(hr))
		throw Error(hr);

	return shb;
}

std::string_view Engine::load_pixel() {
	std::string_view shb = read_all(L"base_pixel.cso");
	if (shb.empty())
		throw Error(L"Could not read pixel shader");
	hr = device->CreatePixelShader(
		shb.data(),
		shb.size(),
		nullptr,
		pixel_shader.GetAddressOf()
	);
	if (FAILED(hr))
		throw Error(hr);

	return shb;
}

void Engine::create_layout() {
	std::string_view shb = load_vertex();

	D3D11_INPUT_ELEMENT_DESC desc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	hr = device->CreateInputLayout(
		desc,
		(UINT)std::size(desc),
		shb.data(),
		shb.size(),
		input_layout.GetAddressOf()
	);
	if (FAILED(hr))
		throw Error(hr);

	delete[] shb.data();

	delete[] load_pixel().data();
}

void Engine::create_constant_buffer() {
	CD3D11_BUFFER_DESC desc {
		sizeof(ConstantBufferStruct),
		D3D11_BIND_CONSTANT_BUFFER
	};
	hr = device->CreateBuffer(
		&desc,
		nullptr,
		constant_buffer.GetAddressOf()
	);
	if (FAILED(hr))
		throw Error(hr);
}

void Engine::create_vertex_buffer() {
	float vertices[] {
		-0.5f, -0.5f, -0.5f, 0, 0, 0,
		-0.5f, -0.5f, 0.5f, 0, 0, 1,
		-0.5f, 0.5f, -0.5f, 0, 1, 0,
		-0.5f, 0.5f, 0.5f, 0, 1, 1,

		0.5f, -0.5f, -0.5f, 1, 0, 0,
		0.5f, -0.5f, 0.5f, 1, 0, 1,
		0.5f, 0.5f, -0.5f, 1, 1, 0,
		0.5f, 0.5f, 0.5f, 1, 1, 1,
	};

	CD3D11_BUFFER_DESC vDesc(
		sizeof(vertices),
		D3D11_BIND_VERTEX_BUFFER
	);

	D3D11_SUBRESOURCE_DATA vData;
	ZeroMemory(&vData, sizeof(D3D11_SUBRESOURCE_DATA));
	vData.pSysMem = vertices;
	vData.SysMemPitch = 0;
	vData.SysMemSlicePitch = 0;

	hr = device->CreateBuffer(
		&vDesc,
		&vData,
		&vertex_buffer
	);
	if (FAILED(hr))
		throw Error(hr);
}

void Engine::create_index_buffer() {
	unsigned short indices[] =
	{
		0, 2, 1, // -x
		1, 2, 3,

		4, 5, 6, // +x
		5, 7, 6,

		0, 1, 5, // -y
		0, 5, 4,
		2, 6, 7, // +y
		2, 7, 3,

		0, 4, 6, // -z
		0, 6, 2,

		1, 3, 7, // +z
		1, 7, 5,
	};

	n_indices = std::size(indices);

	CD3D11_BUFFER_DESC iDesc(
		sizeof(indices),
		D3D11_BIND_INDEX_BUFFER
	);

	D3D11_SUBRESOURCE_DATA iData;
	ZeroMemory(&iData, sizeof(D3D11_SUBRESOURCE_DATA));
	iData.pSysMem = indices;
	iData.SysMemPitch = 0;
	iData.SysMemSlicePitch = 0;

	hr = device->CreateBuffer(
		&iDesc,
		&iData,
		&index_buffer
	);
	if (FAILED(hr))
		throw Error(hr);
}

void Engine::create_view() {
	DirectX::XMVECTOR eye = DirectX::XMVectorSet(0.0f, 0.7f, 1.5f, 0.f);
	DirectX::XMVECTOR at = DirectX::XMVectorSet(0.0f, -0.1f, 0.0f, 0.f);
	DirectX::XMVECTOR up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.f);

	DirectX::XMStoreFloat4x4(
		&matrices.view,
		DirectX::XMMatrixTranspose(
			DirectX::XMMatrixLookAtRH(
				eye,
				at,
				up
			)
		)
	);

	float aspectRatio = (float)buffer_desc.Width / (float)buffer_desc.Height;

	DirectX::XMStoreFloat4x4(
		&matrices.projection,
		DirectX::XMMatrixTranspose(
			DirectX::XMMatrixPerspectiveFovRH(
				DirectX::XMConvertToRadians(70),
				aspectRatio,
				0.01f,
				100.0f
			)
		)
	);
}

void Engine::release_buffer() {
	target.Reset();
	buffer.Reset();
	depth_stencil_view.Reset();
	depth_stencil.Reset();
	context->Flush();
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

void Engine::update() {
	DirectX::XMStoreFloat4x4(
		&matrices.model,
		DirectX::XMMatrixTranspose(
			DirectX::XMMatrixRotationY(
				DirectX::XMConvertToRadians(
				(float)frames++
				)
			)
		)
	);

	if (frames == MAXUINT) frames = 0;
}

void Engine::render() {
	context->UpdateSubresource(
		constant_buffer.Get(),
		0,
		nullptr,
		&matrices,
		0,
		0
	);

	// Clear the render target and the z-buffer.
	const float teal[] = { 0.098f, 0.439f, 0.439f, 1.000f };
	context->ClearRenderTargetView(
		target.Get(),
		teal
	);
	context->ClearDepthStencilView(
		depth_stencil_view.Get(),
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0
	);

	ID3D11RenderTargetView* targets[] = {
		target.Get()
	};
	// Set the render target.
	context->OMSetRenderTargets(
		(UINT)std::size(targets),
		targets,
		depth_stencil_view.Get()
	);

	// Set up the IA stage by setting the input topology and layout.
	UINT stride = sizeof(float) * 6;
	UINT offset = 0;

	context->IASetVertexBuffers(
		0,
		1,
		vertex_buffer.GetAddressOf(),
		&stride,
		&offset
	);

	context->IASetIndexBuffer(
		index_buffer.Get(),
		DXGI_FORMAT_R16_UINT,
		0
	);

	context->IASetPrimitiveTopology(
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST
	);

	context->IASetInputLayout(input_layout.Get());

	// Set up the vertex shader stage.
	context->VSSetShader(
		vertex_shader.Get(),
		nullptr,
		0
	);

	context->VSSetConstantBuffers(
		0,
		1,
		constant_buffer.GetAddressOf()
	);

	// Set up the pixel shader stage.
	context->PSSetShader(
		pixel_shader.Get(),
		nullptr,
		0
	);

	// Calling Draw tells Direct3D to start sending commands to the graphics device.
	context->DrawIndexed(
		(UINT)n_indices,
		0,
		0
	);
}
