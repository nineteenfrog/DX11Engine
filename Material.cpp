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

void Material::AddTextureSRV(std::string name, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv) { textureSRVs.insert({ name, srv }); }
void Material::AddSampler(std::string name, Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler) { samplers.insert({ name, sampler }); }

void Material::PrepareMaterial()
{
	for (auto& t : textureSRVs) { pixelShader->SetShaderResourceView(t.first.c_str(), t.second); }
	for (auto& s : samplers) { pixelShader->SetSamplerState(s.first.c_str(), s.second); }
}