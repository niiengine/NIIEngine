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

#ifndef _NII_SOUND_UNITSTATE_H_
#define _NII_SOUND_UNITSTATE_H_

#include "NiiPreInclude.h"

namespace NII
{
    class _EngineAPI SoundUnitState :
    {
    public:
        enum FilterType
        {
            FT_Direct,
            FT_Effect
        };

        enum FilterPass
        {
            FP_low,
            FP_band,
            FP_high
        };

    public:
        SoundUnitState();
        ~SoundUnitState();
        /// 执行播放操作
        void play();

        /// 执行停止操作
        void stop();

        /// 执行暂停操作
        void pause();

        /// 获取这个声音当前是否处于播放状态中
        bool isPlaying() const;

        /// 获取这个声音当前是否处于停止状态中
        bool isStoped() const;

        /// 获取这个声音当前是否处于播放状态中
        bool isPaused() const;

        /**  设置这个音频的循环次数
        @param[in] count 1为播放一次,n为播放n次,0为播放为无限次
        */
        void setLoop(Nui32 count = 1);

        /** 获取这个音频的循环次数
        @remark 1为播放一次,n为播放n次,0为播放为无限次
        */
        Nui32 getLoop() const;

        /// 启用或禁止指定输出类型
        void setEnable(FilterType type);

        /// 返回是否直接输出
        bool getDisable(FilterType type) const;

        /// 设置直接输出的增量
        void setFactor(FilterType type, NIIf gain = 1.0f);

        /// 获取直接输出的增量
        NIIf getFactor(FilterType type) const;

        /// 启用或禁止直接输出通道的过滤
        void enableFilter(FilterType type);

        /// 获取直接输出通道过滤是否启用
        bool isFilterEnabled(FilterType type) const;

        /// 设置这个直接输出通道的过滤参数
        void setFilter(FilterType type, FilterPass filter, NIIf low = 250.0f,
            NIIf high = 5000.0f, NIIf Gain = 1.0f);

        /// 获取这个直接输出通道的过滤参数
        void getFilter(FilterType type, FilterPass & filter, 
            NIIf & low, NIIf & high, NIIf & Gain) const;

        /// 设置直接输出过滤增量
        void setFilterPassFactor(FilterType type, NIIf passGain = 1.0f);

        /// 获取直接输出过滤增量
        NIIf getFilterPassFactor(FilterType type) const;

        /// 设置这个资源的效果, NULL或者禁用.默认为NULL
        void setEffect(SoundEffect * effect) = 0;

        /// 获取这个资源的效果
        SoundEffect * getEffect() const = 0;

        /// 设置效果通道的增量
        void setEffectFactor(NIIf gain = 1.0f) = 0;

        /// 获取效果通道的增量
        NIIf getEffectFactor() const;
    protected:
        Sound * mSrc;                   ///< 声音源
        SoundEffect * mEffect;          ///< 效果器
        bool mDirect;                   ///< 是否直接输出
        NIIf mDirectFactor;             ///< 直接输出的增量因子
        bool mDirectFilter;             ///< 是否启用直接输出过滤器
        FilterPass mDirectFilterPass;   ///< 直接输出的过滤器
        NIIf mDirectLow;                ///< 直接输出低频因子
        NIIf mDirectHigh;               ///< 直接输出高频因子
        NIIf mDirectTran;               ///< 高频到低频之间的增量
        NIIf mDirectFilterFactor;       ///< 直接输出过滤器因子
        NCount mLoop;                   ///< 循环次数
        bool mEffect;                   ///< 是否启用效果
        NIIf mEffectFactor;             ///< 效果的增量因子
        bool mEffectFilter;             ///<
        FilterPass mEffectFilterPass;   ///< 效果的过滤器
        NIIf mEffectLow;                ///< 效果的低频因子
        NIIf mEffectHigh;               ///< 效果的高频因子
        NIIf mEffectTran;               ///< 高频到低频之间的增量
        NIIf mEffectFilterFactor;       ///< 效果过滤器的因子
    };
}
#endif