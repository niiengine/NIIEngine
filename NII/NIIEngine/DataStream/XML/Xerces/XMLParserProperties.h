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

#ifndef _CEGUIXercesParserProperties_h_
#define _CEGUIXercesParserProperties_h_

#include "../../PropertyCmd.h"

// Start of CEGUI namespace section
namespace CEGUI
{
/// Properties for the XercesParser XML parser module.
namespace XercesParserProperties
{
    /** PropertyCmd to access the resource group used to load .xsd schema files.
    @par Usage:
        - Name: SchemaDefaultResourceGroup
        - Format: "[resource group name]".
     */
    class SchemaDefaultResourceGroup : public PropertyCmd
    {
    public:
        SchemaDefaultResourceGroup() : PropertyCmd("SchemaDefaultResourceGroup",
            "PropertyCmd to get/set the resource group used when loading xsd schema "
            "files.  Value is a string describing the resource group name.",
            ""){}

        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * obj, String & out) const;
        
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * obj, const String & value);
        
        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const;
    };
}
}
#endif