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
#ifndef _NII_XERCES_XML_SYS_H_
#define _NII_XERCES_XML_SYS_H_

#include "NiiPreInclude.h"
#include "NiiXercesXmlPattern.h"
#include "CEGUI/XMLParserModules/Xerces/XMLParserProperties.h"

#if (defined( __WIN32__ ) || defined( _WIN32 )) && !defined(CEGUI_STATIC)
    #ifdef CEGUIXERCESPARSER_EXPORTS
        #define CEGUIXERCESPARSER_API __declspec(dllexport)
    #else
        #define CEGUIXERCESPARSER_API __declspec(dllimport)
    #endif
#else
    #define CEGUIXERCESPARSER_API
#endif


// Xerces-C includes
#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/TransService.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/sax2/Attributes.hpp>
#include <xercesc/sax2/DefaultHandler.hpp>
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>

namespace NII
{
    class XercesHandler : public XERCES_CPP_NAMESPACE::DefaultHandler
    {
    public:
        XercesHandler(XmlAnalyze * handler);
        ~XercesHandler(void);

        // Implementation of methods in Xerces DefaultHandler.
        void begin(const XMLCh * const uri, const XMLCh * const localname, const XMLCh * const qname,
            const XERCES_CPP_NAMESPACE::Attributes & attrs);
        void end(const XMLCh * const uri, const XMLCh * const localname, const XMLCh * const qname);
#if _XERCES_VERSION >= 30000
        void characters(const XMLCh * const chars, const XMLSize_t length);
#else /* _XERCES_VERSION >= 30000 */
        void characters (const XMLCh * const chars, const Nui32 length);
#endif /* _XERCES_VERSION >= 30000 */
        void warning (const XERCES_CPP_NAMESPACE::SAXParseException & exc);
        void error (const XERCES_CPP_NAMESPACE::SAXParseException & exc);
        void fatalError (const XERCES_CPP_NAMESPACE::SAXParseException & exc);
    protected:
        XmlAnalyze * d_handler;      //!< This is the 'real' CEGUI based handler which we interface via.
    };

    /** Xerces-C++
    @version NIIEngine 3.0.0
    */
    class CEGUIXERCESPARSER_API XercesParser : public XmlPattern
    {
    public:
        XercesParser(void);
        ~XercesParser(void);

        /// @copydetails XmlPattern::parse
        void parse(XmlAnalyze * prc, const DataStream * src);

        /** Populate the NII::PropertyData object with attribute data from the Xerces pdc block.
        */
        static void populateAttributesBlock(const XERCES_CPP_NAMESPACE::Attributes & src, PropertyData& dest , XmlAnalyze * prc);

        /** Return a NII::String containing the Xerces XMLChar string data in \a xmlch_str.
        @param xmlch_str The string data.
        @param length
            The size of the string data. It can be computed using \code XMLString::stringLen(xmlch_str) \endcode
         */
        static String transcodeXmlCharToString(const XMLCh * const xmlch_str, Nui32 length);

        /** Sets the default resource group to be used when loading schema files.
        @param gid String describing the default resource group identifier to be used.
        @return Nothing.
        */
        static void setSchemaDefaultResourceGroup(GroupID gid)
        { 
            d_defaultSchemaResourceGroup = gid; 
        }

        /** Returns the default resource group used when loading schema files.
        @return String describing the default resource group identifier..
        */
        static const String & getSchemaDefaultResourceGroup()
        { 
            return d_defaultSchemaResourceGroup; 
        }
    protected:
        static void initialiseSchema(XERCES_CPP_NAMESPACE::SAX2XMLReader* reader, const String & schemaName);
        static XERCES_CPP_NAMESPACE::SAX2XMLReader* createReader(XERCES_CPP_NAMESPACE::DefaultHandler & handler);
        static void doParse(XERCES_CPP_NAMESPACE::SAX2XMLReader * parser, const DataStream * source);

        ///@copydetails Pattern::_init
        bool _init();
        
        ///@copydetails Pattern::_end
        bool _end();

        /// holds the default resource group ID for loading schemas.
        static String d_defaultSchemaResourceGroup;
        /// PropertyCmd for accessing the default schema resource group ID.
        static XercesParserProperties::SchemaDefaultResourceGroup
            s_schemaDefaultResourceGroupProperty;
    };
}
#endif