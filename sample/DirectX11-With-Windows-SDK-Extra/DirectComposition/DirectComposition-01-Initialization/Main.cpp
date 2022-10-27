#include <Windows.h>
#include <wrl/client.h>
#include <dcomp.h>

namespace RuntimeLoader
{
	class DirectComposition
	{
	private:
		HMODULE dll_dcomp{ NULL };
		decltype(DCompositionCreateDevice)* api_DCompositionCreateDevice{ NULL };
		decltype(DCompositionCreateDevice2)* api_DCompositionCreateDevice2{ NULL };
		decltype(DCompositionCreateDevice3)* api_DCompositionCreateDevice3{ NULL };
	public:
		BOOL IsSupported();
		HRESULT CreateDevice(IUnknown* pDevice, REFIID iid, void** dcompositionDevice);
		HRESULT CreateDesktopDevice(IUnknown* pDevice, IDCompositionDevice** ppDevice, IDCompositionDesktopDevice** ppDesktopDevice = NULL);
		static HRESULT CreateVisual(IDCompositionDevice* pDevice, IDCompositionDesktopDevice* pDesktopDevice, IDCompositionVisual** ppVisual, IDCompositionVisual2** ppVisual2 = NULL);
	public:
		DirectComposition();
		~DirectComposition();
	};

	BOOL DirectComposition::IsSupported()
	{
		return dll_dcomp && api_DCompositionCreateDevice;
	}
	HRESULT DirectComposition::CreateDevice(IUnknown* pDevice, REFIID iid, void** dcompositionDevice)
	{
		if (api_DCompositionCreateDevice3)
		{
			return api_DCompositionCreateDevice3(pDevice, iid, dcompositionDevice);
		}
		else if (api_DCompositionCreateDevice2)
		{
			return api_DCompositionCreateDevice2(pDevice, iid, dcompositionDevice);
		}
		else if (api_DCompositionCreateDevice)
		{
			HRESULT hr = S_OK;
			IDXGIDevice* pDXGIDevice = NULL;
			hr = pDXGIDevice->QueryInterface(&pDevice);
			if (FAILED(hr)) return hr;
			hr = api_DCompositionCreateDevice(pDXGIDevice, iid, dcompositionDevice);
			pDXGIDevice->Release();
			return api_DCompositionCreateDevice(pDXGIDevice, iid, dcompositionDevice);
		}
		else
		{
			return E_NOTIMPL;
		}
	}
	HRESULT DirectComposition::CreateDesktopDevice(IUnknown* pDevice, IDCompositionDevice** ppDevice, IDCompositionDesktopDevice** ppDesktopDevice)
	{
		if (api_DCompositionCreateDevice2)
		{
			HRESULT hr = S_OK;
			IDCompositionDesktopDevice* pDesktopDevice = NULL;
			hr = CreateDevice(pDevice, IID_PPV_ARGS(&pDesktopDevice));
			if (FAILED(hr)) return hr;
			hr = pDesktopDevice->QueryInterface(IID_PPV_ARGS(ppDevice));
			if (ppDesktopDevice) *ppDesktopDevice = pDesktopDevice;
			return hr;
		}
		else
		{
			return CreateDevice(pDevice, IID_PPV_ARGS(ppDevice));
		}
	}
	HRESULT DirectComposition::CreateVisual(IDCompositionDevice* pDevice, IDCompositionDesktopDevice* pDesktopDevice, IDCompositionVisual** ppVisual, IDCompositionVisual2** ppVisual2)
	{
		if (pDesktopDevice)
		{
			HRESULT hr = S_OK;
			IDCompositionVisual2* pVisual2 = NULL;
			hr = pDesktopDevice->CreateVisual(&pVisual2);
			if (FAILED(hr)) return hr;
			hr = pVisual2->QueryInterface(IID_PPV_ARGS(ppVisual));
			if (ppVisual2) *ppVisual2 = pVisual2;
			return hr;
		}
		else
		{
			return pDevice->CreateVisual(ppVisual);
		}
	}

	DirectComposition::DirectComposition()
	{
		dll_dcomp = LoadLibraryExW(L"dcomp.dll", NULL, LOAD_LIBRARY_SEARCH_SYSTEM32);
		if (dll_dcomp)
		{
			api_DCompositionCreateDevice = (decltype(api_DCompositionCreateDevice))
				GetProcAddress(dll_dcomp, "DCompositionCreateDevice");
			api_DCompositionCreateDevice2 = (decltype(api_DCompositionCreateDevice2))
				GetProcAddress(dll_dcomp, "DCompositionCreateDevice2");
			api_DCompositionCreateDevice3 = (decltype(api_DCompositionCreateDevice3))
				GetProcAddress(dll_dcomp, "DCompositionCreateDevice3");
		}
	}
	DirectComposition::~DirectComposition()
	{
		if (dll_dcomp)
		{
			FreeLibrary(dll_dcomp);
		}
		dll_dcomp = NULL;
		api_DCompositionCreateDevice = NULL;
		api_DCompositionCreateDevice2 = NULL;
		api_DCompositionCreateDevice3 = NULL;
	}
}

static RuntimeLoader::DirectComposition DCOMP;

_Use_decl_annotations_ int WINAPI wWinMain(HINSTANCE, HINSTANCE, PWSTR, int)
{
	HRESULT hr = S_OK;
	Microsoft::WRL::ComPtr<IDCompositionDevice> dcomp_device;
	Microsoft::WRL::ComPtr<IDCompositionDesktopDevice> dcomp_desktop_device;
	//Microsoft::WRL::ComPtr<IDCompositionTarget> dcomp_target;
	Microsoft::WRL::ComPtr<IDCompositionVisual> dcomp_visual;

	hr = DCOMP.CreateDesktopDevice(NULL, &dcomp_device, &dcomp_desktop_device);
	if (FAILED(hr)) return 1;
	hr = DCOMP.CreateVisual(dcomp_device.Get(), dcomp_desktop_device.Get(), &dcomp_visual);
	if (FAILED(hr)) return 1;
	//hr = dcomp_device->CreateTargetForHwnd(NULL, TRUE, &dcomp_target);
	//if (FAILED(hr)) return 1;

	return 0;
}
