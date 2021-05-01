////////////////////////////////////////////////////////////////////////////////
// Filename: util.cpp
////////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "util.h"
#include "d3dclass.h"

ScopedGPUMarker::ScopedGPUMarker(std::wstring const& markerName, D3DClass* pD3D)
	:mpD3D(pD3D)
{

	if (mpD3D)
	{
		mpD3D->PushMarker(markerName);
	}
}
ScopedGPUMarker::~ScopedGPUMarker()
{
	if (mpD3D)
	{
		mpD3D->PopMarker();
	}
	mpD3D = nullptr;
}