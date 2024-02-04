#ifndef PCH_H
#define PCH_H

#include "framework.h"

// Windows 헤더 파일
#include <windows.h>

// C 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include <d3d11.h>
#include <d2d1.h>
#include <d2d1_1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <DirectXColors.h>

#include <list>
#include <DirectXtk/BufferHelpers.h>
#include <directxtk/SimpleMath.h>
#include <string>
#include <dxgi1_4.h>
#include <memory>


using namespace Microsoft::WRL;
using namespace DirectX::SimpleMath;
using namespace DirectX;
namespace Math = DirectX::SimpleMath;

#include "../Engine/define.h"

constexpr int SND_BUF_SIZE = 1024;
constexpr int RCV_BUF_SIZE = 9604;

#define LOG_CONTENTS(message) \
{\
	std::string file = __FILE__;\
	file = file.substr(file.find_last_of("/\\") + 1);\
	printf("[Contents] FILE : %s, LINE : %d\n[Contents] MESSAGE : %s\n\n", file.c_str(), __LINE__, message);\
}
#endif