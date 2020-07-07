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

#ifndef _NII_MATERIAL_LOD_SCHEME_H_
#define _NII_MATERIAL_LOD_SCHEME_H_

#include "NiiPreInclude.h"
#include "NiiLodScheme.h"

namespace NII
{
    /** 材质LOD
    @version NIIEngine 3.0.0
    */
    class _EngineAPI MaterialLodScheme : public LodScheme
    {
    public:
        typedef vector<std::pair<NIIf, Nui32> >::type LodList;
    public:
        MaterialLodScheme(SchemeID sid);
        virtual ~MaterialLodScheme();

        /// @copydetails LodScheme::getLodList
        void getLodList(SrcList & list) const;

        /// @copydetails LodScheme::add
        void add(NIIf src);

        /// @copydetails LodScheme::removeAll
        void removeAll();
        
        /// @copydetails LodScheme::map
        void map();

        /// @copydetails LodScheme::setValid
        void setValid(bool s) {}

        /// @copydetails LodScheme::isValid
        bool isValid() const { return true; }

        /** 获取列表
        @version NIIEngine 3.0.0
        */
        const LodList & getList() const;
    protected:
        LodList mLods;
    };

    /** 材质视图LOD策略
    @version NIIEngine 3.0.0
    */
    class _EngineInner MaterialViewLodScheme : public MaterialLodScheme
    {
    public:
        MaterialViewLodScheme(SchemeID sid);

        /// @copydetails LodScheme::getOrigin
        virtual NIIf getOrigin() const;

        /// @copydetails LodScheme::get
        Nidx get(Nui32 value) const;

        /// @copydetails LodScheme::sort
        void sort();

        /// @copydetails LodScheme::valid
        bool valid() const;

        /// @copydetails LodScheme::clone
        LodScheme * clone() const;
    protected:
        /// @copydetails LodScheme::getValueImpl
        NIIf getValueImpl(const SpaceObj * obj, const Camera * camera) const;
    };

    /** 摄象机距离材质LOD策略
    @version NIIEngine 3.0.0
    */
    class _EngineInner MaterialCameraLodScheme : public MaterialLodScheme
    {
    public:
        MaterialCameraLodScheme(SchemeID sid);

        /** 设置对比区域
        @param[in] w 视口的宽度
        @param[in] h 视口的高度
        @version NIIEngine 3.0.0
        */
        void setOriArea(NIIf w, NIIf h, Radian fovY);

        /** 设置是否对比区域
        @version NIIEngine 3.0.0
        */
        void setRefArea(bool b);

        /** 获取是否对比区域
        @version NIIEngine 3.0.0
        */
        bool isRefArea() const;

        /// @copydetails LodScheme::getDest
        Nui32 getDest(NIIf src) const;

        /// @copydetails LodScheme::getBias
        NIIf getBias(NIIf f) const;

        /// @copydetails LodScheme::get
        Nidx get(Nui32 value) const;

        /// @copydetails LodScheme::sort
        void sort();

        /// @copydetails LodScheme::valid
        bool valid() const;

        /// @copydetails LodScheme::clone
        LodScheme * clone() const;
    protected:
        /// @copydetails LodScheme::getValueImpl
        NIIf getValueImpl(const SpaceObj * obj, const Camera * camera) const;
    private:
        bool mRelation;
        NIIf mRefArea;
    };
}
#endif