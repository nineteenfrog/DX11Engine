#include "Game.h"
#include "Vertex.h"
#include "Input.h"
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
	XMStoreFloat4x4(&lightViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&lightProjectionMatrix, XMMatrixIdentity());
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
	LoadShaders();
	LoadTextures();
	CreateGeometry();
	LoadSky();

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
		directionalLight1.color = XMFLOAT3(0.0f, 0.0f, 0.0f);
		directionalLight1.intensity = 0.5f;

		directionalLight2.type = LIGHT_TYPE_DIRECTIONAL;
		directionalLight2.direction = XMFLOAT3(1.0f, -1.0f, 0.0f);
		directionalLight2.color = XMFLOAT3(1.0f, 1.0f, 1.0f);
		directionalLight2.intensity = 0.5f;

		directionalLight3.type = LIGHT_TYPE_DIRECTIONAL;
		directionalLight3.direction = XMFLOAT3(0.0f, 0.0f, 1.0f);
		directionalLight3.color = XMFLOAT3(0.0f, 0.0f, 0.0f);
		directionalLight3.intensity = 0.5f;

		pointLight1.type = LIGHT_TYPE_POINT;
		pointLight1.direction = XMFLOAT3(0.0f, 0.0f, -1.0f);
		pointLight1.color = XMFLOAT3(0.0f, 0.0f, 0.0f);
		pointLight1.position = XMFLOAT3(0.0f, 0.0f, 1.0f);
		pointLight1.intensity = 0.5f;
		pointLight1.range = 100.0f;

		pointLight2.type = LIGHT_TYPE_POINT;
		pointLight2.direction = XMFLOAT3(0.0f, 0.0f, 1.0f);
		pointLight2.color = XMFLOAT3(0.0f, 0.0f, 0.0f);
		pointLight2.position = XMFLOAT3(0.0f, -1.0f, 0.0f);
		pointLight2.intensity = 0.5f;
		pointLight2.range = 100.0f;
	}
	CreateShadows();

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

	skyVS = std::make_shared<SimpleVertexShader>(
		device,
		context,
		FixPath(L"SkyVertexShader.cso").c_str());

	skyPS = std::make_shared<SimplePixelShader>(
		device,
		context,
		FixPath(L"SkyPixelShader.cso").c_str());
	
	shadowVS = std::make_shared<SimpleVertexShader>(
		device,
		context,
		FixPath(L"ShadowVS.cso").c_str());
}

void Game::LoadTextures()
{
	D3D11_SAMPLER_DESC samplerDescription = {};
	samplerDescription.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDescription.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDescription.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDescription.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDescription.MaxAnisotropy = 8;
	samplerDescription.MaxLOD = D3D11_FLOAT32_MAX;

	device->CreateSamplerState(&samplerDescription, samplerState.GetAddressOf());

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> bronzeSRVA, bronzeSRVN, bronzeSRVR, bronzeSRVM;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> cobblestoneSRVA, cobblestoneSRVN, cobblestoneSRVR, cobblestoneSRVM;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> floorSRVA, floorSRVN, floorSRVR, floorSRVM;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> paintSRVA, paintSRVN, paintSRVR, paintSRVM;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> scratchSRVA, scratchSRVN, scratchSRVR, scratchSRVM;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> woodSRVA, woodSRVN, woodSRVR, woodSRVM;

	//Bronze Textures 
	CreateWICTextureFromFile(
		device.Get(),
		context.Get(),
		FixPath(L"../../Assets/Textures/PBR/bronze_albedo.png").c_str(),
		0, bronzeSRVA.GetAddressOf());

	CreateWICTextureFromFile(
		device.Get(),
		context.Get(),
		FixPath(L"../../Assets/Textures/PBR/bronze_normals.png").c_str(),
		0, bronzeSRVN.GetAddressOf());

	CreateWICTextureFromFile(
		device.Get(),
		context.Get(),
		FixPath(L"../../Assets/Textures/PBR/bronze_roughness.png").c_str(),
		0, bronzeSRVR.GetAddressOf());

	CreateWICTextureFromFile(
		device.Get(),
		context.Get(),
		FixPath(L"../../Assets/Textures/PBR/bronze_metal.png").c_str(),
		0, bronzeSRVM.GetAddressOf());

	mat1 = std::make_shared<Material>(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), vertexShader, pixelShader, 0.0);
	mat1->AddSampler("BasicSampler", samplerState);
	mat1->AddTextureSRV("Albedo", bronzeSRVA);
	mat1->AddTextureSRV("NormalMap", bronzeSRVN);
	mat1->AddTextureSRV("RoughnessMap", bronzeSRVR);
	mat1->AddTextureSRV("MetalnessMap", bronzeSRVM);

	//Cobblestone Textures 
	CreateWICTextureFromFile(
		device.Get(),
		context.Get(),
		FixPath(L"../../Assets/Textures/PBR/cobblestone_albedo.png").c_str(),
		0, cobblestoneSRVA.GetAddressOf());

	CreateWICTextureFromFile(
		device.Get(),
		context.Get(),
		FixPath(L"../../Assets/Textures/PBR/cobblestone_normals.png").c_str(),
		0, cobblestoneSRVN.GetAddressOf());

	CreateWICTextureFromFile(
		device.Get(),
		context.Get(),
		FixPath(L"../../Assets/Textures/PBR/cobblestone_roughness.png").c_str(),
		0, cobblestoneSRVR.GetAddressOf());

	CreateWICTextureFromFile(
		device.Get(),
		context.Get(),
		FixPath(L"../../Assets/Textures/PBR/cobblestone_metal.png").c_str(),
		0, cobblestoneSRVM.GetAddressOf());

	mat2 = std::make_shared<Material>(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), vertexShader, pixelShader, 0.0);
	mat2->AddSampler("BasicSampler", samplerState);
	mat2->AddTextureSRV("Albedo", cobblestoneSRVA);
	mat2->AddTextureSRV("NormalMap", cobblestoneSRVN);
	mat2->AddTextureSRV("RoughnessMap", cobblestoneSRVR);
	mat2->AddTextureSRV("MetalnessMap", cobblestoneSRVM);

	//Floor Textures 
	CreateWICTextureFromFile(
		device.Get(),
		context.Get(),
		FixPath(L"../../Assets/Textures/PBR/floor_albedo.png").c_str(),
		0, floorSRVA.GetAddressOf());

	CreateWICTextureFromFile(
		device.Get(),
		context.Get(),
		FixPath(L"../../Assets/Textures/PBR/floor_normals.png").c_str(),
		0, floorSRVN.GetAddressOf());

	CreateWICTextureFromFile(
		device.Get(),
		context.Get(),
		FixPath(L"../../Assets/Textures/PBR/floor_roughness.png").c_str(),
		0, floorSRVR.GetAddressOf());

	CreateWICTextureFromFile(
		device.Get(),
		context.Get(),
		FixPath(L"../../Assets/Textures/PBR/floor_metal.png").c_str(),
		0, floorSRVM.GetAddressOf());

	mat3 = std::make_shared<Material>(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), vertexShader, pixelShader, 0.0);
	mat3->AddSampler("BasicSampler", samplerState);
	mat3->AddTextureSRV("Albedo", floorSRVA);
	mat3->AddTextureSRV("NormalMap", floorSRVN);
	mat3->AddTextureSRV("RoughnessMap", floorSRVR);
	mat3->AddTextureSRV("MetalnessMap", floorSRVM);

	//Paint Textures 
	CreateWICTextureFromFile(
		device.Get(),
		context.Get(),
		FixPath(L"../../Assets/Textures/PBR/paint_albedo.png").c_str(),
		0, paintSRVA.GetAddressOf());

	CreateWICTextureFromFile(
		device.Get(),
		context.Get(),
		FixPath(L"../../Assets/Textures/PBR/paint_normals.png").c_str(),
		0, paintSRVN.GetAddressOf());

	CreateWICTextureFromFile(
		device.Get(),
		context.Get(),
		FixPath(L"../../Assets/Textures/PBR/paint_roughness.png").c_str(),
		0, paintSRVR.GetAddressOf());

	CreateWICTextureFromFile(
		device.Get(),
		context.Get(),
		FixPath(L"../../Assets/Textures/PBR/paint_metal.png").c_str(),
		0, paintSRVM.GetAddressOf());

	mat4 = std::make_shared<Material>(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), vertexShader, pixelShader, 0.0);
	mat4->AddSampler("BasicSampler", samplerState);
	mat4->AddTextureSRV("Albedo", paintSRVA);
	mat4->AddTextureSRV("NormalMap", paintSRVN);
	mat4->AddTextureSRV("RoughnessMap", paintSRVR);
	mat4->AddTextureSRV("MetalnessMap", paintSRVM);

	//Rough Textures 
	CreateWICTextureFromFile(
		device.Get(),
		context.Get(),
		FixPath(L"../../Assets/Textures/PBR/scratched_albedo.png").c_str(),
		0, scratchSRVA.GetAddressOf());

	CreateWICTextureFromFile(
		device.Get(),
		context.Get(),
		FixPath(L"../../Assets/Textures/PBR/scratched_normals.png").c_str(),
		0, scratchSRVN.GetAddressOf());

	CreateWICTextureFromFile(
		device.Get(),
		context.Get(),
		FixPath(L"../../Assets/Textures/PBR/scratched_roughness.png").c_str(),
		0, scratchSRVR.GetAddressOf());

	CreateWICTextureFromFile(
		device.Get(),
		context.Get(),
		FixPath(L"../../Assets/Textures/PBR/scratched_metal.png").c_str(),
		0, scratchSRVM.GetAddressOf());

	mat5 = std::make_shared<Material>(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), vertexShader, pixelShader, 0.0);
	mat5->AddSampler("BasicSampler", samplerState);
	mat5->AddTextureSRV("Albedo", scratchSRVA);
	mat5->AddTextureSRV("NormalMap", scratchSRVN);
	mat5->AddTextureSRV("RoughnessMap", scratchSRVR);
	mat5->AddTextureSRV("MetalnessMap", scratchSRVM);

	//Wood Textures 
	CreateWICTextureFromFile(
		device.Get(),
		context.Get(),
		FixPath(L"../../Assets/Textures/PBR/wood_albedo.png").c_str(),
		0, woodSRVA.GetAddressOf());

	CreateWICTextureFromFile(
		device.Get(),
		context.Get(),
		FixPath(L"../../Assets/Textures/PBR/wood_normals.png").c_str(),
		0, woodSRVN.GetAddressOf());

	CreateWICTextureFromFile(
		device.Get(),
		context.Get(),
		FixPath(L"../../Assets/Textures/PBR/wood_roughness.png").c_str(),
		0, woodSRVR.GetAddressOf());

	CreateWICTextureFromFile(
		device.Get(),
		context.Get(),
		FixPath(L"../../Assets/Textures/PBR/wood_metal.png").c_str(),
		0, woodSRVM.GetAddressOf());

	mat6 = std::make_shared<Material>(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), vertexShader, pixelShader, 0.0);
	mat6->AddSampler("BasicSampler", samplerState);
	mat6->AddTextureSRV("Albedo", woodSRVA);
	mat6->AddTextureSRV("NormalMap", woodSRVN);
	mat6->AddTextureSRV("RoughnessMap", woodSRVR);
	mat6->AddTextureSRV("MetalnessMap", woodSRVM);
}

void Game::LoadSky()
{
	sky = Sky(
		skyMesh,
		samplerState,
		device,
		srvSky,
		skyVS,
		skyPS,
		context);

	sky.SetSrv(sky.CreateCubemap(
		FixPath(L"../../Assets/Planet/right.png").c_str(),
		FixPath(L"../../Assets/Planet/left.png").c_str(),
		FixPath(L"../../Assets/Planet/up.png").c_str(),
		FixPath(L"../../Assets/Planet/down.png").c_str(),
		FixPath(L"../../Assets/Planet/front.png").c_str(),
		FixPath(L"../../Assets/Planet/back.png").c_str()));
}

void Game::CreateShadows()
{
	// Create the actual texture that will be the shadow map
	D3D11_TEXTURE2D_DESC shadowDesc = {};
	shadowDesc.Width = shadowMapResolution; // Ideally a power of 2 (like 1024)
	shadowDesc.Height = shadowMapResolution; // Ideally a power of 2 (like 1024)
	shadowDesc.ArraySize = 1;
	shadowDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	shadowDesc.CPUAccessFlags = 0;
	shadowDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	shadowDesc.MipLevels = 1;
	shadowDesc.MiscFlags = 0;
	shadowDesc.SampleDesc.Count = 1;
	shadowDesc.SampleDesc.Quality = 0;
	shadowDesc.Usage = D3D11_USAGE_DEFAULT;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> shadowTexture;
	device->CreateTexture2D(&shadowDesc, 0, shadowTexture.GetAddressOf());

	// Create the depth/stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC shadowDSDesc = {};
	shadowDSDesc.Format = DXGI_FORMAT_D32_FLOAT;
	shadowDSDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	shadowDSDesc.Texture2D.MipSlice = 0;
	device->CreateDepthStencilView(
		shadowTexture.Get(),
		&shadowDSDesc,
		shadowDSV.GetAddressOf());
	// Create the SRV for the shadow map
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;
	device->CreateShaderResourceView(
		shadowTexture.Get(),
		&srvDesc,
		shadowSRV.GetAddressOf());

	Microsoft::WRL::ComPtr<ID3D11SamplerState> shadowSampler;
	D3D11_SAMPLER_DESC shadowSampDesc = {};
	shadowSampDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
	shadowSampDesc.ComparisonFunc = D3D11_COMPARISON_LESS;
	shadowSampDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSampDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSampDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSampDesc.BorderColor[0] = 1.0f; // Only need the first component
	device->CreateSamplerState(&shadowSampDesc, &this->shadowSampler);

	XMVECTOR lightDirection = XMVectorSet(
		directionalLight2.direction.x,
		directionalLight2.direction.y,
		directionalLight2.direction.z, 0.0f);

	XMMATRIX lightView = XMMatrixLookToLH(
		-lightDirection * 20, // Position: "Backing up" 20 units from origin
		lightDirection, // Direction: light's direction
		XMVectorSet(1, 0, 0, 0)); // Up: World up vector (Y axis)

	float lightProjectionSize = 15.0f; // Tweak for your scene!
	XMMATRIX lightProjection = XMMatrixOrthographicLH(
		lightProjectionSize,
		lightProjectionSize,
		1.0f,
		100.0f);

	XMStoreFloat4x4(&lightViewMatrix, lightView);
	XMStoreFloat4x4(&lightProjectionMatrix, lightProjection);
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
	shapes[0]->GetTransform()->MoveAbsolute(-12, 0, 0);

	shapes[1] = std::make_shared<GameEntity>(
		std::make_shared<Mesh>(
			FixPath(L"../../Assets/Models/cylinder.obj").c_str(),
			device,
			context),
		mat2);
	shapes[1]->GetTransform()->MoveAbsolute(-5, 0, 0);

	shapes[2] = std::make_shared<GameEntity>(
		std::make_shared<Mesh>(
			FixPath(L"../../Assets/Models/helix.obj").c_str(),
			device,
			context),
		mat3);
	shapes[2]->GetTransform()->MoveAbsolute(0, 0, 0);

	shapes[3] = std::make_shared<GameEntity>(
		std::make_shared<Mesh>(
			FixPath(L"../../Assets/Models/sphere.obj").c_str(),
			device,
			context),
		mat4);
	shapes[3]->GetTransform()->MoveAbsolute(5, 0, 0);

	shapes[4] = std::make_shared<GameEntity>(
		std::make_shared<Mesh>(
			FixPath(L"../../Assets/Models/torus.obj").c_str(),
			device,
			context),
		mat5);
	shapes[4]->GetTransform()->MoveAbsolute(10, 0, 0);

	shapes[5] = std::make_shared<GameEntity>(std::make_shared<Mesh>(
		FixPath(L"../../Assets/Models/cube.obj").c_str(),
		device,
		context),
		mat6);
	shapes[5]->GetTransform()->Scale(15.0f, 1.0f, 10.0f);
	shapes[5]->GetTransform()->MoveAbsolute(0, -2.5f, 0);

	skyMesh = std::make_shared<Mesh>(
		FixPath(L"../../Assets/Models/cube.obj").c_str(),
		device,
		context);
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
		for (int i = 0; i < 6; i++) {
			ImGui::PushID(i);
			if (ImGui::CollapsingHeader("Shape"))
			{

				if (ImGui::DragFloat3("Translation", translation[i])) {
					shapes[i]->GetTransform()->SetPosition(XMFLOAT3(translation[i]));
				}
				if (ImGui::DragFloat3("Rotation", rotation[i])) {
					shapes[i]->GetTransform()->SetRotation(XMFLOAT3(rotation[i]));
				}
				if (ImGui::DragFloat3("Scale", scale[i])) {
					shapes[i]->GetTransform()->SetScale(XMFLOAT3(scale[i]));
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
				directionalLight3.color = XMFLOAT3(colorOffset[2][0], colorOffset[2][1], colorOffset[2][2]);
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
		//ImGui::Image(shadowSRV.Get(), ImVec2(1024, 1024));

		ImGui::End();
		if (input.KeyPress('C')) {
			activeCamera = (activeCamera + 1) % 3;
		}
	}

	//Shape movement
	if (counter < 200 && going) {
		shapes[0]->GetTransform()->MoveAbsolute(0.02f, 0, 0);
		shapes[1]->GetTransform()->Scale(0.999f, 0.999f, 0.999f);
		shapes[2]->GetTransform()->MoveAbsolute(0, 0.02f, 0);
		shapes[3]->GetTransform()->Scale(1.001f, 1.001f, 1.001f);
		shapes[4]->GetTransform()->MoveAbsolute(0, 0, 0.02f);
		counter++;
	}
	else {
		if (counter <= 0) {
			going = true;
		}
		else {
			going = false;
		}
		shapes[0]->GetTransform()->MoveAbsolute(-0.02f, 0, 0);
		shapes[1]->GetTransform()->Scale(1.001f, 1.001f, 1.001f);
		shapes[2]->GetTransform()->MoveAbsolute(0, -0.02f, 0);
		shapes[3]->GetTransform()->Scale(0.999f, 0.999f, 0.999f);
		shapes[4]->GetTransform()->MoveAbsolute(0, 0, -0.02f);
		counter--;
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
	{
		Microsoft::WRL::ComPtr<ID3D11RasterizerState> shadowRasterizer;
		D3D11_RASTERIZER_DESC shadowRastDesc = {};
		shadowRastDesc.FillMode = D3D11_FILL_SOLID;
		shadowRastDesc.CullMode = D3D11_CULL_BACK;
		shadowRastDesc.DepthClipEnable = true;
		shadowRastDesc.DepthBias = 1000; // Min. precision units, not world units!
		shadowRastDesc.SlopeScaledDepthBias = 1.0f; // Bias more based on slope
		device->CreateRasterizerState(&shadowRastDesc, &shadowRasterizer);
		context->RSSetState(shadowRasterizer.Get());

		//Shadow map render
		context->ClearDepthStencilView(shadowDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
		ID3D11RenderTargetView* nullRTV{};
		context->OMSetRenderTargets(1, &nullRTV, shadowDSV.Get());
		context->PSSetShader(0, 0, 0);
		D3D11_VIEWPORT viewport = {};
		viewport.Width = (float)shadowMapResolution;
		viewport.Height = (float)shadowMapResolution;
		viewport.MaxDepth = 1.0f;
		context->RSSetViewports(1, &viewport);
		shadowVS->SetShader();
		shadowVS->SetMatrix4x4("view", lightViewMatrix);
		shadowVS->SetMatrix4x4("projection", lightProjectionMatrix);
		// Loop and draw all entities
		for (int i = 0; i < 6; i++) {
			shadowVS->SetMatrix4x4("world", shapes[i]->GetTransform()->GetWorldMatrix());
			shadowVS->CopyAllBufferData();

			// Draw the mesh directly to avoid the entity's material
			// Note: Your code may differ significantly here!
			shapes[i]->GetMesh()->Draw();
		}
		viewport.Width = (float)this->windowWidth;
		viewport.Height = (float)this->windowHeight;
		context->RSSetViewports(1, &viewport);
		context->OMSetRenderTargets(
			1,
			backBufferRTV.GetAddressOf(),
			depthBufferDSV.Get());
		context->RSSetState(0);	
	}

	// Frame START
	// - These things should happen ONCE PER FRAME
	// - At the beginning of Game::Draw() before drawing *anything*
	{
		// Clear the back buffer (erases what's on the screen)
		const float bgColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f }; // Black
		context->ClearRenderTargetView(backBufferRTV.Get(), bgColor);

		// Clear the depth buffer (resets per-pixel occlusion information)
		context->ClearDepthStencilView(depthBufferDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
	}

	//setting Ambien color for material
	XMFLOAT3 ambientColor = XMFLOAT3(0.0f, 0.1f, 0.2f);

	//Drawing shapes -A
	for (int i = 0; i < 6; i++) {
		shapes[i]->GetMaterial()->AddTextureSRV(
			"ShadowMap", 
			shadowSRV);

		shapes[i]->GetMaterial()->AddSampler(
			"ShadowSampler",
			shadowSampler);
		shapes[i]->GetMaterial()->PrepareMaterial();
		
		shapes[i]->GetMaterial()->GetVertexShader()->SetMatrix4x4(
			"lightView",
			lightViewMatrix);

		shapes[i]->GetMaterial()->GetVertexShader()->SetMatrix4x4(
			"lightProjection", 
			lightProjectionMatrix);


		shapes[i]->GetMaterial()->GetPixelShader()->SetData(
			"directionalLight1",
			&directionalLight1,
			sizeof(Light));

		shapes[i]->GetMaterial()->GetPixelShader()->SetData(
			"directionalLight2",
			&directionalLight2,
			sizeof(Light));

		shapes[i]->GetMaterial()->GetPixelShader()->SetData(
			"directionalLight3",
			&directionalLight3,
			sizeof(Light));

		shapes[i]->GetMaterial()->GetPixelShader()->SetData(
			"pointLight1",
			&pointLight1,
			sizeof(Light));

		shapes[i]->GetMaterial()->GetPixelShader()->SetData(
			"pointLight2",
			&pointLight2,
			sizeof(Light));
		//set the ambient color
		shapes[i]->GetMaterial()->GetPixelShader()->SetFloat3(
			"ambientColor", 
			ambientColor);
		

		shapes[i]->Draw(context, *camera[activeCamera]);
	}

	sky.Draw(camera[activeCamera]);

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

		ID3D11ShaderResourceView* nullSRVs[128] = {};
		context->PSSetShaderResources(0, 128, nullSRVs);
	}
}