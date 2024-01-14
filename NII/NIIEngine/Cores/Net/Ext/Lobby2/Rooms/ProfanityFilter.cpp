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
#include "ProfanityFilter.h"
#include "Rand.h"
#include "N_assert.h"
#include "LinuxStrings.h"

#if defined(_WIN32)
#include <malloc.h> // alloca
#elif (defined(__GNUC__)  || defined(__GCCXML__))
#include <alloca.h>
#else
#endif

namespace NII
{
namespace NII_NET
{
    char ProfanityFilter::BANCHARS[] = "!@#$%^&*()";
    char ProfanityFilter::WORDCHARS[] = "abcdefghijklmnopqrstuvwxyz0123456789";

    ProfanityFilter::ProfanityFilter()
    {
    }

    ProfanityFilter::~ProfanityFilter()
    {
    }

    char ProfanityFilter::RandomBanChar()
    {
        return BANCHARS[randomMT() % (sizeof(BANCHARS) - 1)];
    }

    bool ProfanityFilter::HasProfanity(const char * str)
    {
        return FilterProfanity(str, 0,false) > 0;
    }

    int ProfanityFilter::FilterProfanity(const char * input, char * output, bool filter)
    {
        if (input==0 || input[0]==0)
            return 0;

        int count = 0;
        char* b = (char *) alloca(strlen(input) + 1); 
        strcpy(b, input);
        _strlwr(b);
        char *start = b;
        if (output)
            strcpy(output,input);

        start = strpbrk(start, WORDCHARS);
        while (start != 0)
        {
            size_t len = strspn(start, WORDCHARS);
            if (len > 0)
            {
                // we a have a word - let's check if it's a BAAAD one
                char saveChar = start[len];
                start[len] = '\0';

                // loop through profanity list			
                for (unsigned int i = 0, size = words.Size(); i < size; i++)
                {
                    if (_stricmp(start, words[i].C_String()) == 0)
                    {
                        count++;

                        // size_t len = words[i].size();
                        if (filter && output)
                        {
                            for (unsigned int j = 0; j < len; j++)
                            {
                                output[start + j - b] = RandomBanChar();
                            }
                        }
                        break;
                    }				
                }
                start[len] = saveChar;
            }

            start += len;
            start = strpbrk(start, WORDCHARS);
        }

        return count;
    }

    int ProfanityFilter::Count()
    {
        return words.Size();
    }
    void ProfanityFilter::AddWord(RakString newWord)
    {
        words.Insert(newWord);
    }
}
}