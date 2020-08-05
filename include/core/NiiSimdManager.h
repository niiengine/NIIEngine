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
    /** ���д��������
    @version NIIEngine 4.0.0
    */
    class _EngineAPI SimdManager : public Singleton<SimdManager>, public ResourceManager, public BufferManager,
        public CPUAlloc
    {
    public:
        enum Type
        {
            T_CPU,
            T_GPU,
            T_APU,
            T_ALL
        };
        typedef map<Nui32, MemDataStream *>::type CacheList;
    public:
        SimdManager();
        ~SimdManager();

        /** �Ƿ�֧��ָ���﷨
        @version NIIEngine 4.0.0
        */
        virtual bool isSupport(ApuLanguage sl) const;

        /** �Ƿ�֧��ָ���﷨��
        @version NIIEngine 4.0.0
        */
        ApuLanguageMark getSupportMark() const;

        /** �������ٻ���
        @param[in] size 
        @param[in] count ����.
        @param[in] mode һ�����ʹ�� Buffer::M_GPU | M_WRITE, ���д����Ӧ���� M_WRITE �� M_CPU
        @version NIIEngine 3.0.0
        */
        virtual void create(ApuBuffer *& out, NCount size, NCount count, Nmark mode);

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
        virtual ApuProgram * create(ResourceID rid, GroupID gid, const String & file, const VString & kernel,
            ApuLanguage sl, Nidx platidx, Type type);

        /** �������ٳ���
        @note ���Զ�����
        @param[in] rid ��Դ��ʶ
        @param[in] gid ��Դ��
        @param[in] type ��������
        @param[in] sl �﷨����
        @param[in] code ���ٳ������
        */
        virtual ApuProgram * create(ResourceID rid, GroupID gid, const VString & kernel, ApuLanguage sl,
            Nidx platidx, Type type, const VString & code);

        /** �������ٳ���
        @note ���Զ�����
        @version NIIEngine 4.0.0
        */
        virtual ApuProgram * create(ResourceID rid, GroupID gid, const VString & kernel, ApuLanguage sl,
            Nidx platidx, Type type, ResLoadScheme * ls = 0, ResResultScheme * rs = 0);

        /** ���ؼ��ٳ���
        @param[in] rid ��ԴID
        @param[in] gid ��Դ��ID
        @param[in] file ���ص��ļ�
        @param[in] type ��������
        @param[in] sl �﷨����
        */
        virtual ApuProgram * load(ResourceID rid, GroupID gid, const String & file, const VString & kernel,
            ApuLanguage sl, Nidx platidx, Type type);

        /** ���ؼ��ٳ���
        @param[in] sl �﷨����
        @param[in] code ���ٳ������
        */
        virtual ApuProgram * load(ResourceID rid, GroupID gid, const VString & kernel, ApuLanguage sl, 
            Nidx platidx, Type type, const VString & code);

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
        bool isCacheBinary();

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
        MemDataStream * createCache(Nui32 cid, NCount size);

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
        MemDataStream * getCache(Nui32 cid) const;

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
            const VString & kernel, ApuLanguage sl, Nidx platidx, Type type);
    protected:
        ApuInfoList mApuInfoList;
        CacheList mCacheList;
        Nmark mCacheMark;
    };
}
#endif