/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2015-3-15

�ı�����: utf-8

������˾: �������ǿƼ����޹�˾

�������: ����������

��̷��: ͳ��ʽ

����ģʽ: �ֲ�ʽ

�ڲ��ɷ�: UI���� ������� ��Ƶ���� ������� �¼���������(��ɢ�����)

��Ҫ�ɷ�: c++(80%) c(20%)

��;: ����ϵͳ����(��Χ����ϵͳ�ں�api)
      ��άӦ�����
        �������������������(CAD)
        ������Ϣϵͳ���(GIS)
        ��Ӱ���������ع����
        ������Ϸ���

ƫ����;: ������Ϸ���

��ҳ: www.niiengine.com ��������: niiengine@gmail.com OR niiengine@163.com

��Ȩ��ʽ:��ҵ��Ȩ(www.niiengine.com/license)(3��)
------------------------------------------------------------------------------
*/

#ifndef _NII_IMAGE_CODEC_H_
#define _NII_IMAGE_CODEC_H_

#include "NiiPreInclude.h"
#include "NiiCodec.h"

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