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

#ifndef _NII_PROCESSORMANAGER_H_
#define _NII_PROCESSORMANAGER_H_
/*
一个渲染通路
SpaceManager::findSpace
|
SpaceManager::renderSpace
|
Camera::render(Viewport * vp)
|
Viewport::update(void)
|
FrameObj::flushImpl(Viewport * obj)
|
FrameObj::updateImpl()
|
FrameObj::flush(bool swap)
|
//Engine::_updateAllFrame(bool swap)
|
Engine::renderFrame(void)
|
Engine::beginRender(void)
*/

/*
Free Tools

Microsoft Visual Studio Express
Visual C++ 2010 Express Edition is an IDE which is provided as a strip
ped-down version of the Visual Studio IDE targeted at students and hobbyists.

Dev-C++
A free C and C++ IDE for Windows that used MinGW as it's compiler suite.
 Should never ever really be used unless you're aiming for an obsolete, bug filled IDE.

Code::Blocks
Codeblocks is a cross-platform IDE built around wxWidgets, designed to 
be extensible and configurable. Runs on Windows, Mac OS X and Linux. http://www.codeblocks.org

Eclipse CDT C/C++ and MinGW
Eclipse is an awesome IDE with the plugin CDT for developing C/C++ and 
the MinGW compiler. It is an industrial strength C/C++ IDE with lots of
features and multiplatform. Runs on Microsoft Windows, Linux, Unix and
Mac OS X. http://www.eclipse.org/cdt/ http://www.mingw.org/ (Using Eclipse with Irrlicht)

Easy Eclipse: Easy to use and install Eclipse with C++ Easy Eclipse

KDevelop
Kdevelop is an excellent IDE for Linux.

NetBeans
NetBeans is a useful cross-platform IDE that allows C++ development via 
a related plugin. A tutorial for using Netbeans with Irrlicht may be found
 here: Using NetBeans with Irrlicht.

Qt Creator
A great IDE for Windows, Linux and Mac OS X which is part of the Qt SDK.
 http://qt.nokia.com/products/developer-tools

Commercial Tools

Microsoft Visual Studio 2008
Microsoft Visual Studio is Microsoft's flagship software development product
for computer programmers. It centers on an integrated development environment 
which lets programmers create standalone applications, web sites, web applications,
and web services that run on any platforms supported by Microsoft's .NET Framework 
(for all versions after 6). Supported platforms include Microsoft Windows servers 
and workstations, PocketPC, Smartphones, and World Wide Web browsers.
Visual Studio includes the following:
    * Visual Basic (.NET)
    * Visual C++
    * Visual C#
    * Visual J#
    * ASP.NET
http://msdn2.microsoft.com/en-us/vstudio/default.aspx

*/
#endif //__NII_PROCESSORMANAGER_H__
