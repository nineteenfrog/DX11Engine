#pragma once
#include <DirectXMath.h>

class Transform
{
public:
	Transform();

	~Transform();

	void UpdateMatrices();

	void MoveAbsolute(float x, float y, float z);
	void MoveAbsolute(DirectX::XMFLOAT3 offset);
	void Rotate(float pitch, float yaw, float roll);
	void Rotate(DirectX::XMFLOAT3 rotation);
	void Scale(float x, float y, float z);
	void Scale(DirectX::XMFLOAT3 scale);

	//Setters for all of these we will be setting the boolean matrixChanged to true. 
	//This will let us know that we need to remake the world matrix
	void SetPosition(float x, float y, float z);
	void SetPosition(DirectX::XMFLOAT3 position);
	void SetRotation(float pitch, float yaw, float roll);
	void SetRotation(DirectX::XMFLOAT3 rotation);
	void SetScale(float x, float y, float z);
	void SetScale(DirectX::XMFLOAT3 scale);

	//Getters for all of these will just return the position, if the matrix is called for 
	//it is checked if the matrix was previously changed, if so, then the world matrix is updated.
	DirectX::XMFLOAT3 GetPosition();
	DirectX::XMFLOAT3 GetPitchYawRoll();
	DirectX::XMFLOAT3 GetScale();
	DirectX::XMFLOAT4X4 GetWorldMatrix();
	DirectX::XMFLOAT4X4 GetWorldInverseTransposeMatrix();
private:
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 rotation;
	DirectX::XMFLOAT3 scale;
	DirectX::XMFLOAT4X4 world;
	DirectX::XMFLOAT4X4 worldInverseTranspose;
	bool matrixChanged;
};

