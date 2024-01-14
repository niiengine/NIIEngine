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
#include "NiiPredefinedControllers.h"
#include "NiiMath.h"
#include "NiiLogManager.h"

namespace NII
{
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // TexCoordModifierControllerValue
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    TexCoordModifierControllerValue::TexCoordModifierControllerValue(bool autoDsy,
        ShaderChTextureUnit * t, bool translateU, bool translateV,
            bool scaleU, bool scaleV, bool rotate) :
                DataValue<NIIf>(autoDsy)
    {
        mTextureLayer = t;
        mTransU = translateU;
        mTransV = translateV;
        mScaleU = scaleU;
        mScaleV = scaleV;
        mRotate = rotate;
    }
    //-----------------------------------------------------------------------
    const NIIf TexCoordModifierControllerValue::getValue() const
    {
        const Matrix4f & pMat = mTextureLayer->getMatrix();
        if(mTransU)
        {
            return pMat[0][3];
        }
        else if(mTransV)
        {
            return pMat[1][3];
        }
        else if(mScaleU)
        {
            return pMat[0][0];
        }
        else if(mScaleV)
        {
            return pMat[1][1];
        }
        // Shouldn't get here
        return 0;
    }
    //-----------------------------------------------------------------------
    void TexCoordModifierControllerValue::setValue(const NIIf & value)
    {
        if(mTransU)
        {
            mTextureLayer->setUOft(value);
        }
        if(mTransV)
        {
            mTextureLayer->setVOft(value);
        }
        if(mScaleU)
        {
            mTextureLayer->setUScale(value);
        }
        if(mScaleV)
        {
            mTextureLayer->setVScale(value);
        }
        if(mRotate)
        {
            mTextureLayer->setRotate(Radian(value * Math::PI2X));
        }
    }
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // ScaleControllerFunction
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    ScaleControllerFunction::ScaleControllerFunction(bool autoDsy, NIIf factor) :
        DataFunc<TimeDurMS, NIIf>(autoDsy)
    {
        mScale = factor;
        mTemp = 0.0f;
    }
    //-----------------------------------------------------------------------
    NIIf ScaleControllerFunction::func(const TimeDurMS & source)
    {
        mTemp += (source * mScale) / 1000;

        while(mTemp >= 1.0)
            mTemp -= 1.0;
        while(mTemp < 0.0)
            mTemp += 1.0;

        return mTemp;
    }
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // WaveformControllerFunction
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    WaveformControllerFunction::WaveformControllerFunction(bool autoDsy,
        WaveformType wType, NIIf base, NIIf frequency, NIIf phase, NIIf amplitude,
        bool filter, NIIf dutyCycle) :
            DataFunc<TimeDurMS, NIIf>(autoDsy)
    {
        mWaveType = wType;
        mBase = base;
        mFrequency = frequency;
        mPhase = phase;
        mAmplitude = amplitude;
        mTemp = phase;
        mDutyCycle = dutyCycle;
        mDeltaInput = filter;
    }
    //-----------------------------------------------------------------------
    NIIf WaveformControllerFunction::func(const TimeDurMS & source)
    {
        NIIf input;

        if(mDeltaInput)
        {
            mTemp += source * mFrequency;

            while(mTemp >= 1.0)
                mTemp -= 1.0;
            while(mTemp < 0.0)
                mTemp += 1.0;

            input =  mTemp;
        }
        else
        {
            // If not delta, adjust by phase here
            // (delta inputs have it input at initialisation)
            input =  source * mFrequency + mPhase;
        }

        // For simplicity, factor input down to {0,1)
        // Use looped subtract rather than divide / round
        while(input >= 1.0)
            input -= 1.0;
        while(input < 0.0)
            input += 1.0;

        NIIf output = 0;

        // Calculate output in -1..1 range
        switch(mWaveType)
        {
        case WFT_SINE:
            output = Math::Sin(Radian(input * Math::PI2X));
            break;
        case WFT_TRIANGLE:
            if(input < 0.25)
                output = input * 4;
            else if(input >= 0.25 && input < 0.75)
                output = 1.0f - ((input - 0.25f) * 4.0f);
            else
                output = ((input - 0.75f) * 4.0f) - 1.0f;

            break;
        case WFT_SQUARE:
            if(input <= 0.5f)
                output = 1.0f;
            else
                output = -1.0f;
            break;
        case WFT_SAWTOOTH:
            output = (input * 2.0f) - 1.0f;
            break;
        case WFT_INVERSE_SAWTOOTH:
            output = -((input * 2.0f) - 1.0f);
            break;
        case WFT_PWM:
            if(input <= mDutyCycle)
                output = 1.0f;
            else
                output = -1.0f;
            break;
        }
        // Scale output into 0..1 range and then by base + amplitude
        return mBase + ((output + 1.0f) * 0.5f * mAmplitude);
    }
    //-----------------------------------------------------------------------
    bool WaveformControllerFunction::isValid() const
    {
        // don't create an effect if the given values are all 0
        if(mBase != 0.0f || mPhase != 0.0f || mFrequency != 0.0f || mAmplitude != 0.0f)
        {
            return true;
        }
        return false;
    }
    //-----------------------------------------------------------------------
}