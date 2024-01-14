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