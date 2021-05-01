////////////////////////////////////////////////////////////////////////////////
// Filename: textureclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _UTIL_H_
#define _UTIL_H_

#include <string>

class D3DClass;
class ScopedGPUMarker
{
public:
	ScopedGPUMarker(std::wstring const& markerName, D3DClass* pD3D);
	~ScopedGPUMarker();

private:
	D3DClass* mpD3D = nullptr;
};
#endif