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

#ifndef _NII_ANIMATION_XmlAnalyze_H__
#define _NII_ANIMATION_XmlAnalyze_H__

#include "NiiUIPreInclude.h"
#include "NiiXmlAnalyze.h"
#include "NiiAnimation.h"
#include "NiiFusionData.h"

namespace NII
{
namespace UI
{
    /** ������xml����
    @version NIIEngine 3.0.0
    */
    class _EngineAPI AnimationCollectorXmlAnalyze : public XmlAnalyze
    {
    public:
        AnimationCollectorXmlAnalyze(LangID lid = N_PrimaryLang);
        virtual ~AnimationCollectorXmlAnalyze();

        /// @copydetails XmlAnalyze::getXSD
        const String & getXSD() const;

        /// @copydetails XmlAnalyze::getUnit
        XmlUnitID getUnit() const;

        /// @copydetails XmlAnalyze::getGroup
        GroupID getGroup() const;
    protected:
        /// @copydetails XmlAnalyze::PrcTagUnitCB
        void PrcTagUnitCB(const PropertyData * pdc);

        /// @copydetails XmlAnalyze::PrcTagUnitECB
        void PrcTagUnitECB(const PropertyData * pdc);
    };

    /** ����xml����
    @version NIIEngine 3.0.0
    */
    class _EngineAPI AnimationXmlAnalyze : public AnimationProperty, public XmlAnalyze
    {
    public:
        AnimationXmlAnalyze(Style * obj, LangID lid = N_PrimaryLang);
        virtual ~AnimationXmlAnalyze();

        void setAnimationObj(Style * obj);

        Style * getAnimationObj() const;

        /// @copydetails XmlAnalyze::getXSD
        const String & getXSD() const;

        /// @copydetails XmlAnalyze::getUnit
        XmlUnitID getUnit() const;

        /// @copydetails XmlAnalyze::getGroup
        GroupID getGroup() const;

        /** д�붯��
        @version NIIEngine 3.0.0
        */
        void write(XmlSerializer * in, const Animation * src);

        /** д��켣
        @version NIIEngine 3.0.0
        */
        void write(XmlSerializer * in, const KeyFrameTrack * src);

        /** д��ؼ�֡
        @version NIIEngine 3.0.0
        */
        void write(XmlSerializer * in, const KeyFrame * src);
    protected:
        /// @copydetails XmlAnalyze::PrcTagUnitCB
        void PrcTagUnitCB(const PropertyData * pdc);

        /** ����ؼ�֡�켣
        @version NIIEngine 3.0.0
        */
        void PrcKeyFrameTrackCB(const PropertyData * pdc);

        /** ����ؼ�֡
        @version NIIEngine 3.0.0
        */
        void PrcKeyFrameCB(const PropertyData * pdc);

        /** �����¼�
        @version NIIEngine 3.0.0
        */
        void PrcEventCB(const PropertyData * pdc);

        /** �����������
        @version NIIEngine 3.0.0
        */
        void setupFusionData(const String & str);

        /** ���û������ֵ
        @version NIIEngine 3.0.0
        */
        void setFusionValue(const String & str);
    protected:
        FusionData mFusionData;
        Animation * mAnimation;
        Style * mAnimationObj;
        PropertyKeyFrameTrack * mTrack;
    };
    
    /** ����xml����
    @version NIIEngine 3.0.0
    */
    /*class _EngineAPI SoundAnimationXmlAnalyze : public AnimationProperty, public XmlAnalyze
    {
    public:
		SoundAnimationXmlAnalyze(Style * obj, LangID lid = N_PrimaryLang);
        virtual ~SoundAnimationXmlAnalyze();

        void setAnimationObj(Style * obj);

        Style * getAnimationObj() const;

        /// @copydetails XmlAnalyze::getXSD
        const String & getXSD() const;

        /// @copydetails XmlAnalyze::getUnit
        XmlUnitID getUnit() const;

        /// @copydetails XmlAnalyze::getGroup
        GroupID getGroup() const;

        /** д�붯��
        @version NIIEngine 3.0.0
        */
     //   void write(XmlSerializer * in, const Animation * src);

        /** д��켣
        @version NIIEngine 3.0.0
        */
    //    void write(XmlSerializer * in, const KeyFrameTrack * src);

        /** д��ؼ�֡
        @version NIIEngine 3.0.0
        */
   //     void write(XmlSerializer * in, const KeyFrame * src);
   // protected:
        /// @copydetails XmlAnalyze::PrcTagUnitCB
    //    void PrcTagUnitCB(const PropertyData * pdc);

        /** ����ؼ�֡�켣
        @version NIIEngine 3.0.0
        */
    //    void PrcKeyFrameTrackCB(const PropertyData * pdc);

        /** ����ؼ�֡
        @version NIIEngine 3.0.0
        */
   //     void PrcKeyFrameCB(const PropertyData * pdc);

        /** �����¼�
        @version NIIEngine 3.0.0
        */
    //    void PrcEventCB(const PropertyData * pdc);

        /** �����������
        @version NIIEngine 3.0.0
        */
    //    void setupFusionData(const String & str);

        /** ���û������ֵ
        @version NIIEngine 3.0.0
        */
    //    void setFusionValue(const String & str);
    //protected:
    //    Animation * mAnimation;
	//	PropertyKeyFrameTrack * mTrack;
    //};
}
}
#endif