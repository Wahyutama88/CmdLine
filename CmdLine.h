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

#ifndef ArraySize
#define ArraySize(Array) sizeof (Array)/sizeof(Array[0])
#endif

namespace CmdLine {
    template <typename T>
    class CLineT
    {
    protected:
        T** m_pCmdLine;
        INT* m_lens;
        INT m_count;
        size_t m_maxLen;
        CLineT()
            : m_pCmdLine(nullptr)
            , m_lens(nullptr)
            , m_count(0)
            , m_maxLen(0)
        {}
    public:
        virtual ~CLineT()
        {
            if (m_lens)
                delete[] m_lens;
        }
        INT count()
        {
            return m_count;
        }
        INT getArgLen(INT index)
        {
            return (index < m_count)
                ? m_lens[index]
                : 0;
        }
        const T* getArg(INT index)
        {
            return (index < m_count)
                ? m_pCmdLine[index]
                : nullptr;
        }
        const T* getArg(const T* arg)
        {
            if (auto len = _len(arg))
            {
                T buffer[MAX_PATH] = { 0 };
                INT size = ArraySize(buffer);
                for (INT index = 0; index < m_count; ++index)
                {
                    if ((getPartialArg(index, buffer, &size)) >= len)
                    {
                        buffer[len] = '\0';
                        if ((_cmpi(buffer, arg)) == 0)
                        {
                            auto p = getArg(index);
                            return &p[len];
                        }
                    }
                }
            }
            return nullptr;
        }
        const T* getArg(INT index, INT* len)
        {
            if (index < m_count)
            {
                if (len)
                    *len = m_lens[index];
                return m_pCmdLine[index];
            }
            if (len)
                *len = 0;
            return nullptr;
        }
#pragma push_macro("min")
#undef min
        size_t getPartialArg(INT index, T* buffer, INT* bufferMax)
        {
            if (bufferMax && (index < m_count))
            {
                if (buffer == nullptr)
                    *bufferMax = getArgLen(index);
                else
                {
                    INT len = 0;
                    if (auto arg = getArg(index, &len))
                    {
                        auto maxLen = *bufferMax - 1;
                        auto max_ = std::min(len, maxLen);
                        memcpy(buffer, arg, (max_ * sizeof (T)));
                        buffer[max_] = '\0';
                        return max_;
                    }
                }
            }
            return 0;
        }
#pragma pop_macro("min")
    protected:
        virtual size_t _len(const T*) = 0;
        virtual int _cmpi(const T*, const T*) = 0;

        LPWSTR* _init()
        {
            auto pCmdLine = CommandLineToArgvW(GetCommandLineW(), &m_count);
            m_lens = new INT[m_count];

            for (int idx = 0; idx < m_count; ++idx)
            {
                auto Len = wcslen(pCmdLine[idx]);
                m_lens[idx] = Len;

                if (m_maxLen < Len)
                    m_maxLen = Len;

            }
            return pCmdLine;
        }
    };

    class CLineA
        : public CLineT<CHAR>
    {
    public:
        CLineA()
            : CLineT()
        {
            auto pCmdLine = _init();
            m_pCmdLine = new LPSTR[m_count];
            for (INT idx = 0; idx < m_count; ++idx)
                m_pCmdLine[idx] = _convert(pCmdLine[idx], m_lens[idx]);

            LocalFree(pCmdLine);
        }
        ~CLineA()
        {
            for (INT index = 0; index < m_count; ++index)
                delete m_pCmdLine[index];

            delete[] m_pCmdLine;
        }
    private:
        LPSTR _convert(LPCWSTR str, INT& Len)
        {
            if (auto len = WideCharToMultiByte(CP_ACP, 0, str, Len, NULL, 0, NULL, NULL))
            {
                auto ret = new CHAR[len + 1];
                WideCharToMultiByte(CP_ACP, 0, str, Len, ret, len, NULL, NULL);
                ret[len] = '\0';
                Len = len;
                return ret;
            }
            return nullptr;
        }
        size_t _len(const CHAR* str) override final
        {
            return strnlen(str, m_maxLen);
        }
        int _cmpi(const CHAR* str1, const CHAR* str2) override final
        {
            return _stricmp(str1, str2);
        }
    };
    class CLineW
        : public CLineT<WCHAR>
    {
    public:
        CLineW()
            : CLineT()
        {
            m_pCmdLine = _init();
        }
        ~CLineW()
        {
            LocalFree(m_pCmdLine);
        }
    private:
        size_t _len(const WCHAR* str) override final
        {
            return wcsnlen(str, m_maxLen);
        }
        int _cmpi(const WCHAR* str1, const WCHAR* str2) override final
        {
            return _wcsicmp(str1, str2);
        }
    };
}

#ifdef UNICODE
#define CLine CLineW
#else
#define CLine CLineA
#endif

#endif // __CMD_LINE_H__
