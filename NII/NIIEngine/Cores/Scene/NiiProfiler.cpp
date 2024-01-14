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
#include "NiiProfiler.h"
#include "NiiOverlayTypeList.h"

/*
    Although the code is original, many of the ideas for the profiler were borrowed from
"Real-Time In-Game Profiling" by Steve Rabin which can be found in Game Programming
Gems 1.

    This code can easily be adapted to your own non-Nii project. The only code that is
Nii-dependent is in the visualization/logging routines and the use of the Timer class.

    Enjoy!

*/

#include "NiiTimer.h"
#include "NiiLogManager.h"
#include "NiiStrConv.h"
#include "NiiOverlayManager.h"

#define N_RES_DEFINE_ProfilerCurrent    0// "Core/ProfilerCurrent"
#define N_RES_DEFINE_ProfilerMin        0// "Core/ProfilerMin"
#define N_RES_DEFINE_ProfilerMax        0// "Core/ProfilerMax"
#define N_RES_DEFINE_ProfilerAvg        0// "Core/ProfilerAvg"
#define N_RES_DEFINE_StatsBlockCenter    0// "Core/StatsBlockCenter"

namespace NII
{
    //-----------------------------------------------------------------------
    // PROFILE DEFINITIONS
    //-----------------------------------------------------------------------
    template<> Profiler * Singleton<Profiler>::mOnly = 0;
    Profiler * Profiler::getOnlyPtr()
    {
        return mOnly;
    }
    //-----------------------------------------------------------------------
    Profiler & Profiler::getOnly()
    {
        N_assert(mOnly, "error");
        return (*mOnly);
    }
    //-----------------------------------------------------------------------
    Profile::Profile(const String & profileName, Nui32 groupID) :
        mName(profileName),
        mGroupID(groupID)
    {
        Profiler::getOnly().beginProfile(profileName, groupID);
    }
    //-----------------------------------------------------------------------
    Profile::~Profile()
    {
        Profiler::getOnly().endProfile(mName, mGroupID);
    }
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // PROFILER DEFINITIONS
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    Profiler::Profiler() :
        mInitialized(false),
        mMaxDisplayProfiles(50),
        mOverlay(0),
        mProfileGui(0),
        mBarHeight(10),
        mGuiHeight(25),
        mGuiWidth(250),
        mGuiLeft(0),
        mGuiTop(0),
        mBarIndent(250),
        mGuiBorderWidth(10),
        mBarLineWidth(2),
        mBarSpacing(3),
        mUpdateDisplayFrequency(10),
        mCurrentFrame(0),
        mTimer(0),
        mTotalFrameTime(0),
        mEnabled(false),
        mEnableStateChangePending(false),
        mNewEnableState(false),
        mProfileMask(0xFFFFFFFF),
        mDisplayMode(DISPLAY_MILLISECONDS),
        mMaxTotalFrameTime(0),
        mAverageFrameTime(0),
        mResetExtents(false){}
    //-----------------------------------------------------------------------
    Profiler::~Profiler()
    {
        if (!mProfileHistory.empty())
        {
            // log the results of our profiling before we quit
            logResults();
        }

        // clear all our lists
        mProfiles.clear();
        mProfileFrame.clear();
        mProfileHistoryMap.clear();
        mProfileHistory.clear();
        mDisabledProfiles.clear();
        mProfileBars.clear();
    }
    //---------------------------------------------------------------------
    void Profiler::setOverlayDimensions(NIIf width, NIIf height)
    {
        mGuiWidth = width;
        mGuiHeight = height;
        mBarIndent = mGuiWidth;

        mProfileGui->setSize(width, height);
    }
    //---------------------------------------------------------------------
    void Profiler::setOverlayPosition(NIIf left, NIIf top)
    {
        mGuiLeft = left;
        mGuiTop = top;

        mProfileGui->setPos(left, top);
    }
    //---------------------------------------------------------------------
    NIIf Profiler::getOverlayWidth() const
    {
        return mGuiWidth;
    }
    //---------------------------------------------------------------------
    NIIf Profiler::getOverlayHeight() const
    {
        return mGuiHeight;
    }
    //---------------------------------------------------------------------
    NIIf Profiler::getOverlayLeft() const
    {
        return mGuiLeft;
    }
    //---------------------------------------------------------------------
    NIIf Profiler::getOverlayTop() const
    {
        return mGuiTop;
    }
    //---------------------------------------------------------------------
    void Profiler::initialize()
    {
        // create a new overlay to hold our Profiler display
        //mOverlay = N_Only(Overlay).create(0, "Profiler");
        mOverlay = N_Only(Overlay).create(0, 0);
        mOverlay->setZOrder(500);

        // this panel will be the main container for our profile bars
        mProfileGui = createContainer();

        OverlayGeo * element;

        // we create an initial pool of 50 profile bars
        for (NCount i = 0; i < mMaxDisplayProfiles; ++i)
        {
            // this is for the profile name and the number of times it was called in a frame
            element = createTextArea(0/*"profileText" + StrConv::conv(i) */, 90, mBarHeight, mGuiBorderWidth + (mBarHeight + mBarSpacing) * i, 0, 14, _T(""), false);
            mProfileGui->add(element);
            mProfileBars.push_back(element);

            // this indicates the current frame time
            element = createPanel(0/*"currBar" + StrConv::conv(i)*/, 0, mBarHeight, mGuiBorderWidth + (mBarHeight + mBarSpacing) * i, mBarIndent, N_RES_DEFINE_ProfilerCurrent/*"Core/ProfilerCurrent" */, false);
            mProfileGui->add(element);
            mProfileBars.push_back(element);

            // this indicates the minimum frame time
            element = createPanel(0/*"minBar" + StrConv::conv(i) */, mBarLineWidth, mBarHeight, mGuiBorderWidth + (mBarHeight + mBarSpacing) * i, 0, N_RES_DEFINE_ProfilerMin/*"Core/ProfilerMin" */, false);
            mProfileGui->add(element);
            mProfileBars.push_back(element);

            // this indicates the maximum frame time
            element = createPanel(0/*"maxBar" + StrConv::conv(i)*/, mBarLineWidth, mBarHeight, mGuiBorderWidth + (mBarHeight + mBarSpacing) * i, 0, N_RES_DEFINE_ProfilerMax/*"Core/ProfilerMax" */, false);
            mProfileGui->add(element);
            mProfileBars.push_back(element);

            // this indicates the average frame time
            element = createPanel(0/*"avgBar" + StrConv::conv(i)*/, mBarLineWidth, mBarHeight, mGuiBorderWidth + (mBarHeight + mBarSpacing) * i, 0, N_RES_DEFINE_ProfilerAvg/*"Core/ProfilerAvg" */, false);
            mProfileGui->add(element);
            mProfileBars.push_back(element);

            // this indicates the text of the frame time
            element = createTextArea(0/*"statText" + StrConv::conv(i)*/, 20, mBarHeight, mGuiBorderWidth + (mBarHeight + mBarSpacing) * i, 0, 14, _T(""), false);
            mProfileGui->add(element);
            mProfileBars.push_back(element);
        }

        // throw everything all the GUI stuff into the overlay and display it
        mOverlay->add(mProfileGui);
        mOverlay->show();
    }
    //-----------------------------------------------------------------------
    void Profiler::setTimer(Timer * t)
    {
        mTimer = t;
    }
    //-----------------------------------------------------------------------
    Timer* Profiler::getTimer()
    {
        N_assert(mTimer, "Timer not set!");
        return mTimer;
    }
    //-----------------------------------------------------------------------
    void Profiler::setEnable(bool enabled)
    {
        if (!mInitialized && enabled)
        {
            // the user wants to enable the Profiler for the first time
            // so we initialize the GUI stuff
            initialize();
            mInitialized = true;
            mEnabled = true;
        }
        else
        {
            // We store this enable/disable request until the frame ends
            // (don't want to screw up any open profiles!)
            mEnableStateChangePending = true;
            mNewEnableState = enabled;
        }
    }
    //-----------------------------------------------------------------------
    bool Profiler::isEnable() const
    {
        return mEnabled;
    }
    //-----------------------------------------------------------------------
    void Profiler::disableProfile(const String & profileName)
    {
        // make sure the profile isn't already disabled
        DisabledProfileMap::iterator iter;
        iter = mDisabledProfiles.find(profileName);

        // make sure you don't disable a profile in the middle of that profile
        ProfileStack::iterator pIter;
        for(pIter = mProfiles.begin(); pIter != mProfiles.end(); ++pIter)
        {
            if (profileName == (*pIter).name)
                break;
        }

        // if those two conditions are met, disable the profile
        if((iter == mDisabledProfiles.end()) && (pIter == mProfiles.end()))
        {
            mDisabledProfiles.insert(std::pair<String, bool>(profileName, true));
        }
    }
    //-----------------------------------------------------------------------
    void Profiler::enableProfile(const String & profileName)
    {
        // make sure the profile is actually disabled
        DisabledProfileMap::iterator iter;
        iter = mDisabledProfiles.find(profileName);

        // make sure you don't enable a profile in the middle of that profile
        ProfileStack::iterator pIter, pIterend = mProfiles.end();
        for(pIter = mProfiles.begin(); pIter != pIterend; ++pIter)
        {
            if(profileName == (*pIter).name)
                break;
        }

        // if those two conditions are met, enable the profile by removing it from
        // the disabled list
        if ( (iter != mDisabledProfiles.end()) && (pIter == mProfiles.end()) )
        {
            mDisabledProfiles.erase(iter);
        }
    }
    //-----------------------------------------------------------------------
    void Profiler::beginProfile(const String & profileName, Nui32 groupID)
    {
        // if the profiler is enabled
        if (!mEnabled)
        {

            return;

        }
        // mask groups
        if ((groupID & mProfileMask) == 0)
        {
            return;
        }
        // we only process this profile if isn't disabled
        DisabledProfileMap::iterator dIter = mDisabledProfiles.find(profileName);
        if (dIter != mDisabledProfiles.end())
        {
            return;
        }
        // empty string is reserved for the root
        N_assert (profileName != _T(""), "Profile name can't be an empty string");

        ProfileStack::iterator iter, iterend = mProfiles.end();
        for (iter = mProfiles.begin(); iter != iterend; ++iter)
        {
            if ((*iter).name == profileName)
            {
                break;
            }
        }
        // make sure this profile isn't being used more than once
        N_assert(iter == mProfiles.end(), "This profile name is already being used");

        ProfileInstance p;
        p.hierarchicalLvl = static_cast<Nui32>(mProfiles.size());

        // this is the root, it has no parent
        if(mProfiles.empty())
        {
            p.parent = _T("");
        }
        // otherwise peek at the stack and use the top as the parent
        else
        {
            ProfileInstance parent = mProfiles.back();
            p.parent = parent.name;
        }

        // need a timer to profile!
        N_assert (mTimer, "Timer not set!");

        ProfileFrameList::iterator fIter, fIterend = mProfileFrame.end();
        ProfileHistoryList::iterator hIter;

        // we check to see if this profile has been called in the frame before
        for(fIter = mProfileFrame.begin(); fIter != fIterend; ++fIter)
        {
            if ((*fIter).name == profileName)
                break;
        }
        // if it hasn't been called before, set its position in the stack
        if (fIter == mProfileFrame.end())
        {
            ProfileFrame f;
            f.name = profileName;
            f.frameTime = 0;
            f.calls = 0;
            f.hierarchicalLvl = (Nui32) mProfiles.size();
            mProfileFrame.push_back(f);
        }

        // we check to see if this profile has been called in the app before
        ProfileHistoryMap::iterator histMapIter;
        histMapIter = mProfileHistoryMap.find(profileName);

        // if not we add a profile with just the name into the history
        if (histMapIter == mProfileHistoryMap.end())
        {
            ProfileHistory h;
            h.name = profileName;
            h.numCallsThisFrame = 0;
            h.totalTimePercent = 0;
            h.totalTimeMillisecs = 0;
            h.totalCalls = 0;
            h.maxTimePercent = 0;
            h.maxTimeMillisecs = 0;
            h.minTimePercent = 1;
            h.minTimeMillisecs = 100000;
            h.hierarchicalLvl = p.hierarchicalLvl;
            h.currentTimePercent = 0;
            h.currentTimeMillisecs = 0;

            // we add this to the history
            hIter = mProfileHistory.insert(mProfileHistory.end(), h);

            // for quick look-ups, we'll add it to the history map as well
            mProfileHistoryMap.insert(std::pair<String, ProfileHistoryList::iterator>(profileName, hIter));
        }

        // add the stats to this profile and push it on the stack
        // we do this at the very end of the function to get the most
        // accurate timing results
        p.name = profileName;
        p.currTime = mTimer->getUS();
        p.accum = 0;
        mProfiles.push_back(p);
    }
    //-----------------------------------------------------------------------
    void Profiler::endProfile(const String & profileName, Nui32 groupID)
    {
        // if the profiler received a request to be enabled or disabled
        // we reached the end of the frame so we can safely do this
        if (mEnableStateChangePending)
        {
            changeEnableState();
        }

        // if the profiler is enabled
        if(!mEnabled)
        {
            return;
        }

        // mask groups
        if ((groupID & mProfileMask) == 0)
        {
            return;
        }

        // we only process this profile if isn't disabled
        DisabledProfileMap::iterator dIter = mDisabledProfiles.find(profileName);
        if (dIter != mDisabledProfiles.end())
        {
            return;
        }

        // need a timer to profile!
        N_assert (mTimer, "Timer not set!");

        // get the end time of this profile
        // we do this as close the beginning of this function as possible
        // to get more accurate timing results
        Nul endTime = mTimer->getUS();

        // empty string is reserved for designating an empty parent
        N_assert(profileName != _T(""), "Profile name can't be an empty string");

        // stack shouldnt be empty
        N_assert(!mProfiles.empty(), "error");

        // get the start of this profile
        ProfileInstance bProfile = mProfiles.back();
        mProfiles.pop_back();

        // calculate the elapsed time of this profile
        Nul timeElapsed = endTime - bProfile.currTime;

        // update parent's accumulator if it isn't the root
        if(bProfile.parent != _T(""))
        {
            // find the parent
            ProfileStack::iterator iter;
            for(iter = mProfiles.begin(); iter != mProfiles.end(); ++iter)
            {
                if ((*iter).name == bProfile.parent)
                    break;
            }
            // the parent should be found
            N_assert(iter != mProfiles.end(), "error");
            // add this profile's time to the parent's accumlator
            (*iter).accum += timeElapsed;
        }
        // we find the profile in this frame
        ProfileFrameList::iterator iter;
        for (iter = mProfileFrame.begin(); iter != mProfileFrame.end(); ++iter)
        {
            if ((*iter).name == bProfile.name)
                break;
        }

        // nested profiles are cumulative
        (*iter).frameTime += timeElapsed;
        (*iter).calls++;

        // the stack is empty and all the profiles have been completed
        // we have reached the end of the frame so process the frame statistics
        if (mProfiles.empty())
        {
            // we know that the time elapsed of the main loop is the total time the frame took
            mTotalFrameTime = timeElapsed;

            if (timeElapsed > mMaxTotalFrameTime)
                mMaxTotalFrameTime = timeElapsed;

            // we got all the information we need, so process the profiles
            // for this frame
            processFrameStats();
            // clear the frame stats for next frame
            mProfileFrame.clear();
            // we display everything to the screen
            displayResults();
        }
    }
    //-----------------------------------------------------------------------
    void Profiler::processFrameStats()
    {
        ProfileFrameList::iterator frameIter;
        ProfileHistoryList::iterator historyIter;

        // we set the number of times each profile was called per frame to 0
        // because not all profiles are called every frame
        for (historyIter = mProfileHistory.begin(); historyIter != mProfileHistory.end(); ++historyIter)
        {
            (*historyIter).numCallsThisFrame = 0;
        }
        NIIf maxFrameTime = 0;
        // iterate through each of the profiles processed during this frame
        for (frameIter = mProfileFrame.begin(); frameIter != mProfileFrame.end(); ++frameIter)
        {
            String s = (*frameIter).name;

            // use our map to find the appropriate profile in the history
            historyIter = (*mProfileHistoryMap.find(s)).second;

            // extract the frame stats
            Nul frameTime = (*frameIter).frameTime;
            Nui32 calls = (*frameIter).calls;
            Nui32 lvl = (*frameIter).hierarchicalLvl;
            // calculate what percentage of frame time this profile took
            NIIf framePercentage = (NIIf) frameTime / (NIIf) mTotalFrameTime;
            NIIf frameTimeMillisecs = (NIIf)frameTime / 1000.0f;
            // update the profile stats
            (*historyIter).currentTimePercent = framePercentage;
            (*historyIter).currentTimeMillisecs = frameTimeMillisecs;
            if (mResetExtents)
            {
                (*historyIter).totalTimePercent = framePercentage;
                (*historyIter).totalTimeMillisecs = frameTimeMillisecs;
                (*historyIter).totalCalls = 1;
            }
            else
            {
                (*historyIter).totalTimePercent += framePercentage;
                (*historyIter).totalTimeMillisecs += frameTimeMillisecs;
                (*historyIter).totalCalls++;
            }
            (*historyIter).numCallsThisFrame = calls;
            (*historyIter).hierarchicalLvl = lvl;

            // if we find a new minimum for this profile, update it
            if (frameTimeMillisecs < ((*historyIter).minTimeMillisecs)
                || mResetExtents)
            {
                (*historyIter).minTimePercent = framePercentage;
                (*historyIter).minTimeMillisecs = frameTimeMillisecs;
            }

            // if we find a new maximum for this profile, update it
            if (frameTimeMillisecs > ((*historyIter).maxTimeMillisecs)
                || mResetExtents)
            {
                (*historyIter).maxTimePercent = framePercentage;
                (*historyIter).maxTimeMillisecs = frameTimeMillisecs;
            }
            if (frameTime > maxFrameTime)
                maxFrameTime = (NIIf)frameTime;
        }

        // Calculate whether the extents are now so out of date they need regenerating
        if (mCurrentFrame == 0)
            mAverageFrameTime = maxFrameTime;
        else
            mAverageFrameTime = (mAverageFrameTime + maxFrameTime) * 0.5f;

        if ((NIIf)mMaxTotalFrameTime > mAverageFrameTime * 4)
        {
            mResetExtents = true;
            mMaxTotalFrameTime = (Nul)mAverageFrameTime;
        }
        else
            mResetExtents = false;
    }
    //-----------------------------------------------------------------------
    void Profiler::displayResults()
    {
        if (!mEnabled)
        {
            return;
        }

        // if its time to update the display
        if (!(mCurrentFrame % mUpdateDisplayFrequency))
        {
            ProfileHistoryList::iterator iter;
            ProfileBarList::iterator bIter;

            OverlayGeo* g;

            NIIf newGuiHeight = mGuiHeight;

            NIIi profileCount = 0;

            NIIf maxTimeMillisecs = (NIIf)mMaxTotalFrameTime / 1000.0f;

            // go through each profile and display it
            for (iter = mProfileHistory.begin(), bIter = mProfileBars.begin();
                iter != mProfileHistory.end() && bIter != mProfileBars.end();
                ++iter, ++bIter)
            {
                // display the profile's name and the number of times it was called in a frame
                g = *bIter;
                g->show();
                g->setCaption(String((*iter).name + _T(" (") + N_conv((*iter).numCallsThisFrame) + _T(")")));
                g->setLeft(10 + (*iter).hierarchicalLvl * 15.0f);

                // display the main bar that show the percentage of the frame time that this
                // profile has taken
                bIter++;
                g = *bIter;
                g->show();
                // most of this junk has been set before, but we do this to get around a weird
                // Nii gui issue (bug?)
                g->setSizeMode(SM_Abs);
                g->setHeight(mBarHeight);
                if (mDisplayMode == DISPLAY_PERCENTAGE)
                    g->setWidth(((*iter).currentTimePercent) * mGuiWidth);
                else
                    g->setWidth(((*iter).currentTimeMillisecs / maxTimeMillisecs) * mGuiWidth);
                g->setLeft(mGuiWidth);
                g->setTop(mGuiBorderWidth + profileCount * (mBarHeight + mBarSpacing));

                // display line to indicate the minimum frame time for this profile
                bIter++;
                g = *bIter;
                g->show();
                if (mDisplayMode == DISPLAY_PERCENTAGE)
                    g->setLeft(mBarIndent + (*iter).minTimePercent * mGuiWidth);
                else
                    g->setLeft(mBarIndent + ((*iter).minTimeMillisecs / maxTimeMillisecs) * mGuiWidth);

                // display line to indicate the maximum frame time for this profile
                bIter++;
                g = *bIter;
                g->show();
                if (mDisplayMode == DISPLAY_PERCENTAGE)
                    g->setLeft(mBarIndent + (*iter).maxTimePercent * mGuiWidth);
                else
                    g->setLeft(mBarIndent + ((*iter).maxTimeMillisecs / maxTimeMillisecs) * mGuiWidth);
                // display line to indicate the average frame time for this profile
                bIter++;
                g = *bIter;
                g->show();
                if ((*iter).totalCalls != 0)
                    if (mDisplayMode == DISPLAY_PERCENTAGE)
                        g->setLeft(mBarIndent + ((*iter).totalTimePercent / (*iter).totalCalls) * mGuiWidth);
                    else
                        g->setLeft(mBarIndent + (((*iter).totalTimeMillisecs / (*iter).totalCalls) / maxTimeMillisecs) * mGuiWidth);
                else
                    g->setLeft(mBarIndent);

                // display text
                bIter++;
                g = *bIter;
                g->show();
                if (mDisplayMode == DISPLAY_PERCENTAGE)
                {
                    g->setLeft(mBarIndent + (*iter).currentTimePercent * mGuiWidth + 2);
                    g->setCaption(N_conv((*iter).currentTimePercent * 100.0f) + _T("%"));
                }
                else
                {
                    g->setLeft(mBarIndent + ((*iter).currentTimeMillisecs / maxTimeMillisecs) * mGuiWidth + 2);
                    g->setCaption(N_conv((*iter).currentTimeMillisecs) + _T("ms"));
                }

                // we set the height of the display with respect to the number of profiles displayed
                newGuiHeight += mBarHeight + mBarSpacing;
                profileCount++;
            }
            // set the main display dimensions
            mProfileGui->setSizeMode(SM_Abs);
            mProfileGui->setHeight(newGuiHeight);
            mProfileGui->setWidth(mGuiWidth * 2 + 15);
            mProfileGui->setTop(5);
            mProfileGui->setLeft(5);

            // we hide all the remaining pre-created bars
            for (; bIter != mProfileBars.end(); ++bIter)
            {
                (*bIter)->hide();
            }
        }
        mCurrentFrame++;
    }
    //-----------------------------------------------------------------------
    bool Profiler::watchForMax(const String& profileName)
    {
        ProfileHistoryMap::iterator mapIter;
        ProfileHistoryList::iterator iter;

        mapIter = mProfileHistoryMap.find(profileName);

        // if we don't find the profile, return false
        if(mapIter == mProfileHistoryMap.end())
            return false;

        iter = (*mapIter).second;

        return ((*iter).currentTimePercent == (*iter).maxTimePercent);
    }
    //-----------------------------------------------------------------------
    bool Profiler::watchForMin(const String & profileName)
    {
        ProfileHistoryMap::iterator mapIter;
        ProfileHistoryList::iterator iter;

        mapIter = mProfileHistoryMap.find(profileName);

        // if we don't find the profile, return false
        if (mapIter == mProfileHistoryMap.end())
            return false;

        iter = (*mapIter).second;

        return ((*iter).currentTimePercent == (*iter).minTimePercent);
    }
    //-----------------------------------------------------------------------
    bool Profiler::watchForLimit(const String & profileName, NIIf limit, bool greaterThan)
    {
        ProfileHistoryMap::iterator mapIter;
        ProfileHistoryList::iterator iter;

        mapIter = mProfileHistoryMap.find(profileName);

        // if we don't find the profile, return false
        if(mapIter == mProfileHistoryMap.end())
            return false;

        iter = (*mapIter).second;

        if (greaterThan)
            return ((*iter).currentTimePercent > limit);
        else
            return ((*iter).currentTimePercent < limit);
    }
    //-----------------------------------------------------------------------
    void Profiler::logResults()
    {
        ProfileHistoryList::iterator iter;

        N_Only(Log).log(_I18n("----------------------Profiler Results----------------------"));

        for (iter = mProfileHistory.begin(); iter != mProfileHistory.end(); ++iter)
        {

            // create an indent that represents the hierarchical order of the profile
            String indent = _T("");
            for (NCount i = 0; i < (*iter).hierarchicalLvl; ++i)
            {
                indent = indent + _T("   ");
            }

            N_Only(Log).log(indent + _I18n("Name ") + (*iter).name +
                _I18n(" | Min ") + N_conv((*iter).minTimePercent) +
                _I18n(" | Max ") + N_conv((*iter).maxTimePercent) +
                _I18n(" | Avg ")+ N_conv((*iter).totalTimePercent / (*iter).totalCalls));
        }
        N_Only(Log).log(_T("------------------------------------------------------------"));
    }
    //-----------------------------------------------------------------------
    void Profiler::reset()
    {
        ProfileHistoryList::iterator iter;
        for (iter = mProfileHistory.begin(); iter != mProfileHistory.end(); ++iter)
        {
            (*iter).currentTimePercent = (*iter).maxTimePercent = (*iter).totalTimePercent = 0;
            (*iter).currentTimeMillisecs = (*iter).maxTimeMillisecs = (*iter).totalTimeMillisecs = 0;
            (*iter).numCallsThisFrame = (*iter).totalCalls = 0;

            (*iter).minTimePercent = 1;
            (*iter).minTimeMillisecs = 100000;
        }
        mMaxTotalFrameTime = 0;
    }
    //-----------------------------------------------------------------------
    void Profiler::setUpdateDisplayFrequency(Nui32 freq)
    {
        mUpdateDisplayFrequency = freq;
    }
    //-----------------------------------------------------------------------
    Nui32 Profiler::getUpdateDisplayFrequency() const
    {
        return mUpdateDisplayFrequency;
    }
    //-----------------------------------------------------------------------
    void Profiler::changeEnableState()
    {
        if (mNewEnableState)
        {
            mOverlay->show();
        }
        else
        {
            mOverlay->hide();
        }
        mEnabled = mNewEnableState;
        mEnableStateChangePending = false;
    }
    //-----------------------------------------------------------------------
    OverlayView* Profiler::createContainer()
    {
        OverlayView * container = static_cast<OverlayView *>(N_Only(Overlay).createGeo(0, 0/*"BorderPanel", "profiler"*/));
        container->setSizeMode(SM_Abs);
        container->setMaterial(N_RES_DEFINE_StatsBlockCenter/*"Core/StatsBlockCenter"*/);
        container->setHeight(mGuiHeight);
        container->setWidth(mGuiWidth * 2 + 15);
        container->set(_T("border_size"), _T("1 1 1 1"));
        container->set(_T("border_material"), _T("Core/StatsBlockBorder"));
        container->set(_T("border_topleft_uv"), _T("0.0000 1.0000 0.0039 0.9961"));
        container->set(_T("border_top_uv"), _T("0.0039 1.0000 0.9961 0.9961"));
        container->set(_T("border_topright_uv"), _T("0.9961 1.0000 1.0000 0.9961"));
        container->set(_T("border_left_uv"), _T("0.0000 0.9961 0.0039 0.0039"));
        container->set(_T("border_right_uv"), _T("0.9961 0.9961 1.0000 0.0039"));
        container->set(_T("border_bottomleft_uv"), _T("0.0000 0.0039 0.0039 0.0000"));
        container->set(_T("border_bottom_uv"), _T("0.0039 0.0039 0.9961 0.0000"));
        container->set(_T("border_bottomright_uv"), _T("0.9961 0.0039 1.0000 0.0000"));
        container->setLeft(5);
        container->setTop(5);
        return container;
    }
    //-----------------------------------------------------------------------
    OverlayGeo* Profiler::createTextArea(OverlayGeoID ogid, NIIf width,
        NIIf height, NIIf top, NIIf left, Nui32 fontSize, const String & caption,
            bool show)
    {
        OverlayGeo * textArea = N_Only(Overlay).createGeo((OverlayGeoType)N_OverlayType_Text, ogid);
        textArea->setSizeMode(SM_Abs);
        textArea->setWidth(width);
        textArea->setHeight(height);
        textArea->setTop(top);
        textArea->setLeft(left);
        textArea->set(_T("font_name"), _T("BlueHighway"));
        textArea->set(_T("char_height"), N_conv(fontSize));
        textArea->setCaption(caption);
        textArea->set(_T("colour_top"), _T("1 1 1"));
        textArea->set(_T("colour_bottom"), _T("1 1 1"));

        if (show)
        {
            textArea->show();
        }
        else
        {
            textArea->hide();
        }
        return textArea;
    }
    //-----------------------------------------------------------------------
    OverlayGeo * Profiler::createPanel(OverlayGeoID name, NIIf width, NIIf height,
        NIIf top, NIIf left, ResourceID mid, bool show)
    {
        OverlayGeo * panel = N_Only(Overlay).createGeo((OverlayGeoType)N_OverlayType_Container, name);
        panel->setSizeMode(SM_Abs);
        panel->setWidth(width);
        panel->setHeight(height);
        panel->setTop(top);
        panel->setLeft(left);
        panel->setMaterial(mid);

        if (show)
        {
            panel->show();
        }
        else
        {
            panel->hide();
        }
        return panel;
    }
    //-----------------------------------------------------------------------
}