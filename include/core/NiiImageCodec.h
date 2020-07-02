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

#ifndef _NII_IMAGE_CODEC_H_
#define _NII_IMAGE_CODEC_H_

#include "NiiPreInclude.h"
#include "NiiCodec.h"
#include "NiiImage.h"

namespace NII
{
    /** ͼƬר�ñ������
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ImageCodec : public Codec
    {
    public:
        ImageCodec(CodecID cid, const String & hint);
        virtual ~ImageCodec();

        /** �������������е����ݽ���
        @param[in] in ͼƬ����
        @param[out] out ���Э��(����ֱ�����ļ���/����ͨ��),�鿴niiengine://Э��
        @param[in] hint ����������������Ŷ������,ָ����һ
        @version NIIEngine 3.0.0
        */
        virtual CodecResult encode(const Image * in, const String & out, const String & hint) const;

        /** �������������е����ݽ���,
        @param[in] in ͼƬ����
        @param[out] out ������,һ�����ڴ���
        @param[in] hint ����������������Ŷ������,ָ����һ(����ʹ��)
        @version NIIEngine 3.0.0
        */
        virtual CodecResult encode(const Image * in, DataStream *& out, const String & hint) const;

        /** �ӱ����ļ��������ݵ�ͼƬ����.
        @param[in] in ����Э��(����ֱ�����ļ���/����ͨ��),�鿴niiengine://Э��
        @param[out] out ���ͼƬ����
        @version NIIEngine 3.0.0
        */
        virtual CodecResult decode(const String & in, Image * out) const;

        /** �ӱ����ļ��������ݵ�ͼƬ����.
        @param[in] in ����niiengine://Э�������һ��ʵ��,������ʵ��
        @param[out] out ���ͼƬ����
        @version NIIEngine 3.0.0
        */
        virtual CodecResult decode(DataStream * in, Image * out) const;
    };
}

#endif