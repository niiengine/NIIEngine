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
        /// ִ�в��Ų���
        void play();

        /// ִ��ֹͣ����
        void stop();

        /// ִ����ͣ����
        void pause();

        /// ��ȡ���������ǰ�Ƿ��ڲ���״̬��
        bool isPlaying() const;

        /// ��ȡ���������ǰ�Ƿ���ֹͣ״̬��
        bool isStoped() const;

        /// ��ȡ���������ǰ�Ƿ��ڲ���״̬��
        bool isPaused() const;

        /**  ���������Ƶ��ѭ������
        @param[in] count 1Ϊ����һ��,nΪ����n��,0Ϊ����Ϊ���޴�
        */
        void setLoop(Nui32 count = 1);

        /** ��ȡ�����Ƶ��ѭ������
        @remark 1Ϊ����һ��,nΪ����n��,0Ϊ����Ϊ���޴�
        */
        Nui32 getLoop() const;

        /// ���û��ָֹ���������
        void setEnable(FilterType type);

        /// �����Ƿ�ֱ�����
        bool getDisable(FilterType type) const;

        /// ����ֱ�����������
        void setFactor(FilterType type, NIIf gain = 1.0f);

        /// ��ȡֱ�����������
        NIIf getFactor(FilterType type) const;

        /// ���û��ֱֹ�����ͨ���Ĺ���
        void enableFilter(FilterType type);

        /// ��ȡֱ�����ͨ�������Ƿ�����
        bool isFilterEnabled(FilterType type) const;

        /// �������ֱ�����ͨ���Ĺ��˲���
        void setFilter(FilterType type, FilterPass filter, NIIf low = 250.0f,
            NIIf high = 5000.0f, NIIf Gain = 1.0f);

        /// ��ȡ���ֱ�����ͨ���Ĺ��˲���
        void getFilter(FilterType type, FilterPass & filter, 
            NIIf & low, NIIf & high, NIIf & Gain) const;

        /// ����ֱ�������������
        void setFilterPassFactor(FilterType type, NIIf passGain = 1.0f);

        /// ��ȡֱ�������������
        NIIf getFilterPassFactor(FilterType type) const;

        /// ���������Դ��Ч��, NULL���߽���.Ĭ��ΪNULL
        void setEffect(SoundEffect * effect) = 0;

        /// ��ȡ�����Դ��Ч��
        SoundEffect * getEffect() const = 0;

        /// ����Ч��ͨ��������
        void setEffectFactor(NIIf gain = 1.0f) = 0;

        /// ��ȡЧ��ͨ��������
        NIIf getEffectFactor() const;
    protected:
        Sound * mSrc;                   ///< ����Դ
        SoundEffect * mEffect;          ///< Ч����
        bool mDirect;                   ///< �Ƿ�ֱ�����
        NIIf mDirectFactor;             ///< ֱ���������������
        bool mDirectFilter;             ///< �Ƿ�����ֱ�����������
        FilterPass mDirectFilterPass;   ///< ֱ������Ĺ�����
        NIIf mDirectLow;                ///< ֱ�������Ƶ����
        NIIf mDirectHigh;               ///< ֱ�������Ƶ����
        NIIf mDirectTran;               ///< ��Ƶ����Ƶ֮�������
        NIIf mDirectFilterFactor;       ///< ֱ���������������
        NCount mLoop;                   ///< ѭ������
        bool mEffect;                   ///< �Ƿ�����Ч��
        NIIf mEffectFactor;             ///< Ч������������
        bool mEffectFilter;             ///<
        FilterPass mEffectFilterPass;   ///< Ч���Ĺ�����
        NIIf mEffectLow;                ///< Ч���ĵ�Ƶ����
        NIIf mEffectHigh;               ///< Ч���ĸ�Ƶ����
        NIIf mEffectTran;               ///< ��Ƶ����Ƶ֮�������
        NIIf mEffectFilterFactor;       ///< Ч��������������
    };
}
#endif