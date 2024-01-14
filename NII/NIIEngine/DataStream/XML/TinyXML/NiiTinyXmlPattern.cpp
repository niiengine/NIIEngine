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
#include "NiiTinyXmlPattern.h"
#include "NiiUIManager.h"
#include "NiiXmlAnalyze.h"
#include "NiiPropertyData.h"
#include "NiiUIException.h"
#include <tinyxml.h>

    //------------------------------------------------------------------------
    // These are to support the <=2.5 and >=2.6 API versions
#ifdef CEGUI_TINYXML_HAS_2_6_API
    #define CEGUI_TINYXML_ELEMENT TINYXML_ELEMENT
    #define CEGUI_TINYXML_TEXT TINYXML_TEXT
#else
    #define CEGUI_TINYXML_ELEMENT ELEMENT
    #define CEGUI_TINYXML_TEXT TEXT
#endif
namespace NII
{
    //------------------------------------------------------------------------
    class TinyXMLDocument : public TiXmlDocument
    {
    public:
        TinyXMLDocument(XmlAnalyze * xc, const DataStream * source);
        ~TinyXMLDocument(){}
    protected:
        void processElement(const TiXmlElement* element);
    private:
        XmlAnalyze * d_handler;
    };
    //------------------------------------------------------------------------
    TinyXMLDocument::TinyXMLDocument(XmlAnalyze * xc, const DataStream * source)
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
        buf[size+1] = 0;

        // Parse the document
        TiXmlDocument doc;
        if (!doc.Parse((const char *)buf))
        {
            // error detected, cleanup out buffers
            delete[] buf;

            // throw exception
            N_EXCEPT(IO, _T("an error occurred while ")
                _T("parsing the XML document - check it for potential errors!."));
        }

        const TiXmlElement* currElement = doc.RootElement();
        if (currElement)
        {
            try
            {
                // function called recursively to parse xml data
                processElement(currElement);
            }
            catch(...)
            {
                delete [] buf;

                throw;
            }
        } // if (currElement)

        // Free memory
        delete [] buf;
    }
    //------------------------------------------------------------------------
    void TinyXMLDocument::processElement(const TiXmlElement * element)
    {
        // build pdc block for the element
        PropertyData attrs;

        const TiXmlAttribute * currAttr = element->FirstAttribute();
        while(currAttr)
        {
            attrs.add(d_handler->get((Nutf8 *)currAttr->Name()), (Nutf8 *)currAttr->Value());
            currAttr = currAttr->Next();
        }

        // start element
        d_handler->onBegin(d_handler->get((Nutf8 *)element->Value()), &attrs);

        // do children
        const TiXmlNode * childNode = element->FirstChild();
        while(childNode)
        {
            switch(childNode->Type())
            {
            case TiXmlNode::CEGUI_TINYXML_ELEMENT:
                processElement(childNode->ToElement());
                break;
            case TiXmlNode::CEGUI_TINYXML_TEXT:
                if(childNode->ToText()->Value() != '\0')
                    d_handler->onData((Nutf8*)childNode->ToText()->Value());
                break;

                // Silently ignore unhandled node type
            };
            childNode = childNode->NextSibling();
        }

        // end element
        d_handler->onEnd(d_handler->get((Nutf8 *)element->Value()));
    }
    //------------------------------------------------------------------------
    TinyXMLParser::TinyXMLParser()
    {
        // set ID string
        mPatternID = N_Pattern_TinyXML;
    }
    //------------------------------------------------------------------------
    TinyXMLParser::~TinyXMLParser()
    {
    }
    //------------------------------------------------------------------------
    void TinyXMLParser::parse(XmlAnalyze * xc, const DataStream * source)
    {
        TinyXMLDocument doc(xc, source);
    }
    //------------------------------------------------------------------------
    bool TinyXMLParser::_init(void)
    {
        // This used to prevent deletion of line ending in the middle of a text.
        // WhiteSpace cleaning will be available throught the use of String methods directly
        //TiXmlDocument::SetCondenseWhiteSpace(false);
        return true;
    }
    //------------------------------------------------------------------------
    void TinyXMLParser::_end()
    {
        return true;
    }
    //------------------------------------------------------------------------
}