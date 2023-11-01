#pragma once

#include <memory>
#include <DirectXMath.h>
#include "SimpleShader.h"
#include <unordered_map>

class Material
{
public:
	Material(DirectX::XMFLOAT4 colorTint,
		std::shared_ptr<SimpleVertexShader> vs,
		std::shared_ptr<SimplePixelShader> ps,
		float roughness);

	std::shared_ptr<SimpleVertexShader> GetVertexShader();
	std::shared_ptr<SimplePixelShader> GetPixelShader();
	void SetVertexShader(std::shared_ptr<SimpleVertexShader> vs);
	void SetPixelShader(std::shared_ptr<SimplePixelShader> ps);
	DirectX::XMFLOAT4 GetTint();
	float GetRoughness();
	void SetRoughness(float value);
private:
	DirectX::XMFLOAT4 colorTint;
	std::shared_ptr<SimpleVertexShader> vertexShader;
	std::shared_ptr<SimplePixelShader> pixelShader;
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> textureSRVs;
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11SamplerState>> samplers;
	float roughness;
};

