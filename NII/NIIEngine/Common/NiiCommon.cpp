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

#include "NiiPreProcess.h"
#include "NiiPreInclude.h"
#include "NiiCommon.h"
#include "NiiString.h"
#include "NiiLogManager.h"

namespace NII
{
    //--------------------------------------------------------------------------
    NIIi findCmdArgs(NIIi args, Ntchar ** argv, SingleArgList & unaryOptList, ComplexArgList & binOptList)
    {
        NIIi startIndex = 1;
        for (NIIi i = 1; i < args; ++i)
        {
            String tmp(argv[i]);
            if (StrUtil::beginsWith(tmp, _T("-")))
            {
                SingleArgList::iterator ui = unaryOptList.find(argv[i]);
                if(ui != unaryOptList.end())
                {
                    ui->second = true;
                    ++startIndex;
                    continue;
                }
                ComplexArgList::iterator bi = binOptList.find(argv[i]);
                if(bi != binOptList.end())
                {
                    bi->second = argv[i+1];
                    startIndex += 2;
                    ++i;
                    continue;
                }
                // Invalid option
                N_Only(Log).log(_I18n("Invalid option ") + tmp);
            }
        }
        return startIndex;
    }
    //-----------------------------------------------------------------
    const Nui32 N32_1OrMark[32] =
    {
        0x01,       0x02,       0x04,       0x08,       0x10,
        0x20,       0x40,       0x80,       0x100,      0x200,
        0x400,      0x800,      0x1000,     0x2000,     0x4000,
        0x8000,     0x10000,    0x20000,    0x40000,    0x80000,
        0x100000,   0x200000,   0x400000,   0x800000,   0x1000000,
        0x2000000,  0x4000000,  0x8000000,  0x10000000, 0x20000000,
        0x40000000, 0x80000000
    };
    //-----------------------------------------------------------------
    const Nui32 N32_1AndMark[32] =
    {
        0x01,       0x03,       0x07,       0x0F,
        0x1F,       0x3F,       0x7F,       0xFF,       0x1FF,
        0x3FF,      0x7FF,      0xFFF,      0x1FF,      0x3FFF,
        0x7FFF,     0xFFFF,     0x1FFFF,    0x3FFFF,    0x7FFFF,
        0xFFFFF,    0x1FFFFF,   0x3FFFFF,   0x7FFFFF,   0xFFFFFF,
        0x1FFFFFF,  0x3FFFFFF,  0x7FFFFFF,  0xFFFFFFF,  0x1FFFFFFF,
        0x3FFFFFFF, 0x7FFFFFFF, 0xFFFFFFFF
    };
    //-----------------------------------------------------------------
    const Nui32 N32_1NotMark[32] =
    {
        0xFFFFFFFE, 0xFFFFFFFD, 0xFFFFFFFB, 0xFFFFFFF7, 0xFFFFFFEF,
        0xFFFFFFDF, 0xFFFFFFBF, 0xFFFFFF7F, 0xFFFFFEFF, 0xFFFFFDFF,
        0xFFFFFBFF, 0xFFFFF7FF, 0xFFFFEFFF, 0xFFFFDFFF, 0xFFFFBFFF,
        0xFFFF7FFF, 0xFFFEFFFF, 0xFFFDFFFF, 0xFFFBFFFF, 0xFFF7FFFF,
        0xFFEFFFFF, 0xFFDFFFFF, 0xFFBFFFFF, 0xFF7FFFFF, 0xFEFFFFFF,
        0xFDFFFFFF, 0xFBFFFFFF, 0xF7FFFFFF, 0xEFFFFFFF, 0xDFFFFFFF,
        0xBFFFFFFF, 0x7FFFFFFF
    };
    //-----------------------------------------------------------------
    const Nui64 N64_1OrMark[64] = 
    {
        0
    };
    //-----------------------------------------------------------------
    const Nui64 N64_1AndMark[64] = 
    {
        0
    };
    //-----------------------------------------------------------------
    const Nui64 N64_1NotMark[64] = 
    {
        0
    };
    //-----------------------------------------------------------------
    const Nui64 N64_16OrMark[4] =
    {
        0xFFFF, 0xFFFF0000, 0xFFFF00000000, 0xFFFF000000000000
    };
    //-----------------------------------------------------------------
    const Nui64 N64_16AndMark[4] =
    {
        0xFFFF, 0xFFFFFFFF, 0xFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF
    };
    //-----------------------------------------------------------------
    const Nui64 N64_16NotMark[4] = 
    { 
        0xFFFFFFFFFFFF0000, 0xFFFFFFFF0000FFFF, 0xFFFF0000FFFFFFFF, 0xFFFFFFFFFFFF 
    }
    //-----------------------------------------------------------------
    const EventID EventCount = 0;
    const PropertyID PropertyCount = 0;
    const StateID StateCount = 0;
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // RenderSysData
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    bool RenderSysData::getPlatformData(const String &, void *) const
    {
        return false;
    }
    //------------------------------------------------------------------------
    Nmark RenderSysData::getPlatformType() const
    { 
        return T_Unknow;
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    CmpMode reverse(CmpMode mode)
    {
        switch(mode)
        {
        case CMPF_LESS:         return CMPF_GREATER;
        case CMPF_LESS_EQUAL:   return CMPF_GREATER_EQUAL;
        case CMPF_GREATER_EQUAL:return CMPF_LESS_EQUAL;
        case CMPF_GREATER:      return CMPF_LESS;
        default:                return mode;
        }

        return mode;
    }
    //-----------------------------------------------------------------
}

#if (N_PLAT != N_PLAT_WIN32) && !defined(__GNUC__)  &&!defined(__GCCXML__)
// From http://www.openasthra.com/c-tidbits/gettimeofday-function-for-windows/

#if defined(_MSC_VER) || defined(_MSC_EXTENSIONS)
  #define DELTA_EPOCH_IN_MICROSECS  11644473600000000Ui64
#else
  #define DELTA_EPOCH_IN_MICROSECS  11644473600000000ULL
#endif

int gettimeofday(struct timeval * tv, struct timezone *tz)
{
#if defined(N_PLAT_WIN8_PHONE) || defined(N_PLAT_WINRT)
	// _tzset not supported
	(void) tv;
	(void) tz;
#else
    FILETIME ft;
    Nui32 __int64 tmpres = 0;
    static int tzflag;

    if (NULL != tv)
    {
        GetSystemTimeAsFileTime(&ft);

        tmpres |= ft.dwHighDateTime;
        tmpres <<= 32;
        tmpres |= ft.dwLowDateTime;

        /*converting file time to unix epoch*/
        tmpres /= 10;  /*convert into microseconds*/
        tmpres -= DELTA_EPOCH_IN_MICROSECS;
        tv->tv_sec = (long)(tmpres / 1000000UL);
        tv->tv_usec = (long)(tmpres % 1000000UL);
    }

    if (NULL != tz)
    {
        if (!tzflag)
        {
          _tzset();
          tzflag++;
        }
        tz->tz_minuteswest = _timezone / 60;
        tz->tz_dsttime = _daylight;
    }
#endif
    return 0;
}

#endif