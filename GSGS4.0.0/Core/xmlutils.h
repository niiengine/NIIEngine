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
#ifndef XMLUTILS_H
#define XMLUTILS_H

#include "gsgsPreInclude.h"
#include "gsgsSerializer.h"

#if wxVERSION_NUMBER < 2900
#    define XmlAddProperty      AddProperty
#    define XmlGetPropVal       GetPropVal
#    define XmlGetProperties    GetProperties
#    define XmlHasProp          HasProp
#    define wxXmlProperty       wxXmlProperty
#else
#    define XmlAddProperty      AddAttribute
#    define XmlGetPropVal       GetAttribute
#    define XmlGetProperties    GetAttributes
#    define XmlHasProp          HasAttribute
#    define wxXmlProperty       wxXmlAttribute
#endif

#include "wx/xml/xml.h"

namespace gsgs
{
    class _gsgsAPI XmlUtils
    {
    public:
        /// Find a child node by name by iterating the parent children. NULL if no childs exist
        /// \param parent  the parent node whom to be searched
        /// \param tagName the element tag name
        /// \param name	   the element 'Name' property value to search
        static wxXmlNode *FindNodeByName(const wxXmlNode *parent, const wxString &tagName, const wxString &name);

        /// Find the last child node of parent with a given name. NULL if no childs exist
        /// \param parent  the parent node whom to be searched
        /// \param name	   the element's tag name
        static wxXmlNode *FindLastByTagName(const wxXmlNode *parent, const wxString &tagName);


        /**
         * update node property. If multiple properties with the same name exist, this function updates
         * the first occurance
         * \param node
         * \param name property name
         * \param value new value to set
         */
        static void UpdateProperty(wxXmlNode *node, const wxString &name, const wxString &value);

        /**
         * Find the first child node of parent with a given name. NULL if no children exist
         * \param the parent node to be searched
         * \param the element's tag name
         */
        static wxXmlNode *FindFirstByTagName(const wxXmlNode *parent, const wxString &tagName);

        /**
         * Read all of the child nodes (presumed to be textnodes) of the passed node, optionally with a given tagname
         * \param node the node to be searched
         * \param tagName the child tag name to find and read their contents. If empty, read all children
         * \return a wxArrayString
         */
        static wxArrayString ChildNodesContentToArray(const wxXmlNode* node, const wxString& tagName = wxT(""));

        /**
         * Read all of the child nodes (presumed to be textnodes) of the passed node, optionally with a given tagname
         * \param node the node to be searched
         * \param tagName the child tag name to find and read their contents. If empty, read all children
         * \param separator the string to use to separate each child's content, ';' by default
         * \return a wxString containing the content of each child, separated by 'separator'
         */
        static wxString ChildNodesContentToString(const wxXmlNode* node, const wxString& tagName = wxT(""), const wxString& separator = wxT(";"));

        /**
         * Set the content of node. This function replaces any existing content of node
         * \param node
         * \param text content
         */
        static void SetNodeContent(wxXmlNode *node, const wxString &text);

        /**
         * Set the content of node. This function replaces any existing content of node
         * \param node
         * \param text content
         */
        static void SetCDATANodeContent(wxXmlNode *node, const wxString &text);

        /**
         * Read string property from the given node
         * \param propName the property name
         * \param defaultValue default value to return if no property exist
         */
        static wxString ReadString(const wxXmlNode *node, const wxString &propName, const wxString &defaultValue = wxEmptyString);

        /**
         * Read long property from the given node
         * \param propName the property name
         * \param defaultValue default value to return if no property exist
         */
        static long ReadLong(const wxXmlNode *node, const wxString &propName, long defaultValue = -1);

        /**
         * Try to read long property from the given node
         * If it doesn't exist, don't provide a default value.
         * \param propName the property name
         * \param answer will contain the result if found
         * \return true if answer is valid, false if not found
         */
        static bool ReadLongIfExists(const wxXmlNode *node, const wxString &propName, long& answer);

        /**
         * Read a boolean property from the given node
         * \param propName the property name
         * \param defaultValue default value to return if no property exist
         */
        static bool ReadBool(const wxXmlNode *node, const wxString &propName, bool defaultValue = false);

        /**
         * Try to read a string property from the given node.
         * If it doesn't exist, don't provide a default value.
         * \param propName the property name
         * \param value will contain the result if found
         * \return true if answer is valid, false if not found
         */
        static bool ReadStringIfExists(const wxXmlNode* node, const wxString& propName, wxString& value);

        /**
         * Try to read a boolean property from the given node.
         * If it doesn't exist, don't provide a default value.
         * \param propName the property name
         * \param answer will contain the result if found
         * \return true if answer is valid, false if not found
         */
        static bool ReadBoolIfExists(const wxXmlNode* node, const wxString& propName, bool& answer);

        /**
         * Remove all children of xml node
         * \param node xml node
         */
        static void RemoveChildren(wxXmlNode *node);

        /**
         * @brief read serialized object with version support
         * @param root
         * @param name
         * @param obj
         * @return
         */
        static bool StaticReadObject(wxXmlNode *root, const wxString & name, ObjectData *obj);
        /**
         * @brief write serialized object with version support
         * @param root
         * @param name
         * @param obj
         * @return
         */
        static bool StaticWriteObject(wxXmlNode *root, const wxString & name, ObjectData *obj);

        static bool SaveXmlToFile(wxXmlDocument* doc, const wxString & filename);
    };
}
#endif
