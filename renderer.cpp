#include <iostream>
#include "Mesh.h"
#include "Render.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
#if 0
    Mesh m;
    m.Load("C:\\Users\\ilian\\Desktop\\TST.off");
    m.Normalize();

#else
    Mesh m;
    m.Load("C:\\Users\\ilian\\Desktop\\CSG.off");
    m.Normalize();
            
    Renderer rend;
    rend.UploadMesh(m);

    if (rend.Init(hInstance, hPrevInstance, lpCmdLine, nCmdShow))
    {                
        rend.Exec();
    }
    else {
        return GetLastError();
    }
#endif
    return 0;
}

