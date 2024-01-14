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

#ifndef _NII_OSXConfigDialog_H__
#define _NII_OSXConfigDialog_H__

#include "NiiPreInclude.h"
#include "NiiEngine.h"
#include "NiiRenderSys.h"
#include "NiiSysOption.h"

#ifdef __OBJC__
#import <Cocoa/Cocoa.h>
#import <AppKit/AppKit.h>

#if defined(MAC_OS_X_VERSION_10_6) && MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_6
@interface OgreConfigWindowDelegate : NSObject <NSWindowDelegate, NSTableViewDelegate, NSTableViewDataSource>
#else
@interface OgreConfigWindowDelegate : NSObject
#endif
{
    NSWindow *mConfigWindow;
    NSImageView *mOgreLogo;
    NSPopUpButton *mRenderSystemsPopUp;
    NSPopUpButton *mOptionsPopUp;
    NSTableView *mOptionsTable;
    NSButton *mOkButton;
    NSButton *mCancelButton;
    NSTextField *mOptionLabel;
    
    NSDictionary *mOptions;
}

- (void)cancelButtonPressed:(id)sender;
- (void)okButtonPressed:(id)sender;
- (void)popUpValueChanged:(id)sender;

// Getters and setters
- (void)setOptions:(NSDictionary *)dict;
- (NSDictionary *)getOptions;
- (void)setRenderSystemsPopUp:(NSPopUpButton *)button;
- (NSPopUpButton *)getRenderSystemsPopUp;
- (void)setOgreLogo:(NSImageView *)image;
- (NSImageView *)getOgreLogo;
- (void)setConfigWindow:(NSWindow *)window;
- (NSWindow *)getConfigWindow;
- (void)setOptionsTable:(NSTableView *)table;
- (NSTableView *)getOptionsTable;
- (void)setOptionsPopUp:(NSPopUpButton *)button;
- (NSPopUpButton *)getOptionsPopUp;
- (void)popUpValueChanged:(id)sender;

@end
#endif

namespace NII
{
	class _OgreExport ConfigDialog : public UtilityAlloc
	{
	public:
		ConfigDialog();
		~ConfigDialog();
	
		void init();
		bool display();

	protected:
#ifdef __OBJC__
        OgreConfigWindowDelegate *mWindowDelegate;
#endif
		RenderSys *mSelectedRenderSystem;
	};
}

#endif // __OSX_CONFIG_DIALOG_H__
