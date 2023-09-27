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
	~Camera();
	//update methods

	void Update(float dt);
	void UpdateViewMatrix();
	void UpdateProjectionMatrix(float aspectRatio);

	Transform* GetTransform();

	DirectX::XMFLOAT4X4 GetView();
	DirectX::XMFLOAT4X4 GetProjection();
private:
	DirectX::XMFLOAT4X4 viewMatrix;
	DirectX::XMFLOAT4X4 projectionMatrix;
	Transform transform;

	float fov;
	float aspectRatio;
	float nearP;
	float farP;
	float moveSpeed;
	float mouseLookSpeed;


};

