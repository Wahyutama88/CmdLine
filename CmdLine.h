#ifndef __CMD_LINE_H__
#define __CMD_LINE_H__
#include <Windows.h>
#include <algorithm>
/*
* Wrapper around CommandLineToArgvW API for convenience
* and easy access to every argument
* Perform very basic Command line parsing
* Windows only, Both Unicode and Multibyte are provided
* Visual studio and Mingw are supported
* provided "AS-IS"
* use this header if you are unable to switch to main function
* which provide such support: int main(int argc, char*argv[])
*/
namespace CmdLine
{
class CLine
{
    union
    {
        LPWSTR* m_pCmdLinesW;
        LPSTR* m_pCmdLinesA;
    };
    INT* m_Lens;
    INT m_Count;
    UINT m_MaxLen;
public:
    CLine()
        : m_pCmdLinesW(nullptr)
        , m_Lens(nullptr)
        , m_Count(0)
        , m_MaxLen(0)
    {
        m_pCmdLinesW = CommandLineToArgvW(GetCommandLineW(), &m_Count);
        m_Lens = new INT[m_Count];
        for (int idx = 0; idx < m_Count; ++idx)
        {
            auto Len = wcslen(m_pCmdLinesW[idx]);
            if (m_MaxLen < Len)
                m_MaxLen = Len;
            m_Lens[idx] = Len;
        }
#ifndef UNICODE
        auto cmdLines = new LPSTR[m_Count];
        for (INT idx = 0; idx < m_Count; ++idx)
            cmdLines[idx] = _convert(m_pCmdLinesW[idx], m_Lens[idx]);

        LocalFree(m_pCmdLinesW);
        m_pCmdLinesA = cmdLines;
#endif
    }
    ~CLine()
    {
#ifdef UNICODE
        if (m_pCmdLinesW)
            LocalFree(m_pCmdLinesW);
#else
        if (m_pCmdLinesA)
            delete[] m_pCmdLinesA;
#endif
        if (m_Lens)
            delete[] m_Lens;
    }
    INT Count()
    {
        return m_Count;
    }
#ifdef UNICODE
    LPCWSTR GetArg(INT idx)
#else
    LPCSTR GetArg(INT idx)
#endif
    {
        return idx < m_Count
#ifdef UNICODE
            ? m_pCmdLinesW[idx]
#else
            ? m_pCmdLinesA[idx]
#endif
            : nullptr;
    }
    // 0 based
#ifdef UNICODE
    LPCWSTR GetArg(INT idx, INT* Len)
#else
    LPCSTR GetArg(INT idx, INT* Len)
#endif
    {
        if (idx < m_Count)
        {
            if (Len)
                *Len = m_Lens[idx];
#ifdef UNICODE
            return m_pCmdLinesW[idx];
#else
            return m_pCmdLinesA[idx];
#endif
        }
        return nullptr;
    }
    // 0 based
    INT GetArgLen(INT idx)
    {
        return idx < m_Count
            ? m_Lens[idx]
            : 0;
    }
    // return copied len to buffer
#ifdef UNICODE
    INT GetPartialArg(INT idx, LPWSTR szBuffer, INT* BufferMax)
#else
    INT GetPartialArg(INT idx, LPSTR szBuffer, INT* BufferMax)
#endif
    {
        if ((idx < m_Count) && BufferMax)
        {
            if (szBuffer == nullptr)
                *BufferMax = GetArgLen(idx);
            else
            {
                INT Len = 0;
                if (auto szArg = GetArg(idx, &Len))
                {
                    auto MaxLen = *BufferMax - 1;
#ifdef min
#undef min
#endif
                    auto Max = std::min(Len, MaxLen);
                    memcpy(szBuffer, szArg, Max
#ifdef UNICODE
                        * sizeof(WCHAR)
#endif
                        );
                    szBuffer[Max] = 0;
                    return Max;
                }
            }
        }
        return 0;
    }
#ifdef UNICODE
    LPCWSTR GetArg(LPCWSTR szArg)
#else
    LPCSTR GetArg(LPCSTR szArg)
#endif
    {
#ifdef UNICODE
#if defined (_MSC_VER)
        if (INT Len = wcsnlen_s(szArg, m_MaxLen))
#else
        if (INT Len = wcslen(szArg))
#endif
        {
            WCHAR
#else
#if defined (_MSC_VER)
        if (INT Len = strnlen_s(szArg, m_MaxLen))
#else
        if (INT Len = strlen(szArg))
#endif
        {
            CHAR
#endif
                Buffer[MAX_PATH] = { 0 };
#define ArraySize(Array) sizeof (Array)/sizeof(Array[0])
            INT size = ArraySize(Buffer);
#undef ArraySize
            for (INT idx = 0; idx < m_Count; ++idx)
                if (GetPartialArg(idx, Buffer, &size) >= Len)
                {
                    Buffer[Len] = 0;
#ifdef UNICODE
                    if (_wcsicmp(Buffer, szArg) == 0)
#else
                    if (_stricmp(Buffer, szArg) == 0)
#endif
                    return &GetArg(idx)[Len];
                }
        }
        return nullptr;
    }
#ifndef UNICODE
private:
    LPSTR _convert(LPCWSTR szStr, INT& Len)
    {
        if (auto LenA = WideCharToMultiByte(CP_ACP, 0, szStr, Len, NULL, 0, NULL, NULL))
        {
            auto ret = new CHAR[LenA + 1];
            WideCharToMultiByte(CP_ACP, 0, szStr, Len, ret, LenA, NULL, NULL);
            ret[LenA] = 0;
            Len = LenA;
            return ret;
        }
        return nullptr;
    }
#endif
    };
}
#endif // __CMD_LINE_H__
