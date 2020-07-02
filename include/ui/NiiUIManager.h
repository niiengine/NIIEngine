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
#ifndef _NII_UI_MANAGER_H_
#define _NII_UI_MANAGER_H_

#include "NiiUIPreInclude.h"
#include "NiiCommandObj.h"
#include "NiiGeometryPixel.h"
#include "NiiMatrix4.h"
#include "NiiSingleton.h"

using namespace NII::UI;

namespace NII
{
    /** UI��Ԫ������
    @remark ����Ӧ�ó�������UI��Ԫ����
    @version NIIEngine 3.0.0
    */
    class _EngineAPI UIManager : public NII_COMMAND::CommandObj, public Singleton<UIManager>,
        public UIAlloc
    {
        friend class Engine;
    public:
        /** ͬ����Ⱦϵͳ״̬
        @version NIIEngine 3.0.0
        */
        void syn() const;

        /** ִ����Ⱦ
        @remark ��Ⱦ���д����ڻ������UI��Ԫ����
        @version NIIEngine 3.0.0
        */
        void render();

        /** ���û��ģʽ
        @remark UIͼ��һ���ǿɼ���
        @param[in] mode ģʽ
        @param[in] force �Ƿ�ǿ��
        @version NIIEngine 3.0.0
        */
        void apply(PixelBlendMode mode, bool force = false);

        /** �����Ƿ�����UI��Ⱦ
        @version NIIEngine 3.0.0
        */
        void setEnable(bool b);

        /** ��ȡ�Ƿ�����UI��Ⱦ
        @version NIIEngine 3.0.0
        */
        bool isEnable() const;

        /** �����α��ʼλ��
        @version NIIEngine 3.0.0
        */
        void setCursorInitPos(const Vector2f & pos);  
        
        /** ��ȡ�α��ʼλ��
        @version NIIEngine 3.0.0
        */
        const Vector2f & getCursorInitPos() const;
        
        /** ����UI����
        @param[out] out ����������
        @version NIIEngine 3.0.0
        */
        UISheet * create(RenderSys * rsys, ViewWindow * win);

        /** ɾ��UI����
        @param[in] out ��Ҫɾ����
        @version NIIEngine 3.0.0
        */
        void destroy(UISheet * obj);

        /** ������Ⱦ��
        @param[in] obj ��Ⱦ�Ӵ�
        @version NIIEngine 3.0.0
        */
        void setWindow(UISheet * dest, ViewWindow * obj);

        /** ��ȡ��Ⱦ��
        @param[in] obj ��Ⱦ�Ӵ�
        @version NIIEngine 3.0.0
        */
        void getWindow(UISheet * dest, ViewWindow *& obj);

        /** ��ȡ����Ļ���
        @param[in] dst
        @version NIIEngine 3.0.0
        */
        UISheet * getSheet(ViewWindow * dst) const;

        /** ��ȡ�����廭��
        @return
            �ܿ���ֻ����һ������,��ʱ������������ʹ��������廭�̵�,�ڶ�������Ϸ�豸
            ���������ϷӦ�ó����кܳ���,����Ǳ༭�����Ӧ�ó�����ʹ�� getSheet ��
            ��
        @version NIIEngine 3.0.0
        */
        UISheet * getMainSheet() const;

        /** ������ʾ���ش�С
        @version NIIEngine 3.0.0
        */
        void setDisplaySize(ViewWindow * dst, const PlaneSizei & size);

        /** ��ȡ�Ӵ���ʾ���ش�С
        @param[in] dst �Ӵ�
        @version NIIEngine 3.0.0
        */
        PlaneSizei getDisplaySize(Nid id) const;

        /** ��ȡ�Ӵ���ʾ���ش�С
        @param[in] dst �Ӵ�
        @version NIIEngine 3.0.0
        */
        PlaneSizei getDisplaySize(ViewWindow * dst) const;

        /** ��ɫ����ģʽ
        @version NIIEngine 3.0.0
        */
        void setShaderEnable(bool b);

        /** ��ɫ����ģʽ
        @version NIIEngine 3.0.0
        */
        bool isShaderEnable() const;

        /** ����ͶӰ����
        @version NIIEngine 3.0.0 �߼�api
        */
        void setProjMatrix(const Matrix4f & m);

        /** ��ȡͶӰ����
        @version NIIEngine 3.0.0
        */
        const Matrix4f & getProjMatrix() const;

        /** ������ͼ����
        @version NIIEngine 3.0.0 �߼�api
        */
        void setViewMatrix(const Matrix4f & m);

        /** ��ȡ��ͼ����
        @version NIIEngine 3.0.0
        */
        const Matrix4f & getViewMatrix() const;

        /** ��ȡģ�;���
        @version NIIEngine 3.0.0 �߼�api
        */
        void setModelMatrix(const Matrix4f & m);

        /** ��ȡģ�;���
        @version NIIEngine 3.0.0
        */
        const Matrix4f & getModelMatrix() const;

        /** ��ȡ�ռ����
        @version NIIEngine 3.0.0
        */
        const Matrix4f & getSpaceMatrix() const;

        /** �����ı���ͼ
        @version NIIEngine 3.0.0
        */
        void setView(TextView * obj);

        /** ��ȡ�ı���ͼ
        @version NIIEngine 3.0.0
        */
        TextView * getView() const;

        /// @copydetails Singleton::getOnly
        static UIManager & getOnly();

        /// @copydetails Singleton::getOnlyPtr
        static UIManager * getOnlyPtr();
    public:
        /** ����ͼ�ߴ�ı�ʱ
        @version NIIEngine 3.0.0
        */
        void onViewSize(ViewWindow * dst, const PlaneSizei & size);
    protected:
        UIManager(RenderSys * sys, ViewWindow * primary);
        ~UIManager();
    public:
        /** ������Ԫʱ
        @version NIIEngine 3.0.0
        */
        void onCreate(const Widget * obj);

        /** ɾ����Ԫʱ
        @version NIIEngine 3.0.0
        */
        void onDestroy(const Widget * obj);

        /** ��ͼ�ߴ�ı��¼�
        @version NIIEngine 3.0.0
        */
        static const EventID ViewSizeChangeEvent;

        /** �ı���ͼ�ı��¼�
        @version NIIEngine 3.0.0
        */
        static const EventID TextViewChangeEvent;

        /** �¼�����
        @version NIIEngine 3.0.0
        */
        static const EventID EventCount;

        NormalTextView * TextView0;        

        EffectTextView * TextView1;
    protected:
        typedef map<UISheet *, ViewWindow *>::type SheetList;
    protected:
        RenderSys * mSys;
        SheetList mSheetList;
        UISheet * mMainSheet;
        Viewport * mPreVP;
        Matrix4f mPreProjMatrix;
        TextView * mTextView;
        PixelBlendMode mBlendMode;
        Vector2f mCursorInitPos;
        Matrix4f mViewMatrix;           ///< ��ͼ����
        Matrix4f mProjMatrix;           ///< ͶӰ����
        Matrix4f mModelMatrix;          ///< ģ�;���
        Matrix4f mSpaceMatrix;          ///< �ռ����
        HighLevelGpuProgram * mVP;
        HighLevelGpuProgram * mFP;
        GpuProgramParam * mVPParams;
        GpuProgramParam * mFPParams;

        bool mValidSpaceMatrix;
        bool mAloneFrame;
        bool mShaderEnable;
        bool mGLSLEnable;
        bool mEnable;
    };
}
#endif