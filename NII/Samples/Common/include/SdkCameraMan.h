/*
 -----------------------------------------------------------------------------
 This source file is part of OGRE
 (Object-oriented Graphics Rendering Engine)
 For the latest info, see http://www.ogre3d.org/
 
 Copyright (c) 2000-2012 Torus Knot Software Ltd
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 -----------------------------------------------------------------------------
 */
#ifndef __SdkCameraMan_H__
#define __SdkCameraMan_H__

#include "Ogre.h"
#include <limits>

namespace OgreBites
{
    //------------------------------------------------------------------------
    enum CameraStyle   // enumerator values for different styles of camera movement
    {
        CS_FREELOOK,
        CS_ORBIT,
        CS_MANUAL
    };
    /*========================================================================
    | Utility class for controlling the camera in samples.
    ========================================================================*/
    class SdkCameraMan
    {
    public:
        SdkCameraMan(Ogre::Camera * cam) : 
            mCamera(0), 
            mTarget(0), 
            mOrbiting(false), 
            mZooming(false), 
            mTopSpeed(150), 
            mVelocity(Ogre::Vector3::ZERO), 
            mGoingForward(false), 
            mGoingBack(false), 
            mGoingLeft(false), 
            mGoingRight(false), 
            mGoingUp(false), 
            mGoingDown(false), 
            mFastMove(false)
        {

            setCamera(cam);
            setStyle(CS_FREELOOK);
        }

        virtual ~SdkCameraMan() {}

        /*-----------------------------------------------------------------------------
        | Swaps the camera on our camera man for another camera.
        -----------------------------------------------------------------------------*/
        virtual void setCamera(Ogre::Camera* cam)
        {
            mCamera = cam;
        }

        virtual Ogre::Camera* getCamera()
        {
            return mCamera;
        }
        /*-----------------------------------------------------------------------------
        | Sets the target we will revolve around. Only applies for orbit style.
        -----------------------------------------------------------------------------*/
        virtual void setTarget(Ogre::SpaceNode * target)
        {
            if(target != mTarget)
            {
                mTarget = target;
                if(target)
                {
                    setYawPitchDist(Ogre::Angle(0), Ogre::Angle(15), 150);
                    mCamera->setAutoFocus(true);
                    mCamera->setAutoFocus(mTarget);
                }
                else
                {
                    mCamera->setAutoFocus(false);
                }
            }
        }

        virtual Ogre::SpaceNode * getTarget()
        {
            return mTarget;
        }
        /*-----------------------------------------------------------------------------
        | Sets the spatial offset from the target. Only applies for orbit style.
        -----------------------------------------------------------------------------*/
        virtual void setYawPitchDist(Ogre::Radian yaw, Ogre::Radian pitch, Ogre::NIIf dist)
        {
            mCamera->setPos(mTarget->getSpacePos());
            mCamera->setOri(mTarget->getSpaceOri());
            mCamera->yaw(yaw);
            mCamera->pitch(-pitch);
            mCamera->moveRel(Ogre::Vector3(0, 0, dist));
        }

        /*-----------------------------------------------------------------------------
        | Sets the camera's top speed. Only applies for free-look style.
        -----------------------------------------------------------------------------*/
        virtual void setTopSpeed(Ogre::NIIf topSpeed)
        {
            mTopSpeed = topSpeed;
        }

        virtual Ogre::NIIf getTopSpeed()
        {
            return mTopSpeed;
        }

        /*-----------------------------------------------------------------------------
        | Sets the movement style of our camera man.
        -----------------------------------------------------------------------------*/
        virtual void setStyle(CameraStyle style)
        {
            if(mStyle != CS_ORBIT && style == CS_ORBIT)
            {
                setTarget(mTarget ? mTarget : mCamera->getManager()->getOriginNode());
                mCamera->setCustomYaw(true);
                manualStop();
                setYawPitchDist(Ogre::Angle(0), Ogre::Angle(15), 150);

            }
            else if(mStyle != CS_FREELOOK && style == CS_FREELOOK)
            {
                mCamera->setAutoFocus(false);
                mCamera->setCustomYaw(true);
            }
            else if(mStyle != CS_MANUAL && style == CS_MANUAL)
            {
                mCamera->setAutoFocus(false);
                manualStop();
            }
            mStyle = style;
        }

        virtual CameraStyle getStyle()
        {
            return mStyle;
        }

        /*-----------------------------------------------------------------------------
        | Manually stops the camera when in free-look mode.
        -----------------------------------------------------------------------------*/
        virtual void manualStop()
        {
            if (mStyle == CS_FREELOOK)
            {
                mGoingForward = false;
                mGoingBack = false;
                mGoingLeft = false;
                mGoingRight = false;
                mGoingUp = false;
                mGoingDown = false;
                mVelocity = Ogre::Vector3::ZERO;
            }
        }

        virtual bool onRenderCore(const Ogre::RenderFrameArgs * evt)
        {
            if (mStyle == CS_FREELOOK)
            {
                // build our acceleration vector based on keyboard input composite
                Ogre::Vector3 accel = Ogre::Vector3::ZERO;
                if (mGoingForward) accel += mCamera->getDirection();
                if (mGoingBack) accel -= mCamera->getDirection();
                if (mGoingRight) accel += mCamera->getRight();
                if (mGoingLeft) accel -= mCamera->getRight();
                if (mGoingUp) accel += mCamera->getUp();
                if (mGoingDown) accel -= mCamera->getUp();

                // if accelerating, try to reach top speed in a certain time
                Ogre::NIIf topSpeed = mFastMove ? mTopSpeed * 20 : mTopSpeed;
                if (accel.lengthSquared() != 0)
                {
                    accel.normalise();
                    mVelocity += accel * topSpeed * evt->mCurrent * 10;
                }
                // if not accelerating, try to stop in a certain time
                else mVelocity -= mVelocity * evt->mCurrent * 10;

                Ogre::NIIf tooSmall = std::numeric_limits<Ogre::NIIf>::epsilon();

                // keep camera velocity below top speed and above epsilon
                if(mVelocity.lengthSquared() > topSpeed * topSpeed)
                {
                    mVelocity.normalise();
                    mVelocity *= topSpeed;
                }
                else if(mVelocity.lengthSquared() < tooSmall * tooSmall)
                    mVelocity = Ogre::Vector3::ZERO;

                if(mVelocity != Ogre::Vector3::ZERO) 
                    mCamera->movePos(mVelocity * evt->mCurrent);
            }

            return true;
        }

        /*-----------------------------------------------------------------------------
        | Processes key presses for free-look style movement.
        -----------------------------------------------------------------------------*/
        virtual void injectKeyDown(const OIS::KeyEvent& evt)
        {
            if (mStyle == CS_FREELOOK)
            {
                if (evt.key == OIS::KC_W || evt.key == OIS::KC_UP) mGoingForward = true;
                else if (evt.key == OIS::KC_S || evt.key == OIS::KC_DOWN) mGoingBack = true;
                else if (evt.key == OIS::KC_A || evt.key == OIS::KC_LEFT) mGoingLeft = true;
                else if (evt.key == OIS::KC_D || evt.key == OIS::KC_RIGHT) mGoingRight = true;
                else if (evt.key == OIS::KC_PGUP) mGoingUp = true;
                else if (evt.key == OIS::KC_PGDOWN) mGoingDown = true;
                else if (evt.key == OIS::KC_LSHIFT) mFastMove = true;
            }
        }

        /*-----------------------------------------------------------------------------
        | Processes key releases for free-look style movement.
        -----------------------------------------------------------------------------*/
        virtual void injectKeyUp(const OIS::KeyEvent& evt)
        {
            if (mStyle == CS_FREELOOK)
            {
                if (evt.key == OIS::KC_W || evt.key == OIS::KC_UP) mGoingForward = false;
                else if (evt.key == OIS::KC_S || evt.key == OIS::KC_DOWN) mGoingBack = false;
                else if (evt.key == OIS::KC_A || evt.key == OIS::KC_LEFT) mGoingLeft = false;
                else if (evt.key == OIS::KC_D || evt.key == OIS::KC_RIGHT) mGoingRight = false;
                else if (evt.key == OIS::KC_PGUP) mGoingUp = false;
                else if (evt.key == OIS::KC_PGDOWN) mGoingDown = false;
                else if (evt.key == OIS::KC_LSHIFT) mFastMove = false;
            }
        }

        /*-----------------------------------------------------------------------------
        | Processes mouse movement differently for each style.
        -----------------------------------------------------------------------------*/
#if (OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS) || (OGRE_PLATFORM == OGRE_PLATFORM_ANDROID)
        virtual void injectMouseMove(const OIS::MultiTouchEvent& evt)
#else
        virtual void injectMouseMove(const OIS::MouseEvent& evt)
#endif
        {
            if (mStyle == CS_ORBIT)
            {
                Ogre::NIIf dist = (mCamera->getPos() - mTarget->getSpacePos()).length();

                if (mOrbiting)   // yaw around the target, and pitch locally
                {
                    mCamera->setPos(mTarget->getSpacePos());

                    mCamera->yaw(Ogre::Angle(-evt.state.X.rel * 0.25f));
                    mCamera->pitch(Ogre::Angle(-evt.state.Y.rel * 0.25f));

                    mCamera->moveRel(NII::Vector3(0, 0, dist));

                    // don't let the camera go over the top or around the bottom of the target
                }
                else if (mZooming)  // move the camera toward or away from the target
                {
                    // the further the camera is, the faster it moves
                    mCamera->moveRel(NII::Vector3(0, 0, evt.state.Y.rel * 0.004f * dist));
                }
                else if (evt.state.Z.rel != 0)  // move the camera toward or away from the target
                {
                    // the further the camera is, the faster it moves
                    mCamera->moveRel(NII::Vector3(0, 0, -evt.state.Z.rel * 0.0008f * dist));
                }
            }
            else if (mStyle == CS_FREELOOK)
            {
                mCamera->yaw(Ogre::Angle(-evt.state.X.rel * 0.15f));
                mCamera->pitch(Ogre::Angle(-evt.state.Y.rel * 0.15f));
            }
        }
        /*-----------------------------------------------------------------------------
        | Processes mouse presses. Only applies for orbit style.
        | Left button is for orbiting, and right button is for zooming.
        -----------------------------------------------------------------------------*/
#if (OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS) || (OGRE_PLATFORM == OGRE_PLATFORM_ANDROID)
        virtual void injectMouseDown(const OIS::MultiTouchEvent& evt)
        {
            if(mStyle == CS_ORBIT)
            {
                mOrbiting = true;
            }
        }
#else
        virtual void injectMouseDown(const OIS::MouseEvent& evt, OIS::MouseButtonID id)
        {
            if (mStyle == CS_ORBIT)
            {
                if(id == OIS::MB_Left) 
                    mOrbiting = true;
                else if(id == OIS::MB_Right) 
                    mZooming = true;
            }
        }
#endif

        /*-----------------------------------------------------------------------------
        | Processes mouse releases. Only applies for orbit style.
        | Left button is for orbiting, and right button is for zooming.
        -----------------------------------------------------------------------------*/
#if (OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS) || (OGRE_PLATFORM == OGRE_PLATFORM_ANDROID)
        virtual void injectMouseUp(const OIS::MultiTouchEvent& evt)
        {
            if(mStyle == CS_ORBIT)
            {
                mOrbiting = false;
            }
        }
#else
        virtual void injectMouseUp(const OIS::MouseEvent& evt, OIS::MouseButtonID id)
        {
            if(mStyle == CS_ORBIT)
            {
                if(id == OIS::MB_Left) 
                    mOrbiting = false;
                else if(id == OIS::MB_Right) 
                    mZooming = false;
            }
        }
#endif

    protected:

        NII::Camera * mCamera;
        CameraStyle mStyle;
        NII::SpaceNode * mTarget;
        bool mOrbiting;
        bool mZooming;
        NII::NIIf mTopSpeed;
        NII::Vector3 mVelocity;
        bool mGoingForward;
        bool mGoingBack;
        bool mGoingLeft;
        bool mGoingRight;
        bool mGoingUp;
        bool mGoingDown;
        bool mFastMove;
    };
}

#endif
