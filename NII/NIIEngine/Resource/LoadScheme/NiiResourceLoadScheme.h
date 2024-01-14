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

#ifndef _NII_RESOURCE_LOAD_SCHEME_H_
#define _NII_RESOURCE_LOAD_SCHEME_H_

#include "NiiPreInclude.h"

namespace NII
{
    /** ��Դ���ز���
    @remark �������ھ�����Դ�ļ��ع���
    @note ��������
    */
    class _EngineAPI ResLoadScheme : public ResourceAlloc
    {
    public:
        ResLoadScheme();
        virtual ~ResLoadScheme();

        /** ���������Դʱ������,ע�����п��ܻ��ں�̨�̵߳���,�����ǰ��߳�
            (NII_THREAD_SUPPORT == 2)���,�����ܴӷ���rendersystem�ص����.
            ��Ҫ��loadResource���ض���
        @param[in] resource ϣ�����ص���Դ
        */
        virtual void plan(Resource * resource) { (void)resource; }

        /** ����׼����Դʱ������.
        @param[in] resource ϣ��׼������Դ
        */
        virtual void load(Resource * resource) {};
    };
}
#endif