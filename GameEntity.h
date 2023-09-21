#pragma once
#include "Transform.h"
#include "Mesh.h"
#include <iostream>

class GameEntity
{
public:
	GameEntity();
	GameEntity(std::shared_ptr<Mesh> mesh);
	~GameEntity();

	std::shared_ptr<Mesh> GetMesh();
	std::shared_ptr<Transform> GetTransform();
	void Draw(Microsoft::WRL::ComPtr<ID3D11Buffer> vsConstantBuffer, 
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> context);

private:
	std::shared_ptr<Transform> transform;
	std::shared_ptr<Mesh> mesh;
};

