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
#ifndef _Samples_xmlHandler_h_
#define _Samples_xmlHandler_h_

#include "CEGUI/ChainedXMLHandler.h"
#include "CEGUI/String.h"

    class SamplesFramework;

    using namespace CEGUI;

    class Samples_xmlHandler : public ChainedXMLHandler
    {
    public:
        Samples_xmlHandler(SamplesFramework * samplesFramework);
        virtual ~Samples_xmlHandler();

        static const String ElementName;

        /// @copydetails
        const String & getXSD() const;
    protected:
        // implement ChainedXMLHandler interface.
        void onComBegin(PropertyID pid, const PropertyData * pdc);
        void onComEnd(PropertyID pid);

        SamplesFramework * d_samplesFramework;
    };

    //------------------------------------------------------------------------
    class SampleDataHandler : public ChainedXMLHandler
    {
    public:
        static const String ElementName;

        static const String NameAttribute;
        static const String SummaryAttribute;
        static const String DescriptionAttribute;
        static const String CreditsAttribute;
        static const String SampleTypeAttribute;
        static const String SampleTypeCppModule;
        static const String SampleTypeLua;
        static const String SampleTypePython;

        SampleDataHandler(const PropertyData * pdc, const String & name_prefix,
            SamplesFramework * samplesFramework);

        virtual ~SampleDataHandler();
    protected:
        // implement ChainedXMLHandler interface.
        void onComBegin(PropertyID pid, const PropertyData * pdc);
        void onComEnd(PropertyID pid);

        SamplesFramework * d_samplesFramework;
    };

#endif