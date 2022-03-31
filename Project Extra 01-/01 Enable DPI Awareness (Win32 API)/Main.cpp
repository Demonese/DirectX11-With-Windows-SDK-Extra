#include <Windows.h>
#include "HighDPI.hpp"

_Use_decl_anno_impl_
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nShowCmd)
{
    UNREFERENCED_PARAMETER(hInstance);
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    UNREFERENCED_PARAMETER(nShowCmd);
    HighDPI::EnableAwareness();
    /*
    *  这里是你的逻辑代码
    */
    return 0;
}
