#pragma once

#include <memory>
#include <DirectXMath.h>
#include "SimpleShader.h"

class Material
{
public:
	Material(DirectX::XMFLOAT4 colorTint,
		std::shared_ptr<SimpleVertexShader> vs,
		std::shared_ptr<SimpleVertexShader> ps);

	std::shared_ptr<SimpleVertexShader> GetVertexShader();
	std::shared_ptr<SimplePixelShader> GetPixelShader();
	void SetVertexShader(std::shared_ptr<SimpleVertexShader> vs);
	void SetPixelShader(std::shared_ptr<SimplePixelShader> ps);

private:
	DirectX::XMFLOAT4 colorTint;
	std::shared_ptr<SimpleVertexShader> vertexShader;
	std::shared_ptr<SimplePixelShader> pixelShader;
};

