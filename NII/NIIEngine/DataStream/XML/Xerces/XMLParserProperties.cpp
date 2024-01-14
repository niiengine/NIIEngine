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

#include "CEGUI/XMLParserModules/Xerces/XMLParserProperties.h"
#include "NiiXercesXmlPattern.h"

namespace CEGUI
{
namespace XercesParserProperties
{    
    //------------------------------------------------------------------------
    void SchemaDefaultResourceGroup::get(const PropertyCmdObj * obj, String & out) const
    {
        out = static_cast<const XercesParser *>(obj)->getSchemaDefaultResourceGroup();
    }
    //------------------------------------------------------------------------
    void SchemaDefaultResourceGroup::set(PropertyCmdObj * obj, const String & value)
    {
        static_cast<XercesParser *>(obj)->setSchemaDefaultResourceGroup(value);
    }
    //------------------------------------------------------------------------
    PropertyCmd * SchemaDefaultResourceGroup::clone() const
    {
        return N_new SchemaDefaultResourceGroup(*this);
    }
    //------------------------------------------------------------------------
}
}
