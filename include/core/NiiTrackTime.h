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

#ifndef _NII_TRACK_TIME_H_
#define _NII_TRACK_TIME_H_

#include "NiiPreInclude.h"

namespace NII
{
    /** �ؼ�֡ʱ������
    @version NIIEngine 3.0.0
    */
    class _EngineAPI TrackTime
    {
    public:
        TrackTime(TimeDurMS pos);
        TrackTime(TimeDurMS pos, Nidx index, Nidx lindex);

        /** �Ƿ���Ч
        @version NIIEngine 3.0.0
        */
        bool isValid() const;

        /** ��ȡʱ��
        @version NIIEngine 3.0.0
        */
        TimeDurMS getTimePos() const;

        /** ��ȡ������ʼ�ؼ�֡
        @version NIIEngine 3.0.0
        */
        Nidx getKeyIndex() const;
        
        /** ���õ�ǰʱ��ؼ�֡
        @version NIIEngine 3.0.0
        */
        void setLocalKeyIndex(Nidx idx);
        
        /** ��ȡ��ǰʱ��ؼ�֡
        @version NIIEngine 3.0.0
        */
        Nidx getLocalKeyIndex() const;
    protected:
        TimeDurMS mTimePos;
        Nidx mKeyIndex;
        Nidx mLocalKeyIndex;
    };
}

#endif