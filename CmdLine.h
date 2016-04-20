#pragma once
#include <Windows.h>
/*
* Wrapper around CommandLineToArgvW API for convenience
* and easy access to every argument
* Perform very basic Command line parsing
* Windows only, UNICODE only
* provided "AS-IS"
*/
namespace CmdLine
{
class CLineW
{
    LPWSTR* m_pCmdLines;
    INT* m_Lens;
    INT m_Count;
    UINT m_MaxLen;
public:
    CLineW()
        : m_pCmdLines(nullptr)
        , m_Lens(nullptr)
        , m_Count(0)
        , m_MaxLen(0)
    {
        m_pCmdLines = CommandLineToArgvW(GetCommandLineW(), &m_Count);
        if (m_Count)
        {
            m_Lens = new INT[m_Count];
            for (int idx = 0; idx < m_Count; ++idx)
            {
                auto Len = wcslen(m_pCmdLines[idx]);
                if (m_MaxLen < Len)
                    m_MaxLen = Len;
                m_Lens[idx] = Len;
            }
        }
    }
    ~CLineW()
    {
        if (m_pCmdLines)
            LocalFree(m_pCmdLines);
        if (m_Lens)
            delete[] m_Lens;
    }
    INT Count()
    {
        return m_Count;
    }
    LPCWSTR GetArg(INT idx)
    {
        if (idx < m_Count)
            return m_pCmdLines[idx];
        return nullptr;
    }
    // 0 based
    LPCWSTR GetArg(INT idx, INT* Len)
    {
        if (idx < m_Count)
        {
            if (Len)
                *Len = m_Lens[idx];
            return m_pCmdLines[idx];
        }
        return nullptr;
    }
    // 0 based
    INT GetArgLen(INT idx)
    {
        if (idx < m_Count)
            return m_Lens[idx];
        return 0;
    }
    // return copied len to buffer
    INT GetPartialArg(INT idx, LPWSTR szBuffer, INT* BufferMax)
    {
        if (idx < m_Count)
        {
            if (szBuffer && BufferMax)
            {
                INT Len = 0;
                if (auto szArg = GetArg(idx, &Len))
                {
                    auto MaxLen = *BufferMax - 1;
                    auto Max = min(Len, MaxLen);
                    memcpy(szBuffer, szArg, Max*sizeof(WCHAR));
                    szBuffer[Max] = 0;
                    return Max;
                }
            }
            else if (szBuffer == nullptr && BufferMax)
                *BufferMax = GetArgLen(idx);
        }
        return 0;
    }
    LPCWSTR GetArg(LPCWSTR szArg)
    {
        if (INT Len = wcsnlen_s(szArg, m_MaxLen))
        {
            WCHAR Buffer[MAX_PATH] = { 0 };
            INT size = ARRAYSIZE(Buffer);
            for (INT idx = 0; idx < m_Count; ++idx)
                if (GetPartialArg(idx, Buffer, &size) >= Len)
                {
                    Buffer[Len] = 0;
                    if (_wcsicmp(Buffer, szArg) == 0)
                        return &GetArg(idx)[Len];
                }
        }
        return nullptr;

    }
};
}