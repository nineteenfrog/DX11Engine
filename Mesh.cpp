#include "Mesh.h"

using namespace DirectX;

/// <summary>
/// Constructor
/// 
/// </summary>
Mesh::Mesh(
	Vertex* vertices, 
	int vertexCount, 
	unsigned int indices[],
	int indexCount, 
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext,
	Microsoft::WRL::ComPtr<ID3D11Device> device) {
	
	this->deviceContext = deviceContext;

	D3D11_BUFFER_DESC vbd = {};
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex) * vertexCount;       
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER; 
	vbd.CPUAccessFlags = 0;	
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

	
	D3D11_SUBRESOURCE_DATA initialVertexData = {};
	initialVertexData.pSysMem = vertices; 

	device->CreateBuffer(&vbd, &initialVertexData, vertexBuffer.GetAddressOf());


	D3D11_BUFFER_DESC ibd = {};
	ibd.Usage = D3D11_USAGE_IMMUTABLE;	
	ibd.ByteWidth = sizeof(unsigned int) * indexCount;	
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;	
	ibd.CPUAccessFlags = 0;	
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA initialIndexData = {};
	initialIndexData.pSysMem = indices; 

	device->CreateBuffer(&ibd, &initialIndexData, indexBuffer.GetAddressOf());
}

/// <summary>
/// Destructor
/// </summary>
Mesh::~Mesh() {

}

Microsoft::WRL::ComPtr<ID3D11Buffer> Mesh::GetVertexBuffer() {
	if (vertexBuffer)
		return this->vertexBuffer;
}
Microsoft::WRL::ComPtr<ID3D11Buffer> Mesh::GetIndexBuffed() {
	if (indexBuffer)
		return this->indexBuffer;
}
int Mesh::GetIndexCount() {
	return indexCount;
}
void Mesh::Draw() {
	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	deviceContext->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
	deviceContext->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	deviceContext->DrawIndexed(indexCount, 0, 0);
}