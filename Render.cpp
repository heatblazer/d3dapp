#include "Render.h"
#include <Windows.h>
#include <d3d11.h>          // d3d interface
#include <d3dcompiler.h>    // shader compiler
// * * * Useful * * * //
#pragma comment (lib, "d3d11.lib")
static const wchar_t ClassName[] = L"MyWindowClass";


struct Vertex
{
	float position[3];
	float color[3];
};

Vertex dbgTriangle[3] = { {{1.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},
							  {{-1.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
							  {{0.0f, -1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}} };


static int dbgidx[] = { 0, 1, 2,
						 0, 2, 3 };

LRESULT CALLBACK WindowProc(HWND Window, UINT Message, WPARAM WParam, LPARAM LParam) {
	switch (Message) {
	case WM_KEYDOWN: {
		switch (WParam) {
		case 'O': { DestroyWindow(Window); };
		}
	} break;
	case WM_DESTROY: { PostQuitMessage(0); } break;

	default: {
		return DefWindowProc(Window, Message, WParam, LParam);
	}
	}

	return 0;
}


Renderer::Renderer() : p_Mesh{nullptr}
{
	ZeroMemory(&m_win32, sizeof(m_win32)); // set to 0 all memebres
	ZeroMemory(&m_bufferData, sizeof(m_bufferData));
}

void Renderer::UploadMesh(const Mesh& mesh)
{
	p_Mesh = const_cast<Mesh*>(&mesh);
}


bool Renderer::CreateVBO()
{

	// Set up the description of the static vertex buffer.
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(dbgTriangle); //p_Mesh->Vertices();
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = dbgTriangle;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	HRESULT hr = m_win32.Device->CreateBuffer(&vertexBufferDesc, &vertexData,
		&m_bufferData.pVertexBuffer);
	if (hr != S_OK)
		return false;

	return true;
}

bool Renderer::CreateIBO()
{

	// Set up the description of the static index buffer.
	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(dbgidx);
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = dbgidx;//p_Mesh->edata();
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	HRESULT hr = m_win32.Device->CreateBuffer(&indexBufferDesc, &indexData,
		&m_bufferData.pIndexBuffer);
	if (hr != S_OK)
		return false;

	return true;
}

bool Renderer::InitShaders()
{
	HRESULT hr = D3DCompileFromFile(L"vs.hlsl", nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "vs_5_0", NULL, NULL, &m_bufferData.pVertexShaderBlob, &m_bufferData.pErrorBlob);
	if (FAILED(hr)) {
		if (m_bufferData.pErrorBlob) {
			OutputDebugStringA((char*)m_bufferData.pErrorBlob->GetBufferPointer());
			m_bufferData.pErrorBlob->Release();
		}

		if (m_bufferData.pVertexShaderBlob) {
			m_bufferData.pVertexShaderBlob->Release();
		}
		return false;
	}

	hr = D3DCompileFromFile(L"ps.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "ps_5_0",
		NULL, NULL, &m_bufferData.pPixelShaderBlob, &m_bufferData.pErrorBlob);
	if (FAILED(hr)) {
		if (m_bufferData.pErrorBlob) {
			OutputDebugStringA((char*)m_bufferData.pErrorBlob->GetBufferPointer());
			m_bufferData.pErrorBlob->Release();
		}

		if (m_bufferData.pPixelShaderBlob) {
			m_bufferData.pPixelShaderBlob->Release();
		}
		return false;
	}

	// Create a vertex and a pixel shader from blob-info to vertex/pixel_ptrs    
	hr = m_win32.Device->CreateVertexShader(m_bufferData.pVertexShaderBlob->GetBufferPointer(),
		m_bufferData.pVertexShaderBlob->GetBufferSize(), NULL, &m_shaders.pVertexShader);
	assert(SUCCEEDED(hr));

	hr = m_win32.Device->CreatePixelShader(m_bufferData.pPixelShaderBlob->GetBufferPointer(),
		m_bufferData.pPixelShaderBlob->GetBufferSize(), NULL, &m_shaders.pPixelShader);
	assert(SUCCEEDED(hr));
	
	return true; 
}

bool Renderer::InitScene()
{

	return true;
}


bool Renderer::CreateRenderTarget()
{

	// Get the back buffer texture.
	HRESULT hr = m_win32.SwapChain->GetBuffer(0, IID_PPV_ARGS(&m_bufferData.backbufferTex));
	// Create the render target view with the back buffer pointer.
	if (hr != S_OK)
		return false;
	
	hr = m_win32.Device->CreateRenderTargetView(m_bufferData.backbufferTex, NULL,
		&m_win32.renderTargetView);

	if (hr != S_OK)
		return false;

	// Create the texture for the depth buffer.
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));
	depthBufferDesc.Width = 1024;
	depthBufferDesc.Height = 768;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;
	hr = m_win32.Device->CreateTexture2D(&depthBufferDesc, NULL,
		&m_win32.pDepthStencilBuffer);

	if (hr != S_OK)
		return false; 
	// Create the depth stencil view.
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;
	hr = m_win32.Device->CreateDepthStencilView(m_win32.pDepthStencilBuffer,
		&depthStencilViewDesc,
		&m_win32.pDepthStencilView);
	if (hr != S_OK)
		return false;

	return true;
}

bool Renderer::Init(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{

	DXGI_SWAP_CHAIN_DESC swapchain_Desc = { 0 };

	WNDCLASSEX wc = { 0 };
	ZeroMemory(&wc, sizeof(WNDCLASSEX));

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = NULL;
	wc.lpszClassName = ClassName;

	if (!RegisterClassEx(&wc)) {
		MessageBeep(1);
		MessageBoxA(0, "[Error] Register class", "Fatal Error", MB_OK);
		return GetLastError();
	}

	// * * * size of the client area * * * //
	RECT client = { 0, 0, 1024, 768};
	
	m_win32.Window = CreateWindowEx(NULL,
		ClassName,
		L"D3D11 App",
		WS_OVERLAPPEDWINDOW,    // WS_OVERLAPPEDWINDOW : standard features
		CW_USEDEFAULT,          // CW_USEDEFAULT:
		CW_USEDEFAULT,          //          standard position of new window 
		1024,
		768,
		NULL,
		NULL,
		hInstance,
		NULL);

	if (!m_win32.Window) {
		return false;
	}

	ShowWindow(m_win32.Window, nCmdShow);

	ZeroMemory(&swapchain_Desc, sizeof(DXGI_SWAP_CHAIN_DESC));
	swapchain_Desc.BufferDesc.RefreshRate.Numerator = 0;
	swapchain_Desc.BufferDesc.RefreshRate.Denominator = 1;
	swapchain_Desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapchain_Desc.SampleDesc.Count = 1;
	swapchain_Desc.SampleDesc.Quality = 0;
	swapchain_Desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;   // used when I wish to draw graphics into the backbuffer
	swapchain_Desc.BufferCount = 1;                                 // total buffers | 1 = one backbuffer
	swapchain_Desc.OutputWindow = m_win32.Window;
	swapchain_Desc.Windowed = true;
	swapchain_Desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	IDXGIFactory* factory;

	CreateDXGIFactory(IID_PPV_ARGS(&factory));

	IDXGIAdapter* adapter;

	factory->EnumAdapters(0, &adapter);

	D3D_FEATURE_LEVEL featureLevelInputs[7] = {
	D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1,
	D3D_FEATURE_LEVEL_10_0, D3D_FEATURE_LEVEL_9_3,  D3D_FEATURE_LEVEL_9_2,
	D3D_FEATURE_LEVEL_9_1 };

	D3D_FEATURE_LEVEL featureLevelOutputs = D3D_FEATURE_LEVEL_11_1;
	HRESULT hr = D3D11CreateDevice(adapter, D3D_DRIVER_TYPE_UNKNOWN, 0,
		D3D11_CREATE_DEVICE_SINGLETHREADED |
		D3D11_CREATE_DEVICE_DEBUG,
		featureLevelInputs, 7u, D3D11_SDK_VERSION,
		&m_win32.Device, &featureLevelOutputs, &m_win32.Context);

	hr = factory->CreateSwapChain(m_win32.Device, &swapchain_Desc, &m_win32.SwapChain);


	if (FAILED(hr)) {
		MessageBeep(1);
		MessageBoxA(0, "Error creating device and swapchain! [Failed]", "Fatal Error", MB_OK | MB_ICONERROR);
		return GetLastError();
	}

	ZeroMemory(&m_win32.viewport, sizeof(D3D11_VIEWPORT));
	m_win32.viewport.TopLeftX = 0;
	m_win32.viewport.TopLeftY = 0;
	m_win32.viewport.Width = (float)1024;
	m_win32.viewport.Height = (float)768;
	m_win32.viewport.MinDepth = 0.0f;
	m_win32.viewport.MaxDepth = 1.0f;

	m_win32.Context->RSSetViewports(1, &m_win32.viewport);

	if (!CreateRenderTarget())
		return false;

	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];

	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "COLOR";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	// Get a count of the elements in the layout.
	unsigned numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	if (!InitShaders())
		return false;

	// Create the vertex input layout.
	hr = m_win32.Device->CreateInputLayout(polygonLayout, numElements,
		m_bufferData.pVertexShaderBlob->GetBufferPointer(),
		m_bufferData.pVertexShaderBlob->GetBufferSize(),
		&m_win32.InputLayout);

	if (hr != S_OK)
		return false;

	// depth stencil
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;
	// Stencil operations if pixel is front-facing.
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	// Stencil operations if pixel is back-facing.
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Create the depth stencil state.
	hr = m_win32.Device->CreateDepthStencilState(&depthStencilDesc,
		&m_win32.pDepthStencilState);

	if (hr != S_OK)
		return false;

	// Set the depth stencil state.
	m_win32.Context->OMSetDepthStencilState(m_win32.pDepthStencilState, 1);

	// 🟨 Rasterization
	D3D11_RASTERIZER_DESC rasterDesc;
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_NONE;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	// Create the rasterizer state from the description we just filled out.
	hr = m_win32.Device->CreateRasterizerState(&rasterDesc, &m_win32.pRasterizerState);

	if (hr != S_OK)
		return false;

	// Now set the rasterizer state.
	m_win32.Context->RSSetState(m_win32.pRasterizerState);

	return true; 
}

void Renderer::Clear(float r, float g, float b, float a)
{
	float c[4] = { r, g, b, a };
	m_win32.SwapChain->Present(1, 0);

	m_win32.Context->ClearRenderTargetView(m_win32.renderTargetView, c);
	// Clear Depth/Stencil view
   m_win32.Context->ClearDepthStencilView(m_win32.pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

}


void Renderer::updateCBuffers(float rot, float transform)
{

}

void Renderer::Draw()
{
	static bool vsync = false;
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	float color[] = { 1.0f, 0.0f, 0.0f, 1.0f };
	

	// Bind the render target view and depth stencil buffer to the output render
   // pipeline.
	m_win32.Context->OMSetRenderTargets(0, &m_win32.renderTargetView, m_win32.pDepthStencilView);

	m_win32.Context->RSSetViewports(1, &m_win32.viewport);

	// Clear textures
	m_win32.Context->ClearRenderTargetView(m_win32.renderTargetView, color);

	// Clear the depth buffer.
	m_win32.Context->ClearDepthStencilView(m_win32.pDepthStencilView, D3D11_CLEAR_DEPTH,
		1.0f, 0);

	// Set the vertex input layout.
	m_win32.Context->IASetInputLayout(m_win32.InputLayout);

	// Set the vertex and pixel shaders that will be used to render this
	// triangle.
	m_win32.Context->VSSetShader(m_shaders.pVertexShader, NULL, 0);
	m_win32.Context->PSSetShader(m_shaders.pPixelShader, NULL, 0);

	// Set the vertex buffer to active in the input assembler so it can be
	// rendered.
	m_win32.Context->IASetVertexBuffers(0, 1, &m_bufferData.pVertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be
	// rendered.
	m_win32.Context->IASetIndexBuffer(m_bufferData.pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex
	// buffer, in this case triangles.
	m_win32.Context->IASetPrimitiveTopology(
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Render the triangle.
	m_win32.Context->DrawIndexed(3, 0, 0);

	m_win32.SwapChain->Present(1, 0);

}


void Renderer::Exec()
{

	bool Running = true;

	CreateVBO();
	CreateIBO();
	while (Running) {
		MSG Message;
		while (PeekMessage(&Message, nullptr, 0, 0, PM_REMOVE)) {
			if (Message.message == WM_QUIT) Running = false;
			TranslateMessage(&Message);
			DispatchMessage(&Message);
		}

		Draw(); 
	}


}