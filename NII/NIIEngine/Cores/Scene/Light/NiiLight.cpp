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
#include "NiiLight.h"
#include "NiiDefine.h"
#include "NiiException.h"
#include "NiiSpaceNode.h"
#include "NiiCamera.h"
#include "NiiSpaceManager.h"
#include "NiiRenderPattern.h"
#include "NiiFusionData.h"

#define N_PropertyLight_diffuseColour           1
#define N_PropertyLight_specularColour          2
#define N_PropertyLight_attenuation             3
#define N_PropertyLight_spotlightInner          4
#define N_PropertyLight_spotlightOuter          5
#define N_PropertyLight_spotlightFalloff        6

namespace NII
{
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // LightFactory
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    LightFactory::LightFactory()
    {
    }
    //-----------------------------------------------------------------------
    LightFactory::~LightFactory()
    {
    }
    //-----------------------------------------------------------------------
    FactoryID LightFactory::getID() const
    {
        return N_FACTORY_Light;
    }
    //-----------------------------------------------------------------------
    SpaceObj * LightFactory::create(SpaceID sid, SpaceManager * mag, const PropertyData * params)
    {
        Light * light = N_new Light(sid);
        light->setFactory(this);
        light->setManager(mag);
        if (params)
        {
            const PropertyData::PropertyList & plist = params->getList();
            PropertyData::PropertyList::const_iterator ni;
            // Setting the light type first before any property specific to a certain light type
            if ((ni = plist.find(N_PropertyLight_Type)) != plist.end())
            {
                if (ni->second == _T("point"))
                    light->setType(LT_POINT);
                else if (ni->second == _T("directional"))
                    light->setType(LT_DIRECTIONAL);
                else if (ni->second == _T("spot"))
                    light->setType(LT_SPOT);
                else
                    N_EXCEPT(InvalidParam, _I18n("Invalid light type '") + ni->second + _T("'."));
            }

            // Common properties
            if ((ni = plist.find(N_PropertyLight_Position)) != plist.end())
            {
                Vector3f temp;
                StrConv::conv(ni->second, temp);
                light->setPos(temp);
            }

            if ((ni = plist.find(N_PropertyLight_Direction)) != plist.end())
            {
                Vector3f temp;
                StrConv::conv(ni->second, temp);
                light->setDirection(temp);
            }

            if ((ni = plist.find(N_PropertyLight_Diffuse)) != plist.end())
            {
                Colour temp;
                StrConv::conv(ni->second, temp);
                light->setDiffuse(temp);
            }

            if ((ni = plist.find(N_PropertyLight_Specular)) != plist.end())
            {
                Colour temp;
                StrConv::conv(ni->second, temp);
                light->setSpecular(temp);
            }
            if ((ni = plist.find(N_PropertyLight_Attenuation)) != plist.end())
            {
                Vector4f attenuation;
                StrConv::conv(ni->second, attenuation);
                light->setRange(attenuation.x);
                light->setAttenuationConstant(attenuation.y);
                light->setAttenuationLinear(attenuation.z);
                light->setAttenuationQuadric(attenuation.w);
            }

            if ((ni = plist.find(N_PropertyLight_CastShadows)) != plist.end())
            {
                bool temp;
                StrConv::conv(ni->second, temp);
                light->setCastShadow(temp);
            }

            if ((ni = plist.find(N_PropertyLight_Visible)) != plist.end())
            {
                bool temp;
                StrConv::conv(ni->second, temp);
                light->setVisible(temp);
            }

            NIIf temp;

            if ((ni = plist.find(N_PropertyLight_PowerScale)) != plist.end())
            {
                StrConv::conv(ni->second, temp);
                light->setBrightFactor(temp);
            }

            if ((ni = plist.find(N_PropertyLight_ShadowFarDistance)) != plist.end())
            {
                StrConv::conv(ni->second, temp);
                light->setShadowExtent(temp);
            }

            Radian temp2;

            // Spotlight properties
            if ((ni = plist.find(N_PropertyLight_SpotlightInner)) != plist.end())
            {
                StrConv::conv(ni->second, temp2);
                light->setSpotlightInner(temp2);
            }

            if ((ni = plist.find(N_PropertyLight_SpotlightOuter)) != plist.end())
            {
                StrConv::conv(ni->second, temp2);
                light->setSpotlightOuter(temp2);
            }
            if ((ni = plist.find(N_PropertyLight_SpotlightFalloff)) != plist.end())
            {
                StrConv::conv(ni->second, temp);
                light->setSpotlightFalloff(temp);
            }
        }

        return light;
    }
    //-----------------------------------------------------------------------
    void LightFactory::destroy(void * obj)
    {
        Light * temp = static_cast<Light *>(obj);
        N_delete temp;
    }
    //-----------------------------------------------------------------------
    //------------------------------------------------------------------------
    // LightDistanceLess
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    bool Light::LightDistanceLess::operator()(const Light * a, const Light * b) const
    {
        return a->mSortValue < b->mSortValue;
    }
    //-----------------------------------------------------------------------
    //------------------------------------------------------------------------
    // ValueDistanceLess
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    bool Light::ValueDistanceLess::operator()(const Light * a, NIIf b) const
    {
        return a->mSortValue < (b * b);
    }
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // Light
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    Light::Light() :
        mLightType(LT_POINT),
        mPosition(Vector3f::ZERO),
        mDiffuse(Colour::White),
        mSpecular(Colour::Black),
        mDirection(Vector3f::Z),
        mSpotOuter(Angle(40.0f)),
        mSpotInner(Angle(30.0f)),
        mSpotFalloff(1.0f),
        mStart(0.0f),
        mEnd(100000),
        mAttenuationConst(1.0f),
        mAttenuationLinear(0.0f),
        mAttenuationQuad(0.0f),
        mBrightFactor(1.0f),
        mIndexInFrame(0),
        mShadowExtent(0),
        mShadowNearClip(-1),
        mShadowFarClip(-1),
        mSpacePos(Vector3f::ZERO),
        mSpaceDirection(Vector3f::Z),
        mRelSpacePos(Vector3f::ZERO),
        mShadowCamera(0)
    {
        //mPixelLimit should always be zero for lights otherwise lights will disapear
        mPixelLimit = 0;
        mMove = false;
        mComMark = 0;
    }
    //-----------------------------------------------------------------------
    Light::Light(SpaceID sid) :
        SpaceObj(sid),
        mLightType(LT_POINT),
        mPosition(Vector3f::ZERO),
        mDiffuse(Colour::White),
        mSpecular(Colour::Black),
        mDirection(Vector3f::Z),
        mSpotOuter(Angle(40.0f)),
        mSpotInner(Angle(30.0f)),
        mSpotFalloff(1.0f),
        mStart(0.0f),
        mEnd(100000.0f),
        mAttenuationConst(1.0f),
        mAttenuationLinear(0.0f),
        mAttenuationQuad(0.0f),
        mBrightFactor(1.0f),
        mIndexInFrame(0),
        mShadowExtent(0),
        mShadowNearClip(-1),
        mShadowFarClip(-1),
        mSpacePos(Vector3f::ZERO),
        mSpaceDirection(Vector3f::Z),
        mShadowCamera(0)
    {
        //mPixelLimit should always be zero for lights otherwise lights will disapear
        mPixelLimit = 0;
        mMove = false;
        mComMark = 0;
    }
    //-----------------------------------------------------------------------
    Light::~Light()
    {
        if(mShadowCamera)
            N_delete mShadowCamera;
    }
    //-----------------------------------------------------------------------
    Vector4f Light::getSpace4() const
    {
        Vector4f re;
        if(mLightType == LT_DIRECTIONAL)
        {
            re = -(getSpaceDirection()); // negate direction as 'position'
            re.w = 0.0; // infinite distance
        }
        else
        {
            re = getSpacePos();
            re.w = 1.0;
        }
        return re;
    }
    //-----------------------------------------------------------------------
    Vector4f Light::getRelSpace4() const
    {
        Vector4f ret;
        if(mLightType == LT_DIRECTIONAL)
        {
            ret = -getSpaceDirection(); // negate direction as 'position'
            ret.w = 0.0; // infinite distance
        }
        else
        {
            ret = getRelSpacePos();
            ret.w = 1.0;
        }
        return ret;
    }
    //-----------------------------------------------------------------------
    const Vector3f & Light::getSpaceDirection() const
    {
        const_cast<Light *>(this)->update();
        return mSpaceDirection;
    }
    //---------------------------------------------------------------------
    NIIf Light::getShadowFarClip(NIIf ext) const
    {
        if(mShadowFarClip >= 0)
        {
            return mShadowFarClip;
        }
        else
        {
            if (mLightType == LT_DIRECTIONAL)
                return 0;
            else
                return mEnd;
        }
    }
    //-----------------------------------------------------------------------
    void Light::update()
    {
        if(mMove)
        {
            if(mSpaceNode)
            {
                // Ok, update with SpaceNode we're attached to
                const Quaternion & parentOrientation = mSpaceNode->getSpaceOri();
                const Vector3f & parentPosition = mSpaceNode->getSpacePos();
                mSpaceDirection = parentOrientation * mDirection;
                mSpacePos = (parentOrientation * mPosition) + parentPosition;
            }
            else
            {
                mSpacePos = mPosition;
                mSpaceDirection = mDirection;
            }
            mMove = false;
        }
    }
    //-----------------------------------------------------------------------
    void Light::attach(PosNode * node, bool tag)
    {
        mMove = true;
        SpaceObj::attach(node, tag);
    }
    //-----------------------------------------------------------------------
    void Light::_notify()
    {
        SpaceObj::_notify();
    }
    //-----------------------------------------------------------------------
    const AABox & Light::getAABB(void) const
    {
        return AABox::Null;
    }
    //-----------------------------------------------------------------------
    void Light::queue(RenderQueue * queue)
    {
        // Do nothing
    }
    //-----------------------------------------------------------------------
    void Light::query(GeometryQuery * query)
    {
        // nothing to render
    }
    //-----------------------------------------------------------------------
    NIIf Light::getShadowExtent() const
    { 
        return mShadowExtent ? mShadowExtent : mManager->getRenderPattern()->getShadowExtentMax(); 
    }
    //-----------------------------------------------------------------------
    FactoryID Light::getFactoryID() const
    {
        return N_FACTORY_Light;
    }
    //-----------------------------------------------------------------------
    const Vector3f & Light::getSpacePos() const
    {
        const_cast<Light *>(this)->update();
        return mSpacePos;
    }
    //-----------------------------------------------------------------------
    const Vector3f & Light::getRelSpacePos() const
    {
        const_cast<Light *>(this)->update();
        if(mRelation)
        {
            const_cast<Light *>(this)->mRelSpacePos = mSpacePos - mRelation->getSpacePos();
            return mRelSpacePos;
        }
        return mSpacePos;
    }
    //-----------------------------------------------------------------------
    NIIf Light::getCenterRange() const
    {
        return 0;
    }
    //-----------------------------------------------------------------------
    void Light::getClip(PlaneList & planes) const
    {
        planes.clear();

        Vector3f pos = getSpacePos();
        NIIf r = mEnd;
        switch(mLightType)
        {
        case LT_POINT:
            {
                planes.push_back(Plane(Vector3f::X, pos + Vector3f(-r, 0, 0)));
                planes.push_back(Plane(Vector3f::INV_X, pos + Vector3f(r, 0, 0)));
                planes.push_back(Plane(Vector3f::Y, pos + Vector3f(0, -r, 0)));
                planes.push_back(Plane(Vector3f::INV_Y, pos + Vector3f(0, r, 0)));
                planes.push_back(Plane(Vector3f::Z, pos + Vector3f(0, 0, -r)));
                planes.push_back(Plane(Vector3f::INV_Z, pos + Vector3f(0, 0, r)));
            }
            break;
        case LT_SPOT:
            {
                Vector3f dir = getSpaceDirection();
                // 近 &远 平面
                planes.push_back(Plane(dir, pos + dir * mStart));
                planes.push_back(Plane(-dir, pos + dir * r));
                // 金字塔4个面
                // 获取方向
                Vector3f up = Vector3f::Y;
                // 检测它不是符合的dir
                if (Math::Abs(up.dotProduct(dir)) >= 1.0f)
                {
                    up = Vector3f::Z;
                }
                // 通过2次获得,仅方向不变
                Vector3f right = dir.crossProduct(up);
                right.normalise();
                up = right.crossProduct(dir);
                up.normalise();
                // 从轴中获取四元数 (否认dir 由于 -Z)
                Quaternion q;
                q.from(right, up, -dir);

                // 在世界方向中,获取金字塔的角落矢量
                Vector3f tl, tr, bl, br, norl;
                NIIf d = Math::Tan(this->getSpotlightOuter() * 0.5) * r;
                tl = q * Vector3f(-d, d, -r);
                tr = q * Vector3f(d, d, -r);
                bl = q * Vector3f(-d, -d, -r);
                br = q * Vector3f(d, -d, -r);

                // 使用cross product去获取法线,通过灯光世界pos
                // 顶
                tl.crossProduct(tr).normalise(norl);
                planes.push_back(Plane(norl, pos));
                // 右
                tr.crossProduct(br).normalise(norl);
                planes.push_back(Plane(norl, pos));
                // 下
                br.crossProduct(bl).normalise(norl);
                planes.push_back(Plane(norl, pos));
                // 左
                bl.crossProduct(tl).normalise(norl);
                planes.push_back(Plane(norl, pos));
            }
            break;
        default:
            // 不处理
            break;
        };
    }
    //-----------------------------------------------------------------------
    Nui32 Light::getTypeMark() const
    {
        return SpaceManager::LightSpaceMark;
    }
    //-----------------------------------------------------------------------
    void Light::setSortBase(const Vector3f & o)
    {
        if(mLightType == LT_DIRECTIONAL)
        {
            mSortValue = 0;
        }
        else
        {
            mSortValue = (o - getSpacePos()).lengthSquared();
        }
    }
    //-----------------------------------------------------------------------
    void Light::setCameraSetup(CameraSetup * obj)
    {
        if(mShadowCamera)
            N_delete mShadowCamera;
        mShadowCamera = obj;
    }
    //-----------------------------------------------------------------------
    TypeID Light::getFusionCollectorType() const
    {
        return N_FACTORY_Light;
    }
    //-----------------------------------------------------------------------
    void Light::initFusionCollector(IDList * ids) const
    {
        ids->push_back(N_PropertyLight_diffuseColour);
        ids->push_back(N_PropertyLight_specularColour);
        ids->push_back(N_PropertyLight_attenuation);
        ids->push_back(N_PropertyLight_spotlightInner);
        ids->push_back(N_PropertyLight_spotlightOuter);
        ids->push_back(N_PropertyLight_spotlightFalloff);
    }
    //-----------------------------------------------------------------------
    class LightDiffuseColourValue : public FusionData
    {
    public:
        LightDiffuseColourValue(Light * l) :
            FusionData(N_new ColourFusion(), false, true)
        {
            mLight = l;
        }
        void setValue(const FusionNum & obj)
        {
            mLight->setDiffuse(static_cast<const ColourFusion &>(obj).getData());
        }
        void addValue(const FusionNum & obj)
        {
            mLight->setDiffuse(mLight->getDiffuse() + static_cast<const ColourFusion &>(obj).getData());
        }
        void subValue(const FusionNum & obj)
        {
            mLight->setDiffuse(mLight->getDiffuse() - static_cast<const ColourFusion &>(obj).getData());
        }
        void mulValue(const FusionNum & obj)
        {
            mLight->setDiffuse(mLight->getDiffuse() * static_cast<const ColourFusion &>(obj).getData());
        }
        void divValue(const FusionNum & obj)
        {
            mLight->setDiffuse(mLight->getDiffuse() / static_cast<const ColourFusion &>(obj).getData());
        }
        void setCurrentAsBase()
        {
            setBaseValue(N_new ColourFusion(mLight->getDiffuse()), false);
        }
    protected:
        Light * mLight;
    };
    //-----------------------------------------------------------------------
    class LightSpecularColourValue : public FusionData
    {
    public:
        LightSpecularColourValue(Light * l) :
            FusionData(N_new ColourFusion(), false, true)
        {
            mLight = l;
        }
        void setValue(const FusionNum & obj)
        {
            mLight->setSpecular(static_cast<const ColourFusion &>(obj).getData());
        }
        void addValue(const FusionNum & obj)
        {
            mLight->setSpecular(mLight->getSpecular() + static_cast<const ColourFusion &>(obj).getData());
        }
        void subValue(const FusionNum & obj)
        {
            mLight->setSpecular(mLight->getSpecular() - static_cast<const ColourFusion &>(obj).getData());
        }
        void mulValue(const FusionNum & obj)
        {
            mLight->setSpecular(mLight->getSpecular() * static_cast<const ColourFusion &>(obj).getData());
        }
        void divValue(const FusionNum & obj)
        {
            mLight->setSpecular(mLight->getSpecular() / static_cast<const ColourFusion &>(obj).getData());
        }
        void setCurrentAsBase()
        {
            setBaseValue(N_new ColourFusion(mLight->getSpecular()), false);
        }
    protected:
        Light * mLight;
    };
    //-----------------------------------------------------------------------
    class LightAttenuationValue : public FusionData
    {
    public:
        LightAttenuationValue(Light * l) :
            FusionData(N_new Vector4fFusion(), false, true)
        {
            mLight = l;
        }
        void setValue(const FusionNum & obj)
        {
            const Vector4f & val = static_cast<const Vector4fFusion &>(obj).getData();
            mLight->setRange(val.x);
            mLight->setAttenuationConstant(val.y);
            mLight->setAttenuationLinear(val.z);
            mLight->setAttenuationQuadric(val.w);
        }
        void addValue(const FusionNum & obj)
        {
            Vector4f val = mLight->getSpace4() + static_cast<const Vector4fFusion &>(obj).getData();
            mLight->setRange(val.x);
            mLight->setAttenuationConstant(val.y);
            mLight->setAttenuationLinear(val.z);
            mLight->setAttenuationQuadric(val.w);
        }
        void subValue(const FusionNum & obj)
        {
            Vector4f val = mLight->getSpace4() - static_cast<const Vector4fFusion &>(obj).getData();
            mLight->setRange(val.x);
            mLight->setAttenuationConstant(val.y);
            mLight->setAttenuationLinear(val.z);
            mLight->setAttenuationQuadric(val.w);
        }
        void mulValue(const FusionNum & obj)
        {
            Vector4f val = mLight->getSpace4() * static_cast<const Vector4fFusion &>(obj).getData();
            mLight->setRange(val.x);
            mLight->setAttenuationConstant(val.y);
            mLight->setAttenuationLinear(val.z);
            mLight->setAttenuationQuadric(val.w);
        }
        void divValue(const FusionNum & obj)
        {
            Vector4f val = mLight->getSpace4() / static_cast<const Vector4fFusion &>(obj).getData();
            mLight->setRange(val.x);
            mLight->setAttenuationConstant(val.y);
            mLight->setAttenuationLinear(val.z);
            mLight->setAttenuationQuadric(val.w);
        }
        void setCurrentAsBase()
        {
            setBaseValue(N_new Vector4fFusion(mLight->getSpace4()), false);
        }
    protected:
        Light * mLight;
    };
    //-----------------------------------------------------------------------
    class LightSpotlightInnerValue : public FusionData
    {
    public:
        LightSpotlightInnerValue(Light * l) :
            FusionData(N_new NIIfFusion(), false, true)
        {
            mLight = l;
        }

        void setValue(const FusionNum & obj)
        {
            mLight->setSpotlightInner(Radian(static_cast<const NIIfFusion &>(obj).getData()));
        }

        void addValue(const FusionNum & obj)
        {
            mLight->setSpotlightInner(Radian(mLight->getSpotlightInner().getValue() + static_cast<const NIIfFusion &>(obj).getData()));
        }

        void subValue(const FusionNum & obj)
        {
            mLight->setSpotlightInner(Radian(mLight->getSpotlightInner().getValue() - static_cast<const NIIfFusion &>(obj).getData()));
        }

        void mulValue(const FusionNum & obj)
        {
            mLight->setSpotlightInner(Radian(mLight->getSpotlightInner().getValue() * static_cast<const NIIfFusion &>(obj).getData()));
        }

        void divValue(const FusionNum & obj)
        {
            N_assert(static_cast<const NIIfFusion &>(obj).getData(), "error");
            mLight->setSpotlightInner(Radian(mLight->getSpotlightInner().getValue() / static_cast<const NIIfFusion &>(obj).getData()));
        }

        void setCurrentAsBase()
        {
            setBaseValue(N_new NIIfFusion(mLight->getSpotlightInner().getValue()), false);
        }
    protected:
        Light * mLight;
    };
    //-----------------------------------------------------------------------
    class LightSpotlightOuterValue : public FusionData
    {
    public:
        LightSpotlightOuterValue(Light * l) :
            FusionData(N_new NIIfFusion(), false, true)
        {
            mLight = l;
        }

        void setValue(const FusionNum & obj)
        {
            mLight->setSpotlightOuter(Radian(static_cast<const NIIfFusion &>(obj).getData()));
        }

        void addValue(const FusionNum & obj)
        {
            mLight->setSpotlightOuter(Radian(mLight->getSpotlightOuter().getValue() + static_cast<const NIIfFusion &>(obj).getData()));
        }

        void subValue(const FusionNum & obj)
        {
            mLight->setSpotlightOuter(Radian(mLight->getSpotlightOuter().getValue() - static_cast<const NIIfFusion &>(obj).getData()));
        }

        void mulValue(const FusionNum & obj)
        {
            mLight->setSpotlightOuter(Radian(mLight->getSpotlightOuter().getValue() * static_cast<const NIIfFusion &>(obj).getData()));
        }

        void divValue(const FusionNum & obj)
        {
            N_assert(static_cast<const NIIfFusion &>(obj).getData(), "error");
            mLight->setSpotlightOuter(Radian(mLight->getSpotlightOuter().getValue() / static_cast<const NIIfFusion &>(obj).getData()));
        }

        void setCurrentAsBase()
        {
            setBaseValue(N_new NIIfFusion(mLight->getSpotlightOuter().getValue()), false);
        }
    protected:
        Light * mLight;
    };
    //-----------------------------------------------------------------------
    class LightSpotlightFalloffValue : public FusionData
    {
    public:
        LightSpotlightFalloffValue(Light * l) :
            FusionData(N_new NIIfFusion(), false, true)
        {
            mLight = l;
        }
        void setValue(const FusionNum & obj)
        {
            mLight->setSpotlightFalloff(static_cast<const NIIfFusion &>(obj).getData());
        }

        void addValue(const FusionNum & obj)
        {
            mLight->setSpotlightFalloff(mLight->getSpotlightFalloff() + static_cast<const NIIfFusion &>(obj).getData());
        }

        void subValue(const FusionNum & obj)
        {
            mLight->setSpotlightFalloff(mLight->getSpotlightFalloff() - static_cast<const NIIfFusion &>(obj).getData());
        }

        void mulValue(const FusionNum & obj)
        {
            mLight->setSpotlightFalloff(mLight->getSpotlightFalloff() * static_cast<const NIIfFusion &>(obj).getData());
        }

        void divValue(const FusionNum & obj)
        {
            N_assert(static_cast<const NIIfFusion &>(obj).getData(), "error");
            mLight->setSpotlightFalloff(mLight->getSpotlightFalloff() / static_cast<const NIIfFusion &>(obj).getData());
        }

        void setCurrentAsBase()
        {
            setBaseValue(N_new NIIfFusion(mLight->getSpotlightFalloff()), false);
        }
    protected:
        Light * mLight;
    };
    //-----------------------------------------------------------------------
    FusionData * Light::createFusionData(Nid id)
    {
        if (id == N_PropertyLight_diffuseColour)
        {
            return N_new LightDiffuseColourValue(this);
        }
        else if(id == N_PropertyLight_specularColour)
        {
            return N_new LightSpecularColourValue(this);
        }
        else if (id == N_PropertyLight_attenuation)
        {
            return N_new LightAttenuationValue(this);
        }
        else if (id == N_PropertyLight_spotlightInner)
        {
            return N_new LightSpotlightInnerValue(this);
        }
        else if (id == N_PropertyLight_spotlightOuter)
        {
            return N_new LightSpotlightOuterValue(this);
        }
        else if (id == N_PropertyLight_spotlightFalloff)
        {
            return N_new LightSpotlightFalloffValue(this);
        }
        else
        {
            return FusionDataCollector::createFusionData(id);
        }
    }
    //------------------------------------------------------------------------
}