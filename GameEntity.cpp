#include "GameEntity.h"
#include "BufferStructs.h"

GameEntity::GameEntity()
{

}

GameEntity::GameEntity(std::shared_ptr<Mesh> mesh)
{
	this->mesh = mesh;
	this->transform = std::make_shared<Transform>();
}

GameEntity::~GameEntity()
{
}

std::shared_ptr<Mesh> GameEntity::GetMesh()
{
	return mesh;
}

std::shared_ptr<Transform> GameEntity::GetTransform()
{
	return transform;
}

void GameEntity::Draw(
	Microsoft::WRL::ComPtr<ID3D11Buffer> vsConstantBuffer,
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> context,
	Camera camera)
{
	mesh->Draw();
	
	//Shader data  - A
	VertexShaderExternalData vsData;
	vsData.colorTint = mesh->GetTint();
	vsData.worldMatrix = transform->GetWorldMatrix();
	vsData.viewMatrix = camera.GetView();
	vsData.projectionMatrix = camera.GetProjection();

	//Passing shader data with constant buffer
	D3D11_MAPPED_SUBRESOURCE mappedBuffer = {};
	context->Map(vsConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer);
	memcpy(mappedBuffer.pData, &vsData, sizeof(vsData));
	context->Unmap(vsConstantBuffer.Get(), 0);

	context->VSSetConstantBuffers(
		0, // Which slot (register) to bind the buffer to?
		1, // How many are we activating? Can do multiple at once
		vsConstantBuffer.GetAddressOf()); // Array of buffers (or the address of one)
	
}
