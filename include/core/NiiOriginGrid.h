/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2016-5-7

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

#ifndef _NII_ORIGIN_GRID_H_
#define _NII_ORIGIN_GRID_H_

#include "NiiPreInclude.h"
#include "NiiRenderObj.h"

namespace NII
{
    /** ԭ������
    @remark �ⶫ���ܻ���,Ҳ�ܳ���,���е���ά�༭�����е�.һ��ƽ����x��,y����Ϊ0������
    @version NIIEngine 3.0.0
    */
    class _EngineAPI OriginGrid : public RenderObj
    {
    public:
        OriginGrid(NCount x, NCount y, Nmark bufmark);
        OriginGrid(SpaceID sid, NCount x, NCount y, Nmark bufmark);
        ~OriginGrid();

        /// @copydetails GeometryObj::distanceSquared
        NIIf distanceSquared(const Camera * cam) const;

        /// @copydetails SpaceObj::getCenterRange
        NIIf getCenterRange() const;
    protected:
        /// @copydetails GeometryObj::getMatrix
        void getMatrix(Matrix4f * out, NCount & count) const;
    private:
        void buildGeo(NCount x, NCount y, Nmark vBufUsage);
    protected:
        NCount mXGrid;
        NCount mYGrid;
    };
}
#endif