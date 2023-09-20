#include "GameEntity.h"

GameEntity::GameEntity(std::shared_ptr<Mesh> mesh)
{
	this->mesh = mesh;
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
	return object;
}
