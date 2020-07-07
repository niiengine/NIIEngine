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

#ifndef _NII_SHADER_CH_PROGRAM_H_
#define _NII_SHADER_CH_PROGRAM_H_

#include "NiiPreInclude.h"
#include "NiiGpuProgram.h"

namespace NII
{
    class ShaderChShader;

    /** Ӧ����ɫ����
    @note ��������,������������Ⱦͨ·���л�
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ShaderChProgram : public ShaderAlloc
    {
    public:
        /** ��ɫ��������
        @see ע��
        @version NIIEngine 3.0.0
        */
        enum ShaderType
        {
            ST_VS = 0,       ///< ����(��Ⱦ���ʱ)
            ST_FS = 1,       ///< Ƭ��(��Ⱦ���ʱ)
            ST_GS = 2,       ///< ����(��Ⱦ���ʱ)
            ST_VS_SR = 3,    ///< ������Ӱ����(��Ⱦʱ����չ)
            ST_FS_SR = 4,    ///< Ƭ����Ӱ����(��Ⱦʱ����չ)
            ST_VS_SC = 5,    ///< ����Ͷ����Ӱ(��Ⱦʱ����չ)
            ST_FS_SC = 6,    ///< Ƭ��Ͷ����Ӱ(��Ⱦʱ����չ)
            ST_FS_N = 7,     ///< ����Ƭ��(����������)(��Ⱦʱ����չ)
            ST_FS_F = 8,     ///< ����Ƭ��(��Ӱ)(��Ⱦʱ����չ)
            ST_Count = 9
        };
    public:
        ShaderChProgram(ShaderCh * ch);
        ShaderChProgram(ShaderCh * ch, const ShaderChProgram & o);
        ShaderChProgram & operator =(const ShaderChProgram & o);
        ~ShaderChProgram();

        /** �Ƿ������ɫ����
        @version NIIEngine 3.0.0
        */
        bool isExist() const;

        /** �Ƿ���ڴ����͵���ɫ����
        @param[in] type ֻ�ܵ���,�������
        @version NIIEngine 3.0.0
        */
        bool isExist(ShaderType type) const;

        /** ������ɫ���򵽶�����
        @remark ��Щ����ᰴ����ʱ��˳������,ע��˳��
        @param[in] type ��������
        @param[in] gpid ��ԴID
        @param[in] param �������
        @version NIIEngine 3.0.0
        */
        void add(ShaderType type, GpuProgramID gpid, GpuProgramParam * param = 0);

        /** ��ȥָ�����͵�������ɫ����
        @param[in] type ��������
        @version NIIEngine 3.0.0
        */
        void remove(ShaderType type);

        /** ��ȥָ�����͵�ID��ɫ����
        @param[in] type ��������
        @param[in] gpid ��ԴID
        @version NIIEngine 3.0.0
        */
        void remove(ShaderType type, GpuProgramID gpid);

        /** ��ȥ�������͵�������ɫ����
        @version NIIEngine 3.0.0
        */
        void removeAll();

        /** ���ò���
        @param[in] type
        @param[in] index
        @param[in] gpid
        @param[in] param
        @version NIIEngine 3.0.0
        */
        void set(ShaderType type, GpuProgramID id, GpuProgramParam * param = 0, Nidx index = 0);

        /** ���ò���
        @param[in] type
        @param[in] index
        @param[in] gpid
        @param[in] param
        @version NIIEngine 3.0.0
        */
        void setParam(ShaderType type, GpuProgramParam * param, Nidx index = 0);

        /** ��ȡ��������
        @param[in] index
        @param[in] type
        @param[in] out1
        @param[in] out2
        @version NIIEngine 3.0.0
        */
        void get(ShaderType type, GpuProgram * & outID, GpuProgramParam *& outParam, Nidx index = 0) const;
        
        /** ��ȡ��������
        @param[in] index
        @param[in] type
        @param[in] out
        @version NIIEngine 3.0.0
        */
        GpuProgram * getProgram(ShaderType type, Nidx index = 0) const;

        /** ��ȡ��������
        @param[in] index
        @param[in] type
        @param[in] out1
        @param[in] out2
        @version NIIEngine 3.0.0
        */
        GpuProgramParam * getParam(ShaderType type, Nidx index = 0) const;

        /** �ֲ������߳�
        @version NIIEngine 3.0.0
        */
        void plan();

        /** �ֲ������߳�
        @version NIIEngine 3.0.0
        */
        void unplan();

        /** �ֲ������߳�
        @version NIIEngine 3.0.0
        */
        void load();

        /** �ֲ������߳�
        @version NIIEngine 3.0.0
        */
        void unload();

        /** �Զ����²���
        @param[in] src
        @param[in] mark
        @version NIIEngine 3.0.0
        */
        void _update(const SysSyncParam * src, Nmark mark);
    private:
        typedef vector<ShaderChShader *>::type Shaders;
    private:
        ShaderCh * mParent;
        Shaders mShaders;
        Nui8 mMark[ST_Count];
    };
}
#endif