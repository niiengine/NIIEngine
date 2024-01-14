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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "NiiPreProcess.h"
#include "rapidxml.hpp"
#include "NiiRapidXmlPattern.h"
#include "NiiUIManager.h"
#include "NiiXmlAnalyze.h"
#include "NiiPropertyData.h"
#include "NiiUIException.h"

namespace NII
{
    //------------------------------------------------------------------------
    class RapidXMLDocument : public rapidxml::xml_document<>
    {
    public:
        RapidXMLDocument(XmlAnalyze * xc, const DataStream * source);

        ~RapidXMLDocument(){}
    protected:
        void processElement(const rapidxml::xml_node<> * element);
    private:
        XmlAnalyze * d_handler;
    };
    //------------------------------------------------------------------------
    RapidXMLDocument::RapidXMLDocument(XmlAnalyze * xc, const DataStream * source)
    {
        d_handler = xc;

        // Create a buffer with extra bytes for a newline and a terminating null
        Nui8 * data;
        source->cloneData(data);

        size_t size = source->size();
        char * buf = new char[size + 2];
        memcpy(buf, data, size);

        N_free(data);
        // PDT: The addition of the newline is a kludge to resolve an issue
        // whereby parse returns 0 if the xml file has no newline at the end but
        // is otherwise well formed.
        buf[size] = '\n';
        buf[size + 1] = 0;

        // Parse the document
        rapidxml::xml_document<> doc;    // character type defaults to char

        try
        {
            doc.parse<0>(buf);          // 0 means default parse flags
        }
        catch (...)
        {
            // error detected, cleanup out buffers
            delete[] buf;
            N_EXCEPT(IO, _T("an error occurred while parsing the XML data - check it for ")
                _T("potential errors!."));
        }

        rapidxml::xml_node<> * currElement = doc.first_node();

        if(currElement)
        {
            try
            {
                // function called recursively to parse xml data
                processElement(currElement);
            }
            catch(...)
            {
                delete[] buf;
                throw;
            }
        }

        // Free memory
        delete[] buf;
    }
    //-----------------------------------------------------------------------
    void RapidXMLDocument::processElement(const rapidxml::xml_node<> * element)
    {
        // build pdc block for the element
        PropertyData attrs;

        rapidxml::xml_attribute<> * currAttr = element->first_attribute(0);

        while(currAttr)
        {
            attrs.add(d_handler->get(reinterpret_cast<Nutf8 *>(currAttr->name())),
                reinterpret_cast<Nutf8 *>(currAttr->value()));
            currAttr = currAttr->next_attribute();
        }

        // start element
        d_handler->onBegin(d_handler->get(reinterpret_cast<Nutf8 *>(element->name())), &attrs);

        // do children
        rapidxml::xml_node<> * childNode = element->first_node();

        while(childNode)
        {
            switch(childNode->type())
            {
            case rapidxml::node_element:
                processElement(childNode);
                break;
            case rapidxml::node_data:
                if(childNode->value() != '\0')
                    d_handler->onData(reinterpret_cast<Nutf8 *>(childNode->value()));
                break;
                // Silently ignore unhandled node type
            };

            childNode = childNode->next_sibling();
        }
        // end element
        d_handler->onEnd(d_handler->get(reinterpret_cast<Nutf8 *>(element->name())));
    }
    //------------------------------------------------------------------------
    RapidXMLParser::RapidXMLParser(void)
    {
        // set ID string
        mPatternID = N_Pattern_RapidXML;
    }
    //------------------------------------------------------------------------
    RapidXMLParser::~RapidXMLParser()
    {
    }
    //------------------------------------------------------------------------
    void RapidXMLParser::parse(XmlAnalyze * xc, const DataStream * source)
    {
        RapidXMLDocument doc(xc, source);
    }
    //------------------------------------------------------------------------
    bool RapidXMLParser::_init()
    {
        // This used to prevent deletion of line ending in the middle of a text.
        // WhiteSpace cleaning will be available throught the use of String methods
        // directly: rapidxml::xml_document<>::SetCondenseWhiteSpace(false);
        return true;
    }
    //------------------------------------------------------------------------
    bool RapidXMLParser::_end()
    {
        return true;
    }
    //------------------------------------------------------------------------
}