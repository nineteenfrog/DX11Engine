#include "Game.h"
#include "Vertex.h"
#include "Input.h"
#include "PathHelpers.h"
#include "Material.h"
#include "WICTextureLoader.h"

// Needed for a helper function to load pre-compiled shader files
#pragma comment(lib, "d3dcompiler.lib")
#include <d3dcompiler.h>

// For the DirectX Math library
using namespace DirectX;

// --------------------------------------------------------
// Constructor
//
// DXCore (base class) constructor will set up underlying fields.
// Direct3D itself, and our window, are not ready at this point!
//
// hInstance - the application's OS-level handle (unique ID)
// --------------------------------------------------------
Game::Game(HINSTANCE hInstance)
	: DXCore(
		hInstance,			// The application's handle
		L"DirectX Game",	// Text for the window's title bar (as a wide-character string)
		1280,				// Width of the window's client area
		720,				// Height of the window's client area
		false,				// Sync the framerate to the monitor refresh? (lock framerate)
		true)				// Show extra stats (fps) in title bar?
{
#if defined(DEBUG) || defined(_DEBUG)
	// Do we want a console window?  Probably only in debug mode
	CreateConsoleWindow(500, 120, 32, 120);
	printf("Console window created successfully.  Feel free to printf() here.\n");
#endif
	directionalLight1 = {};
	directionalLight2 = {};
	directionalLight3 = {};
	pointLight1 = {};
	pointLight2 = {};
}

// --------------------------------------------------------
// Destructor - Clean up anything our game has created:
//  - Delete all objects manually created within this class
//  - Release() all Direct3D objects created within this class
// --------------------------------------------------------
Game::~Game()
{
	// Call delete or delete[] on any objects or arrays you've
	// created using new or new[] within this class
	// - Note: this is unnecessary if using smart pointers

	// Call Release() on any Direct3D objects made within this class
	// - Note: this is unnecessary for D3D objects stored in ComPtrs
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

// --------------------------------------------------------
// Called once per program, after Direct3D and the window
// are initialized but before the game loop.
// --------------------------------------------------------
void Game::Init()
{
	// Helper methods for loading shaders, creating some basic
	// geometry to draw and some simple camera matrices.
	//  - You'll be expanding and/or replacing these later
	XMFLOAT4 rose = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	XMFLOAT4 springGreen = XMFLOAT4(0.0f, 1.0f, 0.5f, 1.0f);
	XMFLOAT4 violet = XMFLOAT4(0.5f, 0.0f, 1.0f, 1.0f);
	LoadShaders();
	LoadTextures();
	mat2 = std::make_shared<Material>(springGreen, vertexShader, pixelShader, 0.0);
	mat3 = std::make_shared<Material>(violet, vertexShader, customShader, 0.0);
	CreateGeometry();

	// Set initial graphics API state
	//  - These settings persist until we change them
	//  - Some of these, like the primitive topology & input layout, probably won't change
	//  - Others, like setting shaders, will need to be moved elsewhere later
	{
		// Tell the input assembler (IA) stage of the pipeline what kind of
		// geometric primitives (points, lines or triangles) we want to draw.  
		// Essentially: "What kind of shape should the GPU draw with our vertices?"
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}

	//Initialize ImGui stuff
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui_ImplWin32_Init(hWnd);
		ImGui_ImplDX11_Init(device.Get(), context.Get());

		ImGui::StyleColorsDark();
	}

	//camera and lights stuff
	{
		camera[0] = std::make_shared<Camera>(10.0f, 0.0f, -10.0f, 5.0f, 10.0f, XM_PI / 2, (float)this->windowWidth / this->windowHeight);
		camera[1] = std::make_shared<Camera>(0.0f, 0.0f, -10.0f, 5.0f, 10.0f, XM_PI / 3, (float)this->windowWidth / this->windowHeight);
		camera[2] = std::make_shared<Camera>(-10.0f, 0.0f, -10.0f, 5.0f, 10.0f, XM_PI / 4, (float)this->windowWidth / this->windowHeight);

		directionalLight1.type = LIGHT_TYPE_DIRECTIONAL;
		directionalLight1.direction = XMFLOAT3(1.0f, 0.0f, 0.0f);
		directionalLight1.color = XMFLOAT3(1.0f, 1.0f, 1.0f);
		directionalLight1.intensity = 0.5f;

		directionalLight2.type = LIGHT_TYPE_DIRECTIONAL;
		directionalLight2.direction = XMFLOAT3(0.0f, 1.0f, 0.0f);
		directionalLight2.color = XMFLOAT3(1.0f, 1.0f, 1.0f);
		directionalLight2.intensity = 0.5f;

		directionalLight3.type = LIGHT_TYPE_DIRECTIONAL;
		directionalLight3.direction = XMFLOAT3(0.0f, 0.0f, 1.0f);
		directionalLight3.color = XMFLOAT3(1.0f, 1.0f, 1.0f);
		directionalLight3.intensity = 0.5f;

		pointLight1.type = LIGHT_TYPE_POINT;
		pointLight1.direction = XMFLOAT3(0.0f, 0.0f, -1.0f);
		pointLight1.color = XMFLOAT3(1.0f, 1.0f, 1.0f);
		pointLight1.position = XMFLOAT3(0.0f, 0.0f, 1.0f);
		pointLight1.intensity = 0.5f;
		pointLight1.range = 100.0f;

		pointLight2.type = LIGHT_TYPE_POINT;
		pointLight2.direction = XMFLOAT3(0.0f, 0.0f, 1.0f);
		pointLight2.color = XMFLOAT3(1.0f, 1.0f, 1.0f);
		pointLight2.position = XMFLOAT3(0.0f, -1.0f, 0.0f);
		pointLight2.intensity = 0.5f;
		pointLight2.range = 100.0f;
	}
}

// --------------------------------------------------------
// Loads shaders from compiled shader object (.cso) files
// and also created the Input Layout that describes our 
// vertex data to the rendering pipeline. 
// - Input Layout creation is done here because it must 
//    be verified against vertex shader byte code
// - We'll have that byte code already loaded below
// --------------------------------------------------------
void Game::LoadShaders()
{
	//vertex shader
	vertexShader = std::make_shared<SimpleVertexShader>(
		device,
		context,
		FixPath(L"VertexShader.cso").c_str());
	//pixel shader
	pixelShader = std::make_shared<SimplePixelShader>(
		device,
		context,
		FixPath(L"PixelShader.cso").c_str());
	customShader = std::make_shared<SimplePixelShader>(
		device,
		context,
		FixPath(L"CustomPS.cso").c_str());
}

void Game::LoadTextures()
{
	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;
	D3D11_SAMPLER_DESC samplerDescription = {};
	samplerDescription.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDescription.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDescription.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDescription.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDescription.MaxAnisotropy = 8;
	samplerDescription.MaxLOD = D3D11_FLOAT32_MAX;

	device->CreateSamplerState(&samplerDescription, samplerState.GetAddressOf());

	CreateWICTextureFromFile(
		device.Get(),
		context.Get(),
		FixPath(L"../../Assets/Textures/brokentiles.png").c_str(),
		0, srvTiles.GetAddressOf());

	CreateWICTextureFromFile(
		device.Get(),
		context.Get(),
		FixPath(L"../../Assets/Textures/rustymetal.png").c_str(),
		0, srvRust.GetAddressOf());

	mat1 = std::make_shared<Material>(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), vertexShader, pixelShader, 0.0);
	mat1->AddSampler("BasicSampler", samplerState);
	mat1->AddTextureSRV("SurfaceTexture", srvTiles);

	mat1->PrepareMaterial();

}

// --------------------------------------------------------
// Creates the geometry we're going to draw - a single triangle for now
// --------------------------------------------------------
void Game::CreateGeometry()
{
	shapes[0] = std::make_shared<GameEntity>(
		std::make_shared<Mesh>(
			FixPath(L"../../Assets/Models/cube.obj").c_str(),
			device,
			context), 
		mat1);
	shapes[0]->GetTransform()->MoveAbsolute(-5, 0, 0);

	shapes[1] = std::make_shared<GameEntity>(
		std::make_shared<Mesh>(
			FixPath(L"../../Assets/Models/cylinder.obj").c_str(),
			device,
			context),
		mat1);
	shapes[1]->GetTransform()->MoveAbsolute(-2, 0, 0);

	shapes[2] = std::make_shared<GameEntity>(
		std::make_shared<Mesh>(
			FixPath(L"../../Assets/Models/helix.obj").c_str(),
			device,
			context),
		mat1);
	shapes[2]->GetTransform()->MoveAbsolute(1, 0, 0);

	shapes[3] = std::make_shared<GameEntity>(
		std::make_shared<Mesh>(
			FixPath(L"../../Assets/Models/sphere.obj").c_str(),
			device,
			context),
		mat1);
	shapes[3]->GetTransform()->MoveAbsolute(4, 0, 0);

	shapes[4] = std::make_shared<GameEntity>(
		std::make_shared<Mesh>(
			FixPath(L"../../Assets/Models/torus.obj").c_str(),
			device,
			context),
		mat1);
	shapes[4]->GetTransform()->MoveAbsolute(7, 0, 0);
}




// --------------------------------------------------------
// Handle resizing to match the new window size.
//  - DXCore needs to resize the back buffer
//  - Eventually, we'll want to update our 3D camera
// --------------------------------------------------------
void Game::OnResize()
{
	for (int i = 0; i < 3; i++) {
		camera[i]->UpdateProjectionMatrix((float)this->windowWidth / this->windowHeight);
	}
	// Handle base-level DX resize stuff
	DXCore::OnResize();
}

// --------------------------------------------------------
// Update your game here - user input, move objects, AI, etc.
// --------------------------------------------------------
void Game::Update(float deltaTime, float totalTime)
{
	{
		// Feed fresh input data to ImGui
		ImGuiIO& io = ImGui::GetIO();
		io.DeltaTime = deltaTime;
		io.DisplaySize.x = (float)this->windowWidth;
		io.DisplaySize.y = (float)this->windowHeight;


		// Reset the frame
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		// Determine new input capture
		Input& input = Input::GetInstance();
		input.SetKeyboardCapture(io.WantCaptureKeyboard);
		input.SetMouseCapture(io.WantCaptureMouse);

		// Show the demo window
		//ImGui::ShowDemoWindow(); 
		ImGui::Begin("Window");
		ImGui::Text("FPS: %f", io.Framerate);
		ImGui::Text("Window dimensions: %i x %i", windowWidth, windowHeight);
		for (int i = 0; i < 5; i++) {
			ImGui::PushID(i);
			if (ImGui::CollapsingHeader("Shape"))
			{

				if (ImGui::DragFloat3("Translation", translation[i])) {
					shapes[i]->GetTransform()->MoveAbsolute(XMFLOAT3(translation[i]));
				}
				if (ImGui::DragFloat3("Rotation", rotation[i])) {
					shapes[i]->GetTransform()->Rotate(XMFLOAT3(rotation[i]));
				}
				if (ImGui::DragFloat3("Scale", scale[i])) {
					shapes[i]->GetTransform()->Scale(XMFLOAT3(scale[i]));
				}
				if (ImGui::ColorEdit3("Color", colorOffset[i])) {
					shapes[i]->GetMesh()->SetTint(colorOffset[i][0], colorOffset[i][1], colorOffset[i][2], colorOffset[i][3]);
				}
			}
			ImGui::PopID();
		}
		if (ImGui::CollapsingHeader("Camera Settings")) {
			ImGui::Text("Camera %i x: %f y: %f z: %f",
				(activeCamera % 3) + 1,
				camera[activeCamera]->GetTransform()->GetPosition().x,
				camera[activeCamera]->GetTransform()->GetPosition().y,
				camera[activeCamera]->GetTransform()->GetPosition().z);
			ImGui::Text("FOV: %f Radians", camera[activeCamera]->GetFov());
			if (ImGui::Button("Change Camera")) {
				activeCamera = (activeCamera + 1) % 3;
			}
		}
		if (ImGui::CollapsingHeader("Light Settings")) {
			ImGui::Text("Directional Light 1 x: %f y: %f z: %f",
				directionalLight1.direction.x,
				directionalLight1.direction.y,
				directionalLight1.direction.z);
			ImGui::PushID(6);
			if (ImGui::ColorEdit3("Color", colorOffset[0])) {
				directionalLight1.color = XMFLOAT3(colorOffset[0][0], colorOffset[0][1], colorOffset[0][2]);
			}
			ImGui::PopID();
			ImGui::Text("Directional Light 2 x: %f y: %f z: %f",
				directionalLight2.direction.x,
				directionalLight2.direction.y,
				directionalLight2.direction.z);
			ImGui::PushID(7);
			if (ImGui::ColorEdit3("Color", colorOffset[1])) {
				directionalLight2.color = XMFLOAT3(colorOffset[1][0], colorOffset[1][1], colorOffset[1][2]);
			}
			ImGui::PopID();
			ImGui::Text("Directional Light 3 x: %f y: %f z: %f",
				directionalLight3.direction.x,
				directionalLight3.direction.y,
				directionalLight3.direction.z);
			ImGui::PushID(8);
			if (ImGui::ColorEdit3("Color", colorOffset[2])) {
				directionalLight2.color = XMFLOAT3(colorOffset[2][0], colorOffset[2][1], colorOffset[2][2]);
			}
			ImGui::PopID();
			ImGui::Text("Point Light 1 x: %f y: %f z: %f",
				pointLight1.direction.x,
				pointLight1.direction.y,
				pointLight1.direction.z);
			ImGui::PushID(9);
			if (ImGui::ColorEdit3("Color", colorOffset[3])) {
				pointLight1.color = XMFLOAT3(colorOffset[3][0], colorOffset[3][1], colorOffset[3][2]);
			}
			ImGui::PopID();
			ImGui::Text("Point Light 2 x: %f y: %f z: %f",
				pointLight2.direction.x,
				pointLight2.direction.y,
				pointLight2.direction.z);
			ImGui::PushID(10);
			if (ImGui::ColorEdit3("Color", colorOffset[4])) {
				pointLight2.color = XMFLOAT3(colorOffset[4][0], colorOffset[4][1], colorOffset[4][2]);
			}
			ImGui::PopID();
		}
		
		ImGui::End();
		if (input.KeyPress('C')) {
			activeCamera = (activeCamera + 1) % 3;
		}
	}
	
	camera[activeCamera]->Update(deltaTime);

	// Example input checking: Quit if the escape key is pressed
	if (Input::GetInstance().KeyDown(VK_ESCAPE))
		Quit();

}

// --------------------------------------------------------
// Clear the screen, redraw everything, present to the user
// --------------------------------------------------------
void Game::Draw(float deltaTime, float totalTime)
{

	// Frame START
	// - These things should happen ONCE PER FRAME
	// - At the beginning of Game::Draw() before drawing *anything*
	{
		// Clear the back buffer (erases what's on the screen)
		const float bgColor[4] = { 0.4f, 0.6f, 0.75f, 1.0f }; // Cornflower Blue
		context->ClearRenderTargetView(backBufferRTV.Get(), bgColor);

		// Clear the depth buffer (resets per-pixel occlusion information)
		context->ClearDepthStencilView(depthBufferDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
	}

	//setting Ambien color for material
	XMFLOAT3 ambientColor = XMFLOAT3(0.0f, 0.1f, 0.2f);

	mat1->GetPixelShader()->SetData(
		"directionalLight1",
		&directionalLight1,
		sizeof(Light));

	mat1->GetPixelShader()->SetData(
		"directionalLight2",
		&directionalLight2,
		sizeof(Light));

	mat1->GetPixelShader()->SetData(
		"directionalLight3",
		&directionalLight3,
		sizeof(Light));

	mat1->GetPixelShader()->SetData(
		"pointLight1",
		&pointLight1,
		sizeof(Light));

	mat1->GetPixelShader()->SetData(
		"pointLight2",
		&pointLight2,
		sizeof(Light));
	
	//Drawing shapes -A
	for (int i = 0; i < 5; i++) {
		//set the ambient color
		shapes[i]->GetMaterial()->GetPixelShader()->SetFloat3("ambientColor", ambientColor);
		shapes[i]->Draw(context, *camera[activeCamera]);
	}

	// Frame END
	// - These should happen exactly ONCE PER FRAME
	// - At the very end of the frame (after drawing *everything*)
	{
		// Present the back buffer to the user
		//  - Puts the results of what we've drawn onto the window
		//  - Without this, the user never sees anything
		bool vsyncNecessary = vsync || !deviceSupportsTearing || isFullscreen;
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
		swapChain->Present(
			vsyncNecessary ? 1 : 0,
			vsyncNecessary ? 0 : DXGI_PRESENT_ALLOW_TEARING);

		// Must re-bind buffers after presenting, as they become unbound
		context->OMSetRenderTargets(1, backBufferRTV.GetAddressOf(), depthBufferDSV.Get());
	}
}