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

#ifndef _NII_GAME_SCRIPT_H_
#define _NII_GAME_SCRIPT_H_

#include "NiiPreInclude.h"
#include "NiiGameScript.h"

namespace NII
{
    class _EngineAPI AngelScriptGameScript : public GameScript
    {
    public:
        AngelScriptGameScript();
        virtual ~AngelScriptGameScript();

        /**
        @remark
        */
        void init();

        /**
        @remark
        */
        void shutdown();

        /** ִ��һ�����ؽű��ļ�
        @remark
        @param[in] file �ļ�����
        @param[in] group ��������Դ��
        */
        void excute(const String & file, const String & group);

        /** ִ��һ��ȫ�ֺ���
        @param[in] function ��������
        */
        void excute(const String & function);

        /**
        @param[in] hander 
        @param[in] arg 
        */
        int excute(const String & hander, const EventArgs & arg);
        
        /** ִ��һ���ı�
        @param[in] src �ı�����
        */
        bool excute(const char * src);
    };
}
#endif