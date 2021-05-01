////////////////////////////////////////////////////////////////////////////////
// Filename: applicationclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _APPLICATIONCLASS_H_
#define _APPLICATIONCLASS_H_


/////////////
// GLOBALS //
/////////////
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;


///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "inputclass.h"
#include "d3dclass.h"
#include "cameraclass.h"
#include "lightclass.h"
#include "modelclass.h"
#include "orthowindowclass.h"
#include "GBuffersClass.h"
#include "deferredshaderclass.h"
#include "lightshaderclass.h"


////////////////////////////////////////////////////////////////////////////////
// Class name: ApplicationClass
////////////////////////////////////////////////////////////////////////////////
class ApplicationClass
{
public:
	ApplicationClass();
	ApplicationClass(const ApplicationClass&);
	~ApplicationClass();

	bool Initialize(HINSTANCE, HWND, int, int);
	void Shutdown();
	bool Frame();

	void OnWindowResized(uint32_t width, uint32_t height);

private:
	bool Render();
	bool RenderSceneToTexture();

private:
	std::unique_ptr<InputClass> m_Input;
	std::unique_ptr<D3DClass> m_D3D;
	std::unique_ptr<CameraClass> m_Camera;
	std::unique_ptr<LightClass> m_Light;
	std::unique_ptr<ModelClass> m_Model;
	std::unique_ptr<OrthoWindowClass> m_FullScreenWindow;
	std::unique_ptr<GBuffersClass> m_DeferredBuffers;
	std::unique_ptr<DeferredShaderClass> m_DeferredShader;
	std::unique_ptr<LightShaderClass> m_LightShader;
};

#endif