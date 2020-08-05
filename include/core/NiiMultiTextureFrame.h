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

#ifndef _NII_MultiTextureFrame_H_
#define _NII_MultiTextureFrame_H_

#include "NiiPreInclude.h"
#include "NiiFrameObj.h"

namespace NII
{
    /** ֡������������
    @note ���������ʽ������ͬ
    @version NIIEngine 3.0.0
    */
    class _EngineAPI MultiTextureFrame : public FrameObj
    {
    public:
        typedef vector<TextureFrame *>::type AttachList;
    public:
        MultiTextureFrame(const String & name);

        /** ����
        @param[in] index ����
        @param[in] obj ����
        @version NIIEngine 3.0.0
        */
        virtual void attach(Nidx index, TextureFrame * obj);

        /** ���
        @param[in] index ����
        @version NIIEngine 3.0.0
        */
        virtual void detach(Nidx index);

        /** ��ȡ
        @param[in] index ����
        @version NIIEngine 3.0.0
        */
        TextureFrame * getAttach(NCount index);

        /** ��ȡ�б�
        @version NIIEngine 3.0.0
        */
        const AttachList & getAttachList() const;

        /// @copydetails FrameObj::fill
        virtual void fill(FrameObj::FaceType buffer, const Box & src, PixelBlock & dst);

        /// @copydetails FrameObj::getFormat
        PixelFormat getFormat() const;
    protected:
        /** ʵ��
        @version NIIEngine 3.0.0 �߼�api
        */
        virtual void attachImpl(Nidx index, TextureFrame * obj) = 0;

        /** ʵ��
        @version NIIEngine 3.0.0 �߼�api
        */
        virtual void detachImpl(Nidx index) = 0;
    protected:
        AttachList mAttachList;
    };
}

#endif