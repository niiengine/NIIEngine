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

#ifndef _NII_SCRIPT_PROPERTY_H_
#define _NII_SCRIPT_PROPERTY_H_

#include "NiiPreInclude.h"

namespace NII
{
    /** �ı����ű�����
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ScriptProperty
    {
    public:
        ScriptProperty(ScriptTypeID stid, LangID lid = N_PrimaryLang);
        virtual ~ScriptProperty();
        
        /** ��ȡʵ�ʵ�����
        @param name �ڵ��ı�����
        @return �ڵ��ı����������԰汾�����������
        @version NIIEngine 3.0.0
        */
        PropertyID getProperty(const String & name) const;

        /** ��ȡʵ�������ı�
        @param[in] pid ����
        @version NIIEngine 3.0.0
        */
        const String & getLang(PropertyID pid) const;

        /** ��ȡ��������
        @version NIIEngine 3.0.0
        */
        LangID getLangID() const;
    public:
        /** ��ǩ�е�Ԫ��(��ֵ)
        @version NIIEngine 3.0.0
        */
        static const PropertyID PropertyDefine;

        /** id/��ʶ���� ��ǩ�е�Ԫ��
        @version NIIEngine 3.0.0
        */
        static const PropertyID IDProperty;

        /** name/���� ��ǩ�е�Ԫ��
        @version NIIEngine 3.0.0
        */
        static const PropertyID NameProperty;

        /** gid/Ⱥ�� ��ǩ�е�Ԫ��
        @version NIIEngine 3.0.0
        */
        static const PropertyID GroupProperty;

        /** type/���� ��ǩ�е�Ԫ��
        @version NIIEngine 3.0.0
        */
        static const PropertyID TypeProperty;

        /** src/��Դ ��ǩ�е�Ԫ��
        @version NIIEngine 3.0.0
        */
        static const PropertyID SourceProperty;

        /** value/ֵ ��ǩ�е�Ԫ��
        @version NIIEngine 3.0.0
        */
        static const PropertyID ValueProperty;

        /** ver/�汾 ��ǩ�е�Ԫ��
        @version NIIEngine 3.0.0
        */
        static const PropertyID VersionProperty;
        
        /** true ��ǩ�е�Ԫ��(��ֵ)
        @version NIIEngine 3.0.0
        */
        static const PropertyID TrueProperty;
        
        /** false ��ǩ�е�Ԫ��(��ֵ)
        @version NIIEngine 3.0.0
        */
        static const PropertyID FalseProperty;
        
        /** ����ı�ǩ�еĸ���
        @version NIIEngine 3.0.0
        */
        static const PropertyID PropertyCount;
    protected:
        ScriptLangMap * mLangMap;
        ScriptTypeID mScriptTypeID;
    };
}

#endif