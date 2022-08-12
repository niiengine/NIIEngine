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
    
#ifndef _NII_DriverVersion_H_
#define _NII_DriverVersion_H_

#include "NiiPreInclude.h"

namespace NII
{
    /** �����汾
    @version NIIEngine 3.0.0
    */
    class _EngineAPI DriverVersion : public SysAlloc
    {
    public:
        DriverVersion();

        /** �����汾�Ƿ����һ����
        @param[in] o ��һ��
        @version NIIEngine 3.0.0
        */
        bool isOlder(DriverVersion o) const;
        
        /** �����汾�Ƿ����һ����
        @param[in] o ��һ��
        @version NIIEngine 3.0.0
        */
        bool isOlder(NIIi _major, NIIi _minor) const;
        
        /** �����汾�Ƿ����һ����
        @param[in] o ��һ��
        @version NIIEngine 3.0.0
        */
        bool isNewer(DriverVersion o) const;
        
        /** �����汾�Ƿ����һ����
        @param[in] o ��һ��
        @version NIIEngine 3.0.0
        */
        bool isNewer(NIIi _major, NIIi _minor) const;
        
        /** �����汾�ַ���
        @version NIIEngine 3.0.0
        */
        String toString() const;

        /** ���ַ�����ȡ�汾
        @version NIIEngine 3.0.0
        */
        void fromString(const String & str);
    public:
        NIIi major;
        NIIi minor;
        NIIi build;
        NIIi release;
    };
}

#endif