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

#ifndef __OSXCocoaView_H__
#define __OSXCocoaView_H__

#include "NiiViewWindow.h"
#include <UIKit/UIKit.h>

@interface OgreView : UIView
{
    NII::ViewWindow *window;
}

- (id)initWithFrame:(CGRect)f;
- (id)initWithGLiPhoneWindow:(NII::ViewWindow*)w;

- (void)setOgreWindow:(NII::ViewWindow*)w;
- (NII::ViewWindow*)ogreWindow;

- (void)setFrameSize:(CGSize)s;

@end

@interface OgreWindow : UIWindow
{}
@end

#endif

