#ifndef FUNCTION_H
#define FUNCTION_H

#include "string"
#include "list"
#include "variable.h"
#include <stdio.h>

#ifndef _gsgsAPI

#ifdef WXMAKINGDLL_CL
#define _gsgsAPI __declspec(dllexport)
#elif defined(WXUSINGDLL_CL)
#define _gsgsAPI __declspec(dllimport)
#else // not making nor using DLL
#define _gsgsAPI
#endif

#endif

class _gsgsAPI clFunction
{
public:
    std::string m_name;
    std::string m_scope; // functions' scope
    std::string m_retrunValusConst; // is the return value a const?
    std::string m_signature;
    Variable m_returnValue;
    int m_lineno;
    bool m_isVirtual;
    bool m_isPureVirtual;
    bool m_isConst;
    std::string m_throws;
    
public:
    clFunction();
    virtual ~clFunction();

    // clear the class content
    void Reset();

    // print the variable to stdout
    void Print();
};

typedef std::list<clFunction> FunctionList;
#endif // FUNCTION_H
