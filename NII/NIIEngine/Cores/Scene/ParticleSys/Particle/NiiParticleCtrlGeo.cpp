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
#include "NiiParticleCtrlGeo.h"
#include "NiiRenderQueue.h"
#include "NiiGeometryObj.h"
#include "NiiSyncParticle.h"
#include "NiiStrConv.h"

namespace NII
{
    //------------------------------------------------------------------------
    class _EngineInner CmdBillboardType : public PropertyCmd
    {
    public:
        CmdBillboardType() : PropertyCmd(N_PropertyParticleCtrlGeo_CtrlMode, _T("billboard_type"),
            _T("The type of billboard to use. 'point' means a simulated spherical particle, "
            "'oriented_common' means all particles in the set are oriented around common_direction, "
            "'oriented_self' means particles are oriented around their own direction, "
            "'perpendicular_common' means all particles are perpendicular to common_direction, "
            "and 'perpendicular_self' means particles are perpendicular to their own direction."))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            ParticleCtrl::CtrlMode t = static_cast<const ParticleCtrlGeo *>(own)->getCtrlMode();
            switch(t)
            {
            case ParticleCtrl::CM_Parallel_View:
                out = _T("point");
                break;
            case ParticleCtrl::CM_Parallel_Ctrl:
                out = _T("oriented_common");
                break;
            case ParticleCtrl::CM_Parallel_Self:
                out = _T("oriented_self");
                break;
            case ParticleCtrl::CM_Vertical_Ctrl:
                out = _T("perpendicular_common");
            case ParticleCtrl::CM_Vertical_Self:
                out = _T("perpendicular_self");
                break;
            default:
                out = _T("point");
                break;
            }
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            ParticleCtrl::CtrlMode t;
            if(val == _T("point"))
            {
                t = ParticleCtrl::CM_Parallel_View;
            }
            else if(val == _T("oriented_common"))
            {
                t = ParticleCtrl::CM_Parallel_Ctrl;
            }
            else if(val == _T("oriented_self"))
            {
                t = ParticleCtrl::CM_Parallel_Self;
            }
            else if(val == _T("perpendicular_common"))
            {
                t = ParticleCtrl::CM_Vertical_Ctrl;
            }
            else if(val == _T("perpendicular_self"))
            {
                t = ParticleCtrl::CM_Vertical_Self;
            }
            else
            {
                t = ParticleCtrl::CM_Parallel_View;
            }
            static_cast<ParticleCtrlGeo *>(own)->setCtrlMode(t);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            ParticleCtrl::CtrlMode * t = (ParticleCtrl::CtrlMode *)out;
            *t = static_cast<const ParticleCtrlGeo *>(own)->getCtrlMode();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const void * in)
        {
            ParticleCtrl::CtrlMode * t = (ParticleCtrl::CtrlMode *)in;
            static_cast<ParticleCtrlGeo *>(own)->setCtrlMode(*t);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdBillboardType();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T("point");
        }
    };

    class _EngineInner CmdBillboardOrigin : public PropertyCmd
    {
    public:
        CmdBillboardOrigin() : PropertyCmd(N_PropertyParticleCtrlGeo_OriginType, _T("billboard_origin"),
        _T("This setting controls the fine tuning of where a billboard appears in relation to it's position. "
        "Possible value are: 'top_left', 'top_center', 'top_right', 'center_left', 'center', 'center_right', "
        "'bottom_left', 'bottom_center' and 'bottom_right'. Default value is 'center'."))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            ParticleCtrl::OriginType o = static_cast<const ParticleCtrlGeo *>(own)->getOriginType();
            switch (o)
            {
            case ParticleCtrl::OT_Top_Left:
                out = _T("top_left");
                break;
            case ParticleCtrl::OT_Top_Center:
                out = _T("top_center");
                break;
            case ParticleCtrl::OT_Top_Right:
                out = _T("top_right");
                break;
            case ParticleCtrl::OT_Center_Left:
                out = _T("center_left");
                break;
            case ParticleCtrl::OT_Center:
                out = _T("center");
                break;
            case ParticleCtrl::OT_Center_Right:
                out = _T("center_right");
                break;
            case ParticleCtrl::OT_Bottom_Left:
                out = _T("bottom_left");
                break;
            case ParticleCtrl::OT_Bottom_Center:
                out = _T("bottom_center");
                break;
            case ParticleCtrl::OT_Bottom_Right:
                out = _T("bottom_right");
                break;
            default:
                out = _T("center");
                break;
            }
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            ParticleCtrl::OriginType o;
            if(val == _T("top_left"))
                o = ParticleCtrl::OT_Top_Left;
            else if(val == _T("top_center"))
                o = ParticleCtrl::OT_Top_Center;
            else if(val == _T("top_right"))
                o = ParticleCtrl::OT_Top_Right;
            else if(val == _T("center_left"))
                o = ParticleCtrl::OT_Center_Left;
            else if(val == _T("center"))
                o = ParticleCtrl::OT_Center;
            else if(val == _T("center_right"))
                o = ParticleCtrl::OT_Center_Right;
            else if(val == _T("bottom_left"))
                o = ParticleCtrl::OT_Bottom_Left;
            else if(val == _T("bottom_center"))
                o = ParticleCtrl::OT_Bottom_Center;
            else if(val == _T("bottom_right"))
                o = ParticleCtrl::OT_Bottom_Right;
            else
            {
                o = ParticleCtrl::OT_Center;
            }

            static_cast<ParticleCtrlGeo *>(own)->setOriginType(o);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            ParticleCtrl::OriginType * temp = (ParticleCtrl::OriginType *)out;
            *temp = static_cast<const ParticleCtrlGeo *>(own)->getOriginType();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const void * in)
        {
            ParticleCtrl::OriginType * temp = (ParticleCtrl::OriginType *)in;
            static_cast<ParticleCtrlGeo *>(own)->setOriginType(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdBillboardOrigin();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T("center");
        }
    };

    class _EngineInner CmdBillboardRotationType : public PropertyCmd
    {
    public:
        CmdBillboardRotationType() : PropertyCmd(N_PropertyParticleCtrlGeo_RotationMode, _T("billboard_rotation_type"),
        _T("This setting controls the billboard rotation type. "
        "'vertex' means rotate the billboard's vertices around their facing direction."
        "'texcoord' means rotate the billboard's texture coordinates. Default value is 'texcoord'."))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            ParticleCtrl::RotationMode r = static_cast<const ParticleCtrlGeo *>(own)->getRotationMode();
            switch(r)
            {
            case ParticleCtrl::RM_VertexPos:
                out = _T("vertex");
                break;
            case ParticleCtrl::RM_TexCoord:
                out = _T("texcoord");
                break;
            default:
                out = _T("vertex");
                break;
            }
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            ParticleCtrl::RotationMode r;
            if(val == _T("vertex"))
                r = ParticleCtrl::RM_VertexPos;
            else if(val == _T("texcoord"))
                r = ParticleCtrl::RM_TexCoord;
            else
            {
                r = ParticleCtrl::RM_VertexPos;
            }

            static_cast<ParticleCtrlGeo *>(own)->setRotationMode(r);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            ParticleCtrl::RotationMode * temp = (ParticleCtrl::RotationMode *)out;
            *temp = static_cast<const ParticleCtrlGeo *>(own)->getRotationMode();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const void * in)
        {
            ParticleCtrl::RotationMode * temp = (ParticleCtrl::RotationMode *)in;
            static_cast<ParticleCtrlGeo *>(own)->setRotationMode(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdBillboardRotationType();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T("texcoord");
        }
    };

    class _EngineInner CmdCommonDirection : public PropertyCmd
    {
    public:
        CmdCommonDirection() : PropertyCmd(N_PropertyParticleCtrlGeo_CtrlDirection, _T("common_direction"),
            _T("Only useful when billboard_type is oriented_common or perpendicular_common. "
        "When billboard_type is oriented_common, this parameter sets the common orientation for "
        "all particles in the set (e.g. raindrops may all be oriented downwards). "
        "When billboard_type is perpendicular_common, this parameter sets the perpendicular vector for "
        "all particles in the set (e.g. an aureola around the player and parallel to the ground)."))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const ParticleCtrlGeo *>(own)->getCtrlDirection(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            Vector3f temp;
            StrConv::conv(val, temp);
            static_cast<ParticleCtrlGeo *>(own)->setCtrlDirection(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            Vector3f * temp = (Vector3f *)out;
            *temp = static_cast<const ParticleCtrlGeo *>(own)->getCtrlDirection();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const void * in)
        {
            Vector3f * temp = (Vector3f *)in;
            static_cast<ParticleCtrlGeo *>(own)->setCtrlDirection(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdCommonDirection();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv(Vector3f::Z, out);
        }
    };

    class _EngineInner CmdCommonUpVector : public PropertyCmd
    {
    public:
        CmdCommonUpVector() : PropertyCmd(N_PropertyParticleCtrlGeo_CtrlUp, _T("common_up_vector"),
            _T("Only useful when billboard_type is perpendicular_self or perpendicular_common. This "
        "parameter sets the common up-vector for all particles in the set (e.g. an aureola around "
        "the player and parallel to the ground)."))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const ParticleCtrlGeo *>(own)->getCtrlUp(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            Vector3f temp;
            StrConv::conv(val, temp);
            static_cast<ParticleCtrlGeo *>(own)->setCtrlUp(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            Vector3f * temp = (Vector3f *)out;
            *temp = static_cast<const ParticleCtrlGeo *>(own)->getCtrlUp();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const void * in)
        {
            Vector3f * temp = (Vector3f *)in;
            static_cast<ParticleCtrlGeo *>(own)->setCtrlUp(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdCommonUpVector();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv(Vector3f::Y, out);
        }
    };
    /// Command object for point rendering (see PropertyCmd).
    class _EngineInner CmdPointRendering : public PropertyCmd
    {
    public:
        CmdPointRendering() : PropertyCmd(N_PropertyParticleCtrlGeo_PointSprites, _T("point_rendering"),
            _T("Set whether or not particles will use point rendering "
        "rather than manually generated quads. This allows for faster "
        "rendering of point-oriented particles although introduces some "
        "limitations too such as requiring a common particle size."
        "Possible values are 'true' or 'false'."))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const ParticleCtrlGeo*>(own)->isPointSprites(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            bool temp;
            StrConv::conv(val, temp);
            static_cast<ParticleCtrlGeo*>(own)->setPointSprites(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            bool * temp = (bool *)out;
            *temp = static_cast<const ParticleCtrlGeo*>(own)->isPointSprites();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const void * in)
        {
            bool * temp = (bool *)in;
            static_cast<ParticleCtrlGeo*>(own)->setPointSprites(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdPointRendering();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv(false, out);
        }
    };
    /// Command object for accurate facing(see PropertyCmd).
    class _EngineInner CmdAccurateFacing : public PropertyCmd
    {
    public:
        CmdAccurateFacing() : PropertyCmd(N_PropertyParticleCtrlGeo_ParallelView, _T("accurate_facing"),
            _T("Set whether or not particles will be oriented to the camera "
        "based on the relative position to the camera rather than just "
        "the camera direction. This is more accurate but less optimal. "
        "Cannot be combined with point rendering."))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const ParticleCtrlGeo *>(own)->isParallelView(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            bool temp;
            StrConv::conv(val, temp);
            static_cast<ParticleCtrlGeo *>(own)->setParallelView(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            bool * temp = (bool *)out;
            *temp = static_cast<const ParticleCtrlGeo *>(own)->isParallelView();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const void * in)
        {
            bool * temp = (bool *)in;
            static_cast<ParticleCtrlGeo *>(own)->setParallelView(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdAccurateFacing();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv(false, out);
        }
    };
    //-----------------------------------------------------------------------
    ParticleCtrlGeo::ParticleCtrlGeo(LangID lid) :
        ParticleGeo(N_CmdObj_ParticleCtrlGeo, lid)
    {
        setupCmd(N_CmdObj_ParticleCtrlGeo);

        // Create billboard set
        mCtrl = N_new ParticleCtrl(0, 0, true);
        // World-relative axes
        mCtrl->setLocalSpace(false);
    }
    //-----------------------------------------------------------------------
    ParticleCtrlGeo::~ParticleCtrlGeo()
    {
        // mCtrl is never actually attached to a node, we just passthrough
        // based on the particle system's attachment. So manually notify that it's
        // no longer attached.
        mCtrl->attach((PosNode *)0);
        N_delete mCtrl;
        mCtrl = 0;
    }
    //-----------------------------------------------------------------------
    bool ParticleCtrlGeo::initCmd(PropertyCmdSet * dest)
    {
        dest->add(N_new CmdBillboardType());
        dest->add(N_new CmdBillboardOrigin());
        dest->add(N_new CmdBillboardRotationType());
        dest->add(N_new CmdCommonDirection());
        dest->add(N_new CmdCommonUpVector());
        dest->add(N_new CmdPointRendering());
        dest->add(N_new CmdAccurateFacing());
        return true;
    }
    //-----------------------------------------------------------------------
    FactoryID ParticleCtrlGeo::getType() const
    {
        return N_FACTORY_ParticleCtrl;
    }
    //-----------------------------------------------------------------------
    void ParticleCtrlGeo::queue(RenderQueue * queue, ParticleList & plist)
    {
        mCtrl->onBeginRender(plist.size());
        SyncParticle bb;
        for(list<Particle *>::type::iterator i = plist.begin(); i != plist.end(); ++i)
        {
            Particle * p = *i;
            bb.mPosition = p->mPosition;
            if(mCtrl->getCtrlMode() == ParticleCtrl::CM_Parallel_Self ||
                mCtrl->getCtrlMode() == ParticleCtrl::CM_Vertical_Self)
            {
                // Normalise direction vector
                bb.mDirection = p->mDirection;
                bb.mDirection.normalise();
            }
            bb.mColour = p->mColour;
            bb.mRotation = p->getRotation();
            // Assign and compare at the same time
            if((bb.mSizeValid = !p->isInheritSize()) == true)
            {
                bb.mWidth = p->getWidth();
                bb.mHeight = p->getHeight();
            }
            mCtrl->onRender(bb);
        }
        mCtrl->onEndRender();

        mCtrl->queue(queue);
    }
    //---------------------------------------------------------------------
    void ParticleCtrlGeo::query(GeometryQuery * query)
    {
        mCtrl->query(query);
    }
    //-----------------------------------------------------------------------
    void ParticleCtrlGeo::setMaterial(ResourceID rid, GroupID gid)
    {
        mCtrl->setMaterial(rid, gid);
    }
    //-----------------------------------------------------------------------
    void ParticleCtrlGeo::_notify(Camera * cam)
    {
        mCtrl->_notify(cam);
    }
    //-----------------------------------------------------------------------
    void ParticleCtrlGeo::attach(PosNode * node, bool b)
    {
        mCtrl->attach(node, b);
    }
    //-----------------------------------------------------------------------
    void ParticleCtrlGeo::onParticleRotate()
    {
        mCtrl->onParticleRotate();
    }
    //-----------------------------------------------------------------------
    void ParticleCtrlGeo::onCtrlResize(NIIf width, NIIf height)
    {
        mCtrl->setParticleWidth(width);
        mCtrl->setParticleHeight(height);
    }
    //-----------------------------------------------------------------------
    void ParticleCtrlGeo::onParticleResize()
    {
        mCtrl->onParticleSize();
    }
    //-----------------------------------------------------------------------
    void ParticleCtrlGeo::onPoolSizeChange(NCount quota)
    {
        mCtrl->setPoolSize(quota);
    }
    //-----------------------------------------------------------------------
    void ParticleCtrlGeo::setRenderGroup(Nui16 rgid)
    {
        N_assert(rgid <= RQG_Count, "Render queue out of range!");
        mCtrl->setRenderGroup(rgid);
    }
    //-----------------------------------------------------------------------
    void ParticleCtrlGeo::setLocalSpace(bool b)
    {
        mCtrl->setLocalSpace(b);
    }
    //-----------------------------------------------------------------------
    void ParticleCtrlGeo::setCullItself(bool b)
    {
        mCtrl->setCullItself(b);
    }
    //-----------------------------------------------------------------------
    ParticleCtrl::SortMode ParticleCtrlGeo::getSortMode() const
    {
        return mCtrl->getSortMode();
    }
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // ParticleCtrlGeoFactory
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    ParticleCtrlGeoFactory::ParticleCtrlGeoFactory():
        ParticleGeoFactory(N_FACTORY_ParticleCtrl)
    {
    }
    //-----------------------------------------------------------------------
    ParticleCtrlGeoFactory::~ParticleCtrlGeoFactory()
    {
    }
    //-----------------------------------------------------------------------
    Nmark ParticleCtrlGeoFactory::getVFSType() const
    {
        return 0;
    }
    //-----------------------------------------------------------------------
    FactoryID ParticleCtrlGeoFactory::getID() const
    {
        return N_FACTORY_ParticleCtrl;
    }
    //-----------------------------------------------------------------------
    void * ParticleCtrlGeoFactory::create(const String & file)
    {
        return N_new ParticleCtrlGeo();
    }
    //-----------------------------------------------------------------------
    void ParticleCtrlGeoFactory::destroy(void * obj)
    {
        ParticleCtrlGeo * temp = static_cast<ParticleCtrlGeo *>(obj);
        N_delete temp;
    }
    //-----------------------------------------------------------------------
}