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

#ifndef _NII_LINUX_CONTROL_PATTERN_PREREQ_H_
#define _NII_LINUX_CONTROL_PATTERN_PREREQ_H_

#include "NiiPreInclude.h"
#include <cstdlib>
#include <errno.h>
#include <memory.h>
#include <sstream>
#include <iostream>
#include <cstring>

#define KEYBOARD_BUFFERSIZE 10
#define MOUSE_BUFFERSIZE 20
#define JOY_BUFFERSIZE 30

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/kd.h>
#include <fcntl.h>
#include <cassert>
#include <linux/input.h>

namespace NII
{
namespace NII_MEDIA
{
	class LinuxControlPatternManager;
	class LinuxKeyboardControlPattern;
	class LinuxJoyStickControlPattern;
	class LinuxMouseControlPattern;
	class LinuxJoyStickForceFeedback;
}
}
#endif