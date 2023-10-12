#include "GameEntity.h"

GameEntity::GameEntity(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material)
{
	this->mesh = mesh;
	this->material = material;
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

std::shared_ptr<Material> GameEntity::GetMaterial()
{
	return material;
}

void GameEntity::SetMaterial(std::shared_ptr<Material> newMat)
{
	this->material = newMat;
}

void GameEntity::Draw(
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> context,
	Camera camera)
{
	material->GetVertexShader()->SetShader();
	material->GetPixelShader()->SetShader();

	mesh->Draw();

	std::shared_ptr<SimpleVertexShader> vs = material->GetVertexShader();
	vs->SetFloat4("colorTint", mesh->GetTint());
	vs->SetMatrix4x4("world", transform->GetWorldMatrix());
	vs->SetMatrix4x4("view", camera.GetView());
	vs->SetMatrix4x4("projection", camera.GetProjection());

	vs->CopyAllBufferData();

	////Shader data  - A
	//VertexShaderExternalData vsData;
	//vsData.colorTint = mesh->GetTint();
	//vsData.worldMatrix = transform->GetWorldMatrix();
	//vsData.viewMatrix = camera.GetView();
	//vsData.projectionMatrix = camera.GetProjection();

	////Passing shader data with constant buffer
	//D3D11_MAPPED_SUBRESOURCE mappedBuffer = {};
	//context->Map(vsConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer);
	//memcpy(mappedBuffer.pData, &vsData, sizeof(vsData));
	//context->Unmap(vsConstantBuffer.Get(), 0);

	//context->VSSetConstantBuffers(
	//	0, // Which slot (register) to bind the buffer to?
	//	1, // How many are we activating? Can do multiple at once
	//	vsConstantBuffer.GetAddressOf()); // Array of buffers (or the address of one)

}
