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

#include "NiiPreProcess.h"
#include "NiiShaderChProgram.h"
#include "NiiGpuProgramManager.h"
#include "NiiException.h"

namespace NII
{
    //------------------------------------------------------------------------
    /** ���������� ����һ�������Ƭ����ɫ����(�ͼ����߼���),��������ȷ�Ĳ�����
        ���÷�
    @remark
        ʹ�ö����Ƭ�γ�ʽ���Եõ��൱�����龰;�����൱�򵥵Ĺ��̽�ϵ���ɫ������
        GPU���Ի���ͼ��,����ʹ�ú��ٻ��������� ��:
        <ul>
        <li>��ɫ������Խϸߵȼ�(��Cg����,��RenderMonkey)���һ��(���).����Ҫ��
        ��ô�൱�޷��,�����һ������ɫ�����ܸ������ƶ��ܹ�ʹ�������Ĳ���,��������
        ���Ĵ���</li>
        <li>����Ͳ���֮����Թ�������÷�,�Ա��ʡ�ڴ�</li>
        <li>���������û��Զ�����ɫ����,����ϵĳ���,�㾭��ϣ���������ڻ�û�б�Ҫʱ
        ����Ǹ��׶�ֻ�ж���,��������/����/װ��.������Ҫ�÷���Ӧ������,���ھ���Ҫ
        ����ǰ.������ɫ������ܲ���Ҫ���Ǹ�ʱ�򱻶���,�������Ҫ�õ��ű�����������,
        ����Ҫ�����ʱ����Щ�ű���������.</li>
        </ul>
        ������װ����Щϸ��,��������ص���.����,����������㶨��һ���߲�ε���ɫ��
        ��,�����������Ĳ���,�������س���(��ͨ��������).����ɫ������غͱ���,�����
        ����֤���ṩ�Ĳ���,�������Ǽ�ʱ�������������в���.
    @par
        �Է�����˵�Ĳ������,����໹�ṩ��GpuProgram��HighLevelGpuProgram�Ĺ���,
        ����������.
    */
    class _EngineAPI ShaderChShader : public ResourceListener, public PassAlloc
    {
    public:
        ShaderChShader(ShaderChProgram::ShaderType type, ShaderCh * ch):
            mType(type),
            mProgram(0),
            mParam(0),
            mParent(ch),
            mLinkParam(false),
            mRecreateParam(false)
        {
        }

        ShaderChShader(ShaderCh * ch, const ShaderChShader & o):
            mType(o.mType),
            mProgram(o.mProgram),
            mParam(N_new GpuProgramParam(*o.mParam)),
            mParent(ch),
            mLinkParam(false),
            mRecreateParam(false)
        {
            mProgram->ref(false);
        }

        ~ShaderChShader()
        {
            if(mProgram != 0)
            {
                mProgram->remove(this);
                mProgram->unref();
            }
            if(mParam != 0 && mLinkParam != true)
                N_delete mParam;
        }

        /** ������ɫ����ʹ�õ�����.
        @param[in] name ��ɫ����ʹ�õ�����
        @param[in] resetParams
            ���Ϊ��,��������������µ���ɫ�����д���һ���µĲ��������֮ǰ����
            �Ĳ����㲻�ò��ٴ�������.������������Ϊ��,��������ȷ��������ȫ
            ƥ��,���������������,���õ������²������,����������
        @version NIIEngine 3.0.0
        */
        void setProgramName(ResourceID rid, bool resetParams = true)
        {
            if(mProgram != 0)
            {
                mProgram->remove(this);
                mProgram->unref();
                mRecreateParam = true;
            }

            mProgram = static_cast<GpuProgram *>(N_Only(GpuProgram).get(rid));
            if(mProgram == 0)
            {
                String type = (mType == ShaderChProgram::ST_VS ? _I18n("����") : (mType == ShaderChProgram::ST_GS ? _I18n("����") : _I18n("Ƭ��")));
                N_EXCEPT(NotExist, _I18n("���ܶ�λ��ɫ����: ") + type + _I18n(" ����ID: ") + N_conv(rid));
            }
            mProgram->ref(false);

            if(resetParams || mParam == 0 || mRecreateParam)
            {
                recreateParameters();
            }
            mProgram->add(this);
        }

        void setProgram(GpuProgram * prog)
        {
            mProgram = prog;
            mProgram->ref(false);

            if(mParam != 0 && mLinkParam != true)
                N_delete mParam;
            mParam = mProgram->createParam();
        }

        void setParam(GpuProgramParam * prog, bool link)
        {
            if (mParam != 0 && mLinkParam != true && link != true)
                N_delete mParam;
            if (link)
            {
                mParam = prog;
                mLinkParam = true;
            }
            else
            {
                mParam->copyDefineValue(*prog);
                mLinkParam = false;
            }
        }

        GpuProgramParam * getParam() const
        {
            if(mParam == 0)
            {
                N_EXCEPT(InvalidParam, _I18n("You must specify a program before you can retrieve parameters."));
            }
            return mParam;
        }

        void load()
        {
            if(mProgram->getState() != LS_LOADED)
                mProgram->load();

            if(mProgram->getState() == LS_LOADED && mProgram->getType() != mType)
            {
                String myType = (mType == ShaderChProgram::ST_VS ? _T("vertex") : (mType == ShaderChProgram::ST_GS ? _T("geometry") : _T("fragment")));

               String yourType = (mProgram->getType() == ShaderChProgram::ST_VS ? _T("vertex") :
                    (mProgram->getType() == ShaderChProgram::ST_GS ? _T("geometry") : _T("fragment")));

                N_EXCEPT(InvalidParam, mProgram->getOriginID() + _I18n("is a ") + yourType +
                    _I18n(" program, but you are assigning it to a ") +
                        myType + _I18n(" program slot. This is invalid."));

            }
        }

        /// ж�����ʹ�÷���
        void unload(void)
        {
        }

        /// @copydetails ResourceListener::onUnloadEnd
        void onUnloadEnd(Resource * prog)
        {
            mRecreateParam = true;
        }

        /// @copydetails ResourceListener::onLoadEnd
        void onLoadEnd(Resource * prog)
        {
            if(mRecreateParam)
                recreateParameters();
        }
    protected:
        void recreateParameters()
        {
            GpuProgramParam * old = mParam;
            mParam = mProgram->createParam();

            if(old != 0)
            {
                mParam->copyDefineValue(*old);
                if (mLinkParam != true)
                {
                    N_delete old;
                }
            }
            mLinkParam = false;
            mRecreateParam = false;
        }
    public:
        ShaderCh * mParent;
        ShaderChProgram::ShaderType mType;
        GpuProgram * mProgram;
        GpuProgramParam * mParam;
        bool mLinkParam;
        bool mRecreateParam;
    };
    //------------------------------------------------------------------------
    ShaderChProgram::ShaderChProgram(ShaderCh * ch) :
        mParent(ch)
    {
        memset(mMark, 0, ShaderChProgram::ST_Count);
    }
    //------------------------------------------------------------------------
    ShaderChProgram::ShaderChProgram(ShaderCh * ch, const ShaderChProgram & o):
        mParent(ch)
    {
        ShaderChShader * temp;
        Shaders::iterator i, iend = mShaders.end();
        for(i = mShaders.begin(); i != iend; ++i)
        {
            N_delete *i;
        }
        mShaders.clear();

        Shaders::const_iterator i2, iend2 = o.mShaders.end();
        for(i2 = o.mShaders.begin(); i2 != iend2; ++i2)
        {
            temp = N_new ShaderChShader(mParent, **i2);
            mShaders.push_back(temp);
        }

        memcpy(mMark, o.mMark, ShaderChProgram::ST_Count);

        mParent->notify();
    }
    //------------------------------------------------------------------------
    ShaderChProgram::~ShaderChProgram()
    {
        memset(mMark, 0, ShaderChProgram::ST_Count);
    }
    //------------------------------------------------------------------------
    ShaderChProgram & ShaderChProgram::operator =(const ShaderChProgram & o)
    {
        ShaderChShader * temp;
        Shaders::const_iterator i, iend = mShaders.end();
        for(i = mShaders.begin(); i != iend; ++i)
        {
            N_delete *i;
        }
        mShaders.clear();

        iend = o.mShaders.end();
        for(i = o.mShaders.begin(); i != iend; ++i)
        {
            temp = N_new ShaderChShader(mParent, **i);
            mShaders.push_back(temp);
        }

        memcpy(mMark, o.mMark, ShaderChProgram::ST_Count);

        mParent->notify();
        return *this;
    }
    //------------------------------------------------------------------------
    bool ShaderChProgram::isExist(ShaderChProgram::ShaderType type)const
    {
        if(type > ST_Count)
            return false;
        else if(type == ST_Count)
            return mMark[ST_VS] + mMark[ST_FS] + mMark[ST_GS] + mMark[ST_TS] + mMark[ST_DS] > 0;
        return mMark[type] != 0;
    }
    //-----------------------------------------------------------------------
    void ShaderChProgram::add(ShaderChProgram::ShaderType type, GpuProgramID gpid,
        GpuProgramParam * param)
    {
        ShaderChShader * temp = N_new ShaderChShader(type, mParent);
        if(param == 0)
        {
            temp->setProgramName(gpid, true);
        }
        else
        {
            temp->setProgramName(gpid, false);
            temp->mParam = param;
            temp->mLinkParam = true;

        }
        NCount count = 0;
        for(NIIi c = type; c >= 0; --c)
        {
            count += mMark[c];
        }
        Shaders::iterator i = mShaders.begin();
        std::advance(i, count);

        if(i == mShaders.end())
            mShaders.push_back(temp);
        else
            mShaders.insert(i, temp);
        ++mMark[type];

        mParent->notify();
    }
    //-----------------------------------------------------------------------
    void ShaderChProgram::remove(ShaderChProgram::ShaderType type)
    {
        N_lock_mutex(mGpuProgramChangeMutex)
        if(mMark[type] != 0)
        {
            NCount count2, count1 = 0;
            for(NIIi c = type; c >= 0; --c)
            {
                count1 += mMark[c];
            }
            count2 = count1 + mMark[type];

            Shaders::iterator i, iend = mShaders.begin() + count2;
            for(i = mShaders.begin() + count1; i != iend; ++i)
            {
                N_delete *i;
            }
            mShaders.erase(mShaders.begin() + count1, mShaders.begin() + 2);

            mMark[type] = 0;

            mParent->notify();
        }
    }
    //-----------------------------------------------------------------------
    void ShaderChProgram::remove(ShaderChProgram::ShaderType type, GpuProgramID gpid)
    {
        N_lock_mutex(mGpuProgramChangeMutex)
        if(mMark[type] != 0)
        {
            NCount count2, count1 = 0;
            for(NIIi c = type; c >= 0; --c)
            {
                count1 += mMark[c];
            }
            count2 = count1 + mMark[type];

            Shaders::iterator i = mShaders.begin();
            std::advance(i, count1);
            
            for(; count1 < count2; ++count1)
            {
                if((*i)->mProgram == 0 || (*i)->mProgram->getOriginID() == gpid)
                {
                    N_delete *i;
                    i = mShaders.erase(i);
                    --mMark[type];

                    mParent->notify();
                }
                else
                {
                    ++i;
                }
            }
        }
    }
    //-----------------------------------------------------------------------
    void ShaderChProgram::removeAll()
    {
        N_lock_mutex(mGpuProgramChangeMutex)
        Shaders::iterator i, iend = mShaders.end();
        for(i = mShaders.begin(); i != iend; ++i)
        {
            N_delete *i;
        }
        mShaders.clear();
        memset(mMark, 0, ShaderChProgram::ST_Count);

        mParent->notify();
    }
    //-----------------------------------------------------------------------
    void ShaderChProgram::get(ShaderType type, GpuProgram *& out1, GpuProgramParam *& out2, Nidx index) const
    {
        N_lock_mutex(mGpuProgramChangeMutex)
        if(mMark[type] != 0)
        {
            NCount count1 = 0;
            for(NIIi c = 0; c >= 0; --c)
            {
                count1 += mMark[c];
            }
            Shaders::const_iterator i = mShaders.begin();
            std::advance(i, count1 + index);

            if(i != mShaders.end())
            {
                if(&out1 != 0)
                    out1 = (*i)->mProgram;
                if(&out2 != 0)
                    out2 = (*i)->mParam;
                return;
            }
        }
        if(&out1 != 0)
            out1 = 0;
        if(&out2 != 0)
            out2 = 0;
    }
    //-----------------------------------------------------------------------
    void ShaderChProgram::get(ShaderType type, GpuProgram *& outProg) const
    {
        N_lock_mutex(mGpuProgramChangeMutex)
        if(mMark[type] != 0)
        {
            NCount count1 = 0;
            for(NIIi c = type; c >= 0; --c)
            {
                count1 += mMark[c];
            }
            Shaders::const_iterator i = mShaders.begin();
            std::advance(i, count1);
            if(i != mShaders.end())
            {
                if(&outProg != 0)
                    outProg = (*i)->mProgram;
            }
        }
        if(&outProg != 0)
            outProg = 0;
    }
    
    //-----------------------------------------------------------------------
    void ShaderChProgram::getParam(ShaderType type, GpuProgramParam *& outParam) const
    {
        N_lock_mutex(mGpuProgramChangeMutex)
        if (mMark[type] != 0)
        {
            NCount count1 = 0;
            for (NIIi c = type; c >= 0; --c)
            {
                count1 += mMark[c];
            }
            Shaders::const_iterator i = mShaders.begin();
            std::advance(i, count1);
            if (i != mShaders.end())
            {
                if(&outParam != 0)
                    outParam = (*i)->mParam;
            }
        }
        if(&outParam != 0)
            outParam = 0;
    }
    //-----------------------------------------------------------------------
    void ShaderChProgram::set(ShaderType type, GpuProgramID gpid, GpuProgramParam * param, Nidx index)
    {
        N_lock_mutex(mGpuProgramChangeMutex)
        N_assert(mMark[type] > index, "�±����");
        if(mMark[type] != 0)
        {
            NCount count1 = 0;
            for(NIIi c = type; c >= 0; --c)
            {
                count1 += mMark[c];
            }
            Shaders::iterator i = mShaders.begin();
            std::advance(i, count1 + index);
            if(i != mShaders.end())
            {
                if(gpid == 0 || (*i)->mProgram == 0)
                {
                    N_delete *i;
                    mShaders.erase(i);
                    --mMark[type];
                }
                else if((*i)->mProgram->getOriginID() != gpid)
                {
                    if(param == 0)
                    {
                        (*i)->setProgramName(gpid, true);
                    }
                    else
                    {
                        (*i)->setProgramName(gpid, false);
                        (*i)->mParam = param;
                        (*i)->mLinkParam = true;
                    }
                }
                // Needs recompilation
                mParent->notify();
            }
        }
    }
    //-----------------------------------------------------------------------
    void ShaderChProgram::setParam(ShaderType type, GpuProgramParam * param, Nidx index)
    {
        N_lock_mutex(mGpuProgramChangeMutex)
        N_assert(mMark[type] > index, "�±����");
        if (mMark[type] != 0)
        {
            NCount count1 = 0;
            for (NIIi c = type; c >= 0; --c)
            {
                count1 += mMark[c];
            }
            Shaders::iterator i = mShaders.begin();
            std::advance(i, count1 + index);
            if(i != mShaders.end())
            {
                (*i)->setParam(param, true);
                // Needs recompilation
                mParent->notify();
            }
        }
    }
    //-----------------------------------------------------------------------
    void ShaderChProgram::plan()
    {

    }
    //-----------------------------------------------------------------------
    void ShaderChProgram::unplan()
    {

    }
    //-----------------------------------------------------------------------
    void ShaderChProgram::load()
    {
        Shaders::iterator i, iend = mShaders.end();
        for(i = mShaders.begin(); i != iend; ++i)
        {
            (*i)->load();
        }
    }
    //-----------------------------------------------------------------------
    void ShaderChProgram::unload()
    {
        Shaders::iterator i, iend = mShaders.end();
        for(i = mShaders.begin(); i != iend; ++i)
        {
            (*i)->unload();
        }
    }
    //-----------------------------------------------------------------------
    void ShaderChProgram::_update(const SysSyncParam * src, Nmark mark)
    {
        Shaders::iterator i, iend = mShaders.end();
        for(i = mShaders.begin(); i != iend; ++i)
        {
            (*i)->mParam->sync(src, mark);
        }
    }
    //------------------------------------------------------------------------
}