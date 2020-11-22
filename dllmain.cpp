// dllmain.cpp : Defines the entry point for the DLL application.
#include "Includes.h"
#include <iostream>
#include "Memory.h"
#include "Hooking.h"
#include "OpenGL.h"
#include "OpenGLText.h"
#include "esp.h"


//get module base
uintptr_t moduleBase = (uintptr_t)GetModuleHandle(L"ac_client.exe");

bool bHealth = false, bAmmo = false, bRecoil = false, bAmor = false, bNoClip = false, bThrow = false, bKnife = false, bESP = false, bFOV = false;


typedef BOOL(__stdcall* twglSwapBuffers) (HDC hDc);

twglSwapBuffers owglSwapBuffers;
twglSwapBuffers wglSwapBuffersGateway;

GL::Font glFont;
const int FONT_HEIGHT = 15;
const int FONT_WIDTH = 9;

ESP esp;


void DrawESP()
{
        HDC currentHDC = wglGetCurrentDC();
        if (!glFont.bBuilt || currentHDC != glFont.hdc)
        {
            glFont.Build(FONT_HEIGHT);

        }
        GL::SetupOrtho();
        esp.Draw(glFont);
        GL::RestoreGL();
}
void DrawFOV()
{
    HDC currentHDC = wglGetCurrentDC();
    GL::SetupOrtho();
    GL::DrawOutline(855, 435, 200, 200, 2.0f, rgb::grey); // fucking took an hour to calulate
    GL::RestoreGL();
}

BOOL __stdcall hkwglSwapBuffers(HDC hDc)
{
    // hack options

    if (GetAsyncKeyState(VK_END) & 1)
    {
        // detaches dll for testing
        

    }

    if (GetAsyncKeyState(VK_NUMPAD1) & 1)
    {

        bHealth = !bHealth;
    }

    if (GetAsyncKeyState(VK_NUMPAD2) & 1)
    {
        bAmor = !bAmor;

    }

    if (GetAsyncKeyState(VK_NUMPAD3) & 1)
    {
        bAmmo = !bAmmo;

    }

    if (GetAsyncKeyState(VK_NUMPAD4) & 1)
    {
        bRecoil = !bRecoil;

        if (bRecoil)
        {
            //nop
            memtools::Nop((BYTE*)(moduleBase + 0x63786), 10);

        }
        else
        {
            // restore the nop
            //\x50\x8D\x4C\x24\x1C\x51\x8B\xCE\xFF\xD2
            memtools::Patch((BYTE*)(moduleBase + 0x63786), (BYTE*)"\x50\x8D\x4C\x24\x1C\x51\x8B\xCE\xFF\xD2", 10);
        }


    }

    if (GetAsyncKeyState(VK_NUMPAD5) & 1)
    {
        bNoClip = !bNoClip;
        if (bNoClip)
        {
            //nop
            memtools::Nop((BYTE*)(moduleBase + 0x637e4), 2);
        }
        else
        {
            // restore the nop
            //\x50\x8D\x4C\x24\x1C\x51\x8B\xCE\xFF\xD2
            memtools::Patch((BYTE*)(moduleBase + 0x637e4), (BYTE*)"\x89\x0A", 2);
        }
    }


    if (GetAsyncKeyState(VK_NUMPAD6) & 1)
    {
        bThrow = !bThrow;

    }


    if (GetAsyncKeyState(VK_NUMPAD7) & 1)
    {
        bKnife = !bKnife;

    }

    if (GetAsyncKeyState(VK_NUMPAD8) & 1)
    {
        bESP = !bESP;

    }
    if (GetAsyncKeyState(VK_NUMPAD9) & 1)
    {
        bFOV = !bFOV;

    }



    //(freeze) just like freeze in cheat engine.
    uintptr_t* localPlayerPtr = (uintptr_t*)(moduleBase + 0x10f4f4);
    if (localPlayerPtr)
    {
        if (bHealth)
        {
            *(int*)(*localPlayerPtr + 0xf8) = 1337;
        }

        if (bAmmo)
        {
            uintptr_t ammoAddr = memtools::FindDMAAddyInternal(moduleBase + 0x10f4f4, { 0x374, 0x14, 0x0 });
            int* ammo = (int*)ammoAddr;
            *ammo = 1337;

            // or just
           /// *(int*)memtools::FindDMAAddyInternal(moduleBase + 0x10f4f4, { 0x374, 0x14, 0x0 });
        }

        if (bAmor)
        {
            uintptr_t amorAddr = memtools::FindDMAAddyInternal(moduleBase + 0x10f4f4, { 0xfc });
            int* amor = (int*)amorAddr;
            *amor = 6660;
        }

        if (bThrow)
        {
            uintptr_t ThrowAddr = memtools::FindDMAAddyInternal(moduleBase + 0x10f4f4, { 0x180 });
            int* throwTime = (int*)ThrowAddr;
            *throwTime = 0;
        }

        if (bKnife)
        {
            uintptr_t knifeAddy = memtools::FindDMAAddyInternal(moduleBase + 0x10f4f4, { 0x160 });
            int* KnifeTime = (int*)knifeAddy;
            *KnifeTime = 0;
        }


    }


    //end hack options

    // start of drawling
        
    if (bESP)
    {
        DrawESP();

    }

    if (bFOV) 
    {
        DrawFOV();
    }

    // end of drawling

    return wglSwapBuffersGateway(hDc);

}


DWORD WINAPI HackThread(HMODULE hModule)
{
    //create console
    
    AllocConsole();
    FILE* f;
    freopen_s(&f, "CONOUT$", "w", stdout);

    SetConsoleTitleA("FuckAC");
    std::cout << "1 -> Health " << std::endl;
    std::cout << "2 -> Armor " << std::endl;
    std::cout << "3 -> Ammo " << std::endl;
    std::cout << "4 -> No Recoil " << std::endl;
    std::cout << "5 -> infinite Clip " << std::endl;
    std::cout << "6 -> Fast Grenade" << std::endl;
    std::cout << "7 -> Fast Knife" << std::endl;
    std::cout << "8 -> ESP" << std::endl;
    std::cout << "9 -> Fov Aimbot" << std::endl;


    // Trampoline Hook more information here -> https://guidedhacking.com/threads/x86-trampoline-hook-source-code-swapbuffers-hook.14414/
    //wglSwapBuffersGateway = (twglSwapBuffers)GetProcAddress(GetModuleHandle(L"opengl32.dll"), "wglSwapBuffers");
    //wglSwapBuffersGateway = (twglSwapBuffers)TrampHook32((BYTE*)wglSwapBuffersGateway, (BYTE*)hkwglSwapBuffers, 5);
    //
    // swap buffers hook
    Hook SwapBuffersHook("wglSwapBuffers", "opengl32.dll", (BYTE*)hkwglSwapBuffers, (BYTE*)&wglSwapBuffersGateway, 5);
    SwapBuffersHook.Enable();
  
    //cleanup & eject

    Sleep(10800000); // 3 hours before the game crashes itself :O

        fclose(f);
        FreeConsole();
        FreeLibraryAndExitThread(hModule, 0);
        return 0;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved 
)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    {
        CloseHandle(CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)HackThread, hModule, 0, nullptr));
    }
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

