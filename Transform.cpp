#pragma once
#include "Transform.h"

using namespace DirectX;

Transform::Transform() {
	position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
	scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
	matrixChanged = false;
	XMStoreFloat4x4(&world, XMMatrixIdentity());
	XMStoreFloat4x4(&worldInverseTranspose, XMMatrixIdentity());
}

Transform::~Transform() {

}

void Transform::UpdateMatrices() {
	//do stuff (update matrix using values)
	XMMATRIX translationMatrix = XMMatrixTranslation(position.x, position.y, position.z);
	XMMATRIX scaleMatrix = XMMatrixScaling(scale.x, scale.y, scale.z);
	XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);

	XMMATRIX worldXM = scaleMatrix * rotationMatrix * translationMatrix;

	XMStoreFloat4x4(&world, worldXM);
	XMStoreFloat4x4(&worldInverseTranspose, XMMatrixInverse(0, XMMatrixTranspose(worldXM)));

	matrixChanged = false;
}

void Transform::MoveAbsolute(float x, float y, float z) {
	SetPosition(position.x + x, position.y + y, position.z + z);
}
void Transform::MoveAbsolute(DirectX::XMFLOAT3 offset) {
	SetPosition(position.x + offset.x, position.y + offset.y, position.z + offset.z);
}
void Transform::Rotate(float pitch, float yaw, float roll) {
	SetRotation(rotation.x + pitch, rotation.y + yaw, rotation.z + roll);
}
void Transform::Rotate(DirectX::XMFLOAT3 rotation) {
	SetRotation(this->rotation.x + rotation.x, this->rotation.y + rotation.y, this->rotation.z + rotation.z);
}
void Transform::Scale(float x, float y, float z) {
	SetScale(scale.x * x, scale.y * y, scale.z * z);
}
void Transform::Scale(DirectX::XMFLOAT3 scale) {
	SetScale(this->scale.x * scale.x, this->scale.y * scale.y, this->scale.z * scale.z);
}

//Setters for all of these we will be setting the boolean matrixChanged to true. 
//This will let us know that we need to remake the world matrix
void Transform::SetPosition(float x, float y, float z) {
	this->position.x = x;
	this->position.y = y;
	this->position.z = z;

	matrixChanged = true;
}
void Transform::SetPosition(DirectX::XMFLOAT3 position) {
	this->position = position;

	matrixChanged = true;
}
void Transform::SetRotation(float pitch, float yaw, float roll) {
	//this->rotation.x = pitch;
	//this->rotation.y = yaw;
	//this->rotation.z = roll;
	XMFLOAT3 temp = XMFLOAT3(pitch,yaw,roll);
	//XMStoreFloat3(&rotation, temp);
	//^Could not figure out how to change the values of a XMFLOAT3 (rotation)
	matrixChanged = true;
}
void Transform::SetRotation(DirectX::XMFLOAT3 rotation) {
	this->rotation = rotation;

	matrixChanged = true;
}
void Transform::SetScale(float x, float y, float z) {
	this->scale.x = x;
	this->scale.y = y;
	this->scale.z = z;

	matrixChanged = true;
}
void Transform::SetScale(DirectX::XMFLOAT3 scale) {
	this->scale = scale;

	matrixChanged = true;
}

//Getters for all of these will just return the position, if the matrix is called for 
//it is checked if the matrix was previously changed, if so, then the world matrix is updated.
DirectX::XMFLOAT3 Transform::GetPosition() {
	return position;
}
DirectX::XMFLOAT3 Transform::GetPitchYawRoll() {
	return rotation;
}
DirectX::XMFLOAT3 Transform::GetScale() {
	return scale;
}
DirectX::XMFLOAT4X4 Transform::GetWorldMatrix() {
	if (matrixChanged) {
		UpdateMatrices();
	}

	return world;
}
DirectX::XMFLOAT4X4 Transform::GetWorldInverseTransposeMatrix() {
	if (matrixChanged) {
		UpdateMatrices();
	}

	return worldInverseTranspose;
}