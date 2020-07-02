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

#ifndef _NII_DADA_FUNC_H_
#define _NII_DADA_FUNC_H_

#include "NiiPreInclude.h"

namespace NII
{
    /** ���ݺ���
    @remark ����ӳ������
    @version NIIEngine 3.0.0
    */
    template <typename in, typename out> class DataFunc : public DataAlloc
    {
    public:
        DataFunc(bool autoDsy) : 
            mAutoDestroy(autoDsy)
        {
        }

        virtual ~DataFunc() {}

        /** ����
        @param[in] src ����ֵ
        @return ���ֵ
        @version NIIEngine 3.0.0
        */
        virtual out func(const in & src) = 0;
        
        /** �����Ƿ���Ч
        @version NIIEngine 3.0.0
        */
        virtual bool isValid() const { return true; }
        
        /** �����Ƿ��Զ�ɾ������
        @version NIIEngine 3.0.0
        */
        void setAutoDestroy(bool b){ mAutoDestroy = b; }
        
        /** ��ȡ�Ƿ��Զ�ɾ������
        @version NIIEngine 3.0.0
        */
        bool isAutoDestroy() const{ return mAutoDestroy; }
    protected:
        bool mAutoDestroy;
    };
}
#endif