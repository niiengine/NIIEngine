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
#ifndef _CEGuiRendererSelector_h_
#define _CEGuiRendererSelector_h_

/*!
\brief
    Enumeration of available renderer types.
*/
enum CEGuiRendererType
{
    OgreGuiRendererType,            //!< Renderer that uses the Ogre engine.
    Direct3D81GuiRendererType,      //!< Renderer that uses the Direct3D 8.1 API.
    Direct3D9GuiRendererType,       //!< Renderer that uses the Direct3D 9 API.
    Direct3D10GuiRendererType,      //!< Renderer that uses the Direct3D 10 API.
    Direct3D11GuiRendererType,      //!< Renderer that uses the Direct3D 11 API.
    OpenGLGuiRendererType,          //!< Renderer that uses the OpenGL API.
	OpenGL3GuiRendererType,         //!< Renderer that uses the OpenGL3 API.
    IrrlichtGuiRendererType,        //!< Renderer that uses the Irrlicht engine.
    DirectFBGuiRendererType,        //!< Renderer that uses the DirectFB engine.
    RendererTypeCount,              //!< Special value that equals the number of renderer types.
    InvalidGuiRendererType          //!< Special value used to represent an invalid selection.
};


/*!
\brief
    Base class for the renderer selection dialog class.
 
    This should be sub-classed to do something useful.  Initialisation should happen in the constructor and cleanup
    in the destructor.  The invokeDialog should only display the dialog (and not return until the dialog is dismissed).
*/
class CEGuiRendererSelector
{
public:
    /*!
    \brief
        Constructor.
    */
    CEGuiRendererSelector();


    /*!
    \brief
        Destructor.
    */
    virtual ~CEGuiRendererSelector();


    /*!
    \brief
        Displays a dialog allowing the user to select a renderer to be used.

    \return
        false if the user cancelled.
    */
    virtual bool invokeDialog()  = 0;


    /*!
    \brief
        Return the CEGuiRendererType value of the renderer most recently selected by the user.

    \return
        One of the CEGuiRendererType enumerated values.
    */
    CEGuiRendererType getSelectedRendererType();


    /*!
    \brief
        Set whether or not a specific renderer type will be available for selection from the dialog the next time it
        is displayed.

    \param rendererType
        One of the CEGuiRendererType enumerated values representing the renderer whos availability will be set.

    \param available
        - true if this renderer should be available to the user.
        - false if this renderer should not be available.
    */
    void setRendererAvailability(CEGuiRendererType rendererType, bool available = true);


protected:
    CEGuiRendererType   d_lastSelected;         //!< Holds the last selected renderer type.
    bool    d_rendererAvailability[RendererTypeCount];  //!< Holds availability of renderer types.
};

#endif  // end of guard _CEGuiRendererSelector_h_
