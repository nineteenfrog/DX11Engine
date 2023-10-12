#pragma once
#include "Transform.h"
#include "Mesh.h"
#include <iostream>
#include "Camera.h"
#include "Material.h"

class GameEntity
{
public:
	GameEntity(
		std::shared_ptr<Mesh> mesh,
		std::shared_ptr<Material> material);
	~GameEntity();

	std::shared_ptr<Mesh> GetMesh();
	std::shared_ptr<Transform> GetTransform();
	std::shared_ptr<Material> GetMaterial();
	void SetMaterial(std::shared_ptr<Material> newMat);
	void Draw(
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> context,
		Camera camera);

private:
	std::shared_ptr<Transform> transform;
	std::shared_ptr<Mesh> mesh;
	std::shared_ptr<Material> material;
};

