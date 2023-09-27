#pragma once
#include "Transform.h"

using namespace DirectX;

Transform::Transform() {
	position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
	scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
	matrixChanged = false;

	//local transform variables
	up = XMFLOAT3(0.0, 1.0, 0.0);
	right = XMFLOAT3(1.0, 0.0, 0.0);
	forward = XMFLOAT3(0.0, 0.0, 1.0);
	bool vectorsChanged = false;

	XMStoreFloat4x4(&world, XMMatrixIdentity());
	XMStoreFloat4x4(&worldInverseTranspose, XMMatrixIdentity());
}

Transform::~Transform() {

}

//UPDATER FUNCTIONS ===========================================

void Transform::UpdateMatrices() {
	//do stuff (update matrix using values)
	if (matrixChanged) {
		XMMATRIX translationMatrix = XMMatrixTranslation(position.x, position.y, position.z);
		XMMATRIX scaleMatrix = XMMatrixScaling(scale.x, scale.y, scale.z);
		XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);

		XMMATRIX worldXM = scaleMatrix * rotationMatrix * translationMatrix;

		XMStoreFloat4x4(&world, worldXM);
		XMStoreFloat4x4(&worldInverseTranspose, XMMatrixInverse(0, XMMatrixTranspose(worldXM)));

		matrixChanged = false;
	}
}

void Transform::UpdateVectors()
{
	if (vectorsChanged) {
		XMVECTOR rotationQuat = XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&rotation)); //get rotation matrix
		XMStoreFloat3(&right, XMVector3Rotate(XMVECTOR({ 1, 0, 0 }), rotationQuat)); //apply rotation to all vectors starting from zero 
		XMStoreFloat3(&up, XMVector3Rotate(XMVECTOR({ 0,1,0 }), rotationQuat));
		XMStoreFloat3(&forward, XMVector3Rotate(XMVECTOR({ 0,0,1 }), rotationQuat));
	}

	vectorsChanged = false; //we are clean
}

//MAIN TRANSFORM OPERATION FUNCTIONS ===========================================

//TRANSLATION
void Transform::MoveAbsolute(float x, float y, float z) {
	UpdateMatrices();
	SetPosition(position.x + x, position.y + y, position.z + z);
}

void Transform::MoveAbsolute(DirectX::XMFLOAT3 offset) {
	UpdateMatrices();
	SetPosition(position.x + offset.x, position.y + offset.y, position.z + offset.z);
}

void Transform::MoveRelative(float x, float y, float z)
{
	XMVECTOR movement = { x, y, z, 0.0f };
	XMVECTOR rotationQuat = XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&rotation));

	XMVECTOR newDirection = XMVector3Rotate(movement, rotationQuat);
	movement = XMLoadFloat3(&position) + newDirection;
	XMStoreFloat3(&position, movement);
}

void Transform::MoveRelative(DirectX::XMFLOAT3 offset)
{
	XMVECTOR movement = XMLoadFloat3(&offset);
	XMVECTOR rotationQuat = XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&rotation));

	XMVECTOR newDirection = XMVector3Rotate(movement, rotationQuat);
	movement = XMLoadFloat3(&position) + newDirection;
	XMStoreFloat3(&position, movement);
}

//ROTATION
void Transform::Rotate(float pitch, float yaw, float roll) {
	UpdateMatrices();
	SetRotation(rotation.x + pitch, rotation.y + yaw, rotation.z + roll);
	UpdateVectors();
}
void Transform::Rotate(DirectX::XMFLOAT3 rotation) {
	UpdateMatrices();
	SetRotation(this->rotation.x + rotation.x, this->rotation.y + rotation.y, this->rotation.z + rotation.z);
	UpdateVectors();
}

//SCALE
void Transform::Scale(float x, float y, float z) {
	UpdateMatrices();
	SetScale(scale.x * x, scale.y * y, scale.z * z);
}
void Transform::Scale(DirectX::XMFLOAT3 scale) {
	UpdateMatrices();
	SetScale(this->scale.x * scale.x, this->scale.y * scale.y, this->scale.z * scale.z);
}


//SETTER FUNCTIONS ===========================================

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
	this->rotation.x = pitch;
	this->rotation.y = yaw;
	this->rotation.z = roll;

	matrixChanged = true;
	vectorsChanged = true;
}
void Transform::SetRotation(DirectX::XMFLOAT3 rotation) {
	this->rotation = rotation;

	matrixChanged = true;
	vectorsChanged = true;
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

//GETTER FUNCTIONS ===========================================

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
	UpdateMatrices();
	return world;
}

DirectX::XMFLOAT3 Transform::GetRight()
{
	UpdateVectors();
	return right;
}

DirectX::XMFLOAT3 Transform::GetForward()
{
	UpdateVectors();
	return forward;
}

DirectX::XMFLOAT3 Transform::GetUp()
{
	UpdateVectors();
	return up;
}

DirectX::XMFLOAT4X4 Transform::GetWorldInverseTransposeMatrix() {

	UpdateMatrices();
	return worldInverseTranspose;
}
