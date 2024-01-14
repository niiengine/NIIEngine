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
#include "gsgsComment.h"
#include "gsgsLanguage.h"
#include "gsgsPreProcess.h"
#include <wx/datetime.h>

#define trimMe(str)            \
    {                          \
        str = str.Trim();      \
        str = str.Trim(false); \
    }

namespace gsgs
{
    CommentConfigData::CommentConfigData()
    {
        m_classPattern << " * @class $(Name)\n";
        m_classPattern << " * @author $(User)\n";
        m_classPattern << " * @date $(Date)\n";
        m_classPattern << " * @file $(CurrentFileName).$(CurrentFileExt)\n";
        m_classPattern << " * @brief \n";
        m_functionPattern << " * @brief \n";
    }

    CommentConfigData::~CommentConfigData() {}

    void CommentConfigData::DeSerialize(Serializer& arch)
    {
        arch.Read("m_addStarOnCComment", m_addStarOnCComment);
        arch.Read("m_continueCppComment", m_continueCppComment);
        arch.Read("m_autoInsert", m_autoInsert);
        arch.Read("m_useQtStyle", m_useQtStyle);
        m_classPattern.Replace("|", "\n");
        arch.Read("m_functionPattern", m_functionPattern);
        m_functionPattern.Replace("|", "\n");
        arch.Read("m_functionPattern", m_functionPattern);
    }

    void CommentConfigData::Serialize(Serializer& arch)
    {
        arch.Write("m_addStarOnCComment", m_addStarOnCComment);
        arch.Write("m_continueCppComment", m_continueCppComment);
        arch.Write("m_autoInsert", m_autoInsert);
        arch.Write("m_useQtStyle", m_useQtStyle);
        m_classPattern.Replace("\n", "|");
        arch.Write("m_classPattern", m_classPattern);
        m_functionPattern.Replace("\n", "|");
        arch.Write("m_functionPattern", m_functionPattern);
    }

    CppCommentCreator::CppCommentCreator(TagEntryPtr tag, wxChar keyPrefix)
        : CommentCreator(keyPrefix)
        , m_tag(tag)
    {
    }

    CppCommentCreator::~CppCommentCreator() {}

    wxString CppCommentCreator::CreateComment()
    {
        if(m_tag->GetKind() == wxT("class") || m_tag->GetKind() == wxT("struct")) {
            return wxT("$(ClassPattern)\n");
        } else if(m_tag->IsMethod()) {
            return FunctionComment();
        } else {
            return wxEmptyString;
        }
    }

    wxString CppCommentCreator::FunctionComment()
    {
        wxString comment;

        // parse the function signature
        std::vector<TagEntryPtr> tags;
        Language* lang = LanguageST::Get();
        lang->GetLocalVariables(m_tag->GetSignature(), tags, true);

        comment << wxT("$(FunctionPattern)\n");
        for(size_t i = 0; i < tags.size(); i++)
            comment << wxT(" * ") << m_keyPrefix << wxT("param ") << tags.at(i)->GetName() << wxT("\n");

        if(m_tag->GetKind() == wxT("function")) {
            gsgs::Function f;
            if(lang->FunctionFromPattern(m_tag, f)) {
                wxString type = _U(f.m_returnValue.m_type.c_str());
                trimMe(type);
                if(type != wxT("void")) { // void has no return value
                    comment << wxT(" * ") << m_keyPrefix << wxT("return \n");
                }
            }

        } else {
            Variable var;
            lang->VariableFromPattern(m_tag->GetPattern(), m_tag->GetName(), var);
            wxString type = _U(var.m_type.c_str());
            wxString name = _U(var.m_name.c_str());
            trimMe(type);
            trimMe(name);

            if(type != wxT("void") // void has no return value
               && name != type) {  // and not a constructor
                comment << wxT(" * ") << m_keyPrefix << wxT("return \n");
            }
        }
        return comment;
    }
}