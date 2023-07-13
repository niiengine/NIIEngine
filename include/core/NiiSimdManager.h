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

#ifndef _NII_SIMD_MANAGER_H_
#define _NII_SIMD_MANAGER_H_

#include "NiiPreInclude.h"
#include "NiiSingleton.h"
#include "NiiSimdInfo.h"
#include "NiiResourceManager.h"
#include "NiiApuProgram.h"
#include "NiiApuBuffer.h"

namespace NII
{
    class _EngineAPI ApuContext
    {
        friend class SimdManager;
    public:
        ApuContext() : mPlatformIndex(-1), mDeviceIndex(-1), mApuLanguage(ApuL_Unknow) {}
        virtual ~ApuContext() {}

        inline ApuLanguage getLang() const { return mApuLanguage; }
    protected:
        Nidx mPlatformIndex;
        Nidx mDeviceIndex;
        ApuLanguage mApuLanguage;
    };
    typedef vector<ApuContext *>::type ApuContextList;

    /** ���д��������
    @version NIIEngine 4.0.0
    */
    class _EngineAPI SimdManager : public Singleton<SimdManager>, public ResourceManager, public BufferManager
    {
    public:
        enum Type
        {
            T_CPU,
            T_GPU,
            T_APU,
            T_ALL
        };
        typedef map<Nui32, MemStream *>::type CacheList;
    public:
        SimdManager();
        ~SimdManager();

        /** ����
        @version NIIEngine 4.0.0
        */
        bool setup();

        /** �Ƿ�֧��ָ���﷨
        @version NIIEngine 4.0.0
        */
        virtual bool isSupport(ApuLanguage al) const;

        /** �Ƿ�֧��ָ���﷨��
        @version NIIEngine 4.0.0
        */
        ApuLanguageMark getSupportMark() const;

        /** ��������Ӧ�ó���
        @version NIIEngine 3.0.0
        */
        virtual void create(ApuContext *& out, ApuLanguage al, Type type, Nidx platidx, Nidx devidx);

        /** ��ȡ����Ӧ�ó���
        @version NIIEngine 3.0.0
        */
        inline ApuContext * getContext(Nidx idx)    { return mApuContextList[idx]; }

        /** ɾ������Ӧ�ó���
        @version NIIEngine 3.0.0
        */
        virtual void destroy(ApuContext * obj);

        /** �������ٻ���
        @param[in] size 
        @param[in] count ����.
        @param[in] mode һ�����ʹ�� Buffer::M_DEVWRITE, ���д����Ӧ���� M_WRITE �� M_HOST
        @version NIIEngine 3.0.0
        */
        virtual void create(ApuBuffer *& out, const ApuContext * ctx, void * src, NCount unitSize, NCount unitCnt, Nmark mode);

        /** ɾ�����ٻ���
        @version NIIEngine 3.0.0
        */
        virtual void destroy(ApuBuffer * obj);

        /** �������ٳ���
        @note ���Զ�����
        @param[in] rid ��Դ��ID
        @param[in] gid ��Դ���ID
        @param[in] file ���ص��ļ�
        @param[in] type ��������
        @param[in] sl �﷨����
        */
        virtual ApuProgram * create(ResourceID rid, GroupID gid, const String & file, const VString & kernel, const ApuContext * ctx);

        /** �������ٳ���
        @note ���Զ�����
        @param[in] rid ��Դ��ʶ
        @param[in] gid ��Դ��
        @param[in] type ��������
        @param[in] sl �﷨����
        @param[in] code ���ٳ������
        */
        virtual ApuProgram * create(ResourceID rid, GroupID gid, const VString & kernel, const ApuContext * ctx, const VString & code);

        /** �������ٳ���
        @note ���Զ�����
        @version NIIEngine 4.0.0
        */
        virtual ApuProgram * create(ResourceID rid, GroupID gid, const VString & kernel, const ApuContext * ctx, 
            ResLoadScheme * ls = 0, ResResultScheme * rs = 0);

        /** ���ؼ��ٳ���
        @param[in] rid ��ԴID
        @param[in] gid ��Դ��ID
        @param[in] file ���ص��ļ�
        @param[in] type ��������
        @param[in] sl �﷨����
        */
        virtual ApuProgram * load(ResourceID rid, GroupID gid, const String & file, const VString & kernel, const ApuContext * ctx);

        /** ���ؼ��ٳ���
        @param[in] sl �﷨����
        @param[in] code ���ٳ������
        */
        virtual ApuProgram * load(ResourceID rid, GroupID gid, const VString & kernel, const ApuContext * ctx, const VString & code);

        ///@copydetails ResourceManager::get
        virtual Resource * get(ResourceID rid, GroupID gid = GroupUnknow);

        /** �����Ƿ��Զ�������ٳ���Ķ�����
        @remark �´����õ�ʱ�����
        @version NIIEngine 4.0.0
        */
        void setCacheBinary(bool b);

        /** ��ȡ�Ƿ��Զ�������ٳ���Ķ�����
        @remark �´����õ�ʱ�����
        @version NIIEngine 4.0.0
        */
        bool isCacheBinary() const;

        /** �����Ƿ��Զ�������ٳ���Ķ����Ƶ��ļ���
        @remark �´����õ�ʱ�����
        @version NIIEngine 4.0.0
        */
        void setCacheBinaryFile(bool b);

        /** ��ȡ�Ƿ��Զ�������ٳ���Ķ����Ƶ��ļ���
        @remark �´����õ�ʱ�����
        @version NIIEngine 4.0.0
        */
        bool isCacheBinaryFile() const;

        /** �����Ƿ��Զ�������ٳ���Ķ����Ƶ��ļ�Ŀ¼
        @version NIIEngine 4.0.0
        */
        inline void setCachePath(const String & path)       { mCachePath = path; }

        /** ��ȡ�Ƿ��Զ�������ٳ���Ķ����Ƶ��ļ�Ŀ¼
        @version NIIEngine 3.0.0
        */
        inline const String & getCachePath() const          { return mCachePath; }

        /** ��������
        @version NIIEngine 4.0.0
        */
        void exportCache(DataStream * out) const;

        /** ��������
        @version NIIEngine 4.0.0
        */
        void inportCache(const DataStream * in);

        /** �������ٳ���Ķ����ƻ���
        @version NIIEngine 4.0.0
        */
        MemStream * createCache(Nui32 cid, NCount size);

        /** ɾ�����ٳ���Ķ����ƻ���
        @version NIIEngine 4.0.0
        */
        void destroyCache(Nui32 cid);

        /** ���ٴ��ڻ����Ƿ����
        @version NIIEngine 4.0.0
        */
        bool isCacheExist(Nui32 cid) const;

        /** ��ȡ���ٳ���Ķ����ƻ���
        @version NIIEngine 4.0.0
        */
        MemStream * getCache(Nui32 cid) const;

        /** ��ȡƽ̨�б�
        @version NIIEngine 4.0.0
        */
        inline const ApuInfoList & getPlatformList() const { return mApuInfoList; }
    public:
        /** �����Ѿ���ɾ��
        @version NIIEngine 3.0.0
        */
        void onDestroy(ApuBuffer * obj);
    protected:
        /// @copydetails ResourceManager::createImpl
        Resource * createImpl(ResourceID rid, GroupID gid, ResLoadScheme * ls, ResResultScheme * rs,
            const PropertyData * params);

        virtual Resource * createImpl(ResourceID rid, GroupID gid, ResLoadScheme * ls, ResResultScheme * rs, 
            const VString & kernel, const ApuContext * ctx);
    protected:
        ApuContextList mApuContextList;
        ApuInfoList mApuInfoList;
        CacheList mCacheList;
        String mCachePath;
        Nmark mCacheMark;
    };
}
#endif