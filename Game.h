#pragma once

#include "DXCore.h"
#include <DirectXMath.h>
#include <wrl/client.h> // Used for ComPtr - a smart pointer for COM objects
#include <memory>
#include "Mesh.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_dx11.h"
#include "ImGui/imgui_impl_win32.h"
#include "GameEntity.h"
#include "Camera.h"
#include "SimpleShader.h"
#include "Lights.h"
#include "Sky.h"
#include "PathHelpers.h"


class Game
	: public DXCore
{

public:
	Game(HINSTANCE hInstance);
	~Game();

	// Overridden setup and game loop methods, which
	// will be called automatically
	void Init();
	void OnResize();
	void Update(float deltaTime, float totalTime);
	void Draw(float deltaTime, float totalTime);

private:

	// Initialization helper methods - feel free to customize, combine, remove, etc.
	void LoadShaders();
	void CreateGeometry();
	void LoadTextures();
	void LoadSky();
	void CreateShadows();
	void PostProcessSetup();

	// Note the usage of ComPtr below
	//  - This is a smart pointer for objects that abide by the
	//     Component Object Model, which DirectX objects do
	//  - More info here: https://github.com/Microsoft/DirectXTK/wiki/ComPtr

	// Buffers to hold actual geometry data
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;

	// Shaders and shader-related constructs
	std::shared_ptr<SimplePixelShader> pixelShader;
	std::shared_ptr<SimpleVertexShader> vertexShader;
	std::shared_ptr<SimplePixelShader> customShader;
	//Sky shaders
	std::shared_ptr<SimpleVertexShader> skyVS;
	std::shared_ptr<SimplePixelShader> skyPS;
	//Shadow shader
	std::shared_ptr<SimpleVertexShader> shadowVS;
	
	//Post process shaders
	std::shared_ptr<SimpleVertexShader> ppVS;
	std::shared_ptr<SimplePixelShader> ppPS;

	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
	std::shared_ptr<GameEntity> shapes[6];
	float translation[5][3] = {
		{ 0.0f,0.0f ,0.0f },
		{ 0.0f,0.0f ,0.0f } ,
		{ 0.0f,0.0f ,0.0f } ,
		{ 0.0f,0.0f ,0.0f } ,
		{ 0.0f,0.0f ,0.0f } };
	float rotation[5][3] = {
		{ 0.0f,0.0f ,0.0f },
		{ 0.0f,0.0f ,0.0f } ,
		{ 0.0f,0.0f ,0.0f } ,
		{ 0.0f,0.0f ,0.0f } ,
		{ 0.0f,0.0f ,0.0f } };
	float scale[5][3] = {
		{ 1.0f,1.0f ,1.0f },
		{ 1.0f,1.0f ,1.0f } ,
		{ 1.0f,1.0f ,1.0f } ,
		{ 1.0f,1.0f ,1.0f } ,
		{ 1.0f,1.0f ,1.0f } };
	float colorOffset[5][4] = {
		{1.0f,1.0f,1.0f, 0.0f},
		{1.0f,1.0f,1.0f, 0.0f},
		{1.0f,1.0f,1.0f, 0.0f},
		{1.0f,1.0f,1.0f, 0.0f},
		{1.0f,1.0f,1.0f, 0.0f}
	};
	std::shared_ptr<Material> mat1;
	std::shared_ptr<Material> mat2;
	std::shared_ptr<Material> mat3;
	std::shared_ptr<Material> mat4;
	std::shared_ptr<Material> mat5;
	std::shared_ptr<Material> mat6;

	//Variables for shape movement
	bool going = true;
	int counter = 0;

	std::shared_ptr<Camera> camera[3];
	int activeCamera = 0;

	Light directionalLight1;
	Light directionalLight2;
	Light directionalLight3;
	Light pointLight1;
	Light pointLight2;

	//Skybox Variables
	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srvSky;
	std::shared_ptr<Mesh> skyMesh;
	Sky sky;

	//Shadow variables
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> shadowDSV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shadowSRV;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> shadowRasterizer;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> shadowSampler;
	DirectX::XMFLOAT4X4 lightViewMatrix;
	DirectX::XMFLOAT4X4 lightProjectionMatrix;
	int shadowMapResolution = 1024;

	//Post Process Variables
	Microsoft::WRL::ComPtr<ID3D11SamplerState> ppSampler;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> ppRTV; // For rendering
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> ppSRV; // For sampling
};