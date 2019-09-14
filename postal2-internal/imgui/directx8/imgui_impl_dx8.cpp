// ImGui Win32 + DirectX9 binding
// In this binding, ImTextureID is used to store a 'LPDIRECT3DTEXTURE9' texture identifier. Read the FAQ about ImTextureID in imgui.cpp.

// You can copy and use unmodified imgui_impl_* files in your project. See main.cpp for an example of using this.
// If you use this binding you'll need to call 4 functions: ImGui_ImplXXXX_Init(), ImGui_ImplXXXX_NewFrame(), ImGui::Render() and ImGui_ImplXXXX_Shutdown().
// If you are new to ImGui, see examples/README.txt and documentation at the top of imgui.cpp.
// https://github.com/ocornut/imgui

#include "../imgui.h"
#include "imgui_impl_dx8.h"

// DirectX
#include <d3d8.h>
#include <directxmath.h>
#define DIRECTINPUT_VERSION 0x0800
#define TOUHOU_ON_D3D8 1
#include <dinput.h>
#include <algorithm>

#include <wrl.h>

using LockedPointer = BYTE *;
using Microsoft::WRL::ComPtr;

// Data
static HWND                     g_hWnd = 0;
static INT64                    g_Time = 0;
static INT64                    g_TicksPerSecond = 0;
static LPDIRECT3DDEVICE8        g_pd3dDevice = NULL;
static LPDIRECT3DVERTEXBUFFER8  g_pVB = NULL;
static LPDIRECT3DINDEXBUFFER8   g_pIB = NULL;
static LPDIRECT3DTEXTURE8       g_FontTexture = NULL;
static int                      g_VertexBufferSize = 5000, g_IndexBufferSize = 10000;
static ImGuiMouseCursor         g_LastMouseCursor = ImGuiMouseCursor_COUNT;

struct CUSTOMVERTEX
{
	float    pos[3];
	DWORD col;
	float    uv[2];
};
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1)

// Structs for updating and restoring device states by RAII

struct ScopedRS
{
	D3DRENDERSTATETYPE renderStateType;
	DWORD previousValue;

	ScopedRS(D3DRENDERSTATETYPE inRenderStateType, DWORD value)
		: renderStateType(inRenderStateType)
	{
		g_pd3dDevice->GetRenderState(renderStateType, &previousValue);
		g_pd3dDevice->SetRenderState(renderStateType, value);
	}

	~ScopedRS()
	{
		g_pd3dDevice->SetRenderState(renderStateType, previousValue);
	}
};

struct ScopedTSS
{
	DWORD textureStage;
	D3DTEXTURESTAGESTATETYPE textureStageStateType;
	DWORD previousValue;

	ScopedTSS(DWORD inTextureStage, D3DTEXTURESTAGESTATETYPE inTextureStageStateType, DWORD value)
		: textureStage(inTextureStage)
		, textureStageStateType(inTextureStageStateType)
	{
		g_pd3dDevice->GetTextureStageState(textureStage, textureStageStateType, &previousValue);
		g_pd3dDevice->SetTextureStageState(textureStage, textureStageStateType, value);
	}

	~ScopedTSS()
	{
		g_pd3dDevice->SetTextureStageState(textureStage, textureStageStateType, previousValue);
	}
};

struct ScopedTex
{
	DWORD textureStage;
	ComPtr<IDirect3DBaseTexture8> previousTexture;

	ScopedTex(DWORD inTextureStage, IDirect3DBaseTexture8* texture)
		: textureStage(inTextureStage)
	{
		g_pd3dDevice->GetTexture(textureStage, &previousTexture);
		g_pd3dDevice->SetTexture(textureStage, texture);
	}

	ScopedTex(DWORD inTextureStage)
		: textureStage(inTextureStage)
	{
		g_pd3dDevice->GetTexture(textureStage, &previousTexture);
	}

	~ScopedTex()
	{
		g_pd3dDevice->SetTexture(textureStage, previousTexture.Get());
	}
};

struct ScopedTransform
{
	D3DTRANSFORMSTATETYPE transformStateType;
	D3DMATRIX previousTransformMatrix;

	ScopedTransform(D3DTRANSFORMSTATETYPE inTransformType, const D3DMATRIX& transformMatrix)
		: transformStateType(inTransformType)
	{
		g_pd3dDevice->GetTransform(transformStateType, &previousTransformMatrix);
		g_pd3dDevice->SetTransform(transformStateType, &transformMatrix);
	}

	ScopedTransform(D3DTRANSFORMSTATETYPE inTransformType)
	{
		g_pd3dDevice->GetTransform(transformStateType, &previousTransformMatrix);
	}

	~ScopedTransform()
	{
		g_pd3dDevice->SetTransform(transformStateType, &previousTransformMatrix);
	}
};


void THRotatorImGui_RenderDrawLists(ImDrawData* drawData)
{
	ImGuiIO& io = ImGui::GetIO();

	// Avoid rendering when minimized
	if (io.DisplaySize.x <= 0.0f || io.DisplaySize.y <= 0.0f)
	{
		return;
	}


	if (!g_pVB || g_VertexBufferSize < drawData->TotalVtxCount)
	{
		g_VertexBufferSize = drawData->TotalVtxCount + 5000;
		if (FAILED(g_pd3dDevice->CreateVertexBuffer(g_VertexBufferSize * sizeof(CUSTOMVERTEX),
			D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, D3DFVF_CUSTOMVERTEX, D3DPOOL_DEFAULT, &g_pVB
		)))
		{
			return;
		}
	}

	if (!g_pIB || g_IndexBufferSize < drawData->TotalIdxCount)
	{
		g_IndexBufferSize = drawData->TotalIdxCount + 10000;
		if (FAILED(g_pd3dDevice->CreateIndexBuffer(g_IndexBufferSize * sizeof(ImDrawIdx),
			D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, sizeof(ImDrawIdx) == 2 ? D3DFMT_INDEX16 : D3DFMT_INDEX32,
			D3DPOOL_DEFAULT, &g_pIB)))
		{
			return;
		}
	}

	// Updating vertex buffer content


	CUSTOMVERTEX* pLockedVerticesData;
	if (FAILED(g_pVB->Lock(0, drawData->TotalVtxCount * sizeof(CUSTOMVERTEX), (LockedPointer*)& pLockedVerticesData,
		D3DLOCK_DISCARD)))
	{
		return;
	}

	for (int n = 0; n < drawData->CmdListsCount; n++)
	{
		const ImDrawList* commandList = drawData->CmdLists[n];
		const ImDrawVert* pSourceVerticesData = commandList->VtxBuffer.Data;
		for (int i = 0; i < commandList->VtxBuffer.Size; i++)
		{
			pLockedVerticesData->pos[0] = pSourceVerticesData->pos.x;
			pLockedVerticesData->pos[1] = pSourceVerticesData->pos.y;
			pLockedVerticesData->pos[2] = 0.0f;
			pLockedVerticesData->col = (pSourceVerticesData->col & 0xFF00FF00) | ((pSourceVerticesData->col & 0xFF0000) >> 16) | ((pSourceVerticesData->col & 0xFF) << 16);     // RGBA --> ARGB for DirectX9
			pLockedVerticesData->uv[0] = pSourceVerticesData->uv.x;
			pLockedVerticesData->uv[1] = pSourceVerticesData->uv.y;
			pLockedVerticesData++;
			pSourceVerticesData++;
		}
	}

	g_pVB->Unlock();

	// Updating index buffer content

	ImDrawIdx* pLockedIndicesData;
	if (FAILED(g_pIB->Lock(0, drawData->TotalIdxCount * sizeof(ImDrawIdx), (LockedPointer*)& pLockedIndicesData,
		D3DLOCK_DISCARD)))
	{
		return;
	}

	for (int n = 0; n < drawData->CmdListsCount; n++)
	{
		const ImDrawList* commandList = drawData->CmdLists[n];
		memcpy(pLockedIndicesData, commandList->IdxBuffer.Data, commandList->IdxBuffer.Size * sizeof(ImDrawIdx));
		pLockedIndicesData += commandList->IdxBuffer.Size;
	}

	g_pIB->Unlock();

#ifdef TOUHOU_ON_D3D8
	DWORD stateBlock;
#else
	ComPtr<IDirect3DStateBlock9> stateBlock;
#endif
	if (FAILED(g_pd3dDevice->CreateStateBlock(D3DSBT_ALL, &stateBlock)))
	{
		return;
	}

	THRotatorImGui_UpdateAndApplyFixedStates();

	auto rawDevice = g_pd3dDevice;

	rawDevice->SetStreamSource(0, g_pVB,
#ifndef TOUHOU_ON_D3D8
		0,
#endif
		sizeof(CUSTOMVERTEX));

#ifndef TOUHOU_ON_D3D8
	D3DMATRIX matProjection;
	const float L = 0.5f, R = io.DisplaySize.x + 0.5f, T = 0.5f, B = io.DisplaySize.y + 0.5f;
	DirectX::XMStoreFloat4x4(reinterpret_cast<DirectX::XMFLOAT4X4*>(&matProjection),
		DirectX::XMMatrixOrthographicOffCenterLH(L, R, B, T, 0.0f, 1.0f));

	rawDevice->SetIndices(g_pIB.Get());
	rawDevice->SetTransform(D3DTS_PROJECTION, &matProjection);

	// Setup viewport
	D3DVIEWPORT9 vp;
	vp.X = vp.Y = 0;
	vp.Width = (DWORD)io.DisplaySize.x;
	vp.Height = (DWORD)io.DisplaySize.y;
	vp.MinZ = 0.0f;
	vp.MaxZ = 1.0f;
	rawDevice->SetViewport(&vp);
#endif

	int vertexBufferOffset = 0;
	int indexBufferOffset = 0;
	for (int commandListIndex = 0; commandListIndex < drawData->CmdListsCount; commandListIndex++)
	{
		ImDrawList* commandList = drawData->CmdLists[commandListIndex];
		for (int commandIndex = 0; commandIndex < commandList->CmdBuffer.Size; commandIndex++)
		{
			const ImDrawCmd& command = commandList->CmdBuffer[commandIndex];
			if (command.UserCallback)
			{
				command.UserCallback(commandList, &command);
			}
			else
			{
				rawDevice->SetTexture(0, static_cast<IDirect3DBaseTexture8*>(command.TextureId));

#ifdef TOUHOU_ON_D3D8
				D3DVIEWPORT8 viewport;
				viewport.X = static_cast<DWORD>((std::max)(0.0f, command.ClipRect.x));
				viewport.Y = static_cast<DWORD>((std::max)(0.0f, command.ClipRect.y));
				viewport.Width = static_cast<DWORD>((std::min)(io.DisplaySize.x, command.ClipRect.z - command.ClipRect.x));
				viewport.Height = static_cast<DWORD>((std::min)(io.DisplaySize.y, command.ClipRect.w - command.ClipRect.y));
				viewport.MinZ = 0.0f;
				viewport.MaxZ = 1.0f;
				rawDevice->SetViewport(&viewport);

				const float L = 0.5f + viewport.X, R = viewport.Width + 0.5f + viewport.X;
				const float T = 0.5f + viewport.Y, B = viewport.Height + 0.5f + viewport.Y;

				D3DMATRIX matProjection;
				DirectX::XMStoreFloat4x4(reinterpret_cast<DirectX::XMFLOAT4X4*>(&matProjection),
					DirectX::XMMatrixOrthographicOffCenterLH(L, R, B, T, 0.0f, 1.0f));

				rawDevice->SetTransform(D3DTS_PROJECTION, &matProjection);
				rawDevice->SetIndices(g_pIB, vertexBufferOffset);
				rawDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, (UINT)commandList->VtxBuffer.Size, indexBufferOffset, command.ElemCount / 3);
#else
				const RECT r = { (LONG)command.ClipRect.x, (LONG)command.ClipRect.y, (LONG)command.ClipRect.z, (LONG)command.ClipRect.w };
				rawDevice->SetScissorRect(&r);
				rawDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, vertexBufferOffset, 0, (UINT)commandList->VtxBuffer.Size, indexBufferOffset, command.ElemCount / 3);
#endif
			}

			indexBufferOffset += command.ElemCount;
		}

		vertexBufferOffset += commandList->VtxBuffer.Size;
	}

#ifdef TOUHOU_ON_D3D8
	rawDevice->ApplyStateBlock(stateBlock);
	rawDevice->DeleteStateBlock(stateBlock);
#else
	stateBlock->Apply();
#endif
}


IMGUI_API LRESULT ImGui_ImplDX8_WndProcHandler(HWND, UINT msg, WPARAM wParam, LPARAM lParam)
{
	ImGuiIO& io = ImGui::GetIO();
	switch (msg)
	{
	case WM_LBUTTONDOWN:
		io.MouseDown[0] = true;
		return true;
	case WM_LBUTTONUP:
		io.MouseDown[0] = false;
		return true;
	case WM_RBUTTONDOWN:
		io.MouseDown[1] = true;
		return true;
	case WM_RBUTTONUP:
		io.MouseDown[1] = false;
		return true;
	case WM_MBUTTONDOWN:
		io.MouseDown[2] = true;
		return true;
	case WM_MBUTTONUP:
		io.MouseDown[2] = false;
		return true;
	case WM_MOUSEWHEEL:
		io.MouseWheel += GET_WHEEL_DELTA_WPARAM(wParam) > 0 ? +1.0f : -1.0f;
		return true;
	case WM_MOUSEMOVE:
		io.MousePos.x = (signed short)(lParam);
		io.MousePos.y = (signed short)(lParam >> 16);
		return true;
	case WM_KEYDOWN:
		if (wParam < 256)
			io.KeysDown[wParam] = 1;
		return true;
	case WM_KEYUP:
		if (wParam < 256)
			io.KeysDown[wParam] = 0;
		return true;
	case WM_CHAR:
		// You can also use ToAscii()+GetKeyboardState() to retrieve characters.
		if (wParam > 0 && wParam < 0x10000)
			io.AddInputCharacter((unsigned short)wParam);
		return true;
	}
	return 0;
}

void ImGui_ImplDX8_UpdateDevice(IDirect3DDevice8* device) {
	if (device == g_pd3dDevice)
		return;
	g_pd3dDevice = device;
	ImGui_ImplDX8_CreateDeviceObjects();
}

bool ImGui_ImplDX8_Init(void* hwnd, IDirect3DDevice8* device) {
	g_hWnd = (HWND)hwnd;
	g_pd3dDevice = device;

	if (!QueryPerformanceFrequency((LARGE_INTEGER *)&g_TicksPerSecond))
		return false;
	if (!QueryPerformanceCounter((LARGE_INTEGER *)&g_Time))
		return false;

	ImGuiIO& io = ImGui::GetIO();
	io.KeyMap[ImGuiKey_Tab] = VK_TAB;                       // Keyboard mapping. ImGui will use those indices to peek into the io.KeyDown[] array that we will update during the application lifetime.
	io.KeyMap[ImGuiKey_LeftArrow] = VK_LEFT;
	io.KeyMap[ImGuiKey_RightArrow] = VK_RIGHT;
	io.KeyMap[ImGuiKey_UpArrow] = VK_UP;
	io.KeyMap[ImGuiKey_DownArrow] = VK_DOWN;
	io.KeyMap[ImGuiKey_PageUp] = VK_PRIOR;
	io.KeyMap[ImGuiKey_PageDown] = VK_NEXT;
	io.KeyMap[ImGuiKey_Home] = VK_HOME;
	io.KeyMap[ImGuiKey_End] = VK_END;
	io.KeyMap[ImGuiKey_Delete] = VK_DELETE;
	io.KeyMap[ImGuiKey_Backspace] = VK_BACK;
	io.KeyMap[ImGuiKey_Enter] = VK_RETURN;
	io.KeyMap[ImGuiKey_Escape] = VK_ESCAPE;
	io.KeyMap[ImGuiKey_A] = 'A';
	io.KeyMap[ImGuiKey_C] = 'C';
	io.KeyMap[ImGuiKey_V] = 'V';
	io.KeyMap[ImGuiKey_X] = 'X';
	io.KeyMap[ImGuiKey_Y] = 'Y';
	io.KeyMap[ImGuiKey_Z] = 'Z';

	io.RenderDrawListsFn = THRotatorImGui_RenderDrawLists;   // Alternatively you can set this to NULL and call ImGui::GetDrawData() after ImGui::Render() to get the same ImDrawData pointer.
	io.ImeWindowHandle = g_hWnd;

	return true;
}

void ImGui_ImplDX8_Shutdown()
{
	ImGui_ImplDX8_InvalidateDeviceObjects();
	//ImGui::Shutdown();
	g_pd3dDevice = NULL;
	g_hWnd = 0;
}

static bool ImGui_ImplDX8_CreateFontsTexture()
{
	// Build texture atlas
	ImGuiIO& io = ImGui::GetIO();
	unsigned char* pixels;
	int width, height, bytes_per_pixel;
	io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height, &bytes_per_pixel);

	// Upload texture to graphics system
	g_FontTexture = NULL;
	if (g_pd3dDevice->CreateTexture(width, height, 1, D3DUSAGE_DYNAMIC, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &g_FontTexture) < 0)
		return false;
	D3DLOCKED_RECT tex_locked_rect;
	if (g_FontTexture->LockRect(0, &tex_locked_rect, NULL, 0) != D3D_OK)
		return false;
	for (int y = 0; y < height; y++)
		memcpy((unsigned char *)tex_locked_rect.pBits + tex_locked_rect.Pitch * y, pixels + (width * bytes_per_pixel) * y, (width * bytes_per_pixel));
	g_FontTexture->UnlockRect(0);

	// Store our identifier
	io.Fonts->TexID = (void *)g_FontTexture;

	return true;
}

bool ImGui_ImplDX8_CreateDeviceObjects()
{
	if (!g_pd3dDevice)
		return false;
	if (!ImGui_ImplDX8_CreateFontsTexture())
		return false;
	return true;
}

void ImGui_ImplDX8_InvalidateDeviceObjects()
{
	if (!g_pd3dDevice)
		return;
	if (g_pVB)
	{
		g_pVB->Release();
		g_pVB = NULL;
	}
	if (g_pIB)
	{
		g_pIB->Release();
		g_pIB = NULL;
	}

	// At this point note that we set ImGui::GetIO().Fonts->TexID to be == g_FontTexture, so clear both.
	ImGuiIO& io = ImGui::GetIO();
	IM_ASSERT(g_FontTexture == io.Fonts->TexID);
	if (g_FontTexture)
		g_FontTexture->Release();
	g_FontTexture = NULL;
	io.Fonts->TexID = NULL;
}

static DWORD d3d8_state_block = NULL;


#define INVALID_STATE_BLOCK_HANDLE 0xFFFFFFFF

void THRotatorImGui_UpdateAndApplyFixedStates()
{

#ifdef TOUHOU_ON_D3D8
	//if (d3d8_state_block != INVALID_STATE_BLOCK_HANDLE)
	//{
	//	// Apply recorded device states for sprite rendering
	//	g_pd3dDevice->ApplyStateBlock(d3d8_state_block);
	//	return;
	//}
#else
	if (pUserData->stateBlock)
	{
		// Apply recorded device states for sprite rendering
		pUserData->stateBlock->Apply();
		return;
	}
#endif

	g_pd3dDevice->BeginStateBlock();

#if TOUHOU_ON_D3D8
	g_pd3dDevice->SetVertexShader(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1);
#else
	g_pd3dDevice->SetFVF(CUSTOMVERTEX::FVF);
#endif

	g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
	g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	g_pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	g_pd3dDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
#ifndef TOUHOU_ON_D3D8
	g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, TRUE);
#endif

	D3DMATRIX matIdentity;
	DirectX::XMStoreFloat4x4(reinterpret_cast<DirectX::XMFLOAT4X4*>(&matIdentity), DirectX::XMMatrixIdentity());

	g_pd3dDevice->SetTransform(D3DTS_WORLD, &matIdentity);
	g_pd3dDevice->SetTransform(D3DTS_VIEW, &matIdentity);

	g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
#ifdef TOUHOU_ON_D3D8
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
#endif

#ifdef TOUHOU_ON_D3D8
	g_pd3dDevice->SetPixelShader(0);
#else
	g_pd3dDevice->SetPixelShader(nullptr);
	g_pd3dDevice->SetVertexShader(nullptr);
	g_pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	g_pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
#endif

	g_pd3dDevice->EndStateBlock(&d3d8_state_block);
}

static bool ImGui_ImplWin32_UpdateMouseCursor()
{
	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange)
		return false;

	ImGuiMouseCursor imgui_cursor = ImGui::GetMouseCursor();
	if (imgui_cursor == ImGuiMouseCursor_None || io.MouseDrawCursor)
	{
		// Hide OS mouse cursor if imgui is drawing it or if it wants no cursor
		::SetCursor(NULL);
	}
	else
	{
		// Show OS mouse cursor
		LPTSTR win32_cursor = IDC_ARROW;
		switch (imgui_cursor)
		{
		case ImGuiMouseCursor_Arrow:        win32_cursor = IDC_ARROW; break;
		case ImGuiMouseCursor_TextInput:    win32_cursor = IDC_IBEAM; break;
		case ImGuiMouseCursor_ResizeAll:    win32_cursor = IDC_SIZEALL; break;
		case ImGuiMouseCursor_ResizeEW:     win32_cursor = IDC_SIZEWE; break;
		case ImGuiMouseCursor_ResizeNS:     win32_cursor = IDC_SIZENS; break;
		case ImGuiMouseCursor_ResizeNESW:   win32_cursor = IDC_SIZENESW; break;
		case ImGuiMouseCursor_ResizeNWSE:   win32_cursor = IDC_SIZENWSE; break;
		}
		::SetCursor(::LoadCursor(NULL, win32_cursor));
	}
	return true;
}

void ImGui_ImplDX8_NewFrame()
{
	if (!g_FontTexture)
		ImGui_ImplDX8_CreateDeviceObjects();

	ImGuiIO& io = ImGui::GetIO();

	// Setup display size (every frame to accommodate for window resizing)
	RECT rect;
	GetClientRect(g_hWnd, &rect);
	io.DisplaySize = ImVec2((float)(rect.right - rect.left), (float)(rect.bottom - rect.top));

	// Setup time step
	INT64 current_time;
	QueryPerformanceCounter((LARGE_INTEGER*)& current_time);
	io.DeltaTime = (float)(current_time - g_Time) / g_TicksPerSecond;
	g_Time = current_time;

	// Read keyboard modifiers inputs
	io.KeyCtrl = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
	io.KeyShift = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
	io.KeyAlt = (GetKeyState(VK_MENU) & 0x8000) != 0;
	io.KeySuper = false;
	// io.KeysDown : filled by WM_KEYDOWN/WM_KEYUP events
	// io.MousePos : filled by WM_MOUSEMOVE events
	// io.MouseDown : filled by WM_*BUTTON* events
	// io.MouseWheel : filled by WM_MOUSEWHEEL events

	// Set OS mouse position if requested (only used when ImGuiConfigFlags_NavEnableSetMousePos is enabled by user)
	if (io.WantSetMousePos)
	{
		POINT pos = { (int)io.MousePos.x, (int)io.MousePos.y };
		ClientToScreen(g_hWnd, &pos);
		SetCursorPos(pos.x, pos.y);
	}

	// Update OS mouse cursor with the cursor requested by imgui
	ImGuiMouseCursor mouse_cursor = io.MouseDrawCursor ? ImGuiMouseCursor_None : ImGui::GetMouseCursor();
	if (g_LastMouseCursor != mouse_cursor)
	{
		g_LastMouseCursor = mouse_cursor;
		ImGui_ImplWin32_UpdateMouseCursor();
	}

	// Start the frame. This call will update the io.WantCaptureMouse, io.WantCaptureKeyboard flag that you can use to dispatch inputs (or not) to your application.
	ImGui::NewFrame();
}
