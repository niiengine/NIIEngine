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
#include "NiiAnimationCollectorXmlAnalyze.h"
#include "NiiUIException.h"
#include "NiiUICommon.h"
#include "NiiUIStyle.h"
#include "NiiAnimationManager.h"
#include "NiiScriptHelperManager.h"
#include "NiiLogManager.h"
#include "NiiXmlAnalyzeList.h"
#include "NiiDefine.h"
#include "NiiQuaternionFusion.h"
#include "NiiBoolFusion.h"
#include "NiiPropertyKeyFrameTrack.h"
#include "NiiUnitKeyFrame.h"
#include "NiiPropertyKeyFrame.h"
#include "NiiUIStrConv.h"

namespace NII
{
    inline RelVector2f operator + (const RelVector2f & o, NIIf c)
    {
        return RelVector2f(o.x + c, o.y + c);
    }

    inline RelVector2f operator - (const RelVector2f & o, NIIf c)
    {
        return RelVector2f(o.x - c, o.y - c);
    }

    inline RelVector2f operator * (const RelVector2f & o, NIIf c)
    {
        return RelVector2f(o.x * c, o.y * c);
    }

    inline RelVector2f operator / (const RelVector2f & o, NIIf c)
    {
        return RelVector2f(c == 0 ? o.x : o.x / c, c == 0 ? o.y : o.y / c);
    }

    inline RelRectf operator + (const RelRectf & o, NIIf c)
    {
        return RelRectf(o.mLeft + c, o.mTop + c, o.mRight + c, o.mBottom + c);
    }

    inline RelRectf operator - (const RelRectf & o, NIIf c)
    {
        return RelRectf(o.mLeft - c, o.mTop - c, o.mRight - c, o.mBottom - c);
    }

    inline RelRectf operator * (const RelRectf & o, NIIf c)
    {
        return RelRectf(o.mLeft * c, o.mTop * c, o.mRight * c, o.mBottom * c);
    }

    inline RelRectf operator / (const RelRectf & o, NIIf c)
    {
        return RelRectf(c == 0 ? o.mLeft : o.mLeft / c, c == 0 ? o.mTop : o.mTop / c,
            c == 0 ? o.mRight : o.mRight / c, c == 0 ? o.mBottom : o.mBottom / c);
    }

    inline RelPlaneSizef operator + (const RelPlaneSizef & o, NIIf x)
    {
        return o + RelPosf(0, x);
    }

    inline RelPlaneSizef operator - (const RelPlaneSizef & o, NIIf x)
    {
        return o - RelPosf(0, x);
    }

    inline RelPlaneSizef operator * (const RelPlaneSizef & o, NIIf x)
    {
        return o * RelPosf(x, x);
    }
    inline RelPlaneSizef operator / (const RelPlaneSizef & o, NIIf x)
    {
        return o / RelPosf(x, x);
    }
namespace UI
{
    typedef TypeFusionNum<StrConv, RelRectf> RelRectFusion;
    typedef TypeFusionNum<UI::StrConv, RelVector2f> RelVector2Fusion;
    typedef TypeFusionNum<UI::StrConv, RelPlaneSizef> RelPlaneSizeFusion;
    typedef TypeFusionNum<UI::StrConv, ColourArea> ColourAreaFusion;
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // AnimationCollectorXmlAnalyze
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    AnimationCollectorXmlAnalyze::AnimationCollectorXmlAnalyze(LangID lid)  :
        XmlAnalyze(0, N_Script_AnimationCollector, _T("Animation.xsd"), lid)
    {
    }
    //------------------------------------------------------------------------
    AnimationCollectorXmlAnalyze::~AnimationCollectorXmlAnalyze()
    {
    }
    //------------------------------------------------------------------------
    XmlUnitID AnimationCollectorXmlAnalyze::getUnit() const
    {
        return N_XML_TAG_AnimationCollector;
    }
    //------------------------------------------------------------------------
    GroupID AnimationCollectorXmlAnalyze::getGroup() const
    {
        return N_Only(Animation).getGroup();
    }
    //------------------------------------------------------------------------
    const String & AnimationCollectorXmlAnalyze::getXSD() const
    {
        return mXSD;
    }
    //------------------------------------------------------------------------
    void AnimationCollectorXmlAnalyze::PrcTagUnitCB(const PropertyData * pdc)
    {
        N_Only(Log).log(_I18n("**** 开始解析动画集XML"));
    }
    //------------------------------------------------------------------------
    void AnimationCollectorXmlAnalyze::PrcTagUnitECB(const PropertyData * pdc)
    {
        N_Only(Log).log(_I18n("**** 结束解析动画集XML"));
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // AnimationXmlAnalyze
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    AnimationXmlAnalyze::AnimationXmlAnalyze(Style * obj, LangID lid)  :
        XmlAnalyze(0, N_Script_Animation, _T("Animation.xsd"), lid),
        mAnimationObj(obj),
        mAnimation(0),
        mTrack(0)
    {
        setBeginPrc(KeyFrameTrackProperty, (PrcCB)&AnimationXmlAnalyze::PrcKeyFrameTrackCB);
        setBeginPrc(KeyFrameProperty, (PrcCB)&AnimationXmlAnalyze::PrcKeyFrameCB);
        setBeginPrc(AnimationEventProperty, (PrcCB)&AnimationXmlAnalyze::PrcEventCB);
    }
    //------------------------------------------------------------------------
    AnimationXmlAnalyze::~AnimationXmlAnalyze()
    {
    }
    //------------------------------------------------------------------------
    void AnimationXmlAnalyze::setAnimationObj(Style * obj)
    {
        mAnimationObj = obj;
    }
    //------------------------------------------------------------------------
    Style * AnimationXmlAnalyze::getAnimationObj() const
    {
        return mAnimationObj;
    }
    //------------------------------------------------------------------------
    XmlUnitID AnimationXmlAnalyze::getUnit() const
    {
        return N_XML_TAG_AnimationUnit;
    }
    //------------------------------------------------------------------------
    GroupID AnimationXmlAnalyze::getGroup() const
    {
        return GroupUnknow;
    }
    //------------------------------------------------------------------------
    const String & AnimationXmlAnalyze::getXSD() const
    {
        return mXSD;
    }
    //------------------------------------------------------------------------
    void AnimationXmlAnalyze::PrcTagUnitCB(const PropertyData * pdc)
    {
        Nui32 animid;
        pdc->get(IDProperty, animid);

        TimeDurMS tf;
        pdc->get(DurationProperty, tf);

        PropertyID replayMode;
        pdc->get(mLangMap->getLangID(), mScriptTypeID, ReplayModeAttribute, replayMode, ReplayModeLoop);

        bool temp;
        pdc->get(AutoStartProperty, temp, false);

        N_Only(Log).log(_I18n("Defining animation named: ") + N_conv(animid) +
            _I18n("  Duration: ") + N_conv(tf) +
            _I18n("  Replay mode: ") + N_conv(replayMode) +
            _I18n("  Auto start: ") + N_conv(temp));

        mAnimation = N_Only(Animation).createAnimation(mAnimationObj, animid, tf, ANIM_PropertyID);

        if(replayMode == ReplayModeOnce)
            mAnimation->setPlayMode(Animation::RM_Once);
        else if(replayMode == ReplayModeBounce)
            mAnimation->setPlayMode(Animation::RM_Bounce);
        else
            mAnimation->setPlayMode(Animation::RM_Loop);

        mAnimation->setAutoStart(temp);
    }
    //------------------------------------------------------------------------
    void AnimationXmlAnalyze::PrcKeyFrameTrackCB(const PropertyData * pdc)
    {
        PropertyID calcm;
        pdc->get(mLangMap->getLangID(), mScriptTypeID, CalcModeProperty, calcm, PropertyKeyFrameTrack::M_ABS);
        PropertyID cmdproperty;
        pdc->get(TargetPropertyAttribute, cmdproperty);

        N_Only(Log).log(
            _I18n("\tAdding affector for property: ") + pdc->get(TargetPropertyAttribute) +
            _I18n("  DataType: ") + pdc->get(DataTypeProperty) +
            _I18n("  Application method: ") + pdc->get(CalcModeProperty));

        setupFusionData(pdc->get(DataTypeProperty));

        mTrack = N_new PropertyKeyFrameTrack(mAnimation, 0, cmdproperty);

        if(calcm == ValueMethodRelProperty)
        {
            mTrack->setMode(PropertyKeyFrameTrack::M_REL);
        }
        else if(calcm == ValueMethodMulRelProperty)
        {
            mTrack->setMode(PropertyKeyFrameTrack::M_MulREL);
        }
        else
        {
            mTrack->setMode(PropertyKeyFrameTrack::M_ABS);
        }

        mAnimation->add(mTrack);
    }
    //------------------------------------------------------------------------
    void AnimationXmlAnalyze::PrcKeyFrameCB(const PropertyData * pdc)
    {
        PropertyID calcm;
        pdc->get(mLangMap->getLangID(), mScriptTypeID, CalcModeProperty, calcm, FactorConstantProperty);

        PropertyID valuep;
        pdc->get(ValuePropertySrcProperty, valuep);

        String log_event(_T("\t\tAdding KeyFrame at position: ") + pdc->get(PositionProperty) +
            _T("  Value: ") + pdc->get(ValueProperty) +
                _T("  Mode: ") + pdc->get(CalcModeProperty));

        N_Only(Log).log(log_event);

        setFusionValue(pdc->get(ValueProperty));

        UnitKeyFrame::Mode mode;
        if(calcm == FactorPointProperty)
            mode = UnitKeyFrame::M_Point;
        else if(calcm == FactorSquareProperty)
            mode = UnitKeyFrame::M_Square;
        else if(calcm == FactorSquareRootProperty)
            mode = UnitKeyFrame::M_SquareRoot;
        else
            mode = UnitKeyFrame::M_Constant; // constant

        TimeDurMS tf;
        pdc->get(PositionProperty, tf);
        PropertyKeyFrame * kf = static_cast<PropertyKeyFrame *>(mTrack->create(tf));
        kf->setValue(mFusionData);
        kf->setMode(mode);
        kf->setProperty(valuep);

        if(mTrack->getCount() == 1)
            N_Only(Log).log(_I18n("WARNING: mode type specified for first keyframe in ")
                _I18n("animation will be ignored."));
    }
    //------------------------------------------------------------------------
    void AnimationXmlAnalyze::PrcEventCB(const PropertyData * pdc)
    {
        Nui32 eid, action;
        pdc->get(EventAttribute, eid);
        pdc->get(ActionAttribute, action);
        N_Only(Log).log(_I18n("\tAdding subscription to event: ") + 
            pdc->get(EventAttribute) + _I18n("  Action: ") + pdc->get(ActionAttribute));

        mAnimation->add(eid, (Animation::ActionType)action);
    }
    //------------------------------------------------------------------------
    void AnimationXmlAnalyze::setupFusionData(const String & str)
    {
        if(str == _T("float"))
            mFusionData.setBaseValue(N_new NIIfFusion(), false);
        else if(str == _T("uint"))
            mFusionData.setBaseValue(N_new Nui32Fusion(), false);
        else if(str == _T("int"))
            mFusionData.setBaseValue(N_new Ni32Fusion(), false);
        else if(str == _T("Vector2"))
            mFusionData.setBaseValue(N_new Vector2fFusion(), false);
        else if(str == _T("Vector3"))
            mFusionData.setBaseValue(N_new Vector3fFusion(), false);
        else if(str == _T("Vector4"))
            mFusionData.setBaseValue(N_new Vector4fFusion(), false);
        else if(str == _T("Colour"))
            mFusionData.setBaseValue(N_new ColourFusion(), false);
        else if(str == _T("bool"))
            mFusionData.setBaseValue(N_new BoolFusion(), false);
        else if(str == _T("Quaternion"))
            mFusionData.setBaseValue(N_new QuaternionFusion(), false);
        else if(str == _T("RelVector2"))
            mFusionData.setBaseValue(N_new RelVector2Fusion(), false);
        else if(str == _T("RelPlaneSize"))
            mFusionData.setBaseValue(N_new RelPlaneSizeFusion(), false);
        else if(str == _T("ColourArea"))
            mFusionData.setBaseValue(N_new ColourAreaFusion(), false);
        else
            N_assert(0, "错误");
    }
    //------------------------------------------------------------------------
    void AnimationXmlAnalyze::setFusionValue(const String & str)
    {
        N_assert(mFusionData.getBaseValue(), "error");
        mFusionData.getBaseValue()->from(str);
    }
    //------------------------------------------------------------------------
    void AnimationXmlAnalyze::write(XmlSerializer * in, const Animation * src)
    {
        in->begin(mLangMap->get(IDProperty));

        in->attribute(mLangMap->get(NameProperty), src->getID());
        in->attribute(mLangMap->get(DurationProperty), src->getDur());

        PropertyID replayMode;
        switch(src->getPlayMode())
        {
        case Animation::RM_Once:
            replayMode = ReplayModeOnce;
            break;
        case Animation::RM_Loop:
            replayMode = ReplayModeLoop;
            break;
        case Animation::RM_Bounce:
            replayMode = ReplayModeBounce;
            break;
        default:
            N_assert(0, "How did we get here?");
            break;
        }

        in->attribute(mLangMap->get(ReplayModeAttribute), replayMode);
        in->attribute(mLangMap->get(AutoStartProperty), src->isAutoStart());

        Animation::TrackList::const_iterator i, iend = src->getTrack().end();
        for(i = src->getTrack().begin(); i != iend; ++i)
        {
            write(in, i->second);
        }

        Animation::Actions::const_iterator j, jend = src->getEvent().end();
        for(j = src->getEvent().begin(); j != jend; ++j)
        {
            in->begin(mLangMap->get(AnimationEventProperty));

            in->attribute(mLangMap->get(EventAttribute), j->first);
            in->attribute(mLangMap->get(ActionAttribute), j->second);

            in->end();
        }
        in->end();
    }
    //------------------------------------------------------------------------
    void AnimationXmlAnalyze::write(XmlSerializer * in, const KeyFrameTrack * data)
    {
        const PropertyKeyFrameTrack * src = static_cast<const PropertyKeyFrameTrack *>(data);
        in->begin(mLangMap->get(KeyFrameTrackProperty));

        PropertyID valuemethod;
        switch(src->getMode())
        {
        case PropertyKeyFrameTrack::M_ABS:
            valuemethod = ValueMethodAbsProperty;
            break;
        case PropertyKeyFrameTrack::M_REL:
            valuemethod = ValueMethodRelProperty;
            break;
        case PropertyKeyFrameTrack::M_MulREL:
            valuemethod = ValueMethodMulRelProperty;
            break;
        default:
            N_assert(0, "error logic");
            break;
        }

        in->attribute(mLangMap->get(CalcModeProperty), valuemethod);

        in->attribute(mLangMap->get(TargetPropertyAttribute), src->getTarget());

        KeyFrameTrack::KeyFrames::const_iterator i, iend = src->getList().end();
        for(i = src->getList().begin(); i != iend; ++i)
        {
            AnimationXmlAnalyze::write(in, *i);
        }

        in->end();
    }
    //------------------------------------------------------------------------
    void AnimationXmlAnalyze::write(XmlSerializer * in, const KeyFrame * data)
    {
        const PropertyKeyFrame * src = static_cast<const PropertyKeyFrame *>(data);
        in->begin(mLangMap->get(KeyFrameProperty));

        in->attribute(mLangMap->get(PositionProperty), N_conv((Nui32)src->getTime()));

        if(src->getProperty() == 0)
        {
            String temp;
            mLangMap->get(ValueProperty), src->getValue().getBaseValue()->to(temp);
            in->attribute(mLangMap->get(ValueProperty), temp);
        }
        else
        {
            in->attribute(mLangMap->get(ValuePropertySrcProperty), src->getProperty());
        }

        PropertyID mode;
        switch(src->getMode())
        {
        case UnitKeyFrame::M_Constant:
            mode = FactorConstantProperty;
            break;
        case UnitKeyFrame::M_Point:
            mode = FactorPointProperty;
            break;
        case UnitKeyFrame::M_Square:
            mode = FactorSquareProperty;
            break;
        case UnitKeyFrame::M_SquareRoot:
            mode = FactorSquareRootProperty;
            break;
        default:
            N_assert(0, "error logic");
            break;
        }
        in->attribute(mLangMap->get(CalcModeProperty), mode);

        in->end();
    }
    //------------------------------------------------------------------------
    /*
    void CSoundAnimation::load()
    {
        CIFile file;

        // Open the file
        if(!file.open(_Filename.c_str()))
        {
            throw NLMISC::Exception("Can't open the file for reading");
        }

        // Create the XML stream
        CIXml input;

        // Init
        if(input.init (file))
        {
            xmlNodePtr animNode = input.getRootNode ();
            xmlNodePtr markerNode = input.getFirstChildNode(animNode, "MARKER");

            while (markerNode != 0)
            {
                SoundKeyFrameTrack * marker = new SoundKeyFrameTrack();

                const char * time = (const char *) xmlGetProp(markerNode, (xmlChar *) "time");
                if (time == 0)
                {
                    throw NLMISC::Exception("Invalid sound animation marker");
                }

                marker->setTime((float) atof(time));
                xmlFree((void *)time);


                xmlNodePtr soundNode = input.getFirstChildNode(markerNode, "SOUND");

                while(soundNode != 0)
                {
                    char * name = (char *)xmlGetProp(soundNode, (xmlChar *) "name");
                    if(name == 0)
                    {
                        throw NLMISC::Exception("Invalid sound animation marker");
                    }

                    marker->addSound(CStringMapper::map(string(name)));

                    xmlFree ((void *)name);

                    soundNode = input.getNextChildNode(soundNode, "SOUND");
                }

                add(marker);

                markerNode = input.getNextChildNode(markerNode, "MARKER");
            }
        }
        // Close the file
        file.close ();
        _Dirty = false;
    }
    
    void CSoundAnimation::save()
    {
        // File stream
        COFile file;
        vector<const String *> sounds;

        // Open the file
        if(!file.open(_Filename.c_str()))
        {
            throw NLMISC::Exception("Can't open the file for writing");
        }

        // Create the XML stream
        COXml output;

        // Init
        if (output.init (&file, "1.0"))
        {
            xmlDocPtr xmlDoc = output.getDocument();

            // Create the first node
            xmlNodePtr root = xmlNewDocNode (xmlDoc, NULL, (const xmlChar*)"SOUNDANIMATION", NULL);
            xmlDocSetRootElement (xmlDoc, root);

            vector<SoundKeyFrameTrack *>::iterator iter;
            for(iter = mTrackList.begin(); iter != mTrackList.end(); iter++)
            {
                SoundKeyFrameTrack * marker = (*iter);
            
                set<string>::iterator iter;

                char s[64];
                smprintf(s, 64, "%f", marker->getTime());

                xmlNodePtr markerNode = xmlNewChild (root, NULL, (const xmlChar*)"MARKER", NULL);
                xmlSetProp (markerNode, (const xmlChar*) "time", (const xmlChar*) s);

                marker->getSounds(sounds);

                vector<const String *>::iterator iter2;
                for (iter2 = sounds.begin(); iter2 != sounds.end(); iter2++)
                {
                    xmlNodePtr soundNode = xmlNewChild ( markerNode, NULL, (const xmlChar*)"SOUND", NULL );
                    xmlSetProp (soundNode, (const xmlChar*)"name", (const xmlChar*) CStringMapper::unmap(*iter2).c_str());
                }

                sounds.clear();
            }

            // Flush the stream, write all the output file
            output.flush ();
        }

        // Close the file
        file.close ();

        _Dirty = false;
    }
    */
}
}