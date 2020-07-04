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

#ifndef _NII_SKY_H_
#define _NII_SKY_H_

#include "NiiPreInclude.h"

namespace NII
{
    /** ���
    @version NIIEngine 3.0.0
    */
    class _EngineAPI Sky : public SpaceAlloc
    {
    public:
        enum PlaneType
        {
            PT_Front = 0,
            PT_Back = 1,
            PT_Left = 2,
            PT_Right = 3,
            PT_Up = 4,
            PT_Down = 5
        };
    public:
        Sky();
        Sky(const SpaceManager * sm);
        virtual ~Sky();

        /** �������Ⱦ���������
        @param[in] obj �����
        @version NIIEngine 3.0.0
        */
        virtual void render(Camera * obj);

        /** ���ÿռ���
        @version NIIEngine 3.0.0
        */
        void setSpace(SpaceManager * sm)                { mParent = sm; }
        
        /** ��ȡ�ռ���
        @version NIIEngine 3.0.0
        */
        SpaceManager * getSpace() const                 { return mParent; }
        
        /** ������Ⱦ��
        @version NIIEngine 3.0.0
        */
        inline void setRenderGroup(GroupID gid)         { mRenderGroup = gid; }

        /** ��ȡ��Ⱦ��
        @version NIIEngine 3.0.0
        */
        inline GroupID getRenderGroup()const            { return mRenderGroup; }

        /** ������Դ��
        @version NIIEngine 3.0.0
        */
        inline void setResourceGroup(GroupID gid)       { mResourceGroup = gid; }

        /** ��ȡ��Դ��
        @version NIIEngine 3.0.0
        */
        inline GroupID getResourceGroup() const         { return mResourceGroup; }

        /** �����Ƿ�����
        @version NIIEngine 3.0.0
        */
        inline void setEnable(bool b)                   { mEnable = b; }

        /** ��ȡ�Ƿ�����
        @version NIIEngine 3.0.0
        */
        inline bool isEnable() const                    { return mEnable; }
    protected:
        /** ��Ⱦ��������
        @version NIIEngine 3.0.0 �߼�api
        */
        virtual void renderImpl(RenderQueue * rq) = 0;
    protected:
        SpaceManager * mParent;
        GroupID mRenderGroup;
        GroupID mResourceGroup;
        SpaceNode * mNode;
        bool mEnable;
    };
    
    /** �����
    @version NIIEngine 3.0.0
    */
    class _EngineAPI BoxSky : public Sky
    {
    public:
        BoxSky();
        BoxSky(const SpaceManager * gm);
        virtual ~BoxSky();
        
        /** ���ò���
        @param[in] rid ��ԴID
        @version NIIEngine 3.0.0
        */
        void setMaterial(ResourceID rid);
        
        /** ���÷���ƫ��
        @version NIIEngine 3.0.0
        */
        inline void setOriOft(const Quaternion & o)     { mOriOft = o;  }
        
        /** ��ȡ����ƫ��
        @version NIIEngine 3.0.0
        */
        inline const Quaternion & getOriOft() const     { return mOriOft;  }
        
        /** ��������
        @version NIIEngine 3.0.0
        */
        void buildGeo(NIIf distance, const Quaternion & ori_oft);
    
        /** ��ȡ���ӿڵľ���
        @version NIIEngine 3.0.0
        */
        inline NIIf getRange() const                    { return mRange; }
    protected:
        /// @copydetails BoxSky::renderImpl
        virtual void renderImpl(RenderQueue * rq);
    protected:
        NIIf mRange;
        CustomSpaceObj * mGeo;
        Quaternion mOriOft;
        ResourceID mMaterial[7];
    };
    
    /** ����
    @version NIIEngine 3.0.0
    */
    class _EngineAPI DomeSky : public Sky
    {
    public:
        DomeSky();
        DomeSky(const SpaceManager * gm);
        virtual ~DomeSky();

        /** ���ò���
        @version NIIEngine 3.0.0
        */
        void setMaterial(ResourceID rid);

        /** ��������
        @version NIIEngine 3.0.0
        */
        void buildGeo(NIIf cuvfactor = 10, NIIf range = 1000, NIIf xtiling = 20,
            NIIf ytiling = 20, NIIi xsegments = 20, NIIi ysegments = 20,
                const Quaternion & ori_oft = Quaternion::IDENTITY);
    protected:
        /// @copydetails Sky::renderImpl
        virtual void renderImpl(RenderQueue * rq);

        /** ����һ����
        @version NIIEngine 3.0.0
        */
        Mesh * createPlaneImpl(ResourceID & rid, GroupID gid, PlaneType bp,
            NIIf cuvfactor, NIIf range, const Quaternion & ori_oft,
                NIIf xtiling, NIIf ytiling, NIIi xsegments, NIIi ysegments) const;
    protected:
        SpaceNode * mNode;
        GeometrySpace * mGeo[5];
        ResourceID mResource[5];
        ResourceID mMaterial;
        NIIf mRange;
        NIIf mCurveFactor;
        NIIf mXTiling;
        NIIf mYTiling;
        NIIi mXSegment;
        NIIi mYSegment;
    };
    
    /** �����
    @version NIIEngine 3.0.0
    */
    class _EngineAPI PlaneSky : public Sky
    {
    public:
        PlaneSky();
        PlaneSky(const SpaceManager * gm);
        virtual ~PlaneSky();

        /** ���ò���
        @param[in] rid ��ԴID
        @version NIIEngine 3.0.0
        */
        void setMaterial(ResourceID rid);

        /** ��������
        @version NIIEngine 3.0.0
        */
        void buildGeo(const Plane & plane, NIIf scale = 1000, NIIf xtiling = 10,
            NIIf ytiling = 10, NIIi xsegments = 10, NIIi ysegments = 10, NIIf bow = 0);

        /** �����Ĵ�С����
        @return ����
        @version NIIEngine 3.0.0
        */
        inline NIIf getScale() const                { return mScale;  }

        /** ��������������
        @return ��������
        @version NIIEngine 3.0.0
        */
        inline NIIf getBow() const                  { return mBow;  }

        /** ��ȡ���ƽ��Ķ���
        @version NIIEngine 3.0.0
        */
        inline const Plane & getPlane() const       { return mPlane; }
    protected:
        /// @copydetails Sky::renderImpl
        virtual void renderImpl(RenderQueue * rq);
    protected:
        Plane mPlane;
        NIIf mScale;
        NIIf mXTiling;
        NIIf mYTiling;
        NIIi mXSegment;
        NIIi mYSegment;
        NIIf mBow;
        ResourceID mMaterial;
        ResourceID mMesh;
        GeometrySpace * mGeo;
    };
    
    /** һ��ģ�⼫�����
    @remark һ����������ϵͳ�޷����,��Ч�ʺܲ������ϵͳ�Ĵ��淽ʽ
    @version NIIEngine 3.0.0
    */
    class _EngineAPI VideoSky : public Sky
    {
    public:
        VideoSky();
        VideoSky(const SpaceManager * gm);
        virtual ~VideoSky();

        /** ������Ƶ��Դ
        @version NIIEngine 3.0.0
        */
        inline void setVideo(ResourceID rid)        { mVideoRID = rid; }

        /** ��ȡ��Ƶ��Դ
        @version NIIEngine 3.0.0
        */
        inline ResourceID getVideo() const          { return mVideoRID; }

        /** ��ȡƽ��Ķ���
        @version NIIEngine 3.0.0
        */
        inline const Plane & getPlane() const       { return mPlane; }

        /** ����ʱ������
        @version NIIEngine 3.0.0
        */
        inline void setTimeFactor(NIIf f)           { mTimeFactor = f; }

        /** ��ȡʱ������
        @version NIIEngine 3.0.0
        */
        inline NIIf getTimeFactor() const           { return mTimeFactor; }

        /** ��������
        @version NIIEngine 3.0.0
        */
        void buildGeo(const Plane & plane, NIIf scale = 1000, NIIf width = 1000, NIIf height = 1000);
    protected:
        /// @copydetails Sky::renderImpl
        virtual void renderImpl(RenderQueue * rq);
    protected:
        ResourceID mVideoRID;
        Plane mPlane;
        NIIf mTimeFactor;
        Texture * mGeo;
    };
}

#endif