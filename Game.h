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

	// Note the usage of ComPtr below
	//  - This is a smart pointer for objects that abide by the
	//     Component Object Model, which DirectX objects do
	//  - More info here: https://github.com/Microsoft/DirectXTK/wiki/ComPtr

	// Buffers to hold actual geometry data
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
	
	// Shaders and shader-related constructs
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
	std::shared_ptr<GameEntity> shapes[5];
	Microsoft::WRL::ComPtr<ID3D11Buffer> vsConstantBuffer;
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
		{0.0f,0.0f,0.0f, 0.0f},
		{0.0f,0.0f,0.0f, 0.0f},
		{0.0f,0.0f,0.0f, 0.0f},
		{0.0f,0.0f,0.0f, 0.0f},
		{0.0f,0.0f,0.0f, 0.0f}
	};

	//Variables for shape movement
	bool going = true;
	int counter = 0;
};

