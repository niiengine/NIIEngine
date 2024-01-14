#ifndef PP_INCLUDE_H
#define PP_INCLUDE_H

#include <vector>
#include <wx/string.h>

#ifndef _gsgsAPI

#ifdef WXMAKINGDLL_CL
#    define _gsgsAPI __declspec(dllexport)
#elif defined(WXUSINGDLL_CL)
#    define _gsgsAPI __declspec(dllimport)
#else // not making nor using DLL
#    define _gsgsAPI
#endif

#endif

/**
 * @brief the main interface to the PP API.
 * @param filePath
 * @return 
 */
extern _gsgsAPI int PPScan      ( const wxString &filePath, bool forCC );
/**
 * @brief scan input string
 */
extern _gsgsAPI int PPScanString( const wxString &inputString );

#endif
