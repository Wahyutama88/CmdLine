// Mingw compile using: g++ example.cpp -std=gnu++11
// and add / uncomment this line
// #define UNICODE
// to make UNICODE APP
#include <Windows.h>
#include <iostream>
#include "CmdLine.h"

int main()
{
    CmdLine::CLine Cline;

    std::cout << Cline.Count() << std::endl;

    for (int idx = 0; idx < Cline.Count(); ++idx)
        std::wcout << idx << " " << Cline.GetArg(idx) << std::endl;

    TCHAR Buffer[9] = { 0 };
    INT len = 9;
    Cline.GetPartialArg(0, Buffer, &len);
    std::wcout << __TEXT("partial arg: ")<< Buffer << std::endl;

    TCHAR* arg = const_cast<TCHAR*>(Cline.getArg(__TEXT("open:")));
    if (arg == nullptr)
        arg = __TEXT("\"open\" arg not available");
    std::wcout << __TEXT("value after open arg: ")<< arg << std::endl;
}
