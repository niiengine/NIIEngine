/*
-----------------------------------------------------------------------------
A
     __      _   _   _   ______
    |   \   | | | | | | |  ____)                    _
    | |\ \  | | | | | | | |         ___      ___   (_)   ___
    | | \ \ | | | | | | | |____    / _ \   / ___ \  _   / _ \   ___
    | |  \ \| | | | | | |  ____)  | / \ | | |  | | | | | / \ | / _ )
    | |   \ | | | | | | | |_____  | | | | | |__| | | | | | | | | __/
    |_|    \ _| |_| |_| |_______) |_| |_|  \___| | |_| |_| |_| |___|
                                             __/ |                 
                                            \___/   
                                                
                                                
                                                                 F i l e


Copyright: NIIEngine Team Group

Home page: www.niiengine.com 

Email: niiengine@gmail.com OR niiengine@163.com

Licence: commerce(www.niiengine.com/license)(Three kinds)
------------------------------------------------------------------------------
*/

#define __PERFORMANCE
#include "gsgsProfile.h"

#ifdef __WXMSW__

#include <assert.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <windows.h>

#if 0
static FILE* dbgFile(NULL);
static std::string output = "";
static std::vector<std::pair<DWORD, DWORD> > tickCounts;
#endif
void PERF_OUTPUT(const char* path) 
{ 
#if 0
    output = path; 
#endif
}

void PERF_START(const char* func_name)
{
#if 0
    if(!dbgFile) {
        dbgFile = fopen(output.c_str(), "w+");
        assert(dbgFile != 0);
    }
    fprintf(dbgFile, "%*c<block name=\"%s\">\n", 4 * tickCounts.size(), ' ', func_name);
    fflush(dbgFile);
    tickCounts.push_back(std::make_pair(DWORD(GetTickCount()), DWORD(0)));
#endif
}

void PERF_END()
{
#if 0
    assert(dbgFile != 0);
    if(tickCounts.empty()) { return; }
    DWORD tickCount = GetTickCount();
    DWORD elapsed = tickCount - tickCounts.back().first;
    DWORD unaccounted = elapsed - tickCounts.back().second;

    fprintf(dbgFile, "%*c<elapsed time=\"%ld\"", 4 * tickCounts.size(), ' ', elapsed);
    if(0 < unaccounted && unaccounted < elapsed) { fprintf(dbgFile, " unaccounted=\"%ld\"", unaccounted); }
    fprintf(dbgFile, "/>\n");

    tickCounts.pop_back();
    if(!tickCounts.empty()) { tickCounts.back().second += elapsed; }

    fprintf(dbgFile, "%*c</block>\n", 4 * tickCounts.size(), ' ');
    fflush(dbgFile);
#endif
}

#endif
