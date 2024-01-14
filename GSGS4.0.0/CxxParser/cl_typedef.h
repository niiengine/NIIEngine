#ifndef TYPEDEF_H
#define TYPEDEF_H

#include <string>
#include "variable.h"

#ifndef _gsgsAPI

#ifdef WXMAKINGDLL_CL
#    define _gsgsAPI __declspec(dllexport)
#elif defined(WXUSINGDLL_CL)
#    define _gsgsAPI __declspec(dllimport)
#else // not making nor using DLL
#    define _gsgsAPI
#endif

#endif

namespace gsgs
{
    class _gsgsAPI Typedef
    {
    public:
        std::string m_name;
        Variable    m_realType;
        
    public:
        gsgs::Typedef()  {}
        ~gsgs::Typedef() {}
        
        void print() {
            printf("Name: %s\n", m_name.c_str());
            m_realType.Print();
        }
        
        void clear() {
            m_realType.Reset();
            m_name.clear();
        }
    };

    typedef std::list<gsgs::Typedef> TypedefList;
}
#endif // TYPEDEF_H
