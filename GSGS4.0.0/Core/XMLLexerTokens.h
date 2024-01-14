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

#ifndef XMLLexerTokens_H
#define XMLLexerTokens_H

enum {
    kXML_T_XML_OPEN_TAG = 400, // <?
    kXML_T_XML_CLOSE_TAG,      // ?>
    kXML_T_CLOSE_TAG_PREFIX,   // </
    kXML_T_CLOSE_TAG_SUFFIX,   // />
    kXML_T_IDENTIFIER,
    kXML_T_STRING,
    kXML_T_COMMENT_START,
    kXML_T_COMMENT_END,
    kXML_T_CDATA_START,
    kXML_T_CDATA_END,
    // XML entitites
    kXML_T_ENTITY_LT,
    kXML_T_ENTITY_GT,
    kXML_T_ENTITY_AMP,
    kXML_T_ENTITY_APOS,
    kXML_T_ENTITY_QUOATATION_MARK,
    kXML_T_HTML_DOCTYPE,
};

#endif
