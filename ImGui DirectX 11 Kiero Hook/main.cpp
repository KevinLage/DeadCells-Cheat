#include "includes.h"
#include <iostream>
#include <string>
#include "mem.h"
#include <TlHelp32.h>
#pragma warning(disable : 4996)

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

Present oPresent;
HWND window = NULL;
WNDPROC oWndProc;
ID3D11Device* pDevice = NULL;
ID3D11DeviceContext* pContext = NULL;
ID3D11RenderTargetView* mainRenderTargetView;


using namespace std;

DWORD GamePID;
int * money;
int * helth;
int * maxhelth;
int * cells;
int * ammo;
int * ammo_armbrust;
int * x;
int * y;
int range;
int money_input;
int cells_input;
int helth_input = 100;
int max_helth_input = 100;
int x_auto = 0;
int y_auto = 0;
bool godmode;
bool ammo_input;
bool changed;
bool click;
bool autoclicker;

void teleport() {
	while (true) {
		if (GetAsyncKeyState(VK_LEFT)) {
			*x -= range;
		}
		if (GetAsyncKeyState(VK_RIGHT)) {
			*x += range;
		}
		if (GetAsyncKeyState(VK_UP)) {
			*y -= range;
		}
		if (GetAsyncKeyState(VK_DOWN)) {
			*y += range;
		}
		Sleep(100);
	}
}

void onetimechange() {
	while (true) {
		if (changed) {
			*helth = helth_input;
			*maxhelth = max_helth_input;
			*money = money_input;
			*cells = cells_input;
			changed = false;
		}
	}
}

void Clicker()
{
	while (true)
	{
		if (autoclicker)
		
		{
			mouse_event(MOUSEEVENTF_LEFTDOWN, x_auto, y_auto, 0, 0);
			Sleep(5);
			mouse_event(MOUSEEVENTF_LEFTUP, x_auto, y_auto, 0, 0);
		}
	}
}

void main() {

	HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); //all processes

	PROCESSENTRY32W entry; //current process
	entry.dwSize = sizeof entry;


	do {
		if (std::wstring(entry.szExeFile) == L"deadcells.exe") {
			GamePID = entry.th32ProcessID;
			break;
		}
	} while (Process32NextW(snap, &entry));
	uintptr_t base = mem::GetModuleBaseAddress(GamePID, L"libhl.dll"); // für Geld, Leben, max leben, cells
	ammo = (int *)mem::FindDMAAddy(base + 0x00049184, { 0x440, 0x0, 0x58, 0x9C, 0x8C, 0xE0, 0x18 });
	CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)teleport, NULL, 0, nullptr);
	CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)onetimechange, NULL, 0, nullptr);
	CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)Clicker, NULL, 0, nullptr);

	money = (int *)mem::FindDMAAddy(base + 0x00049184, { 0x440, 0x0, 0x58, 0x58, 0x3C, 0x5C, 0x2C });
	helth = (int *)mem::FindDMAAddy(base + 0x00049184, { 0x440, 0x0, 0x58, 0x50, 0xC, 0x64, 0xE8 });
	maxhelth = (int *)(mem::FindDMAAddy(base + 0x00049184, { 0x440, 0x0, 0x58, 0x50, 0xC, 0x64, 0xE8 }) + 4);
	cells = (int *)mem::FindDMAAddy(base + 0x00049184, { 0x584, 0x0, 0x18, 0x64, 0x300, 0x5C, 0x338 });
	ammo = (int *)mem::FindDMAAddy(base + 0x00049184, { 0x440, 0x0, 0x58, 0x9C, 0x8C, 0xE0, 0x18 });
	x = (int *)mem::FindDMAAddy(base + 0x00049184, { 0x440, 0x0, 0x58, 0x68, 0xF8, 0xA0, 0x4C });
	y = (int *)(mem::FindDMAAddy(base + 0x00049184, { 0x440, 0x0, 0x58, 0x68, 0xF8, 0xA0, 0x4C }) + 4);


		while (true)
		{

			//AllocConsole();
			//freopen("CONOUT$", "w", stdout);
			//cout << *y << endl;
			if (godmode) {
				*helth = helth_input;
				*maxhelth = max_helth_input;
			}

			if (ammo_input) {
				*ammo = 50;

		}
	}

}



void InitImGui()
{
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX11_Init(pDevice, pContext);
}

LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	if (true && ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
		return true;

	return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
}

bool init = false;

int f = 1;
HRESULT __stdcall hkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
{
	if (!init)
	{
		if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)& pDevice)))
		{
			pDevice->GetImmediateContext(&pContext);
			DXGI_SWAP_CHAIN_DESC sd;
			pSwapChain->GetDesc(&sd);
			window = sd.OutputWindow;
			ID3D11Texture2D* pBackBuffer;
			pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)& pBackBuffer);
			pDevice->CreateRenderTargetView(pBackBuffer, NULL, &mainRenderTargetView);
			pBackBuffer->Release();
			oWndProc = (WNDPROC)SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)WndProc);
			InitImGui();
			init = true;
		}

		else
			return oPresent(pSwapChain, SyncInterval, Flags);
	}

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("Deadcells cheat by KevinLage");
	ImGui::SliderInt("Teleport Range", &range, 0.0, 20.0f);
	ImGui::InputInt("Money", &money_input);
	ImGui::InputInt("Cells", &cells_input);
	ImGui::InputInt("Health", &helth_input);
	ImGui::InputInt("Max Health", &max_helth_input);
	ImGui::CheckboxFlags("GodMode", (unsigned int *)&godmode, ImGuiBackendFlags_HasGamepad);
	ImGui::CheckboxFlags("Ammunition", (unsigned int *)&ammo_input, ImGuiBackendFlags_HasGamepad);
	ImGui::CheckboxFlags("AutoClicker", (unsigned int *)&autoclicker, ImGuiBackendFlags_HasGamepad);
	if (ImGui::Button("Update")) {
		changed = true;
	}
	if (ImGui::Button("PanicMode")) {
		kiero::shutdown();
		return oPresent(pSwapChain, SyncInterval, Flags);
	}
	ImGui::End();

	ImGui::Render();

	pContext->OMSetRenderTargets(1, &mainRenderTargetView, NULL);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	return oPresent(pSwapChain, SyncInterval, Flags);
}

DWORD WINAPI MainThread(LPVOID lpReserved)
{
	bool init_hook = false;
	do
	{
		if (kiero::init(kiero::RenderType::D3D11) == kiero::Status::Success)
		{
			kiero::bind(8, (void**)& oPresent, hkPresent);
			init_hook = true;
			CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)main, NULL, 0, nullptr);
		}
	} while (!init_hook);
	return TRUE;
}

BOOL WINAPI DllMain(HMODULE hMod, DWORD dwReason, LPVOID lpReserved)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(hMod);
		CreateThread(nullptr, 0, MainThread, hMod, 0, nullptr);
		break;
	case DLL_PROCESS_DETACH:
		kiero::shutdown();
		break;
	}
	return TRUE;
}
