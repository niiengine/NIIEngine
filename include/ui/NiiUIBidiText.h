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
#ifndef _NII_UI_BidiTEXT_H_
#define _NII_UI_BidiTEXT_H_

#include "NiiUIPreInclude.h"

namespace NII
{
namespace UI
{
    /** ˫����������
    @version NIIEngine 3.0.0
    */
    enum BidiType
    {
        BT_RTL,
        BT_LTR,
        BT_Normal
    };

    /** ˫�����ִ���
    @version NIIEngine 3.0.0
    */
    class _EngineAPI BidiText : public UIAlloc
    {
    public:
        typedef vector<NIIi> IndexList;
    public:
        virtual ~BidiText();

        /** ��ȡ����˫������ 
        @param chr �ַ�
        @note Ĭ�ϵĻ�������,��Ӣ��/����Ļ�������Ϊ������,��������Ļ�������Ϊ���ҵ���.
        @version NIIEngine 3.0.0
        */
        virtual BidiType getBidiType(Nui32 chr) const = 0;

        /** ������Դ�ַ���
        @version NIIEngine 3.0.0
        */
        bool setSrc(const String & text);
        
        /** ��ȡĿ���ַ���
        @version NIIEngine 3.0.0
        */
        const String & getDst() const;
        
        /** �����Ų�
        @version NIIEngine 3.0.0
        */
        virtual bool reorder(const String & in, String & out) const = 0;

        /** �߼����ɼ�ӳ��
        @version NIIEngine 3.0.0
        */
        const IndexList & getLogicalToVisual() const;
        
        /** �ɼ����߼�ӳ��
        @version NIIEngine 3.0.0
        */
        const IndexList & getVisualToLogical() const;
    protected:
        IndexList mLTV;
        IndexList mVTL;
        String mDst;
    };
}
}
#endif