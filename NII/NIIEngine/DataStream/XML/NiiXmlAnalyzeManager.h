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

#ifndef _NII_XmlAnalyzeManager_H_
#define _NII_XmlAnalyzeManager_H_

#include "NiiPreInclude.h"
#include "NiiSingleton.h"
#include "NiiScriptProperty.h"

#define N_XmlAnalyzeFactory(c, lid) PreDefineXmlAnalyzeFactory<c>(lid)

namespace NII
{
    typedef void (XmlAnalyze::*PrcCB)(const PropertyData * pdc);
    /** xml�ɷ���
    @remark �����԰汾���Խڵ�,<?xml version="1.0" encoding="UTF-8"?>,����xml���ı������
        ������ʹ�ú�������ͬ��utf-8.
    @version NIIEngine 3.0.0
    */
    class _EngineAPI XmlAnalyze : public ScriptProperty, public ScriptAlloc
    {
    public:
        typedef map<PropertyID, PrcCB>::type PrcCBMap;
    public:
        XmlAnalyze(VersionID version, ScriptTypeID stid, const String & xsd = N_StrBlank, LangID lid = N_PrimaryLang);
        virtual ~XmlAnalyze();

        /** �����Ƿ񲻴�������(��)�ɷ���
        @note Ĭ�ϴ��������ɷ���
        @version NIIEngine 3.0.0
        */
        void setSkipOther(bool b)                   { mSkipOther = b; }

        /** ��ȡ�Ƿ񲻴��������ɷ���
        @note Ĭ�ϴ��������ɷ���
        @version NIIEngine 3.0.0
        */
        bool isSkipOther() const                    { return mSkipOther; }

        /** ���ÿ�ʼ������
        @param[in] pid ����
        @param[in] prc ����ص�
        @version NIIEngine 3.0.0
        */
        void setBeginPrc(PropertyID pid, PrcCB prc);

        /** �����ʼ������
        @param[in] pid
        @version NIIEngine 3.0.0
        */
        void unBeginPrc(PropertyID pid);

        /** ���ý���������
        @param[in] pid ����
        @param[in] prc ����ص�
        @version NIIEngine 3.0.0
        */
        void setEndPrc(PropertyID pid, PrcCB prc);

        /** �������������
        @param[in] pid
        @version NIIEngine 3.0.0
        */
        void unEndPrc(PropertyID pid);

        /** �����ǩ
        @param[in] parent ����ǩ
        @param[in] pid ��ǩ����
        @param[in] pdc ��ǩ���ݳɷ�
        @version NIIEngine 3.0.0
        */
        virtual void onBegin(XmlAnalyze * parent, PropertyID pid, const PropertyData * pdc);

        /** �����ǩ
        @param[in] parent ����ǩ
        @param[in] pid ����ID
        @version NIIEngine 3.0.0
        */
        virtual void onEnd(XmlAnalyze * parent, PropertyID pid, const PropertyData * pdc);

        /** ��������
        @param data ����ֵ
        @version NIIEngine 3.0.0
        */
        virtual void onData(const String & data);

        /** ��������
        @param data ����ֵ
        @version NIIEngine 3.0.0
        */
        virtual void onData(const Nutf8 * data);

        /** ��ȡXml���Ͷ����ļ�
        @return �ļ�(�����ļ�����չ��)
        @version NIIEngine 3.0.0
        */
        virtual const String & getXSD() const;

        /** ��ȡXml�ɷ�ID
        @note �������ʵ��
        @version NIIEngine 3.0.0
        */
        virtual XmlUnitID getUnit() const = 0;

        /** ��ȡȺ��ID
        @note �������ʵ��
        @version NIIEngine 3.0.0
        */
        virtual GroupID getGroup() const = 0;
    protected:
        /** �����ڿ�ʼ
        @version NIIEngine 3.0.0
        */
        virtual void PrcTagUnitCB(const PropertyData * pdc) = 0;

        /** �����ڽ���
        @version NIIEngine 3.0.0
        */
        virtual void PrcTagUnitECB(const PropertyData * pdc);
    protected:
        PrcCBMap mBeginMap;
        PrcCBMap mEndMap;
        XmlAnalyze * mTag;
        VersionID mVersion;
        String mXSD;
        bool mAutoTag;
        bool mSkipOther;
    };

    /** xml����������
    @versioin NIIEngine 3.0.0
    */
    class _EngineAPI XmlAnalyzeFactory : public XmlAlloc
    {
    public:
        XmlAnalyzeFactory() {}
        virtual ~XmlAnalyzeFactory() {}

        /** ��ȡ����ID
        @version NIIEngine 3.0.0
        */
        virtual FactoryID getID() const = 0;

        /** ��������
        @version NIIEngine 3.0.0
        */
        virtual XmlAnalyze * create(LangID lid) = 0;

        /** ɾ������
        @version NIIEngine 3.0.0
        */
        virtual void destroy(XmlAnalyze * obj) = 0;
    };

    /** Ԥ����UI��Ԫ������
    @version NIIEngine 3.0.0
    */
    template <typename T> class PreDefineXmlAnalyzeFactory : public XmlAnalyzeFactory
    {
    public:
        PreDefineXmlAnalyzeFactory(FactoryID fid);

        /// @copydetails XmlAnalyzeFactory::getID
        FactoryID getID() const;

        /// @copydetails XmlAnalyzeFactory::create
        XmlAnalyze * create(LangID lid);

        /// @copydetails XmlAnalyzeFactory::destroy
        void destroy(XmlAnalyze * obj);

        /** ע�뵽ϵͳ��
        @version NIIEngine 3.0.0
        */
        static void plugin();

        /** ��ϵͳ���Ƴ�
        @version NIIEngine 3.0.0
        */
        static void unplugin();
    protected:
        FactoryID mID;
    };
    //------------------------------------------------------------------------
    template <typename T> PreDefineXmlAnalyzeFactory<T>::PreDefineXmlAnalyzeFactory(FactoryID fid) :
        mID(fid)
    {
    }
    //------------------------------------------------------------------------
    template <typename T> FactoryID PreDefineXmlAnalyzeFactory<T>::getID() const
    {
        return mID;
    }
    //------------------------------------------------------------------------
    template <typename T> XmlAnalyze * PreDefineXmlAnalyzeFactory<T>::create(LangID lid)
    {
        return N_new T(lid);
    }
    //------------------------------------------------------------------------
    template <typename T> void PreDefineXmlAnalyzeFactory<T>::destroy(XmlAnalyze * obj)
    {
        N_delete obj;
    }
    //------------------------------------------------------------------------
    template <typename T> void PreDefineXmlAnalyzeFactory<T>::plugin()
    {
        if (N_OnlyPtr(XmlAnalyze))
        {
            XmlAnalyzeFactory * factory = N_new PreDefineXmlAnalyzeFactory<T>(mID);
            if (N_Only(XmlAnalyze).addFactory(factory) != true)
                N_delete factory;
        }
    }
    //------------------------------------------------------------------------
    template <typename T> void PreDefineXmlAnalyzeFactory<T>::unplugin()
    {
        if (N_OnlyPtr(XmlAnalyze))
        {
            XmlAnalyzeFactory * factory = N_Only(XmlAnalyze).getFactory(mID);
            if (factory != 0)
            {
                N_Only(XmlAnalyze).removeFactory(mID);
                N_delete factory;
            }
        }
    }
    //------------------------------------------------------------------------

    /** Xml����������
    @version NIIEngine 3.0.0
    */
    class _EngineAPI XmlAnalyzeManager : public Singleton<XmlAnalyzeManager>, public ManagerAlloc
    {
    public:
        XmlAnalyzeManager(LangID lid = N_PrimaryLang);
        ~XmlAnalyzeManager();

        /** ��ӹ�����
        @version NIIEngine 3.0.0
        */
        bool addFactory(XmlAnalyzeFactory * f);

        /** ��ȥ������
        @version NIIEngine 3.0.0
        */
        void removeFactory(FactoryID fid);

        /** ��ȡ������
        @version NIIEngine 3.0.0
        */
        XmlAnalyzeFactory * getFactory(FactoryID fid) const;

        /** ����xml������
        @version NIIEngine 3.0.0
        */
        XmlAnalyze * create(FactoryID fid, LangID lid);

        /** ɾ��xml������
        @version NIIEngine 3.0.0
        */
        void destroy(XmlAnalyze * obj);
    protected:
        typedef map<FactoryID, XmlAnalyzeFactory *>::type XmlAnalyzeFactoryList;
        XmlAnalyzeFactoryList mXmlAnalyzes;
        LangID mLangID;
    };
}
#endif