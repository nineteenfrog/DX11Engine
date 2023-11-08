#pragma once
#include "DXCore.h"
#include <memory>
#include "Mesh.h"
#include "SimpleShader.h"
#include "WICTextureLoader.h"
#include "Camera.h"

class Sky
{
public:
	Sky(std::shared_ptr <Mesh> mesh,
		Microsoft::WRL::ComPtr <ID3D11SamplerState> samplerState,
		Microsoft::WRL::ComPtr <ID3D11Device> device,
		Microsoft::WRL::ComPtr <ID3D11ShaderResourceView> srv,
		std::shared_ptr<SimpleVertexShader> vs,
		std::shared_ptr <SimplePixelShader> ps,
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> context);
	void Draw(std::shared_ptr<Camera> camera);
private:
	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState;
	Microsoft::WRL::ComPtr<ID3D11Device> device;
	std::shared_ptr<Mesh> mesh;
	std::shared_ptr<SimpleVertexShader> vs;
	std::shared_ptr<SimplePixelShader> ps;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;

	// Helper for creating a cubemap from 6 individual textures
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> CreateCubemap(
		const wchar_t* right,
		const wchar_t* left,
		const wchar_t* up,
		const wchar_t* down,
		const wchar_t* front,
		const wchar_t* back);
};