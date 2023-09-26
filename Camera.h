#pragma once
#include "Transform.h"
class Camera
{
public:
	Camera(float x, float y, float z,
		float moveSpeed,
		float mouslookSpeed,
		float fov,
		float aspectRatio);
	//destructor

	//update methods

	void Update(float dt);
	void UpdateViewMatrix();
	void UpdateProjectionMatrix();

	Transform* GetTransform();

private:
	DirectX::XMFLOAT4X4 viewMatrix;
	DirectX::XMFLOAT4X4 projectionMatri;

	float moveSpeed;


};

