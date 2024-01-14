#include "CxxScannerBase.h"
#include "CxxPreProcessor.h"
#include "gsgsFileUtil.h"
#include "CxxScannerTokens.h"
#include "gsgsLogManager.h"

namespace gsgs
{

    CxxScannerBase::CxxScannerBase(CxxPreProcessor* preProcessor, const wxFileName& filename)
        : m_scanner(NULL)
        , m_filename(filename)
        , m_options(0)
        , m_preProcessor(preProcessor)
    {
        wxString content;
        gsgs::FileUtil::ReadFileContent(filename, content, wxConvISO8859_1);
        m_scanner = ::LexerNew(content, m_preProcessor->GetOptions());
    }

    CxxScannerBase::~CxxScannerBase()
    {
        if(m_scanner) {
            ::LexerDestroy(&m_scanner);
        }
    }

    void CxxScannerBase::Parse()
    {
        CxxLexerToken token;
        while(::LexerNext(m_scanner, token)) {
            OnToken(token);
        }
    }

    CIncludeStatementCollector::CIncludeStatementCollector(CxxPreProcessor* pp, const wxFileName& filename) : 
        CxxScannerBase(pp, filename)
    {
    }

    CIncludeStatementCollector::~CIncludeStatementCollector() {}

    void CIncludeStatementCollector::OnToken(CxxLexerToken& token)
    {
        // Pre Processor state
        switch (token.GetType()) {
        case T_PP_INCLUDE_FILENAME: {
            // we found an include statement, recurse into it
            wxFileName include;
            if (m_preProcessor->ExpandInclude(m_filename, token.GetWXString(), include)) {
                CIncludeStatementCollector* scanner = new CIncludeStatementCollector(m_preProcessor, include);
                scanner->Parse();
                wxDELETE(scanner);
                gsgsDEBUG1() << "<== Resuming parser on file:" << m_filename << gsgs::clEndl;
            }
            break;
        }
        default:
            break;
        }
    }
}