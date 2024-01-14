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
#ifndef __PROFANITY_FILTER__H__
#define __PROFANITY_FILTER__H__

#include "DS_List.h"
#include "RakString.h"

namespace NII 
{
namespace NII_NET
{
    class ProfanityFilter
    {
    public:
        ProfanityFilter();
        ~ProfanityFilter();

        // Returns true if the string has profanity, false if not.
        bool HasProfanity(const char *str);

        // Removes profanity. Returns number of occurrences of profanity matches (including 0)
        int FilterProfanity(const char *str, char *output, bool filter = true); 		
        
        // Number of profanity words loaded
        int Count();

        void AddWord(RakString newWord);
    private:	
        DataStructures::List<RakString> words;

        char RandomBanChar();

        static char BANCHARS[];
        static char WORDCHARS[];
    };
}
}
#endif