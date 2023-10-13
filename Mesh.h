#pragma once
#include "DXCore.h"
#include <DirectXMath.h>
#include <wrl/client.h>
#include <d3d11.h>
#include "Vertex.h"
#include <fstream>
#include <vector>

class Mesh
{
public:
	Mesh(Vertex* vertices, 
		int vertexCount, 
		unsigned int indices[],
		int indexCount, 
		Microsoft::WRL::ComPtr<ID3D11Device> device,
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext);
	Mesh(
		const wchar_t* objFile,
		Microsoft::WRL::ComPtr<ID3D11Device> device,
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext);
	~Mesh();

	Microsoft::WRL::ComPtr<ID3D11Buffer> GetVertexBuffer();
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetIndexBuffed();
	int GetIndexCount();
	void Draw();
	void SetTint(float r, float g, float b, float a);
	DirectX::XMFLOAT4 GetTint();
private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext;
	int indexCount;
	DirectX::XMFLOAT4 colorTint;
};

