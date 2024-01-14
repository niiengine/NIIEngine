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
#include "NiiXercesXmlPattern.h"
#include "CEGUI/String.h"
#include "NiiUIException.h"
#include "NiiUIManager.h"
#include "NiiXmlAnalyze.h"
#include "NiiPropertyData.h"

#include <xercesc/validators/schema/SchemaValidator.hpp>

#include <iostream> // Debug

namespace NII
{
    // Static data definition for default schema resource group name
    String XercesParser::d_defaultSchemaResourceGroup("");
    // static data defiinition of the SchemaDefaultResourceGroup property.
    XercesParserProperties::SchemaDefaultResourceGroup
        XercesParser::s_schemaDefaultResourceGroupProperty;
    //------------------------------------------------------------------------
    XercesParser::XercesParser(void)
    {
        // set ID string
        mPatternID = N_Pattern_Xerces;
        // add property
        getProperty()->add(&s_schemaDefaultResourceGroupProperty);
    }
    //------------------------------------------------------------------------
    XercesParser::~XercesParser(void)
    {
    }
    //------------------------------------------------------------------------
    void XercesParser::parse(XmlAnalyze * xc, const DataStream * source)
    {
        XERCES_CPP_NAMESPACE_USE;

        XercesHandler xercesHandler(xc);

        // create parser
        SAX2XMLReader * reader = createReader(xercesHandler);

        try
        {
            // set up schema
            initialiseSchema(reader, xc->getXSD());
            // do parse
            doParse(reader, source);
        }
        catch(const XMLException & exc)
        {
            if(exc.getCode() != XMLExcepts::NoError)
            {
                delete reader;

                char * excmsg = XMLString::transcode(exc.getMessage());
                String message("An error occurred at line nr. " + Nid(exc.getSrcLine()) + " while parsing XML.  Additional information: ");
                message += excmsg;
                XMLString::release(&excmsg);

                N_EXCEPT(IO, message);
            }

        }
        catch(const SAXParseException & exc)
        {
            delete reader;

            char * excmsg = XMLString::transcode(exc.getMessage());
            String message("An error occurred at line nr. " + Nid(exc.getLineNumber()) + " while parsing XML.  Additional information: ");
            message += excmsg;
            XMLString::release(&excmsg);

            N_EXCEPT(IO, message);
        }
        catch(...)
        {
            delete reader;

            N_Only(Log).log(_T("An unexpected error occurred while parsing XML"));
            throw;
        }

        // cleanup
        delete reader;
    }
    //------------------------------------------------------------------------
    bool XercesParser::_init()
    {
        XERCES_CPP_NAMESPACE_USE;

        // initialise Xerces-C XML system
        try
        {
            XMLPlatformUtils::Initialize();
        }
        catch(XMLException & exc)
        {
            // plan a message about the failure
            char * excmsg = XMLString::transcode(exc.getMessage());
            String message("An exception occurred while initialising the Xerces-C XML system.  Additional information: ");
            message += excmsg;
            XMLString::release(&excmsg);

            // throw a C string (because it won't try and use logger, which may not be available)
            throw(message.c_str());
        }

        return true;
    }
    //------------------------------------------------------------------------
    bool XercesParser::_end()
    {
        // cleanup XML stuff
        XERCES_CPP_NAMESPACE_USE;
        XMLPlatformUtils::Terminate();
        return true;
    }
    //------------------------------------------------------------------------
    void XercesParser::populateAttributesBlock(const XERCES_CPP_NAMESPACE::Attributes & src,
        PropertyData & dest, XmlAnalyze * prc)
    {
        XERCES_CPP_NAMESPACE_USE;
        String attributeName;
        String attributeValue;

        for(NCount i = 0; i < src.getLength(); ++i)
        {
            // TODO dalfy: Optimize this using temporary value.
            attributeName = transcodeXmlCharToString(src.getLocalName(i), XMLString::stringLen(src.getLocalName(i)));
            attributeValue = transcodeXmlCharToString(src.getValue(i), XMLString::stringLen(src.getValue(i)));
            dest.add(prc->get(attributeName), attributeValue);
        }
    }
    //------------------------------------------------------------------------
    String XercesParser::transcodeXmlCharToString(const XMLCh * const xmlch_str, Nui32 inputLength)
    {
        XERCES_CPP_NAMESPACE_USE;

        XMLTransService::Codes  res;
        XMLTranscoder * transcoder = XMLPlatformUtils::fgTransService->makeNewTranscoderFor(XMLRecognizer::UTF_8, res, 4096, XMLPlatformUtils::fgMemoryManager );

        if(res == XMLTransService::Ok)
        {
            String out;
#if _XERCES_VERSION >= 30000
            XMLByte outBuff[128];
            XMLSize_t outputLength;
            XMLSize_t eaten = 0;
            XMLSize_t offset = 0;
#else /* _XERCES_VERSION >= 30000 */
            utf8 outBuff[128];
            Nui32 outputLength;
            Nui32 eaten = 0;
            Nui32 offset = 0;
#endif /* _XERCES_VERSION >= 30000 */

            while(inputLength)
            {
                outputLength = transcoder->transcodeTo(xmlch_str + offset, inputLength, outBuff, 128, eaten, XMLTranscoder::UnRep_RepChar);
                out.append((Nutf8 *)outBuff, outputLength);
                offset += eaten;
                inputLength -= eaten;
            }

            delete transcoder;

            return out;
        }
        else
        {
            N_EXCEPT(Generic, _T("Internal Error: Could not create UTF-8 string transcoder."));
        }

    }
    //------------------------------------------------------------------------
    void XercesParser::initialiseSchema(XERCES_CPP_NAMESPACE::SAX2XMLReader * reader,
        const String & schemaName)
    {
        XERCES_CPP_NAMESPACE_USE;

        // enable schema use and set validation options
        reader->setFeature(XMLUni::fgXercesSchema, true);
        reader->setFeature(XMLUni::fgSAX2CoreValidation, true);
        reader->setFeature(XMLUni::fgXercesValidationErrorAsFatal, true);

        // load the schema from the resource group
        N_Only(Log).log(_T("Attempting to load schema from file '") + schemaName + "'.");
        DataStream * data = N_Only(ResourceScheme).open(schemaName, d_defaultSchemaResourceGroup);

        Nui8 * data1;
        if(data->isSysMemory() == false)
            data->cloneData(data1);
        else
            data1 = static_cast<MemStream *>(data)->getData();

        // wrap schema data in a xerces MemBufInputSource object
        MemBufInputSource schemaData(data1,
            static_cast<const Nui32>(data.getSize()), schemaName.c_str(), false);

        reader->loadGrammar(schemaData, Grammar::SchemaGrammarType, true);
        // enable grammar reuse
        reader->setFeature(XMLUni::fgXercesUseCachedGrammarInParse, true);

        // set schema for usage
        XMLCh * pval = XMLString::transcode(schemaName.c_str());
        reader->set(XMLUni::fgXercesSchemaExternalNoNameSpaceSchemaLocation, pval);
        XMLString::release(&pval);
        N_Only(Log).log(_T("XML schema file '") + schemaName + _T("' has been initialised."));

        N_delete data;

        if(data->isSysMemory() == false)
            N_free(data1);
    }
    //------------------------------------------------------------------------
    XERCES_CPP_NAMESPACE::SAX2XMLReader * XercesParser::createReader(XERCES_CPP_NAMESPACE::DefaultHandler& xc)
    {
        XERCES_CPP_NAMESPACE_USE;

        SAX2XMLReader * reader = XMLReaderFactory::createXMLReader();

        // set basic settings we want from parser
        reader->setFeature(XMLUni::fgSAX2CoreNameSpaces, true);

        // set handlers
        reader->setContentHandler(&xc);
        reader->setErrorHandler(&xc);

        return reader;
    }
    //------------------------------------------------------------------------
    void XercesParser::doParse(XERCES_CPP_NAMESPACE::SAX2XMLReader * parser, const DataStream * source)
    {
        XERCES_CPP_NAMESPACE_USE;

        Nui8 * data;
        if(source->isSysMemory() == false)
            source->cloneData(data);
        else
            data = static_cast<MemStream *>(source)->getData();

        MemBufInputSource fileData(data, source->size(), "Unknown", false);

         parser->parse(fileData);

        if(source->isSysMemory() == false)
            N_free(data);
    }
    //------------------------------------------------------------------------
    XercesHandler::XercesHandler(XmlAnalyze * xc) :
        d_handler(xc)
    {
    }
    //------------------------------------------------------------------------
    XercesHandler::~XercesHandler()
    {
    }
    //------------------------------------------------------------------------
    void XercesHandler::begin(const XMLCh * const /*uri*/, const XMLCh * const localname,
        const XMLCh* const /*qname*/, const XERCES_CPP_NAMESPACE::Attributes & attrs)
    {
        XERCES_CPP_NAMESPACE_USE;
        PropertyData pdc;
        XercesParser::populateAttributesBlock(attrs, pdc, d_handler);
        String element(XercesParser::transcodeXmlCharToString(localname, XMLString::stringLen(localname)));
        d_handler->onBegin(d_handler->get(element), &pdc);
    }
    //------------------------------------------------------------------------
    void XercesHandler::end(const XMLCh * const /*uri*/, const XMLCh * const localname, const XMLCh* const /*qname*/)
    {
        XERCES_CPP_NAMESPACE_USE;
        String element(XercesParser::transcodeXmlCharToString(localname,XMLString::stringLen(localname)));
        d_handler->onEnd(d_handler->get(element));
    }
    //------------------------------------------------------------------------
#if _XERCES_VERSION >= 30000
    void XercesHandler::characters(const XMLCh * const chars, const XMLSize_t length)
#else /* _XERCES_VERSION >= 30000 */
    void XercesHandler::characters(const XMLCh * const chars, const Nui32 length)
#endif /* _XERCES_VERSION >= 30000 */
    {
        d_handler->onData(XercesParser::transcodeXmlCharToString(chars, length));
    }
    //------------------------------------------------------------------------
    void XercesHandler::warning(const XERCES_CPP_NAMESPACE::SAXParseException & exc)
    {
        XERCES_CPP_NAMESPACE_USE;

        // plan a message about the warning
        char * excmsg = XMLString::transcode(exc.getMessage());
        String message("Xerces warning: ");
        message += excmsg;
        XMLString::release(&excmsg);
        N_Only(Log).log(message);
    }
    //------------------------------------------------------------------------
    void XercesHandler::error(const XERCES_CPP_NAMESPACE::SAXParseException & exc)
    {
        throw(exc);
    }
    //------------------------------------------------------------------------
    void XercesHandler::fatalError(const XERCES_CPP_NAMESPACE::SAXParseException & exc)
    {
        throw(exc);
    }
    //------------------------------------------------------------------------
}