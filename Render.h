#pragma once
#include "Mesh.h"
#define WIN32_LEAN_AND_MEAN
#define UNICODE
#include <assert.h>
#include <d3d11_1.h>
#include <DirectXMath.h>
#include <windows.h>
#include <iostream>


struct cBuffer
{
	DirectX::XMMATRIX WVP;      // WorldViewProjection Matrix (Combined)
	DirectX::XMMATRIX World;    // World view
};

// Light struct, and a cBufferLight that contains a Light struct
struct Light
{
	Light()
	{
		ZeroMemory(this, sizeof(Light));
	}
	DirectX::XMFLOAT3 ambientLightColor;
	float ambientLightStrength; // makes it 16 byte aligned

	DirectX::XMFLOAT3 dynamicLightColor;
	float dynamicLightStrength; // makes it 16 byte aligned

	DirectX::XMFLOAT3 dynamicLightPosition;
	float padding;              // makes it 16 byte aligned

	// How lightning decreases when moving away from object
	DirectX::XMFLOAT3 dynamicAttenuation;
	float padding2;             // makes it 16 byte aligned

};

struct cBufferLight
{
	Light light;
};



class Renderer
{
	Mesh* p_Mesh;

	struct {
		WNDCLASS WindowClass;
		HWND Window;
		ID3D11Device* Device;
		ID3D11DeviceContext* Context;
		DXGI_SWAP_CHAIN_DESC SwapChainDesc;
		IDXGISwapChain* SwapChain;
		IDXGIDevice2* DxgiDevice;
		IDXGIAdapter* DxgiAdapter;
		IDXGIFactory2* DxgiFactory;
		ID3D11Texture2D* FrameBuffer;
		ID3D11RenderTargetView* FrameBufferView;
		ID3D11InputLayout* InputLayout;
		//redner target
		ID3D11RenderTargetView* renderTargetView;


		// Depth stencil
		ID3D11Texture2D* pDepthStencilBuffer;
		ID3D11DepthStencilState* pDepthStencilState ;
		ID3D11DepthStencilView* pDepthStencilView ;
		ID3D11RasterizerState* pRasterizerState ;

		// viwport
		D3D11_VIEWPORT viewport;

	} m_win32;
	

	struct {
		D3D11_SUBRESOURCE_DATA vertexBuffer;
		D3D11_BUFFER_DESC vertexBufferDesc;
		D3D11_SUBRESOURCE_DATA indexBuffer;
		D3D11_BUFFER_DESC indexBufferDesc;
		ID3DBlob* pVertexShaderBlob;
		ID3DBlob* pPixelShaderBlob;
		ID3DBlob* pErrorBlob;
		ID3D11Buffer* pVertexBuffer;
		ID3D11Buffer* pIndexBuffer;
		ID3D11Buffer* pCBuffer;
		ID3D11Buffer* pCBufferLight;
		ID3D11Texture2D* backbufferTex;
	} m_bufferData;

	struct {
		// Shader ptrs
		ID3D11VertexShader* pVertexShader;
		ID3D11PixelShader* pPixelShader;

	} m_shaders;



	void updateCBuffers(float, float);

public:

	Renderer();

	~Renderer() = default;

	void UploadMesh(const Mesh& mesh);


	bool Init(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow);

	bool InitScene();

	bool InitShaders(); 
	
	bool CreateRenderTarget();

	bool CreateVBO();

	bool CreateIBO();

	void Clear(float r, float g, float b, float a);

	void Draw();

	void Exec();

};