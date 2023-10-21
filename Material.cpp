#include "Material.h"

Material::Material(DirectX::XMFLOAT4 colorTint, std::shared_ptr<SimpleVertexShader> vs, std::shared_ptr<SimplePixelShader> ps, float roughness)
{
	this->colorTint = colorTint;
	this->vertexShader = vs;
	this->pixelShader = ps;
	this->roughness = roughness;
}

std::shared_ptr<SimpleVertexShader> Material::GetVertexShader() { return vertexShader; }
std::shared_ptr<SimplePixelShader> Material::GetPixelShader() { return pixelShader; }

void Material::SetVertexShader(std::shared_ptr<SimpleVertexShader> vs) { this->vertexShader = vs; }
void Material::SetPixelShader(std::shared_ptr<SimplePixelShader> ps) { this->pixelShader = ps; }

DirectX::XMFLOAT4 Material::GetTint() { return colorTint; }

float Material::GetRoughness()
{
	return roughness;
}

void Material::SetRoughness(float value)
{
	roughness = value;
}
