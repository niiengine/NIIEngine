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
#include "NiiLibxmlXmlPattern.h"
#include "NiiUIManager.h"
#include "NiiUIException.h"
#include "NiiPropertyData.h"
#include "NiiXmlAnalyze.h"
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlmemory.h>
#include <libxml/xmlerror.h>

namespace NII
{
    //------------------------------------------------------------------------
    void processXMLElement(XmlAnalyze * xc, xmlNode * node)
    {
        // build pdc block for the element
        PropertyData attrs;

        xmlAttrPtr currAttr = node->properties;
        while(currAttr)
        {
            xmlChar * val = xmlGetProp(node, currAttr->name);
            attrs.add(xc->get(reinterpret_cast<const Nutf8 *>(currAttr->name)),
                reinterpret_cast<const Nutf8 *>(val));
            xmlFree(val);
            currAttr = currAttr->next;
        }

        // element start processing
        xc->onBegin(xc->get(reinterpret_cast<const Nutf8 *>(node->name)), &attrs);

        for(xmlNode * cur_node = node->children; cur_node; cur_node = cur_node->next)
        {
            switch(cur_node->type)
            {
            case XML_ELEMENT_NODE:
                processXMLElement(xc, cur_node);
                break;
            case XML_TEXT_NODE:
                if(cur_node->content != 0 && *cur_node->content!= '\0')
                    xc->onData(reinterpret_cast<const Nutf8*>(cur_node->content));
                break;
            default:
                break;
            }
        }

        // element end processing
        xc->onEnd(xc->get(reinterpret_cast<const Nutf8 *>(node->name)));
    }
    //------------------------------------------------------------------------
    LibxmlParser::LibxmlParser()
    {
        mPatternID = N_Pattern_Libxml;
    }
    //------------------------------------------------------------------------
    LibxmlParser::~LibxmlParser()
    {
    }
    //------------------------------------------------------------------------
    void LibxmlParser::parse(XmlAnalyze * xc, const DataStream * source)
    {
        Nui8 * data;
        if(source->isSysMemory() == false)
            source->cloneData(data);
        else
            data = static_cast<MemStream *>(source)->getData();

        xmlDocPtr doc = xmlParseMemory(reinterpret_cast<const char *>(data), source->size());

        if(!doc)
        {
            xmlError * err = xmlGetLastError();

            N_EXCEPT(Generic, _T("xmlParseMemory failed in file: '") + err->file +
                _T("' at line number") + Nid(err->line) + _T(".  Error is:") + err->message));
        }

        // get root element
        xmlNode * root = xmlDocGetRootElement(doc);

        // process all elements from root to end of doc
        processXMLElement(xc, root);

        // release the xmlDoc
        xmlFreeDoc(doc);

        if(source->isSysMemory() == false)
            N_free(data);
    }
    //------------------------------------------------------------------------
    bool LibxmlParser::_init()
    {
        LIBXML_TEST_VERSION;
        return true;
    }
    //------------------------------------------------------------------------
    bool LibxmlParser::_end()
    {
        xmlCleanupParser();
        return true;
    }
    //------------------------------------------------------------------------
}