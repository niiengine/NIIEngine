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
#include "NiiLinuxMouseControlPattern.h"

#define EV_MAKE   1  // when key pressed
#define EV_BREAK  0  // when key released
#define EV_REPEAT 2  // when key switches to repeating after short delay

namespace NII
{
namespace NII_MEDIA
{
    #define test_bit(array, bit)  (array[(bit)/8] & (1 << ((bit)%8)))
    //------------------------------------------------------------------------
    LinuxMouseControlPattern::LinuxMouseControlPattern(){}
    //------------------------------------------------------------------------
    LinuxMouseControlPattern::LinuxMouseControlPattern(Nid id, NIIi main) :
        MouseControlPattern(id),
        mMouse(main)
    {

    }
    //------------------------------------------------------------------------
    LinuxMouseControlPattern::~LinuxMouseControlPattern()
    {
        if(mMouse >= 0)
            close(mMouse);
    }
    //------------------------------------------------------------------------
    NIIb LinuxMouseControlPattern::getQuality() const
    {
        return 255;
    }
    //------------------------------------------------------------------------
    NIIb LinuxMouseControlPattern::getPerformance() const
    {
        return 255;
    }
    //------------------------------------------------------------------------
    void LinuxMouseControlPattern::_init()
    {
        if(mMouse == -1)
            N_EXCEPT(CreateDevice, _T("Could not init keyboard device!"));
    }
    //------------------------------------------------------------------------
    void LinuxMouseControlPattern::_update()
    {
        if(mActive)
        {
            struct input_event raw[MOUSE_BUFFERSIZE];
            NCount i;
            bool move = false;
            Ni32 tempx = 0;
            Ni32 tempy = 0;
            Ni32 tempz = 0;
            ssize_t count = read(mMouse, raw, sizeof(raw)) / sizeof(struct input_event);
            for(i = 0; i < count; ++i)
            {
                switch(raw[i].type)
                {
                case EV_KEY:
                    /* raw.value = 1 for first, 2 for repeat */
                    if(raw.value == EV_MAKE)
                    {
                        press((MouseButton)(raw[i].code - BTN_MOUSE));
                    }
                    else if(raw.value == EV_BREAK)
                    {
                        rlease((MouseButton)(raw[i].code - BTN_MOUSE));
                    }
                    break;
                case EV_REL:
                    switch (raw.code)
                    {
                    case REL_X:
                        tempx = raw.value;
                        move = true;
                        break;
                    case REL_Y:
                        tempy = raw.value;
                        move = true;
                        break;
                    case REL_WHEEL:
                        tempz = raw.value;
                        move = true;
                        break;
                    }
                    break;
                }
            }

            if(move)
                move(tempx, tempy, tempz);
        }
    }
    //------------------------------------------------------------------------
    void LinuxMouseControlPattern::_reset()
    {
        // donothing
    }
    //------------------------------------------------------------------------
    void LinuxMouseControlPattern::_start()
    {
        mActive = true;
    }
    //------------------------------------------------------------------------
    void LinuxMouseControlPattern::_pause()
    {
        mActive = false;
    }
    //------------------------------------------------------------------------
    void LinuxMouseControlPattern::_stop()
    {
        mActive = false;
    }
    //------------------------------------------------------------------------
    PatternID LinuxMouseControlPattern::getPID() const
    {
        return 0;
    }
    //------------------------------------------------------------------------
}
}