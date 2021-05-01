////////////////////////////////////////////////////////////////////////////////
// Filename: applicationclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "applicationclass.h"
#include "util.h"

ApplicationClass::ApplicationClass()
{
}


ApplicationClass::ApplicationClass(const ApplicationClass& other)
{
}


ApplicationClass::~ApplicationClass()
{
}


bool ApplicationClass::Initialize(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight)
{
	bool result;

	
	// Create the input object.  The input object will be used to handle reading the keyboard and mouse input from the user.
	m_Input = std::make_unique<InputClass>();
	if(!m_Input)
	{
		return false;
	}

	// Initialize the input object.
	result = m_Input->Initialize(hinstance, hwnd, screenWidth, screenHeight);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the input object.", L"Error", MB_OK);
		return false;
	}

	// Create the Direct3D object.
	m_D3D = std::make_unique<D3DClass>();
	if(!m_D3D)
	{
		return false;
	}

	// Initialize the Direct3D object.
	result = m_D3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize DirectX 11.", L"Error", MB_OK);
		return false;
	}

	// Create the camera object.
	m_Camera = std::make_unique<CameraClass>();
	if(!m_Camera)
	{
		return false;
	}

	// Set the initial position of the camera and build the matrices needed for rendering.
	m_Camera->SetPosition(0.0f, 0.0f, -10.0f);
	m_Camera->Render();
	m_Camera->RenderBaseViewMatrix();

	// Create the light object.
	m_Light = std::make_unique<LightClass>();
	if(!m_Light)
	{
		return false;
	}

	// Initialize the light object.
	m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetDirection(0.0f, 0.0f, 1.0f);
	
	// Create the model object.
	m_Model = std::make_unique<ModelClass>();
	if(!m_Model)
	{
		return false;
	}

	// Initialize the model object.
	result = m_Model->Initialize(m_D3D->GetDevice(), "Data/Models/cube.txt", L"Data/Textures/seafloor.dds");
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	// Create the full screen ortho window object.
	m_FullScreenWindow = std::make_unique<OrthoWindowClass>();
	if(!m_FullScreenWindow)
	{
		return false;
	}

	// Initialize the full screen ortho window object.
	result = m_FullScreenWindow->Initialize(m_D3D->GetDevice(), screenWidth, screenHeight);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the full screen ortho window object.", L"Error", MB_OK);
		return false;
	}

	// Create the deferred buffers object.
	m_DeferredBuffers = std::make_unique<GBuffersClass>();
	if(!m_DeferredBuffers)
	{
		return false;
	}

	// Initialize the deferred buffers object.
	result = m_DeferredBuffers->Initialize(m_D3D->GetDevice(), screenWidth, screenHeight, SCREEN_DEPTH, SCREEN_NEAR);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the deferred buffers object.", L"Error", MB_OK);
		return false;
	}

	// Create the deferred shader object.
	m_DeferredShader = std::make_unique<DeferredShaderClass>();
	if(!m_DeferredShader)
	{
		return false;
	}

	// Initialize the deferred shader object.
	result = m_DeferredShader->Initialize(m_D3D->GetDevice(), hwnd);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the deferred shader object.", L"Error", MB_OK);
		return false;
	}

	// Create the light shader object.
	m_LightShader = std::make_unique<LightShaderClass>();
	if(!m_LightShader)
	{
		return false;
	}

	// Initialize the light shader object.
	result = m_LightShader->Initialize(m_D3D->GetDevice(), hwnd);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the light shader object.", L"Error", MB_OK);
		return false;
	}

	return true;
}


void ApplicationClass::Shutdown()
{
	// Release the light shader object.
	if(m_LightShader)
	{
		m_LightShader->Shutdown();
		m_LightShader.reset();
	}

	// Release the deferred shader object.
	if(m_DeferredShader)
	{
		m_DeferredShader->Shutdown();
		m_DeferredShader.reset();
	}

	// Release the deferred buffers object.
	if(m_DeferredBuffers)
	{
		m_DeferredBuffers->Shutdown();
		m_DeferredBuffers.reset();
	}

	// Release the full screen ortho window object.
	if(m_FullScreenWindow)
	{
		m_FullScreenWindow->Shutdown();
		m_FullScreenWindow.reset();
	}

	// Release the model object.
	if(m_Model)
	{
		m_Model->Shutdown();
		m_Model.reset();
	}

	
	// Release the D3D object.
	if(m_D3D)
	{
		m_D3D->Shutdown();
		m_D3D.reset();
	}

	// Release the input object.
	if(m_Input)
	{
		m_Input->Shutdown();
		m_Input.reset();
	}

	return;
}


bool ApplicationClass::Frame()
{
	bool result;


	// Read the user input.
	result = m_Input->Frame();
	if(!result)
	{
		return false;
	}
	
	// Check if the user pressed escape and wants to exit the application.
	if(m_Input->IsEscapePressed() == true)
	{
		return false;
	}

	// Render the graphics.
	result = Render();
	if(!result)
	{
		return false;
	}

	return true;
}

void ApplicationClass::OnWindowResized(uint32_t width, uint32_t height)
{

}

bool ApplicationClass::Render()
{
	bool result;
	D3DXMATRIX worldMatrix, baseViewMatrix, orthoMatrix;


	// Render the scene to the render buffers.
	result = RenderSceneToTexture();
	if(!result)
	{
		return false;
	}

	ScopedGPUMarker marker(L"Render Scene", m_D3D.get());
	// Clear the scene.
	m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// Get the matrices.
	m_D3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetBaseViewMatrix(baseViewMatrix);
	m_D3D->GetOrthoMatrix(orthoMatrix);

	// Turn off the Z buffer to begin all 2D rendering.
	m_D3D->TurnZBufferOff();

	// Put the full screen ortho window vertex and index buffers on the graphics pipeline to prepare them for drawing.
	m_FullScreenWindow->Render(m_D3D->GetDeviceContext());

	// Render the full screen ortho window using the deferred light shader and the render buffers.
	{
		ScopedGPUMarker marker(L"LightShader", m_D3D.get());

		m_LightShader->Render(m_D3D->GetDeviceContext(), m_FullScreenWindow->GetIndexCount(), worldMatrix, baseViewMatrix, orthoMatrix,
			m_DeferredBuffers->GetShaderResourceView(0), m_DeferredBuffers->GetShaderResourceView(1),
			m_Light->GetDirection());
	}
	// Turn the Z buffer back on now that all 2D rendering has completed.
	m_D3D->TurnZBufferOn();
	
	// Present the rendered scene to the screen.
	m_D3D->EndScene();

	return true;
}


bool ApplicationClass::RenderSceneToTexture()
{
	ScopedGPUMarker marker(L"GBuffer Pass", m_D3D.get());

	D3DXMATRIX worldMatrix, viewMatrix, projectionMatrix;


	// Set the render buffers to be the render target.
	m_DeferredBuffers->SetRenderTargets(m_D3D->GetDeviceContext());

	// Clear the render buffers.
	m_DeferredBuffers->ClearRenderTargets(m_D3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	// Get the matrices from the camera and d3d objects.
	m_D3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);

	// Update the rotation variable each frame.
	static float rotation = 0.0f;
	rotation += (float)D3DX_PI * 0.01f;
	if(rotation > 360.0f)
	{
		rotation -= 360.0f;
	}

	// Rotate the world matrix by the rotation value so that the cube will spin.
	D3DXMatrixRotationY(&worldMatrix, rotation);

	// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	m_Model->Render(m_D3D->GetDeviceContext());

	// Render the model using the deferred shader.
	m_DeferredShader->Render(m_D3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model->GetTexture());

	// Reset the render target back to the original back buffer and not the render buffers anymore.
	m_D3D->SetBackBufferRenderTarget();

	// Reset the viewport back to the original.
	m_D3D->ResetViewport();

	return true;
}