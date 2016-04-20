#include <Windows.h>
#include <iostream>
#include "CLine.h"

int main()
{
    CmdLine::CLineW Cline;

    std::cout << Cline.Count() << std::endl;

    for (int idx = 0; idx < Cline.Count(); ++idx)
        std::wcout << idx << " " << Cline.GetArg(idx) << std::endl;

    WCHAR Buffer[9] = { 0 };
    INT len = 9;
    Cline.GetPartialArg(0, Buffer, &len);
    std::wcout << "partial arg: "<< Buffer << std::endl;

    auto arg = Cline.GetArg(L"open:");
    if (arg == nullptr)
        arg = L"\"open\" arg not available";
    std::wcout <<  "value after open arg: \""<< arg <<"\""<< std::endl;
}