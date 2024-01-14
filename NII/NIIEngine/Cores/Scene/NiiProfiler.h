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

#ifndef _NII_Profiler_H__
#define _NII_Profiler_H__

#include "NiiPreInclude.h"
#include "NiiSingleton.h"
#include "NiiString.h"
#include "NiiOverlay.h"

#if NII_PROFILING == 1
    #define NiiProfile(a) Profile _NiiProfileInstance((a))
    #define NiiProfileBegin(a) Profiler::getOnly().beginProfile((a))
    #define NiiProfileEnd(a) Profiler::getOnly().endProfile((a))
    #define NiiProfileGroup(a, g) Profile _NiiProfileInstance((a), (g))
    #define NiiProfileBeginGroup(a, g) Profiler::getOnly().beginProfile((a), (g))
    #define NiiProfileEndGroup(a, g) Profiler::getOnly().endProfile((a), (g))
#else
    #define NiiProfile(a)
    #define NiiProfileBegin(a)
    #define NiiProfileEnd(a)
    #define NiiProfileGroup(a, g)
    #define NiiProfileBeginGroup(a, g)
    #define NiiProfileEndGroup(a, g)
#endif

namespace NII
{
    /** 保存档案标志列表
    */
    enum ProfileGroupMask
    {
        /// User default profile
        NIIPROF_USER_DEFAULT = 0x00000001,
        /// All in-built Nii profiling will match this mask
        NIIPROF_ALL = 0xFF000000,
        /// General processing
        NIIPROF_GENERAL = 0x80000000,
        /// Culling
        NIIPROF_CULLING = 0x40000000,
        /// Rendering
        NIIPROF_RENDERING = 0x20000000
    };

    /** 可通过Profiler处理的个人概述
    @remark
        Use the macro NiiProfile(name) instead of instantiating this profile directly
    @remark
        We use this Profile to allow scoping rules to signify the beginning and end of
        the profile. Use the Profiler singleton (through the macro NiiProfileBegin(name)
        and NiiProfileEnd(name)) directly if you want a profile to last
        outside of a scope (i.e. the main game loop).
    @author Amit Mathew (amitmathew (at) yahoo (dot) com)
    */
    class _EngineAPI Profile : public ProfilerAlloc
    {
    public:
        Profile(const String & profileName, Nui32 groupID = (Nui32)NIIPROF_USER_DEFAULT);
        ~Profile();
    protected:
        String mName;           ///< 这个概述的名字
        Nui32 mGroupID;         ///< 群组ID
    };

    /** 该分析器允许你来衡量你的代码的性能
    @remark
        Do not create profiles directly from this unless you want a profile to last
        outside of its scope (i.e. the main game loop). For most cases, use the macro
        NiiProfile(name) and braces to limit the scope. You must enable the Profile
        before you can used it with setEnable(true). If you want to disable profiling
        in Nii, simply set the macro NII_PROFILING to 0.
    @author Amit Mathew (amitmathew (at) yahoo (dot) com)
    @todo resolve artificial cap on number of profiles displayed
    @todo fix display ordering of profiles not called every frame
    */
    class _EngineAPI Profiler : public Singleton<Profiler>, public ProfilerAlloc
    {
    public:
        Profiler();
        ~Profiler();

        /// 为分析器设置计时器
        void setTimer(Timer * t);

        /// 获取为分析器的计时器
        Timer * getTimer();

        /** 开始一个配置
        @remark
            Use the macro NiiProfileBegin(name) instead of calling this directly
            so that profiling can be ignored in the release version of your app.
        @remark
            You only use the macro (or this) if you want a profile to last outside
            of its scope (i.e. the main game loop). If you use this function, make sure you
            use a corresponding NiiProfileEnd(name). Usually you would use the macro
            NiiProfile(name). This function will be ignored for a profile that has been
            disabled or if the profiler is disabled.
        @param[in] profileName Must be unique and must not be an empty string
        @param[in] groupID A profile group identifier, which can allow you to mask profiles
        */
        void beginProfile(const String & profileName, Nui32 groupID = (Nui32)NIIPROF_USER_DEFAULT);

        /** 结束一个配置
        @remark
            Use the macro NiiProfileEnd(name) instead of calling this directly so that
            profiling can be ignored in the release version of your app.
        @remark
            This function is usually not called directly unless you want a profile to
            last outside of its scope. In most cases, using the macro NiiProfile(name)
            which will call this function automatically when it goes out of scope. Make
            sure the name of this profile matches its corresponding beginProfile name.
            This function will be ignored for a profile that has been disabled or if the
            profiler is disabled.
        @param[in] profileName Must be unique and must not be an empty string
        @param[in] groupID A profile group identifier, which can allow you to mask profiles
        */
        void endProfile(const String & profileName, Nui32 groupID = (Nui32)NIIPROF_USER_DEFAULT);

        /** 设置这个分析器是否启用,只有帧结束后才有效
        @remark When this is called the first time with the parameter true,
            it initializes the GUI for the Profiler
        */
        void setEnable(bool set);

        /// 获取这个分析器是否启用
        bool isEnable() const;

        /** 启用一个之前禁用的配置
        @remark Only enables the profile if this function is not
            called during the profile it is trying to enable.
        */
        void enableProfile(const String & profileName);

        /** 禁用一个配置
        @remark Only disables the profile if this function is not called during
            the profile it is trying to disable.
        */
        void disableProfile(const String& profileName);

        /// 设置标志,所有配置必须通过才能启用
        void setProfileGroupMask(Nui32 mask) { mProfileMask = mask; }

        /// 获取标志,所有配置必须通过才能启用
        Nui32 getProfileGroupMask() const { return mProfileMask; }

        /** 返回真,如果指定配置达到一个新的帧时间最大值
        @remark
            If this is called during a frame, it will be reading the results
            from the previous frame. Therefore, it is best to use this after the frame
            has ended.
        */
        bool watchForMax(const String & profileName);

        /** 返回真,如果指定配置达到一个新的帧时间最小值
        @remark
            If this is called during a frame, it will be reading the results
            from the previous frame. Therefore, it is best to use this after the frame
            has ended.
        */
        bool watchForMin(const String & profileName);

        /** 返回真 如果指定配置超过或低于给定的限制帧时间
        @remark
            If this is called during a frame, it will be reading the results
            from the previous frame. Therefore, it is best to use this after the frame
            has ended.
        @param[in] limit A number between 0 and 1 representing the percentage of frame time
        @param[in] greaterThan If true, this will return whether the limit is exceeded. Otherwise,
            it will return if the frame time has gone under this limit.
        */
        bool watchForLimit(const String & profileName, NIIf limit, bool greaterThan = true);

        /// 输出当前配置统计到日志中
        void logResults();

        /// 清除配置统计
        void reset();

        enum DisplayMode
        {
            /// Display % frame usage on the overlay
            DISPLAY_PERCENTAGE,
            /// Display milliseconds on the overlay
            DISPLAY_MILLISECONDS
        };

        /// 设置覆盖层的显示模式
        void setDisplayMode(DisplayMode d) { mDisplayMode = d; }

        /// 获取覆盖层的显示模式
        DisplayMode getDisplayMode() const { return mDisplayMode; }

        /// 设置分析器没N帧更新显示结果
        void setUpdateDisplayFrequency(Nui32 freq);

        /// 获取分析器没N帧更新显示结果
        Nui32 getUpdateDisplayFrequency() const;

        /// 设置分析器覆盖层大小,以像素为单位
        void setOverlayDimensions(NIIf width, NIIf height);

        /// 设置分析器覆盖层位置,以像素为单位
        void setOverlayPosition(NIIf left, NIIf top);

        /**
        @version NIIEngine 3.0.0
        */
        NIIf getOverlayWidth() const;
        
        /**
        @version NIIEngine 3.0.0
        */
        NIIf getOverlayHeight() const;
        
        /**
        @version NIIEngine 3.0.0
        */
        NIIf getOverlayLeft() const;
        
        /**
        @version NIIEngine 3.0.0
        */
        NIIf getOverlayTop() const;

        /// @copydetails Singleton::getOnly
        static Profiler & getOnly();

        /// @copydetails Singleton::getOnlyPtr
        static Profiler * getOnlyPtr();
    protected:
        /// 初始化分析器的GUI元素
        void initialize();

        /// 打印每帧的分析结果
        void displayResults();

        /// 处理每帧分析器数据
        void processFrameStats();

        /// 改变分析器启用状态的句柄
        void changeEnableState();

        /// 内部函数,建立保存我们显示元素的容器
        OverlayView * createContainer();

        /// 内部函数创建一个文字区域
        OverlayGeo * createTextArea(OverlayGeoID name, NIIf width, NIIf height, 
            NIIf top, NIIf left,  Nui32 fontSize, const String & caption, bool show = true);

        /// 内部函数创建一个面板
        OverlayGeo * createPanel(OverlayGeoID name, NIIf width, NIIf height,
            NIIf top, NIIf left, ResourceID materialid, bool show = true);
    protected:
        /// 描述一个个人配置调用
        struct ProfileInstance
        {
            String name;            ///< The name of the profile
            String parent;          ///< The name of the parent, empty string if root
            Nul currTime;           ///< The time this profile was started
            Nul accum;              ///< Represents the total time of all child profiles to subtract from this profile
            Nui32 hierarchicalLvl;  ///< The hierarchical level of this profile, 0 being the root profile
        };

        /// 描述配置总时间,当配置调用超过一帧以上
        struct ProfileFrame
        {
            String name;            ///< The name of the profile
            Nul frameTime;        ///< The total time this profile has taken this frame
            Nui32 calls;            ///< The number of times this profile was called this frame
            Nui32 hierarchicalLvl;  ///< The hierarchical level of this profile, 0 being the main loop
        };

        /// 描述每个配置在应用时的一个历史结构
        struct ProfileHistory
        {
            /// The name of the profile
            String name;

            /// The current percentage of frame time this profile has taken
            NIIf currentTimePercent;
            /// The current frame time this profile has taken in milliseconds
            NIIf currentTimeMillisecs;

            /// The maximum percentage of frame time this profile has taken
            NIIf maxTimePercent;
            /// The maximum frame time this profile has taken in milliseconds
            NIIf maxTimeMillisecs;

            /// The minimum percentage of frame time this profile has taken
            NIIf minTimePercent;
            /// The minimum frame time this profile has taken in milliseconds
            NIIf  minTimeMillisecs;

            /// The number of times this profile has been called each frame
            Nui32 numCallsThisFrame;

            /// The total percentage of frame time this profile has taken
            NIIf totalTimePercent;
            /// The total frame time this profile has taken in milliseconds
            NIIf totalTimeMillisecs;

            /// The total number of times this profile was called
            /// (used to calculate average)
            Nul totalCalls;

            /// The hierarchical level of this profile, 0 being the root profile
            Nui32 hierarchicalLvl;
        };

        typedef list<ProfileInstance>::type ProfileStack;
        typedef list<ProfileFrame>::type ProfileFrameList;
        typedef list<ProfileHistory>::type ProfileHistoryList;
        typedef map<String, ProfileHistoryList::iterator>::type ProfileHistoryMap;
        typedef map<String, bool>::type DisabledProfileMap;

        typedef list<OverlayGeo*>::type ProfileBarList;

        /// A stack for each individual profile per frame
        ProfileStack mProfiles;

        /// Accumulates the results of each profile per frame (since a profile can be called
        /// more than once a frame)
        ProfileFrameList mProfileFrame;

        /// Keeps track of the statistics of each profile
        ProfileHistoryList mProfileHistory;

        /// We use this for quick look-ups of profiles in the history list
        ProfileHistoryMap mProfileHistoryMap;

        /// Holds the names of disabled profiles
        DisabledProfileMap mDisabledProfiles;

        /// Holds the display bars for each profile results
        ProfileBarList mProfileBars;

        /// Whether the GUI elements have been initialized
        bool mInitialized;

        /// The max number of profiles we can display
        Nui32 mMaxDisplayProfiles;

        /// The overlay which contains our profiler results display
        Overlay * mOverlay;

        /// The window that displays the profiler results
        OverlayView * mProfileGui;

        /// The height of each bar
        NIIf mBarHeight;

        /// The height of the stats window
        NIIf mGuiHeight;

        /// The width of the stats window
        NIIf mGuiWidth;

        /// The horz position of the stats window
        NIIf mGuiLeft;

        /// The vertical position of the stats window
        NIIf mGuiTop;

        /// The size of the indent for each profile display bar
        NIIf mBarIndent;

        /// The width of the border between the profile window and each bar
        NIIf mGuiBorderWidth;

        /// The width of the min, avg, and max lines in a profile display
        NIIf mBarLineWidth;

        /// The distance between bars
        NIIf mBarSpacing;

        /// The number of frames that must elapse before the current
        /// frame display is updated
        Nui32 mUpdateDisplayFrequency;

        /// The number of elapsed frame, used with mUpdateDisplayFrequency
        Nui32 mCurrentFrame;

        /// The timer used for profiling
        Timer * mTimer;

        /// The total time each frame takes
        Nul mTotalFrameTime;

        /// Whether this profiler is enabled
        bool mEnabled;

        /// Keeps track of whether this profiler has
        /// received a request to be enabled/disabled
        bool mEnableStateChangePending;

        /// Keeps track of the new enabled/disabled state that the user has requested
        /// which will be applied after the frame ends
        bool mNewEnableState;

        /// Mask to decide whether a type of profile is enabled or not
        Nui32 mProfileMask;

        /// How to display the overlay
        DisplayMode mDisplayMode;

        /// The max frame time recorded
        Nul mMaxTotalFrameTime;

        /// Rolling average of millisecs
        NIIf mAverageFrameTime;
        bool mResetExtents;
    }; // end class
}
#endif