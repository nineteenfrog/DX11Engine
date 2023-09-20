#pragma once
#include "Transform.h"
#include "Mesh.h"
#include <iostream>

class GameEntity
{
public:
	GameEntity(std::shared_ptr<Mesh> mesh);
	~GameEntity();

	std::shared_ptr<Mesh> GetMesh();
	std::shared_ptr<Transform> GetTransform();

private:
	std::shared_ptr<Transform> object;
	std::shared_ptr<Mesh> mesh;
};

