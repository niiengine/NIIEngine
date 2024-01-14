#include <stdio.h>
#include <vector>

#include "crawler_include.h"
#include <wx/wxcrtvararg.h>

//#include <windows.h>

wxString extract_path(const wxString &filePath);

int main(int argc, char **argv)
{
//	FileOpen::Get()->AddSearchPath("C:/MinGW-3.4.5/include");
//	FileOpen::Get()->AddSearchPath("C:/wxWidgets-2.8.10/include");
//	FileOpen::Get()->AddSearchPath("C:\\MinGW-4.4.0\\include");
//	FileOpen::Get()->AddSearchPath("C:\\MinGW-4.4.0\\lib\\gcc\\mingw32\\4.4.0\\include\\c++");
    FileOpen::Get()->AddSearchPath("D:/wxWidgets-3.0.1/include");
    FileOpen::Get()->AddExcludePath("D:/wxWidgets-3.0.1/include/wx/android");
    FileOpen::Get()->AddExcludePath("D:/wxWidgets-3.0.1/include/wx/univ");
    FileOpen::Get()->AddExcludePath("D:/wxWidgets-3.0.1/include/wx/unix");
//	FileOpen::Get()->AddExcludePath("C:\\MinGW-4.4.0\\lib\\gcc\\mingw32\\4.4.0\\include\\c++\\debug");
//	FileOpen::Get()->AddExcludePath("C:/wxWidgets-2.8.10/include/wx/unix");
//	FileOpen::Get()->AddExcludePath("C:/wxWidgets-2.8.10/include/wx/univ");

//	FileOpen::Get()->AddSearchPath("/usr/include/");
//	FileOpen::Get()->AddSearchPath("/usr/include/c++/4.2.4/");
//	FileOpen::Get()->AddSearchPath("/usr/include/wx-2.8/");

    FileOpen::Get()->ClearResults();
    for (size_t i=0; i<1; i++) {
        //	double start = GetTickCount();
        if(crawlerScan("../test.h") < 0) {
            printf("failed to scan files\n");
        }
        printf("Found %d files \n", FileOpen::Get()->GetResults().size());

        std::set<wxString>::const_iterator iter = FileOpen::Get()->GetResults().begin();

        printf("---->\n");
        for (; iter != FileOpen::Get()->GetResults().end(); iter ++) {
            wxPrintf("%s\n", (*iter));
        }

        //printf("----> Includes \n");
        //const FileOpen::List_t& includeStatements = FileOpen::Get()->GetIncludeStatements();
        //FileOpen::List_t::const_iterator it = includeStatements.begin();
        //for(; it != includeStatements.end(); ++it) {
        //    printf("%s\n", (*it).c_str());
        //}
    }

    std::set<wxString> ns = FileOpen::Get()->GetNamespaces();
    std::set<wxString>::iterator iter = ns.begin();
    for(; iter != ns.end(); iter++) {
        wxPrintf("Using namespace: %s\n", iter->c_str());
    }

    std::set<wxString> nsa = FileOpen::Get()->GetNamespaceAliases();
    iter = nsa.begin();
    for(; iter != nsa.end(); iter++) {
        wxPrintf("Namespace alias: %s\n", iter->c_str());
    }
    return 0;
}
