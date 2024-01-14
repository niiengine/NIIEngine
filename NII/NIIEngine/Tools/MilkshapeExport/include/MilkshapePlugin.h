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

#ifndef __MilkshapePlugin_H__
#define __MilkshapePlugin_H__

#include "msPlugIn.h"
#include "windows.h"
#include "Ogre.h"


/** A plugin class for Milkshape3D to export to OGRE model formats. 
@remarks
    This class is the implementor of the exporting interface for Milkshape3D, to allow
    you to export your models to OGRE format from that tool.
    Note that this plugin delegates most of the detail of exporting the model to the
    generic model export framework.
*/
class MilkshapePlugin : public cMsPlugIn
{
    
    char mTitle[64];

public:
	MilkshapePlugin ();
    virtual ~MilkshapePlugin ();

public:
    /// As required by Milkshape
    int             GetType ();
    /// As required by Milkshape
    const char *    GetTitle ();
    /// As required by Milkshape
    int             Execute (msModel* pModel);

    /** Callback to process window events */
#if OGRE_ARCHITECTURE_64 == OGRE_ARCH_TYPE
    static INT_PTR CALLBACK DlgProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam);
#else
    static BOOL CALLBACK DlgProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam);
#endif

    bool exportMaterials;
    bool generateLods;
    bool generateEdgeLists;
    bool generateTangents;
	Ogre::VertexType tangentSemantic;
	bool tangentsSplitMirrored;
	bool tangentsSplitRotated;
	bool tangentsUseParity;
    unsigned short numLods;
    float lodDepthIncrement;
    Ogre::ProgressiveMesh::VertexReductionQuota lodReductionMethod;
    float lodReductionAmount;
    bool exportMesh;
    bool exportSkeleton;
    bool splitAnimations;
    float fps; 

protected:
    bool showOptions(void);
    void doExportMesh(msModel* pModel);
    void doExportMaterials(msModel* pModel);
    void doExportAnimations(msModel* pModel, Ogre::SkeletonPtr& skel);
    Ogre::SkeletonPtr doExportSkeleton(msModel* pModel, Ogre::MeshPtr& mesh); // Skeleton returned for deletion later
    bool locateSkeleton(Ogre::MeshPtr& mesh);
	Ogre::Colour msVec4ToColourValue(float prop[4]);
};

#endif

