#include "Render.h"
#include <Windows.h>
#include <d3d11.h>          // d3d interface
#include <d3dcompiler.h>    // shader compiler
// * * * Useful * * * //
#pragma comment (lib, "d3d11.lib")
static const wchar_t ClassName[] = L"MyWindowClass";

#define APPWIDTH 1024
#define APPHEIGHT 768


float dbgTriangle[] = { 0.0f,  0.5f,  0.0f, // point at top
   0.5f, -0.5f,  0.0f, // point at bottom-right
  -0.5f, -0.5f,  0.0f};


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
	D3D11_BUFFER_DESC vertex_buff_descr = {};
	vertex_buff_descr.ByteWidth = p_Mesh->Vertices() * sizeof(float); // sizeof(dbgTriangle);
	vertex_buff_descr.Usage = D3D11_USAGE_DEFAULT ;
	vertex_buff_descr.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	D3D11_SUBRESOURCE_DATA sr_data = { 0 };
	sr_data.pSysMem = p_Mesh->norms();
	HRESULT hr = m_win32.Device->CreateBuffer(
		&vertex_buff_descr,
		&sr_data,
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
	indexBufferDesc.ByteWidth = p_Mesh->Edges();
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = p_Mesh->edata();
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
		D3D_COMPILE_STANDARD_FILE_INCLUDE, "vs_main", "vs_5_0", NULL, NULL, &m_bufferData.pVertexShaderBlob, &m_bufferData.pErrorBlob);
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

	hr = D3DCompileFromFile(L"ps.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "ps_main", "ps_5_0",
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



bool Renderer::CreateRenderTarget()
{

	// Get the back buffer texture.


	HRESULT hr = m_win32.SwapChain->GetBuffer(
		0,
		__uuidof(ID3D11Texture2D),
		(void**)&m_bufferData.framebuffer);
	if (hr != S_OK)
		return false;

	hr = m_win32.Device->CreateRenderTargetView(m_bufferData.framebuffer, NULL,
		&m_win32.renderTargetView);

	if (hr != S_OK)
		return false; 
	// Create the depth stencil view.	

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
	RECT client = { 0, 0, APPWIDTH, APPHEIGHT};
	
	m_win32.Window = CreateWindowEx(NULL,
		ClassName,
		L"D3D11 App",
		WS_OVERLAPPEDWINDOW,    // WS_OVERLAPPEDWINDOW : standard features
		CW_USEDEFAULT,          // CW_USEDEFAULT:
		CW_USEDEFAULT,          //          standard position of new window 
		APPWIDTH,
		APPHEIGHT,
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
	D3D_FEATURE_LEVEL feature_level;

	HRESULT hr = D3D11CreateDeviceAndSwapChain(
		NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		D3D11_CREATE_DEVICE_SINGLETHREADED |
		D3D11_CREATE_DEVICE_DEBUG,
		NULL,
		0,
		D3D11_SDK_VERSION,
		&swapchain_Desc,
		&m_win32.SwapChain,
		&m_win32.Device,
		&feature_level,
		&m_win32.Context);

	if (FAILED(hr)) {
		MessageBeep(1);
		MessageBoxA(0, "Error creating device and swapchain! [Failed]", "Fatal Error", MB_OK | MB_ICONERROR);
		return GetLastError();
	}

	if (!CreateRenderTarget())
		return false;
	
	if (!InitShaders())
		return false;


	D3D11_INPUT_ELEMENT_DESC inputElementDesc[] = {
  { "POS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
  /*
  { "COL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
  { "NOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
  { "TEX", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
  */
	};

	// Get a count of the elements in the layout.
	unsigned numElements = sizeof(inputElementDesc) / sizeof(inputElementDesc[0]);

	// Create the vertex input layout.
	hr = m_win32.Device->CreateInputLayout(inputElementDesc, numElements,
		m_bufferData.pVertexShaderBlob->GetBufferPointer(),
		m_bufferData.pVertexShaderBlob->GetBufferSize(),
		&m_win32.InputLayout);

	if (hr != S_OK)
		return false;

	if (!CreateVBO())
		return false;
	
	return true; 
}


void Renderer::Draw()
{
	static bool vsync = false;
	UINT stride = sizeof(float) * 3;
	UINT offset = 0;
	float color[] = { 0.0f, 0.3f, 0.3f, 1.0f };

	RECT winRect;
	GetClientRect(m_win32.Window, &winRect);
	D3D11_VIEWPORT viewport = {
				0.0f,
				0.0f,
				(FLOAT)(winRect.right - winRect.left),
				(FLOAT)(winRect.bottom - winRect.top),
				0.0f,
				1.0f };


	m_win32.Context->RSSetViewports(1, &viewport);
	m_win32.Context->ClearRenderTargetView(m_win32.renderTargetView, color);
	m_win32.Context->OMSetRenderTargets(1, &m_win32.renderTargetView, NULL);
	m_win32.Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_win32.Context->IASetInputLayout(m_win32.InputLayout);
	m_win32.Context->IASetVertexBuffers(0, 1, &m_bufferData.pVertexBuffer, &stride, &offset);
	m_win32.Context->VSSetShader(m_shaders.pVertexShader, NULL, 0);
	m_win32.Context->PSSetShader(m_shaders.pPixelShader, NULL, 0);

	/*** draw the vertex buffer with the shaders ****/
	m_win32.Context->Draw(p_Mesh->Vertices()/ 3, 0);
	m_win32.SwapChain->Present(1, 0);

	/**** swap the back and front buffers (show the frame we just drew) ****/

}


void Renderer::Exec()
{

	bool Running = true;
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