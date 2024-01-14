#include "gsgsStyleManager.h"
#include "EclipseThemeImporterManager.h"
#include "gsgsPathManager.h"
#include "gsgsFileUtil.h"
#include "globals.h"
#include <wx/stc/stc.h>
#include <wx/colour.h>
#include "xmlutils.h"

namespace gsgs
{
    EclipseThemeImporterBase::EclipseThemeImporterBase() {}

    EclipseThemeImporterBase::~EclipseThemeImporterBase() {}

    bool EclipseThemeImporterBase::GetProperty(const wxString& name, EclipseThemeImporterBase::Property& prop) const
    {
        prop.colour = "";
        prop.isBold = false;
        prop.isItalic = false;
        if (!m_doc.IsOk()) return false;

        wxXmlNode* child = m_doc.GetRoot()->GetChildren();
        while (child) {
            if (child->GetName() == name) {
                prop.colour = child->GetAttribute("color");
                prop.isBold = child->GetAttribute("bold", "false") == "true";
                prop.isItalic = child->GetAttribute("italic", "false") == "true";
                return true;
            }
            child = child->GetNext();
        }
        return false;
    }

    LexerStyle::Ptr_t
        EclipseThemeImporterBase::InitializeImport(const wxFileName& eclipseXml, const wxString& langName, int langId)
    {
        m_langName = langName;
        if (!m_doc.Load(eclipseXml.GetFullPath())) return NULL;

        LexerStyle::Ptr_t lexer(new LexerStyle());

        // Add the lexer basic properties (laguage, file extensions, keywords, name)
        AddBaseProperties(lexer, m_langName, wxString::Format("%d", langId));

        // Read the basic properties
        if (!GetProperty("background", m_background)) return NULL;
        if (!GetProperty("foreground", m_foreground)) return NULL;
        if (!GetProperty("selectionForeground", m_selectionForeground)) return NULL;
        if (!GetProperty("selectionBackground", m_selectionBackground)) return NULL;
        if (!GetProperty("lineNumber", m_lineNumber)) return NULL;
        if (!GetProperty("singleLineComment", m_singleLineComment)) return NULL;
        if (!GetProperty("multiLineComment", m_multiLineComment)) return NULL;
        if (!GetProperty("number", m_number)) return NULL;
        if (!GetProperty("string", m_string)) return NULL;
        if (!GetProperty("operator", m_oper)) return NULL;
        if (!GetProperty("keyword", m_keyword)) return NULL;
        if (!GetProperty("class", m_klass)) {
            m_klass = m_foreground;
        }
        if (!GetProperty("parameterVariable", m_variable)) {
            m_variable = m_foreground;
        }
        if (!GetProperty("javadoc", m_javadoc)) {
            m_javadoc = m_multiLineComment;
        }

        if (!GetProperty("javadocKeyword", m_javadocKeyword)) {
            m_javadocKeyword = m_multiLineComment;
        }

        m_oper = m_foreground;
        return lexer;
    }

    void EclipseThemeImporterBase::FinalizeImport(LexerStyle::Ptr_t lexer)
    {
        AddCommonProperties(lexer);
        gsgs_Style().UpdateLexerColours(lexer, true);
    }

    bool EclipseThemeImporterBase::IsDarkTheme() const
    {
        // load the theme background colour
        EclipseThemeImporterBase::Property p;
        if (!GetProperty("background", p)) return false;

        // test the colour
        return gsgs::Colour::IsDark(p.colour);
    }

    wxString EclipseThemeImporterBase::GetName() const
    {
        if (!IsValid()) return "";
        return m_doc.GetRoot()->GetAttribute("name");
    }

    wxString EclipseThemeImporterBase::GetOutputFile(const wxString& language) const
    {
        wxString name = GetName();
        name.MakeLower();

        // Normalize the file name
        name.Replace(" ", "_");
        name.Replace("::", "_");
        name.Replace("(", "_");
        name.Replace(")", "_");
        name.Replace(":", "_");
        name.Replace(",", "_");
        name.Replace(".", "_");
        name.Replace(";", "_");

        wxString xmlFileName;
        xmlFileName << "lexer_" << language.Lower() << "_" << name << ".xml";
        return xmlFileName;
    }

    void EclipseThemeImporterBase::AddProperty(LexerStyle::Ptr_t lexer,
        const wxString& id,
        const wxString& name,
        const wxString& colour,
        const wxString& bgColour,
        bool bold,
        bool italic,
        bool isEOLFilled)
    {
        wxASSERT(!colour.IsEmpty());
        wxASSERT(!bgColour.IsEmpty());

        long ID;
        id.ToCLong(&ID);

        StyleProperty sp(ID, colour, bgColour, 11, name, "", bold, italic, false, isEOLFilled, 50);
        lexer->GetLexerProperties().insert(std::make_pair(sp.GetId(), sp));
    }

    void EclipseThemeImporterBase::AddBaseProperties(LexerStyle::Ptr_t lexer, const wxString& lang, const wxString& id)
    {
        lexer->SetName(lang);
        lexer->SetThemeName(GetName());
        lexer->SetIsActive(false);
        lexer->SetUseCustomTextSelectionFgColour(true);
        lexer->SetStyleWithinPreProcessor(true);
        long ID;
        id.ToCLong(&ID);
        lexer->SetLexerId(ID);
        lexer->SetKeyWords(GetKeywords0(), 0);
        lexer->SetKeyWords(GetKeywords1(), 1);
        lexer->SetKeyWords(GetKeywords2(), 2);
        lexer->SetKeyWords(GetKeywords3(), 3);
        lexer->SetKeyWords(GetKeywords4(), 4);
        lexer->SetFileSpec(GetFileExtensions());
    }

    void EclipseThemeImporterBase::AddCommonProperties(LexerStyle::Ptr_t lexer)
    {
        // Set the brace match based on the background colour
        Property background, foreground, selectionBackground, selectionForeground, lineNumber;

        GetProperty("background", background);
        GetProperty("foreground", foreground);
        GetProperty("selectionForeground", selectionForeground);
        GetProperty("selectionBackground", selectionBackground);
        GetProperty("lineNumber", lineNumber);

        wxString whitespaceColour;
        if (IsDarkTheme()) {
            // dark theme
            // Whitespace should be a bit lighether
            whitespaceColour = wxColour(background.colour).ChangeLightness(150).GetAsString(wxC2S_HTML_SYNTAX);
            AddProperty(lexer, "34", "Brace match", "yellow", background.colour, true);
            AddProperty(lexer, "35", "Brace bad match", "red", background.colour, true);
            AddProperty(lexer, "37", "Indent Guide", background.colour, background.colour);

        }
        else {
            // light theme
            whitespaceColour = wxColour(background.colour).ChangeLightness(50).GetAsString(wxC2S_HTML_SYNTAX);
            AddProperty(lexer, "34", "Brace match", "black", "cyan", true);
            AddProperty(lexer, "35", "Brace bad match", "black", "red", true);
            AddProperty(lexer, "37", "Indent Guide", whitespaceColour, background.colour);
        }
        AddProperty(lexer, "-1", "Fold Margin", background.colour, background.colour);
        AddProperty(lexer, "-2", "Text Selection", selectionForeground.colour, selectionBackground.colour);
        AddProperty(lexer, "-3", "Caret Colour", IsDarkTheme() ? "white" : "black", background.colour);
        AddProperty(lexer, "-4", "Whitespace", whitespaceColour, background.colour);
        AddProperty(lexer, "38", "Calltip", foreground.colour, background.colour);
        AddProperty(
            lexer, "33", "Line Numbers", lineNumber.colour, background.colour, lineNumber.isBold, lineNumber.isItalic);
    }

    void EclipseThemeImporterBase::DoSetKeywords(wxString& wordset, const wxString& words)
    {
        wordset.clear();
        wxArrayString arr = ::wxStringTokenize(words, " \t\n", wxTOKEN_STRTOK);
        arr.Sort();

        wordset = ::wxJoin(arr, ' ');
    }

    class _gsgsAPI EclipseXMLThemeImporter : public EclipseThemeImporterBase
    {
    public:
        EclipseXMLThemeImporter();
        virtual ~EclipseXMLThemeImporter();

    public:
        virtual LexerStyle::Ptr_t Import(const wxFileName& eclipseXmlFile);
    };

    class _gsgsAPI EclipseYAMLThemeImporter : public EclipseThemeImporterBase
    {
    public:
        EclipseYAMLThemeImporter();
        virtual ~EclipseYAMLThemeImporter();

    public:
        virtual LexerStyle::Ptr_t Import(const wxFileName& eclipseXmlFile);
    };

    class _gsgsAPI EclipseASMThemeImporter : public EclipseThemeImporterBase
    {
    public:
        virtual LexerStyle::Ptr_t Import(const wxFileName& eclipseXmlFile);
        EclipseASMThemeImporter();
        ~EclipseASMThemeImporter();
    };

    class _gsgsAPI EclipseBatchThemeImporter : public EclipseThemeImporterBase
    {
    public:
        virtual LexerStyle::Ptr_t Import(const wxFileName& eclipseXmlFile);
        EclipseBatchThemeImporter();
        ~EclipseBatchThemeImporter();
    };

    class _gsgsAPI EclipseCMakeThemeImporter : public EclipseThemeImporterBase
    {
    public:
        EclipseCMakeThemeImporter();
        virtual ~EclipseCMakeThemeImporter();

        virtual LexerStyle::Ptr_t Import(const wxFileName& eclipseXmlFile);
    };

    class _gsgsAPI EclipsePythonThemeImporter : public EclipseThemeImporterBase
    {
    public:
        virtual LexerStyle::Ptr_t Import(const wxFileName& eclipseXmlFile);
        EclipsePythonThemeImporter();
        ~EclipsePythonThemeImporter();
    };

    class _gsgsAPI EclipseCobraAltThemeImporter : public EclipsePythonThemeImporter
    {
    public:
        EclipseCobraAltThemeImporter();
        virtual ~EclipseCobraAltThemeImporter();

    };

    class _gsgsAPI EclipseCobraThemeImporter : public EclipsePythonThemeImporter
    {
    public:
        EclipseCobraThemeImporter();
        virtual ~EclipseCobraThemeImporter();

    };

    class _gsgsAPI EclipseCSSThemeImporter : public EclipseThemeImporterBase
    {
    public:
        EclipseCSSThemeImporter();
        virtual ~EclipseCSSThemeImporter();

    public:
        virtual LexerStyle::Ptr_t Import(const wxFileName& eclipseXmlFile);
    };

    class _gsgsAPI EclipseCXXThemeImporter : public EclipseThemeImporterBase
    {
    public:
        EclipseCXXThemeImporter();
        virtual ~EclipseCXXThemeImporter();

        /**
         * @brief convert _all_ C++ xml files into eclipse format
         */
        static std::vector<wxFileName> ToEclipseXMLs();

        /**
         * @brief import an eclipse XML colour theme
         * @param eclipseXmlFile
         * @param gsgsXml [output] the output file name
         */
        virtual LexerStyle::Ptr_t Import(const wxFileName& eclipseXmlFile);
    private:
        /**
         * @brief convert a lexer object into eclipse XML format
         */
        static wxFileName ToEclipseXML(LexerStyle::Ptr_t cxxLexer, size_t id);
    };

    class _gsgsAPI EclipseDiffThemeImporter : public EclipseThemeImporterBase
    {
    public:
        EclipseDiffThemeImporter();
        virtual ~EclipseDiffThemeImporter();

    public:
        virtual LexerStyle::Ptr_t Import(const wxFileName& eclipseXmlFile);
    };

    class _gsgsAPI EclipseDockerfileThemeImporter : public EclipseThemeImporterBase
    {
    public:
        EclipseDockerfileThemeImporter();
        virtual ~EclipseDockerfileThemeImporter();

    public:
        virtual LexerStyle::Ptr_t Import(const wxFileName& eclipseXmlFile);
    };

    class _gsgsAPI EclipseFortranThemeImporter : public EclipseThemeImporterBase
    {
    public:
        virtual LexerStyle::Ptr_t Import(const wxFileName& eclipseXmlFile);
        EclipseFortranThemeImporter();
        virtual ~EclipseFortranThemeImporter();

    };

    class _gsgsAPI EclipseINIThemeImporter : public EclipseThemeImporterBase
    {
    public:
        virtual LexerStyle::Ptr_t Import(const wxFileName& eclipseXmlFile);

        EclipseINIThemeImporter();
        ~EclipseINIThemeImporter();
    };

    class _gsgsAPI EclipseInnoSetupThemeImporter : public EclipseThemeImporterBase
    {
    public:
        virtual LexerStyle::Ptr_t Import(const wxFileName& eclipseXmlFile);
        EclipseInnoSetupThemeImporter();
        virtual ~EclipseInnoSetupThemeImporter();
    };

    class _gsgsAPI EclipseJavaScriptThemeImporter : public EclipseThemeImporterBase
    {
    public:
        EclipseJavaScriptThemeImporter();
        virtual ~EclipseJavaScriptThemeImporter();

        /**
         * @brief import an eclipse XML colour theme
         * @param eclipseXmlFile
         * @param gsgsXml [output] the output file name
         */
        virtual LexerStyle::Ptr_t Import(const wxFileName& eclipseXmlFile);
    };

    class _gsgsAPI EclipseJavaThemeImporter : public EclipseThemeImporterBase
    {
    public:
        virtual LexerStyle::Ptr_t Import(const wxFileName& eclipseXmlFile);
        EclipseJavaThemeImporter();
        virtual ~EclipseJavaThemeImporter();
    };

    class _gsgsAPI EclipseLuaThemeImporter : public EclipseThemeImporterBase
    {
    public:
        EclipseLuaThemeImporter();
        virtual ~EclipseLuaThemeImporter();

    public:
        virtual LexerStyle::Ptr_t Import(const wxFileName& eclipseXmlFile);
    };

    class _gsgsAPI EclipseMakefileThemeImporter : public EclipseThemeImporterBase
    {
    public:
        EclipseMakefileThemeImporter();
        virtual ~EclipseMakefileThemeImporter();

    public:
        virtual LexerStyle::Ptr_t Import(const wxFileName& eclipseXmlFile);
    };

    class EclipsePHPThemeImporter : public EclipseThemeImporterBase
    {
    public:
        EclipsePHPThemeImporter();
        virtual ~EclipsePHPThemeImporter();

    public:
        virtual LexerStyle::Ptr_t Import(const wxFileName& eclipseXmlFile);
    };

    class _gsgsAPI EclipseScriptThemeImporter : public EclipseThemeImporterBase
    {
    public:
        EclipseScriptThemeImporter();
        virtual ~EclipseScriptThemeImporter();

    public:
        virtual LexerStyle::Ptr_t Import(const wxFileName& eclipseXmlFile);
    };

    class _gsgsAPI EclipseSCSSThemeImporter : public EclipseThemeImporterBase
    {
    public:
        EclipseSCSSThemeImporter();
        virtual ~EclipseSCSSThemeImporter();

    public:
        virtual LexerStyle::Ptr_t Import(const wxFileName& eclipseXmlFile);
    };

    class _gsgsAPI EclipseSQLThemeImporter : public EclipseThemeImporterBase
    {
    public:
        EclipseSQLThemeImporter();
        virtual ~EclipseSQLThemeImporter();

    public:
        virtual LexerStyle::Ptr_t Import(const wxFileName& eclipseXmlFile);
    };

    class _gsgsAPI EclipseTextThemeImporter : public EclipseThemeImporterBase
    {
    public:
        EclipseTextThemeImporter();
        virtual ~EclipseTextThemeImporter();

    public:
        virtual LexerStyle::Ptr_t Import(const wxFileName& eclipseXmlFile);
    };

    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    // EclipseXMLThemeImporter
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    EclipseXMLThemeImporter::EclipseXMLThemeImporter() { SetFileExtensions("*.xml;*.project;*.xrc;*.plist"); }
    //-------------------------------------------------------------------------------
    EclipseXMLThemeImporter::~EclipseXMLThemeImporter() {}
    //-------------------------------------------------------------------------------
    LexerStyle::Ptr_t EclipseXMLThemeImporter::Import(const wxFileName& eclipseXmlFile)
    {
        LexerStyle::Ptr_t lexer = InitializeImport(eclipseXmlFile, "xml", 5);
        CHECK_PTR_RET_NULL(lexer);

        wxString defaultTextColour;
        if (IsDarkTheme()) {
            defaultTextColour = "white";
            AddProperty(lexer, wxSTC_H_DEFAULT, "Default", "white", m_background.colour);

        }
        else {
            defaultTextColour = "black";
            AddProperty(lexer, wxSTC_H_DEFAULT, "Default", "black", m_background.colour);
        }

        AddProperty(lexer, wxSTC_H_TAG, "Tag", m_keyword.colour, m_background.colour);
        AddProperty(lexer, wxSTC_H_TAGUNKNOWN, "Tag Unknown", m_keyword.colour, m_background.colour);
        AddProperty(lexer, wxSTC_H_ATTRIBUTE, "Attribute", m_klass.colour, m_background.colour);
        AddProperty(lexer, wxSTC_H_ATTRIBUTEUNKNOWN, "Attribute Unknown", m_klass.colour, m_background.colour);
        AddProperty(lexer, wxSTC_H_NUMBER, "Number", m_number.colour, m_background.colour);
        AddProperty(lexer, wxSTC_H_DOUBLESTRING, "Double String", m_string.colour, m_background.colour);
        AddProperty(lexer, wxSTC_H_SINGLESTRING, "Single String", m_string.colour, m_background.colour);
        AddProperty(lexer, wxSTC_H_OTHER, "Other", m_foreground.colour, m_background.colour);
        AddProperty(lexer, wxSTC_H_COMMENT, "Comment", m_singleLineComment.colour, m_background.colour);
        AddProperty(lexer, wxSTC_H_ENTITY, "Entity", m_number.colour, m_background.colour);
        AddProperty(lexer, wxSTC_H_TAGEND, "Tag End", m_keyword.colour, m_background.colour);
        AddProperty(lexer, wxSTC_H_XMLSTART, "XML Start", m_keyword.colour, m_background.colour);
        AddProperty(lexer, wxSTC_H_XMLEND, "XML End", m_keyword.colour, m_background.colour);
        AddProperty(lexer, wxSTC_H_CDATA, "CDATA", defaultTextColour, m_background.colour);
        FinalizeImport(lexer);
        return lexer;
    }

    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    // EclipseYAMLThemeImporter
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    EclipseYAMLThemeImporter::EclipseYAMLThemeImporter()
    {
        SetFileExtensions("*.yml;*.yaml");
        m_langName = "yaml";
    }
    //-------------------------------------------------------------------------------
    EclipseYAMLThemeImporter::~EclipseYAMLThemeImporter() {}
    //-------------------------------------------------------------------------------
    LexerStyle::Ptr_t EclipseYAMLThemeImporter::Import(const wxFileName& eclipseXmlFile)
    {
        LexerStyle::Ptr_t lexer = InitializeImport(eclipseXmlFile, GetLangName(), wxSTC_LEX_YAML);
        wxString errorColour;
        if (lexer->IsDark()) {
            errorColour = "PINK";
        }
        else {
            errorColour = "RED";
        }
        AddProperty(lexer, wxSTC_YAML_DEFAULT, "Default", m_foreground.colour, m_background.colour);
        AddProperty(lexer, wxSTC_YAML_COMMENT, "Line comment", m_singleLineComment.colour, m_background.colour);
        AddProperty(lexer, wxSTC_YAML_IDENTIFIER, "Identifier", m_klass.colour, m_background.colour);
        AddProperty(lexer, wxSTC_YAML_KEYWORD, "Keyword", m_keyword.colour, m_background.colour);
        AddProperty(lexer, wxSTC_YAML_NUMBER, "Number", m_number.colour, m_background.colour);
        AddProperty(lexer, wxSTC_YAML_REFERENCE, "Reference", m_variable.colour, m_background.colour);
        AddProperty(lexer, wxSTC_YAML_DOCUMENT, "Document", m_foreground.colour, m_background.colour);
        AddProperty(lexer, wxSTC_YAML_TEXT, "Text", m_string.colour, m_background.colour);
        AddProperty(lexer, wxSTC_YAML_ERROR, "Error", errorColour, m_background.colour);
        AddProperty(lexer, wxSTC_YAML_OPERATOR, "Operator", m_oper.colour, m_background.colour);
        FinalizeImport(lexer);
        return lexer;
    }

    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    // EclipseASMThemeImporter
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    EclipseASMThemeImporter::EclipseASMThemeImporter()
    {
        SetKeywords0("aaa aad aam aas adc add and call cbw clc cld cli cmc cmp cmps cmpsb cmpsw cwd daa das dec div esc "
            "hlt idiv imul in inc int into iret ja jae jb jbe jc jcxz je jg jge jl jle jmp jna jnae jnb jnbe jnc "
            "jne jng jnge jnl jnle jno jnp jns jnz jo jp jpe jpo js jz lahf lds lea les lods lodsb lodsw loop "
            "loope loopew loopne loopnew loopnz loopnzw loopw loopz loopzw mov movs movsb movsw mul neg nop not "
            "or out pop popf push pushf rcl rcr ret retf retn rol ror sahf sal sar sbb scas scasb scasw shl shr "
            "stc std sti stos stosb stosw sub test wait xchg xlat xlatb xor bound enter ins insb insw leave outs "
            "outsb outsw popa pusha pushw arpl lar lsl sgdt sidt sldt smsw str verr verw clts lgdt lidt lldt lmsw "
            "ltr bsf bsr bt btc btr bts cdq cmpsd cwde insd iretd iretdf iretf jecxz lfs lgs lodsd loopd looped "
            "loopned loopnzd loopzd lss movsd movsx movzx outsd popad popfd pushad pushd pushfd scasd seta setae "
            "setb setbe setc sete setg setge setl setle setna setnae setnb setnbe setnc setne setng setnge setnl "
            "setnle setno setnp setns setnz seto setp setpe setpo sets setz shld shrd stosd bswap cmpxchg invd "
            "invlpg wbinvd xadd lock rep repe repne repnz repz cflush cpuid emms femms cmovo cmovno cmovb cmovc "
            "cmovnae cmovae cmovnb cmovnc cmove cmovz cmovne cmovnz cmovbe cmovna cmova cmovnbe cmovs cmovns "
            "cmovp cmovpe cmovnp cmovpo cmovl cmovnge cmovge cmovnl cmovle cmovng cmovg cmovnle cmpxchg486 "
            "cmpxchg8b loadall loadall286 ibts icebp int1 int3 int01 int03 iretw popaw popfw pushaw pushfw rdmsr "
            "rdpmc rdshr rdtsc rsdc rsldt rsm rsts salc smi smint smintold svdc svldt svts syscall sysenter "
            "sysexit sysret ud0 ud1 ud2 umov xbts wrmsr wrshr callq retq");

        SetKeywords1("f2xm1 fabs fadd faddp fbld fbstp fchs fclex fcom fcomp fcompp fdecstp fdisi fdiv fdivp fdivr fdivrp "
            "feni ffree fiadd ficom ficomp fidiv fidivr fild fimul fincstp finit fist fistp fisub fisubr fld fld1 "
            "fldcw fldenv fldenvw fldl2e fldl2t fldlg2 fldln2 fldpi fldz fmul fmulp fnclex fndisi fneni fninit "
            "fnop fnsave fnsavew fnstcw fnstenv fnstenvw fnstsw fpatan fprem fptan frndint frstor frstorw fsave "
            "fsavew fscale fsqrt fst fstcw fstenv fstenvw fstp fstsw fsub fsubp fsubr fsubrp ftst fwait fxam fxch "
            "fxtract fyl2x fyl2xp1 fsetpm fcos fldenvd fnsaved fnstenvd fprem1 frstord fsaved fsin fsincos "
            "fstenvd fucom fucomp fucompp fcomi fcomip ffreep fcmovb fcmove fcmovbe fcmovu fcmovnb fcmovne "
            "fcmovnbe fcmovnu");
        SetKeywords2("ah al ax bh bl bp bx ch cl cr0 cr2 cr3 cr4 cs cx dh di dl dr0 dr1 dr2 dr3 dr6 dr7 ds dx eax ebp ebx "
            "ecx edi edx es esi esp fs gs si sp ss st tr3 tr4 tr5 tr6 tr7 st0 st1 st2 st3 st4 st5 st6 st7 mm0 mm1 "
            "mm2 mm3 mm4 mm5 mm6 mm7 xmm0 xmm1 xmm2 xmm3 xmm4 xmm5 xmm6 xmm7");
        SetKeywords3(
            ".186 .286 .286c .286p .287 .386 .386c .386p .387 .486 .486p .8086 .8087 .alpha .break .code .const .continue "
            ".cref .data .data? .dosseg .else .elseif .endif .endw .err .err1 .err2 .errb .errdef .errdif .errdifi .erre "
            ".erridn .erridni .errnb .errndef .errnz .exit .fardata .fardata? .if .lall .lfcond .list .listall .listif "
            ".listmacro .listmacroall .model .no87 .nocref .nolist .nolistif .nolistmacro .radix .repeat .sall .seq "
            ".sfcond .stack .startup .tfcond .type .until .untilcxz .while .xall .xcref .xlist alias align assume catstr "
            "comm comment db dd df dosseg dq dt dup dw echo else elseif elseif1 elseif2 elseifb elseifdef elseifdif "
            "elseifdifi elseife elseifidn elseifidni elseifnb elseifndef end endif endm endp ends eq equ even exitm extern "
            "externdef extrn for forc ge goto group gt high highword if if1 if2 ifb ifdef ifdif ifdifi ife ifidn ifidni "
            "ifnb ifndef include includelib instr invoke irp irpc label le length lengthof local low lowword lroffset lt "
            "macro mask mod .msfloat name ne offset opattr option org %out page popcontext proc proto ptr public purge "
            "pushcontext record repeat rept seg segment short size sizeof sizestr struc struct substr subtitle subttl "
            "textequ this title type typedef union while width db dw dd dq dt resb resw resd resq rest incbin equ times "
            "%define %idefine %xdefine %xidefine %undef %assign %iassign %strlen %substr %macro %imacro %endmacro %rotate "
            ".nolist %if %elif %else %endif %ifdef %ifndef %elifdef %elifndef %ifmacro %ifnmacro %elifmacro %elifnmacro "
            "%ifctk %ifnctk %elifctk %elifnctk %ifidn %ifnidn %elifidn %elifnidn %ifidni %ifnidni %elifidni %elifnidni "
            "%ifid %ifnid %elifid %elifnid %ifstr %ifnstr %elifstr %elifnstr %ifnum %ifnnum %elifnum %elifnnum %error %rep "
            "%endrep %exitrep %include %push %pop %repl struct endstruc istruc at iend align alignb %arg %stacksize %local "
            "%line bits use16 use32 section absolute extern global common cpu org section group import export");

        SetKeywords4("$ ? @b @f addr basic byte c carry? dword far far16 fortran fword near near16 overflow? parity? "
            "pascal qword real4 real8 real10 sbyte sdword sign? stdcall sword syscall tbyte vararg word zero? "
            "flat near32 far32 abs all assumes at casemap common compact cpu dotname emulator epilogue error "
            "export expr16 expr32 farstack flat forceframe huge language large listing ljmp loadds m510 medium "
            "memory nearstack nodotname noemulator nokeyword noljmp nom510 none nonunique nooldmacros "
            "nooldstructs noreadonly noscoped nosignextend nothing notpublic oldmacros oldstructs os_dos para "
            "private prologue radix readonly req scoped setif2 smallstack tiny use16 use32 uses a16 a32 o16 o32 "
            "byte word dword nosplit $ $$ seq wrt flat large small .text .data .bss near far %0 %1 %2 %3 %4 %5 %6 "
            "%7 %8 %9");
        SetFileExtensions("*.asm;*.s;*.S");
    }
    //-------------------------------------------------------------------------------
    EclipseASMThemeImporter::~EclipseASMThemeImporter() {}
    //-------------------------------------------------------------------------------
    LexerStyle::Ptr_t EclipseASMThemeImporter::Import(const wxFileName& eclipseXmlFile)
    {
        // Initialize the import
        LexerStyle::Ptr_t lexer = InitializeImport(eclipseXmlFile, "assembly", wxSTC_LEX_ASM);

        AddProperty(lexer, wxSTC_ASM_DEFAULT, "Default", m_foreground.colour, m_background.colour);
        AddProperty(lexer, wxSTC_ASM_COMMENT, "Comment", m_singleLineComment.colour, m_background.colour);
        AddProperty(lexer, wxSTC_ASM_NUMBER, "Number", m_number.colour, m_background.colour);
        AddProperty(lexer, wxSTC_ASM_STRING, "String", m_string.colour, m_background.colour);
        AddProperty(lexer, wxSTC_ASM_OPERATOR, "Operator", m_oper.colour, m_background.colour);
        AddProperty(lexer, wxSTC_ASM_IDENTIFIER, "Identifier", m_foreground.colour, m_background.colour);
        AddProperty(lexer, wxSTC_ASM_CPUINSTRUCTION, "CPU Instruction", m_keyword.colour, m_background.colour);
        AddProperty(lexer, wxSTC_ASM_MATHINSTRUCTION, "Math Instruction", m_keyword.colour, m_background.colour);
        AddProperty(lexer, wxSTC_ASM_MATHINSTRUCTION, "Math Instruction", m_keyword.colour, m_background.colour);
        AddProperty(lexer, wxSTC_ASM_REGISTER, "Register", m_klass.colour, m_background.colour);
        AddProperty(lexer, wxSTC_ASM_DIRECTIVE, "Directive", m_foreground.colour, m_background.colour);
        AddProperty(lexer, wxSTC_ASM_DIRECTIVEOPERAND, "Directive Operand", m_foreground.colour, m_background.colour);
        AddProperty(lexer, wxSTC_ASM_COMMENTBLOCK, "Comment block", m_multiLineComment.colour, m_background.colour);
        AddProperty(lexer, wxSTC_ASM_CHARACTER, "Character", m_string.colour, m_background.colour);
        AddProperty(lexer, wxSTC_ASM_STRINGEOL, "String EOL", m_string.colour, m_background.colour);
        AddProperty(lexer, wxSTC_ASM_EXTINSTRUCTION, "Ext Instruction", m_keyword.colour, m_background.colour);
        AddProperty(
            lexer, wxSTC_ASM_COMMENTDIRECTIVE, "Comment directive", m_singleLineComment.colour, m_background.colour);

        // Finalize the import
        FinalizeImport(lexer);
        return lexer;
    }
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    // EclipseBatchThemeImporter
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    EclipseBatchThemeImporter::EclipseBatchThemeImporter()
    {
        SetKeywords0("rem set if exist errorlevel for in do break call chcp cd chdir choice cls country ctty date del "
            "erase dir echo exit goto loadfix loadhigh mkdir md move path pause prompt rename ren rmdir rd shift "
            "time type ver verify vol com con lpt nul color copy defined else not start off");
        SetFileExtensions("*.bat;*.batch");
    }

    EclipseBatchThemeImporter::~EclipseBatchThemeImporter() {}
    LexerStyle::Ptr_t EclipseBatchThemeImporter::Import(const wxFileName& eclipseXmlFile)
    {
        LexerStyle::Ptr_t lexer = InitializeImport(eclipseXmlFile, "batch", wxSTC_LEX_BATCH);
        AddProperty(lexer, wxSTC_BAT_DEFAULT, "Default", m_foreground.colour, m_background.colour);
        AddProperty(lexer, wxSTC_BAT_COMMENT, "Comment", m_singleLineComment.colour, m_background.colour);
        AddProperty(lexer, wxSTC_BAT_WORD, "Word", m_keyword.colour, m_background.colour);
        AddProperty(lexer, wxSTC_BAT_LABEL, "Label", m_klass.colour, m_background.colour);
        AddProperty(lexer, wxSTC_BAT_HIDE, "Hide (@)", m_keyword.colour, m_background.colour);
        AddProperty(lexer, wxSTC_BAT_COMMAND, "Command", m_keyword.colour, m_background.colour);
        AddProperty(lexer, wxSTC_BAT_IDENTIFIER, "Identifier", m_variable.colour, m_background.colour);
        AddProperty(lexer, wxSTC_BAT_OPERATOR, "Operator", m_oper.colour, m_background.colour);
        FinalizeImport(lexer);
        return lexer;
    }
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    // EclipseCMakeThemeImporter
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    EclipseCMakeThemeImporter::EclipseCMakeThemeImporter()
    {
        SetKeywords0("add_custom_command add_custom_target add_definitions add_dependencies add_executable add_library "
            "add_subdirectory add_test aux_source_directory build_command build_name cmake_minimum_required "
            "configure_file create_test_sourcelist else elseif enable_language enable_testing endforeach "
            "endfunction endif endmacro endwhile exec_program execute_process export_library_dependencies file "
            "find_file find_library find_package find_path find_program fltk_wrap_ui foreach function "
            "get_cmake_property get_directory_property get_filename_component get_source_file_property "
            "get_target_property get_test_property if include include_directories include_external_msproject "
            "include_regular_expression install install_files install_programs install_targets link_directories "
            "link_libraries list load_cache load_command macro make_directory mark_as_advanced math message "
            "option output_required_files project qt_wrap_cpp qt_wrap_ui remove remove_definitions "
            "separate_arguments set set_directory_properties set_source_files_properties set_target_properties "
            "set_tests_properties site_name source_group string subdir_depends subdirs target_link_libraries "
            "try_compile try_run unset use_mangled_mesa utility_source variable_requires vtk_make_instantiator "
            "vtk_wrap_java vtk_wrap_python vtk_wrap_tcl while write_file");
        SetFileExtensions("*.cmake;*.CMAKE;*CMakeLists.txt");
    }

    EclipseCMakeThemeImporter::~EclipseCMakeThemeImporter() {}

    LexerStyle::Ptr_t EclipseCMakeThemeImporter::Import(const wxFileName& eclipseXmlFile)
    {
        LexerStyle::Ptr_t lexer = InitializeImport(eclipseXmlFile, "cmake", 80);
        CHECK_PTR_RET_NULL(lexer);

        // Covnert to gsgs's XML properties
        AddProperty(lexer, "0", "Default", m_foreground.colour, m_background.colour);
        AddProperty(lexer, "1", "Comment", m_singleLineComment.colour, m_background.colour);
        AddProperty(lexer, "2", "String DQ", m_string.colour, m_background.colour);
        AddProperty(lexer, "3", "String LQ", m_string.colour, m_background.colour);
        AddProperty(lexer, "4", "String RQ", m_string.colour, m_background.colour);
        AddProperty(lexer, "5", "Commands", m_keyword.colour, m_background.colour);
        AddProperty(lexer, "6", "Parameters", m_foreground.colour, m_background.colour);
        AddProperty(lexer, "7", "Variables", m_klass.colour, m_background.colour);
        AddProperty(lexer, "8", "UserDefined", m_foreground.colour, m_background.colour);
        AddProperty(lexer, "9", "While Def", m_keyword.colour, m_background.colour);
        AddProperty(lexer, "10", "Foreach Def", m_keyword.colour, m_background.colour);
        AddProperty(lexer, "11", "If Defined Def", m_keyword.colour, m_background.colour);
        AddProperty(lexer, "12", "Macro Def", m_keyword.colour, m_background.colour);
        AddProperty(lexer, "13", "String Var", m_klass.colour, m_background.colour);
        AddProperty(lexer, "14", "Number", m_number.colour, m_background.colour);

        FinalizeImport(lexer);
        return lexer;
    }
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    // EclipseCobraAltThemeImporter
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    EclipseCobraAltThemeImporter::EclipseCobraAltThemeImporter()
    {
        SetKeywords0("adds all and any as assert base body bool branch break callable catch char class const continue cue "
            "decimal def do dynamic each else end ensure enum event every except expect extend false finally "
            "float for from get has if ignore implements implies import in inherits inlined int interface "
            "invariant is listen lock mixin must namespace new nil not number objc of old or pass passthrough "
            "post print pro raise ref require return same set sig stop struct success test this throw to trace "
            "true try uint use using var where while yield");
        SetKeywords1(
            "abstract fake internal nonvirtual private protected public shared vari inout out override partial virtual ");
        SetFileExtensions("*.cobra-A");
        m_langName = "cobraalt";
    }

    EclipseCobraAltThemeImporter::~EclipseCobraAltThemeImporter() {}
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    // EclipseCobraThemeImporter
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    EclipseCobraThemeImporter::EclipseCobraThemeImporter()
    {
        SetKeywords0("abstract adds all and any as assert base body bool branch break callable catch char class const "
            "continue cue decimal def do dynamic each else end ensure enum event every except expect extend "
            "extern fake false finally float for from get has if ignore implements implies import in inherits "
            "inlined inout int interface internal invariant is listen lock mixin must namespace new nil "
            "nonvirtual not number objc of old or out override partial pass passthrough post print private pro "
            "protected public raise ref require return same set shared sig stop struct success test this throw to "
            "to? trace true try uint use using var vari virtual where while yield");
        SetFileExtensions("*.cobra;cob");
        m_langName = "cobra";
    }

    EclipseCobraThemeImporter::~EclipseCobraThemeImporter() {}
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    //EclipseCSSThemeImporter
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    EclipseCSSThemeImporter::EclipseCSSThemeImporter()
    {
        SetKeywords0("color background-color background-image background-repeat background-attachment background-position "
            "background font-family"
            " font-style font-variant font-weight font-size font word-spacing letter-spacing text-decoration "
            "vertical-align text-transform"
            " text-align text-indent line-height margin-top margin-right margin-bottom margin-left margin "
            "padding-top padding-right padding-bottom"
            " padding-left padding border-top-width border-right-width border-bottom-width border-left-width "
            "border-width border-top"
            " border-right border-bottom border-left border border-color border-style width height float clear "
            "display white-space"
            " list-style-type list-style-image list-style-position list-style auto none normal italic oblique "
            "small-caps bold bolder"
            " lighter xx-small x-small small medium large x-large xx-large larger smaller transparent repeat "
            "repeat-x repeat-y"
            " no-repeat scroll fixed top bottom left center right justify both underline overline line-through "
            "blink baseline"
            " sub super text-top middle text-bottom capitalize uppercase lowercase thin medium thick dotted "
            "dashed solid double"
            " groove ridge inset outset block inline list-item pre no-wrap inside outside disc circle square "
            "decimal lower-roman"
            " upper-roman lower-alpha upper-alpha aqua black blue fuchsia gray green lime maroon navy olive "
            "purple red silver teal"
            "white yellow ");
        SetKeywords1("      link active visited first-child focus hover lang left right first empty enabled disabled "
            "checked not root target       only-child last-child nth-child nth-last-child       first-of-type "
            "last-of-type nth-of-type nth-last-of-type only-of-type       valid invalid required optional     "
            "indeterminate default");
        SetKeywords2(
            "border-top-color border-right-color border-bottom-color border-left-color border-color border-top-style "
            "border-right-style border-bottom-style border-left-style border-style top right bottom left position z-index "
            "direction unicode-bidi min-width max-width min-height max-height overflow clip visibility content quotes "
            "counter-reset counter-increment marker-offset size marks page-break-before page-break-after page-break-inside "
            "page orphans widows font-stretch font-size-adjust unicode-range units-per-em src panose-1 stemv stemh slope "
            "cap-height x-height ascent descent widths bbox definition-src baseline centerline mathline topline "
            "text-shadow caption-side table-layout border-collapse border-spacing empty-cells speak-header cursor outline "
            "outline-width outline-style outline-color volume speak pause-before pause-after pause cue-before cue-after "
            "cue play-during azimuth elevation speech-rate voice-family pitch pitch-range stress richness "
            "speak-punctuation speak-numeral inherit run-in compact marker table inline-table table-row-group "
            "table-header-group table-footer-group table-row table-column-group table-column table-cell table-caption "
            "static relative absolute fixed ltr rtl embed bidi-override visible hidden scroll collapse open-quote "
            "close-quote no-open-quote no-close-quote decimal-leading-zero lower-greek lower-latin upper-latin hebrew "
            "armenian georgian cjk-ideographic hiragana katakana hiragana-iroha katakana-iroha landscape portrait crop "
            "cross always avoid wider narrower ultra-condensed extra-condensed condensed semi-condensed semi-expanded "
            "expanded extra-expanded ultra-expanded caption icon menu message-box small-caption status-bar separate show "
            "hide once crosshair default pointer move text wait help e-resize ne-resize nw-resize n-resize se-resize "
            "sw-resize s-resize w-resize ActiveBorder ActiveCaption AppWorkspace Background ButtonFace ButtonHighlight "
            "ButtonShadow InactiveCaptionText ButtonText CaptionText GrayText Highlight HighlightText InactiveBorder "
            "InactiveCaption InfoBackground InfoText Menu MenuText Scrollbar ThreeDDarkShadow ThreeDFace ThreeDHighlight "
            "ThreeDLightShadow ThreeDShadow Window WindowFrame WindowText silent x-soft soft medium loud x-loud spell-out "
            "mix left-side far-left center-left center-right far-right right-side behind leftwards rightwards below level "
            "above higher lower x-slow slow medium fast x-fast faster slower male female child x-low low high x-high code "
            "digits continuous");

        SetKeywords3("background-size border-radius border-top-right-radius border-bottom-right-radius "
            "border-bottom-left-radius border-top-left-radius box-shadow columns column-width "
            "column-count column-rule column-gap column-rule-color column-rule-style column-rule-width       "
            "resize opacity word-wrap ");
        SetKeywords4("first-letter first-line before after selection");
        SetFileExtensions("*.css");
    }

    EclipseCSSThemeImporter::~EclipseCSSThemeImporter() {}

    LexerStyle::Ptr_t EclipseCSSThemeImporter::Import(const wxFileName& eclipseXmlFile)
    {
        LexerStyle::Ptr_t lexer = InitializeImport(eclipseXmlFile, "css", wxSTC_LEX_CSS);
        CHECK_PTR_RET_NULL(lexer);

        // Set error colour
        wxColour defaultBg(m_background.colour);
        bool isDark = gsgs::Colour::IsDark(defaultBg);
        wxString errorColour = "RED";
        if (isDark) {
            errorColour = "PINK";
        }
        wxString operatorColour = isDark ? "WHITE" : "BLACK";

        /// Lexical states for SCLEX_CSS
        // Convert to gsgs's XML properties
        AddProperty(lexer, "0", "Default", m_foreground.colour, m_background.colour);
        AddProperty(lexer, "1", "Tag", m_foreground.colour, m_background.colour);
        AddProperty(lexer, "2", "Class", m_klass.colour, m_background.colour);
        AddProperty(lexer, "3", "Pseudo Class", m_klass.colour, m_background.colour);
        AddProperty(lexer, "4", "Unknown Pseudo Class", m_klass.colour, m_background.colour);
        AddProperty(lexer, "5", "Operator", operatorColour, m_background.colour);
        AddProperty(lexer, "6", "Identifier", m_variable.colour, m_background.colour);
        AddProperty(lexer, "7", "Unknown Identifier", m_variable.colour, m_background.colour);
        AddProperty(lexer, "8", "Value", m_foreground.colour, m_background.colour);
        AddProperty(lexer, "9", "Comment", m_multiLineComment.colour, m_background.colour);
        AddProperty(lexer, "10", "ID", m_variable.colour, m_background.colour);
        AddProperty(lexer, "11", "Important", m_foreground.colour, m_background.colour);
        AddProperty(lexer, "12", "Directive", m_klass.colour, m_background.colour);
        AddProperty(lexer, "13", "String", m_string.colour, m_background.colour);
        AddProperty(lexer, "14", "Double String", m_string.colour, m_background.colour);
        AddProperty(lexer, "15", "Identifier 2", m_variable.colour, m_background.colour);
        AddProperty(lexer, "16", "Attribute", m_foreground.colour, m_background.colour);
        AddProperty(lexer, "17", "Identifier 3", m_variable.colour, m_background.colour);
        AddProperty(lexer, "18", "Pseudo Element", m_foreground.colour, m_background.colour);
        AddProperty(lexer, "19", "Extended Identifier", m_variable.colour, m_background.colour);
        AddProperty(lexer, "20", "Extended Pseudo Class", m_foreground.colour, m_background.colour);
        AddProperty(lexer, "21", "Extended Pseudo Element", m_foreground.colour, m_background.colour);
        AddProperty(lexer, "22", "Media", m_klass.colour, m_background.colour);
        AddProperty(lexer, "23", "Variable", m_variable.colour, m_background.colour);
        FinalizeImport(lexer);
        return lexer;
    }
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    // EclipseCXXThemeImporter
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    EclipseCXXThemeImporter::EclipseCXXThemeImporter()
    {
        SetKeywords0("and and_eq asm auto bitand bitor bool break case catch char class compl const const_cast "
            "continue default delete do double dynamic_cast else enum explicit export extern false final "
            "float for friend goto if inline int long mutable namespace new not not_eq operator or "
            "or_eq override private protected public register reinterpret_cast return short signed sizeof "
            "static static_cast struct switch template this throw true try typedef typeid typename "
            "union unsigned using virtual void volatile wchar_t while xor xor_eq alignas alignof "
            "char16_t char32_t constexpr decltype noexcept nullptr static_assert thread_local");

        SetKeywords1("abstract boolean break byte case catch char class const continue debugger default delete "
            "do double else enum export extends final finally float for function goto if implements "
            "import in instanceof int interface long native new package private protected public "
            "return short static super switch synchronized this throw throws transient try typeof var "
            "void volatile while with");

        SetKeywords2("a addindex addtogroup anchor arg attention author b brief bug c callgraph callergraph category class "
            "code "
            "cond copybrief copydetails copydoc 	date def defgroup deprecated details dir  dontinclude dot "
            "dotfile e else elseif em endcode endcond enddot endhtmlonly endif endlatexonly endlink endmanonly "
            "endmsc "
            "endverbatim 	endxmlonly  enum example exception extends  file fn headerfile hideinitializer "
            "htmlinclude htmlonly if ifnot image implements include includelineno ingroup internal invariant "
            "	"
            "interface  latexonly li line link mainpage manonly memberof msc n name namespace nosubgrouping note "
            "overload p package page par paragraph param post pre private privatesection property 	protected  "
            "protectedsection protocol public publicsection ref relates relatesalso remarks return retval sa "
            "section "
            "see showinitializer since skip skipline struct 	subpage  subsection subsubsection test throw "
            "todo "
            "tparam typedef union until var verbatim verbinclude version warning weakgroup xmlonly xrefitem");
        SetFileExtensions("*.cxx;*.hpp;*.cc;*.h;*.c;*.cpp;*.l;*.y;*.c++;*.hh;*.js;*.javascript;*.ipp;*.hxx;*.h++");
    }

    EclipseCXXThemeImporter::~EclipseCXXThemeImporter() {}

    LexerStyle::Ptr_t EclipseCXXThemeImporter::Import(const wxFileName& eclipseXmlFile)
    {
        LexerStyle::Ptr_t lexer = InitializeImport(eclipseXmlFile, "c++", 3);
        CHECK_PTR_RET_NULL(lexer);

        // Covnert to gsgs's XML properties
        AddProperty(lexer, "0", "Default", m_foreground.colour, m_background.colour);
        AddProperty(lexer, "1", "Common C style comment", m_multiLineComment.colour, m_background.colour,
            m_multiLineComment.isBold, m_multiLineComment.isItalic);
        AddProperty(lexer, "2", "Common C++ style comment", m_singleLineComment.colour, m_background.colour,
            m_singleLineComment.isBold, m_singleLineComment.isItalic);
        AddProperty(lexer, "3", "Doxygen C style comment", m_javadoc.colour, m_background.colour, m_javadoc.isBold,
            m_javadoc.isItalic);
        AddProperty(lexer, "4", "Number", m_number.colour, m_background.colour, m_number.isBold, m_number.isItalic);
        AddProperty(lexer, "5", "C++ keyword", m_keyword.colour, m_background.colour, m_keyword.isBold, m_keyword.isItalic);
        AddProperty(lexer, "6", "String", m_string.colour, m_background.colour, m_string.isBold, m_string.isItalic);
        AddProperty(lexer, "7", "Character", m_string.colour, m_background.colour, m_string.isBold, m_string.isItalic);
        AddProperty(lexer, "8", "Uuid", m_number.colour, m_background.colour, m_number.isBold, m_number.isItalic);
        AddProperty(lexer, "9", "Preprocessor", m_foreground.colour, m_background.colour);
        AddProperty(lexer, "10", "Operator", m_foreground.colour, m_background.colour);
        AddProperty(lexer, "11", "Identifier", m_foreground.colour, m_background.colour);
        AddProperty(lexer, "12", "Open String", m_string.colour, m_background.colour, m_string.isBold, m_string.isItalic);
        AddProperty(lexer, "15", "Doxygen C++ style comment", m_javadoc.colour, m_background.colour, m_javadoc.isBold,
            m_javadoc.isItalic);
        AddProperty(lexer, "17", "Doxygen keyword", m_javadocKeyword.colour, m_background.colour, m_javadocKeyword.isBold,
            m_javadocKeyword.isItalic);
        AddProperty(lexer, "18", "Doxygen keyword error", m_javadocKeyword.colour, m_background.colour,
            m_javadocKeyword.isBold, m_javadocKeyword.isItalic);
        AddProperty(lexer, "16", "Workspace tags", m_klass.colour, m_background.colour, m_klass.isBold, m_klass.isItalic);
        AddProperty(lexer, "19", "Local variables", m_variable.colour, m_background.colour, m_variable.isBold,
            m_variable.isItalic);

        FinalizeImport(lexer);
        return lexer;
    }

#define GET_LEXER_STYLE_FG_COLOUR(style_number) cxxLexer->GetProperty(style_number).GetFgColour()
#define GET_LEXER_STYLE_BG_COLOUR(style_number) cxxLexer->GetProperty(style_number).GetBgColour()

    wxFileName EclipseCXXThemeImporter::ToEclipseXML(LexerStyle::Ptr_t cxxLexer, size_t id)
    {
        wxString eclipseXML;
        wxString themeName = cxxLexer->GetThemeName();
        eclipseXML << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
        eclipseXML << "<colorTheme id=\"2\" name=\"" << themeName << "\">\n";
        eclipseXML << "  <foreground color=\"" << GET_LEXER_STYLE_FG_COLOUR(11) << "\" />\n";
        eclipseXML << "  <background color=\"" << GET_LEXER_STYLE_BG_COLOUR(0) << "\" />\n";
        eclipseXML << "  <lineNumber color=\"" << GET_LEXER_STYLE_FG_COLOUR(33) << "\" />\n";
        eclipseXML << "  <selectionForeground color=\"" << GET_LEXER_STYLE_FG_COLOUR(SEL_TEXT_ATTR_ID) << "\" />\n";
        eclipseXML << "  <selectionBackground color=\"" << GET_LEXER_STYLE_BG_COLOUR(SEL_TEXT_ATTR_ID) << "\" />\n";
        eclipseXML << "  <singleLineComment color=\"" << GET_LEXER_STYLE_FG_COLOUR(2) << "\" />\n";
        eclipseXML << "  <multiLineComment color=\"" << GET_LEXER_STYLE_FG_COLOUR(1) << "\" />\n";
        eclipseXML << "  <number color=\"" << GET_LEXER_STYLE_FG_COLOUR(4) << "\" />\n";
        eclipseXML << "  <string color=\"" << GET_LEXER_STYLE_FG_COLOUR(6) << "\" />\n";
        eclipseXML << "  <operator color=\"" << GET_LEXER_STYLE_FG_COLOUR(10) << "\" />\n";
        eclipseXML << "  <keyword color=\"" << GET_LEXER_STYLE_FG_COLOUR(5) << "\" />\n";
        eclipseXML << "  <class color=\"" << GET_LEXER_STYLE_FG_COLOUR(16) << "\" />\n";
        eclipseXML << "  <localVariable color=\"" << GET_LEXER_STYLE_FG_COLOUR(19) << "\" />\n";
        eclipseXML << "</colorTheme>\n";
        wxString xmlFile;
        xmlFile << gsgs_Path().GetTempDir() << "/"
            << "eclipse-theme-" << id << ".xml";
        FileUtil::WriteFileContent(xmlFile, eclipseXML);
        return wxFileName(xmlFile);
    }

    std::vector<wxFileName> EclipseCXXThemeImporter::ToEclipseXMLs()
    {
        std::vector<wxFileName> arr;
        wxArrayString themes = gsgs_Style().GetAllThemes();
        for (size_t i = 0; i < themes.size(); ++i) {
            LexerStyle::Ptr_t cxxLexer = gsgs_Style().GetLexer("c++", themes.Item(i));
            if (!cxxLexer) continue;
            arr.push_back(ToEclipseXML(cxxLexer, i));
        }
        return arr;
    }
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    // EclipseDiffThemeImporter
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    EclipseDiffThemeImporter::EclipseDiffThemeImporter() { SetFileExtensions("*.diff;*.patch;Diff;*.Diff"); }

    EclipseDiffThemeImporter::~EclipseDiffThemeImporter() {}

    LexerStyle::Ptr_t EclipseDiffThemeImporter::Import(const wxFileName& eclipseXmlFile)
    {
        LexerStyle::Ptr_t lexer = InitializeImport(eclipseXmlFile, "diff", 16);
        CHECK_PTR_RET_NULL(lexer);

        //
        // Add lexer specific code here
        //
        AddProperty(lexer, "0", "Default", m_foreground.colour, m_background.colour);
        AddProperty(lexer, "1", "Comment", m_singleLineComment.colour, m_background.colour);
        AddProperty(lexer, "2", "Command", m_klass.colour, m_background.colour);
        AddProperty(lexer, "3", "Header", m_keyword.colour, m_background.colour);
        AddProperty(lexer, "4", "Position", m_keyword.colour, m_background.colour);

        // Use pink and forest green
        if (IsDarkTheme()) {
            AddProperty(lexer, "5", "Line Deleted", "#FF8080", m_background.colour, false, false, true);
            AddProperty(lexer, "6", "Line Added", "#80FF80", m_background.colour, false, false, true);
        }
        else {
            AddProperty(lexer, "5", "Line Deleted", "RED", "PINK", false, false, true);
            AddProperty(lexer,
                "6",
                "Line Added",
                "DARK GREEN",
                wxColour("DARK GREEN").ChangeLightness(170).GetAsString(wxC2S_HTML_SYNTAX),
                false,
                false,
                true);
        }
        //
        // Finalize the importer
        //
        FinalizeImport(lexer);
        return lexer;
    }
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    // EclipseDockerfileThemeImporter
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    EclipseDockerfileThemeImporter::EclipseDockerfileThemeImporter()
    {
        SetKeywords0("echo cp rm ls chmod if then fi done touch for in do while switch else cd pwd  cat mkdir rmdir grep "
            "awk print printf xargs find mv gzip tar bzip zip gunzip ADD ARG CMD COPY ENTRYPOINT ENV EXPOSE FROM "
            "LABEL MAINTAINER ONBUILD RUN  STOPSIGNAL USER VOLUME WORKDIR");
        SetFileExtensions("dockerfile");
        m_langName = "dockerfile";
    }

    EclipseDockerfileThemeImporter::~EclipseDockerfileThemeImporter() {}

    LexerStyle::Ptr_t EclipseDockerfileThemeImporter::Import(const wxFileName& eclipseXmlFile)
    {
        LexerStyle::Ptr_t lexer = InitializeImport(eclipseXmlFile, GetLangName(), wxSTC_LEX_BASH);
        AddProperty(lexer, wxSTC_SH_DEFAULT, "Default", m_foreground.colour, m_background.colour);
        wxString errorColour;
        if (lexer->IsDark()) {
            errorColour = "PINK";
        }
        else {
            errorColour = "RED";
        }

        AddProperty(lexer, wxSTC_SH_ERROR, "Error", errorColour, m_background.colour);
        AddProperty(lexer, wxSTC_SH_COMMENTLINE, "Line comment", m_singleLineComment.colour, m_background.colour);
        AddProperty(lexer, wxSTC_SH_NUMBER, "Number", m_number.colour, m_background.colour);
        AddProperty(lexer, wxSTC_SH_WORD, "Word (Set 0)", m_keyword.colour, m_background.colour);
        AddProperty(lexer, wxSTC_SH_STRING, "String", m_string.colour, m_background.colour);
        AddProperty(lexer, wxSTC_SH_CHARACTER, "Character", m_string.colour, m_background.colour);
        AddProperty(lexer, wxSTC_SH_OPERATOR, "Operator", m_oper.colour, m_background.colour);
        AddProperty(lexer, wxSTC_SH_IDENTIFIER, "Identifier", m_foreground.colour, m_background.colour);
        AddProperty(lexer, wxSTC_SH_SCALAR, "Scalar", m_foreground.colour, m_background.colour);
        AddProperty(lexer, wxSTC_SH_PARAM, "Param", m_variable.colour, m_background.colour);
        AddProperty(lexer, wxSTC_SH_BACKTICKS, "Backticks", m_string.colour, m_background.colour);
        AddProperty(lexer, wxSTC_SH_HERE_Q, "Here Q", m_klass.colour, m_background.colour);
        AddProperty(lexer, wxSTC_SH_HERE_DELIM, "Here delim", m_klass.colour, m_background.colour);
        FinalizeImport(lexer);
        return lexer;
    }
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    // EclipseFortranThemeImporter
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    EclipseFortranThemeImporter::EclipseFortranThemeImporter()
    {
        SetKeywords0("access action advance allocatable allocate  apostrophe assign assignment associate asynchronous "
            "backspace  bind blank blockdata call case character class close common  complex contains continue "
            "cycle data deallocate decimal delim  default dimension direct do dowhile double doubleprecision else "
            " elseif elsewhere encoding end endassociate endblockdata enddo  endfile endforall endfunction endif "
            "endinterface endmodule endprogram  endselect endsubroutine endtype endwhere entry eor equivalence  "
            "err errmsg exist exit external file flush fmt forall form format  formatted function go goto id if "
            "implicit in include inout  integer inquire intent interface intrinsic iomsg iolength  iostat kind "
            "len logical module name named namelist nextrec nml  none nullify number only open opened operator "
            "optional out pad  parameter pass pause pending pointer pos position precision  print private program "
            "protected public quote read readwrite  real rec recl recursive result return rewind save select  "
            "selectcase selecttype sequential sign size stat status stop stream  subroutine target then to type "
            "unformatted unit use value  volatile wait where while write");
        SetKeywords1(
            "abs achar acos acosd adjustl adjustr  aimag aimax0 aimin0 aint ajmax0 ajmin0 akmax0 akmin0 all allocated alog "
            " alog10 amax0 amax1 amin0 amin1 amod anint any asin asind associated  atan atan2 atan2d atand bitest bitl "
            "bitlr bitrl bjtest bit_size bktest break  btest cabs ccos cdabs cdcos cdexp cdlog cdsin cdsqrt ceiling cexp "
            "char  clog cmplx conjg cos cosd cosh count cpu_time cshift csin csqrt dabs  dacos dacosd dasin dasind datan "
            "datan2 datan2d datand date  date_and_time dble dcmplx dconjg dcos dcosd dcosh dcotan ddim dexp  dfloat dflotk "
            "dfloti dflotj digits dim dimag dint dlog dlog10 dmax1 dmin1  dmod dnint dot_product dprod dreal dsign dsin "
            "dsind dsinh dsqrt dtan dtand  dtanh eoshift epsilon errsns exp exponent float floati floatj floatk floor "
            "fraction  free huge iabs iachar iand ibclr ibits ibset ichar idate idim idint idnint ieor ifix  iiabs iiand "
            "iibclr iibits iibset iidim iidint iidnnt iieor iifix iint iior iiqint iiqnnt iishft  iishftc iisign ilen "
            "imax0 imax1 imin0 imin1 imod index inint inot int int1 int2 int4  int8 iqint iqnint ior ishft ishftc isign "
            "isnan izext jiand jibclr jibits jibset jidim jidint  jidnnt jieor jifix jint jior jiqint jiqnnt jishft "
            "jishftc jisign jmax0 jmax1 jmin0 jmin1  jmod jnint jnot jzext kiabs kiand kibclr kibits kibset kidim kidint "
            "kidnnt kieor kifix  kind kint kior kishft kishftc kisign kmax0 kmax1 kmin0 kmin1 kmod knint knot kzext  "
            "lbound leadz len len_trim lenlge lge lgt lle llt log log10 logical lshift malloc matmul  max max0 max1 "
            "maxexponent maxloc maxval merge min min0 min1 minexponent minloc  minval mod modulo mvbits nearest nint not "
            "nworkers number_of_processors pack popcnt  poppar precision present product radix random random_number "
            "random_seed range real  repeat reshape rrspacing rshift scale scan secnds selected_int_kind  "
            "selected_real_kind set_exponent shape sign sin sind sinh size sizeof sngl snglq spacing  spread sqrt sum "
            "system_clock tan tand tanh tiny transfer transpose trim ubound unpack verify");

        SetKeywords2("cdabs cdcos cdexp cdlog cdsin cdsqrt cotan cotand  dcmplx dconjg dcotan dcotand decode dimag "
            "dll_export dll_import doublecomplex dreal  dvchk encode find flen flush getarg getcharqq getcl "
            "getdat getenv gettim hfix ibchng  identifier imag int1 int2 int4 intc intrup invalop iostat_msg isha "
            "ishc ishl jfix  lacfar locking locnear map nargs nbreak ndperr ndpexc offset ovefl peekcharqq "
            "precfill  prompt qabs qacos qacosd qasin qasind qatan qatand qatan2 qcmplx qconjg qcos qcosd  qcosh "
            "qdim qexp qext qextd qfloat qimag qlog qlog10 qmax1 qmin1 qmod qreal qsign qsin  qsind qsinh qsqrt "
            "qtan qtand qtanh ran rand randu rewrite segment setdat settim system  timer undfl unlock union val "
            "virtual volatile zabs zcos zexp zlog zsin zsqrt");

        SetFileExtensions("*f;*.f90");
        m_langName = "fortran";
    }

    EclipseFortranThemeImporter::~EclipseFortranThemeImporter() {}

    LexerStyle::Ptr_t EclipseFortranThemeImporter::Import(const wxFileName& eclipseXmlFile)
    {
        LexerStyle::Ptr_t lexer = InitializeImport(eclipseXmlFile, GetLangName(), wxSTC_LEX_FORTRAN);
        AddProperty(lexer, wxSTC_F_DEFAULT, "Default", m_foreground.colour, m_background.colour);
        AddProperty(lexer, wxSTC_F_COMMENT, "Comment", m_singleLineComment.colour, m_background.colour);
        AddProperty(lexer, wxSTC_F_NUMBER, "Number", m_number.colour, m_background.colour);
        AddProperty(lexer, wxSTC_F_STRING1, "String 1", m_string.colour, m_background.colour);
        AddProperty(lexer, wxSTC_F_STRING2, "String 2", m_string.colour, m_background.colour);
        AddProperty(lexer, wxSTC_F_STRING2, "String 3", m_string.colour, m_background.colour);
        AddProperty(lexer, wxSTC_F_OPERATOR, "Operator", m_oper.colour, m_background.colour);
        AddProperty(lexer, wxSTC_F_IDENTIFIER, "Identifier", m_foreground.colour, m_background.colour);
        AddProperty(lexer, wxSTC_F_WORD, "Word", m_keyword.colour, m_background.colour);
        AddProperty(lexer, wxSTC_F_WORD2, "Word 2", m_keyword.colour, m_background.colour);
        AddProperty(lexer, wxSTC_F_WORD3, "Word 3", m_keyword.colour, m_background.colour);
        AddProperty(lexer, wxSTC_F_PREPROCESSOR, "Pre processor", m_keyword.colour, m_background.colour);
        AddProperty(lexer, wxSTC_F_OPERATOR2, "Operator 2", m_oper.colour, m_background.colour);
        AddProperty(lexer, wxSTC_F_LABEL, "Label", m_oper.colour, m_background.colour);
        AddProperty(lexer, wxSTC_F_CONTINUATION, "Continuation", m_foreground.colour, m_background.colour);
        FinalizeImport(lexer);
        return lexer;
    }
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    // EclipseINIThemeImporter
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    EclipseINIThemeImporter::EclipseINIThemeImporter()
    {
        SetFileExtensions("*.ini;*.properties;*.desktop");
    }

    EclipseINIThemeImporter::~EclipseINIThemeImporter() {}

    LexerStyle::Ptr_t EclipseINIThemeImporter::Import(const wxFileName& eclipseXmlFile)
    {
        LexerStyle::Ptr_t lexer = InitializeImport(eclipseXmlFile, "properties", wxSTC_LEX_PROPERTIES);
        CHECK_PTR_RET_NULL(lexer);

        AddProperty(lexer, wxSTC_PROPS_DEFAULT, "Default", m_foreground.colour, m_background.colour);
        AddProperty(lexer, wxSTC_PROPS_COMMENT, "Comment", m_singleLineComment.colour, m_background.colour);
        AddProperty(lexer, wxSTC_PROPS_SECTION, "Section", m_klass.colour, m_background.colour);
        AddProperty(lexer, wxSTC_PROPS_ASSIGNMENT, "Assignment", m_foreground.colour, m_background.colour);
        AddProperty(lexer, wxSTC_PROPS_DEFVAL, "Default Value", m_foreground.colour, m_background.colour);
        AddProperty(lexer, wxSTC_PROPS_KEY, "Key", m_variable.colour, m_background.colour);

        FinalizeImport(lexer);
        return lexer;
    }
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    // EclipseInnoSetupThemeImporter
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    EclipseInnoSetupThemeImporter::EclipseInnoSetupThemeImporter()
    {
        SetKeywords0("code components custommessages dirs files icons ini installdelete langoptions languages messages "
            "registry run setup types tasks uninstalldelete uninstallrun _istool");

        SetKeywords1(
            "  allowcancelduringinstall allownoicons allowrootdirectory allowuncpath   alwaysrestart "
            "alwaysshowcomponentslist alwaysshowdironreadypage   alwaysshowgrouponreadypage alwaysusepersonalgroup "
            "appcomments appcontact   appcopyright appenddefaultdirname appenddefaultgroupname appid appmodifypath   "
            "appmutex appname apppublisher apppublisherurl appreadmefile appsupporturl   appupdatesurl appvername "
            "appversion architecturesallowed   architecturesinstallin64bitmode backcolor backcolor2 backcolordirection   "
            "backsolid changesassociations changesenvironment compression copyrightfontname   copyrightfontsize "
            "createappdir createuninstallregkey defaultdirname   defaultgroupname defaultuserinfoname defaultuserinfoorg "
            "defaultuserinfoserial   dialogfontname dialogfontsize direxistswarning disabledirpage   disablefinishedpage "
            "disableprogramgrouppage disablereadymemo disablereadypage   disablestartupprompt diskclustersize "
            "diskslicesize diskspanning   enablesdirdoesntexistwarning encryption extradiskspacerequired   "
            "flatcomponentslist infoafterfile infobeforefile internalcompresslevel   languagedetectionmethod "
            "languagecodepage languageid languagename licensefile   mergeduplicatefiles minversion onlybelowversion "
            "outputbasefilename outputdir   outputmanifestfile password privilegesrequired reservebytes   "
            "restartifneededbyrun setupiconfile showcomponentsizes showlanguagedialog   showtaskstreelines slicesperdisk "
            "solidcompression sourcedir timestamprounding   timestampsinutc titlefontname titlefontsize touchdate "
            "touchtime uninstallable   uninstalldisplayicon uninstalldisplayname uninstallfilesdir uninstalllogmode   "
            "uninstallrestartcomputer updateuninstalllogappname usepreviousappdir   usepreviousgroup useprevioussetuptype "
            "useprevioustasks useprevioususerinfo   userinfopage usesetupldr versioninfocompany versioninfocopyright   "
            "versioninfodescription versioninfotextversion versioninfoversion   welcomefontname welcomefontsize "
            "windowshowcaption windowstartmaximized   windowresizable windowvisible wizardimagebackcolor wizardimagefile   "
            "wizardimagestretch wizardsmallimagefile   ");

        SetKeywords2("  afterinstall attribs beforeinstall check comment components copymode   description destdir "
            "destname excludes extradiskspacerequired filename flags   fontinstall groupdescription hotkey "
            "infoafterfile infobeforefile iconfilename   iconindex key languages licensefile messagesfile "
            "minversion name   onlybelowversion parameters permissions root runonceid section source   statusmsg "
            "string subkey tasks type types valuedata valuename valuetype   workingdir   ");

        SetKeywords3("  append define dim else emit endif endsub error expr file for if ifdef ifexist   ifndef ifnexist "
            "include insert pragma sub undef   ");

        SetKeywords4("  begin break case const continue do downto else end except finally for function   if of procedure "
            "repeat then to try until uses var while with   ");
        SetFileExtensions("*.iss");
        m_langName = "innosetup";
    }

    EclipseInnoSetupThemeImporter::~EclipseInnoSetupThemeImporter() {}
    LexerStyle::Ptr_t EclipseInnoSetupThemeImporter::Import(const wxFileName& eclipseXmlFile)
    {
        LexerStyle::Ptr_t lexer = InitializeImport(eclipseXmlFile, GetLangName(), wxSTC_LEX_INNOSETUP);
        AddProperty(lexer, wxSTC_INNO_DEFAULT, "Default", m_foreground.colour, m_background.colour);
        AddProperty(lexer, wxSTC_INNO_COMMENT, "Comment", m_singleLineComment.colour, m_background.colour);
        AddProperty(lexer, wxSTC_INNO_KEYWORD, "Keyword", m_keyword.colour, m_background.colour);
        AddProperty(lexer, wxSTC_INNO_PARAMETER, "Parameter", m_variable.colour, m_background.colour);
        AddProperty(lexer, wxSTC_INNO_SECTION, "Section", m_klass.colour, m_background.colour);
        AddProperty(lexer, wxSTC_INNO_PREPROC, "Pre processor", m_string.colour, m_background.colour);
        AddProperty(lexer, wxSTC_INNO_INLINE_EXPANSION, "Inline Expansion", m_string.colour, m_background.colour);
        AddProperty(lexer, wxSTC_INNO_COMMENT_PASCAL, "Pascal Comment", m_singleLineComment.colour, m_background.colour);
        AddProperty(lexer, wxSTC_INNO_KEYWORD_PASCAL, "Pascal Keyword", m_keyword.colour, m_background.colour);
        AddProperty(lexer, wxSTC_INNO_KEYWORD_USER, "User Keyword", m_keyword.colour, m_background.colour);
        AddProperty(lexer, wxSTC_INNO_STRING_DOUBLE, "Double string", m_string.colour, m_background.colour);
        AddProperty(lexer, wxSTC_INNO_STRING_SINGLE, "Single string", m_string.colour, m_background.colour);
        AddProperty(lexer, wxSTC_INNO_IDENTIFIER, "Identifier", m_foreground.colour, m_background.colour);
        FinalizeImport(lexer);
        return lexer;
    }
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    // EclipseJavaScriptThemeImporter
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    EclipseJavaScriptThemeImporter::EclipseJavaScriptThemeImporter()
    {
        // Reserved words
        SetKeywords0("abstract	arguments	boolean	break	byte "
            "case	catch	char	class*	const "
            "continue	debugger	default	delete	do "
            "double	else	enum*	eval	export* "
            "extends*	false	final	finally	float "
            "for	function	goto	if	implements "
            "import*	in	instanceof	int	interface "
            "let	long	native	new	null "
            "package	private	protected	public	return "
            "short	static	super*	switch	synchronized "
            "this	throw	throws	transient	true "
            "try	typeof	var	void	volatile "
            "while	with	yield prototype undefined StringtoString NaN ");

        // Used for wxSTC_C_WORD2 ("functions")
        SetKeywords1(
            "activeElement addEventListener adoptNode	anchors applets "
            "baseURI body close cookie createAttribute createComment createDocumentFragment createElement createTextNode "
            "doctype documentElement documentMode documentURI domain "
            "domConfig embeds forms getElementById getElementsByClassName getElementsByName getElementsByTagName hasFocus "
            "head images implementation importNode inputEncoding lastModified links normalize normalizeDocument open "
            "querySelector"
            "querySelectorAll readyState referrer removeEventListener renameNode scripts strictErrorChecking title URL "
            "write writeln Math NaN name NumberObject valueOf");

        // Used for wxSTC_C_GLOBALCLASS ("classes")
        SetKeywords3("Math Array Date document window");

        SetFileExtensions("*.js;*.javascript;*.qml;*.json");
    }

    EclipseJavaScriptThemeImporter::~EclipseJavaScriptThemeImporter() {}

    LexerStyle::Ptr_t EclipseJavaScriptThemeImporter::Import(const wxFileName& eclipseXmlFile)
    {
        LexerStyle::Ptr_t lexer = InitializeImport(eclipseXmlFile, "javascript", 3);
        CHECK_PTR_RET_NULL(lexer);

        // Covnert to gsgs's XML properties
        AddProperty(lexer, wxSTC_C_DEFAULT, "Default", m_foreground.colour, m_background.colour);
        AddProperty(lexer,
            wxSTC_C_COMMENT,
            "Block comment",
            m_multiLineComment.colour,
            m_background.colour,
            m_multiLineComment.isBold,
            m_multiLineComment.isItalic);
        AddProperty(lexer,
            wxSTC_C_COMMENTLINE,
            "Single line comment",
            m_singleLineComment.colour,
            m_background.colour,
            m_singleLineComment.isBold,
            m_singleLineComment.isItalic);
        AddProperty(lexer,
            wxSTC_C_COMMENTDOC,
            "Doxygen block comment",
            m_javadoc.colour,
            m_background.colour,
            m_javadoc.isBold,
            m_javadoc.isItalic);
        AddProperty(
            lexer, wxSTC_C_NUMBER, "Number", m_number.colour, m_background.colour, m_number.isBold, m_number.isItalic);
        AddProperty(lexer,
            wxSTC_C_WORD,
            "JavaScript keyword",
            m_keyword.colour,
            m_background.colour,
            m_keyword.isBold,
            m_keyword.isItalic);
        AddProperty(
            lexer, wxSTC_C_STRING, "String", m_string.colour, m_background.colour, m_string.isBold, m_string.isItalic);
        AddProperty(lexer,
            wxSTC_C_CHARACTER,
            "Character",
            m_string.colour,
            m_background.colour,
            m_string.isBold,
            m_string.isItalic);
        AddProperty(lexer, wxSTC_C_OPERATOR, "Operator", m_foreground.colour, m_background.colour);
        AddProperty(lexer, wxSTC_C_IDENTIFIER, "Identifier", m_foreground.colour, m_background.colour);
        AddProperty(lexer,
            wxSTC_C_STRINGEOL,
            "Open String",
            m_string.colour,
            m_background.colour,
            m_string.isBold,
            m_string.isItalic);
        AddProperty(lexer,
            wxSTC_C_COMMENTLINEDOC,
            "Doxygen single line comment",
            m_javadoc.colour,
            m_background.colour,
            m_javadoc.isBold,
            m_javadoc.isItalic);
        AddProperty(lexer,
            wxSTC_C_WORD2,
            "JavaScript functions",
            m_variable.colour,
            m_background.colour,
            m_javadoc.isBold,
            m_javadoc.isItalic);
        AddProperty(lexer,
            wxSTC_C_GLOBALCLASS,
            "JavaScript global classes",
            m_klass.colour,
            m_background.colour,
            m_javadoc.isBold,
            m_javadoc.isItalic);
        AddProperty(lexer,
            wxSTC_C_COMMENTDOCKEYWORD,
            "Doxygen keyword",
            m_javadocKeyword.colour,
            m_background.colour,
            m_javadocKeyword.isBold,
            m_javadocKeyword.isItalic);
        AddProperty(lexer,
            wxSTC_C_COMMENTDOCKEYWORDERROR,
            "Doxygen keyword error",
            m_javadocKeyword.colour,
            m_background.colour,
            m_javadocKeyword.isBold,
            m_javadocKeyword.isItalic);
        FinalizeImport(lexer);
        return lexer;
    }

    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    // EclipseJavaThemeImporter
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    EclipseJavaThemeImporter::EclipseJavaThemeImporter()
    {
        SetKeywords0("abstract assert boolean break byte case catch char class const continue default do double else "
            "extends final  finally float for future generic goto if implements import inner "
            "instanceof int interface long native new null outer  package private protected public rest "
            "return short static super switch synchronized this throw throws transient try  var "
            "void volatile while");
        SetFileExtensions("*.java");
        m_langName = "java";
    }

    EclipseJavaThemeImporter::~EclipseJavaThemeImporter() {}
    LexerStyle::Ptr_t EclipseJavaThemeImporter::Import(const wxFileName& eclipseXmlFile)
    {
        LexerStyle::Ptr_t lexer = InitializeImport(eclipseXmlFile, GetLangName(), wxSTC_LEX_CPP);
        CHECK_PTR_RET_NULL(lexer);

        // Covnert to gsgs's XML properties
        AddProperty(lexer, "0", "Default", m_foreground.colour, m_background.colour);
        AddProperty(lexer,
            "1",
            "Block comment",
            m_multiLineComment.colour,
            m_background.colour,
            m_multiLineComment.isBold,
            m_multiLineComment.isItalic);
        AddProperty(lexer,
            "2",
            "Single line comment",
            m_singleLineComment.colour,
            m_background.colour,
            m_singleLineComment.isBold,
            m_singleLineComment.isItalic);
        AddProperty(lexer,
            "3",
            "Javadoc block comment",
            m_javadoc.colour,
            m_background.colour,
            m_javadoc.isBold,
            m_javadoc.isItalic);
        AddProperty(lexer, "4", "Number", m_number.colour, m_background.colour, m_number.isBold, m_number.isItalic);
        AddProperty(
            lexer, "5", "Java keyword", m_keyword.colour, m_background.colour, m_keyword.isBold, m_keyword.isItalic);
        AddProperty(lexer, "6", "String", m_string.colour, m_background.colour, m_string.isBold, m_string.isItalic);
        AddProperty(lexer, "7", "Character", m_string.colour, m_background.colour, m_string.isBold, m_string.isItalic);
        AddProperty(lexer, "8", "Uuid", m_number.colour, m_background.colour, m_number.isBold, m_number.isItalic);
        AddProperty(lexer, "9", "Preprocessor", m_foreground.colour, m_background.colour);
        AddProperty(lexer, "10", "Operator", m_foreground.colour, m_background.colour);
        AddProperty(lexer, "11", "Identifier", m_foreground.colour, m_background.colour);
        AddProperty(lexer, "12", "Open String", m_string.colour, m_background.colour, m_string.isBold, m_string.isItalic);
        AddProperty(lexer,
            "15",
            "Javadoc single line comment",
            m_javadoc.colour,
            m_background.colour,
            m_javadoc.isBold,
            m_javadoc.isItalic);
        AddProperty(lexer,
            "17",
            "Javadoc keyword",
            m_javadocKeyword.colour,
            m_background.colour,
            m_javadocKeyword.isBold,
            m_javadocKeyword.isItalic);
        AddProperty(lexer,
            "18",
            "Javadoc keyword error",
            m_javadocKeyword.colour,
            m_background.colour,
            m_javadocKeyword.isBold,
            m_javadocKeyword.isItalic);
        AddProperty(lexer, "16", "Workspace tags", m_klass.colour, m_background.colour, m_klass.isBold, m_klass.isItalic);
        AddProperty(
            lexer, "19", "Local variables", m_variable.colour, m_background.colour, m_variable.isBold, m_variable.isItalic);

        FinalizeImport(lexer);
        return lexer;
    }
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    // EclipseLuaThemeImporter
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------

    EclipseLuaThemeImporter::EclipseLuaThemeImporter()
    {
        SetKeywords0(
            "if elseif else then end do while nil true false in for and or function local not repeat return until");
        SetFileExtensions("*.lua;*.glua;*.gluaw;");
        m_langName = "lua";
    }

    EclipseLuaThemeImporter::~EclipseLuaThemeImporter() {}

    LexerStyle::Ptr_t EclipseLuaThemeImporter::Import(const wxFileName& eclipseXmlFile)
    {
        LexerStyle::Ptr_t lexer = InitializeImport(eclipseXmlFile, GetLangName(), wxSTC_LEX_LUA);
        AddProperty(lexer, wxSTC_LUA_DEFAULT, "Default", m_foreground.colour, m_background.colour);
        AddProperty(lexer, wxSTC_LUA_COMMENT, "Block comment", m_multiLineComment.colour, m_background.colour);
        AddProperty(lexer, wxSTC_LUA_COMMENTLINE, "Comment", m_singleLineComment.colour, m_background.colour);
        AddProperty(lexer, wxSTC_LUA_COMMENTDOC, "Comment doc", m_multiLineComment.colour, m_background.colour);
        AddProperty(lexer, wxSTC_LUA_NUMBER, "Number", m_number.colour, m_background.colour);
        AddProperty(lexer, wxSTC_LUA_WORD, "Word (Set 0)", m_keyword.colour, m_background.colour);
        AddProperty(lexer, wxSTC_LUA_WORD2, "Word (Set 1)", m_variable.colour, m_background.colour);
        AddProperty(lexer, wxSTC_LUA_WORD3, "Word (Set 2)", m_variable.colour, m_background.colour);
        AddProperty(lexer, wxSTC_LUA_WORD4, "Word (Set 3)", m_klass.colour, m_background.colour);
        AddProperty(lexer, wxSTC_LUA_WORD5, "Word (Set 4)", m_klass.colour, m_background.colour);
        AddProperty(lexer, wxSTC_LUA_STRING, "String", m_string.colour, m_background.colour);
        AddProperty(lexer, wxSTC_LUA_CHARACTER, "Character", m_string.colour, m_background.colour);
        AddProperty(lexer, wxSTC_LUA_LITERALSTRING, "Literal string", m_string.colour, m_background.colour);
        AddProperty(lexer, wxSTC_LUA_PREPROCESSOR, "Preprocessor", m_string.colour, m_background.colour);
        AddProperty(lexer, wxSTC_LUA_OPERATOR, "Operator", m_oper.colour, m_background.colour);
        AddProperty(lexer, wxSTC_LUA_IDENTIFIER, "Identifier", m_foreground.colour, m_background.colour);
        AddProperty(lexer, wxSTC_LUA_STRINGEOL, "String EOL", m_string.colour, m_background.colour);
        AddProperty(lexer, wxSTC_LUA_LABEL, "Label", m_klass.colour, m_background.colour);
        FinalizeImport(lexer);
        return lexer;
    }
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    // EclipseMakefileThemeImporter
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    EclipseMakefileThemeImporter::EclipseMakefileThemeImporter()
    {
        SetFileExtensions("*.mk;makefile;Makefile;*.mak;*.make");
    }

    EclipseMakefileThemeImporter::~EclipseMakefileThemeImporter() {}

    LexerStyle::Ptr_t EclipseMakefileThemeImporter::Import(const wxFileName& eclipseXmlFile)
    {
        LexerStyle::Ptr_t lexer = InitializeImport(eclipseXmlFile, "makefile", 11);
        CHECK_PTR_RET_NULL(lexer);

        AddProperty(lexer, "0", "Default", m_foreground.colour, m_background.colour);
        AddProperty(lexer, "1", "Comment", m_singleLineComment.colour, m_background.colour);
        AddProperty(lexer, "2", "Preprocessor", m_keyword.colour, m_background.colour);
        AddProperty(lexer, "3", "Identifier", m_variable.colour, m_background.colour);
        AddProperty(lexer, "4", "Operator", m_oper.colour, m_background.colour);
        AddProperty(lexer, "5", "Target", m_klass.colour, m_background.colour);
        AddProperty(lexer, "9", "ID End Of Line", m_foreground.colour, m_background.colour);

        FinalizeImport(lexer);
        return lexer;
    }
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    // EclipsePHPThemeImporter
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    EclipsePHPThemeImporter::EclipsePHPThemeImporter()
    {
        SetKeywords4("implements abstract and array as bool boolean break case cfunction class const continue declare "
            "default die directory do double echo else elseif empty enddeclare endfor endforeach endif endswitch "
            "endwhile eval exit extends false float for foreach function global if include include_once int "
            "integer isset list new null object old_function or parent print real require require_once resource "
            "return static stdclass string switch true unset use var while xor __class__ __file__ __function__ "
            "__line__ __sleep __namespace__ __trait__ __wakeup public protected private trait interface namespace "
            "final throw extends try catch self yield");

        // Javascript keywords
        SetKeywords1("break do instanceof  typeof case else new var catch finally return void continue for switch while "
            "debugger function this with default if throw delete in try abstract  export interface  static "
            "boolean  extends "
            "long super"
            "byte final native synchronized char float package throws class goto private transient const "
            "implements  protected "
            " volatile double import  public enum "
            "int short null true false");
        SetFileExtensions("*.php;*.php3;*.phtml;*.inc;*.php5");
    }

    EclipsePHPThemeImporter::~EclipsePHPThemeImporter() {}

    LexerStyle::Ptr_t EclipsePHPThemeImporter::Import(const wxFileName& eclipseXmlFile)
    {
        LexerStyle::Ptr_t lexer = InitializeImport(eclipseXmlFile, "php", 4);
        CHECK_PTR_RET_NULL(lexer);

        // PHP can also be found in a file which contains HTML and JavaScript
        // so we define them all here

        // HTML
        AddProperty(lexer, wxSTC_H_DEFAULT, "HTML Default", m_foreground.colour, m_background.colour);
        AddProperty(lexer, wxSTC_H_TAG, "HTML Tag", m_variable.colour, m_background.colour);
        AddProperty(lexer, wxSTC_H_TAGUNKNOWN, "Unknown HTML Tag", m_variable.colour, m_background.colour);
        AddProperty(lexer, wxSTC_H_ATTRIBUTE, "HTML Attribute", m_klass.colour, m_background.colour);
        AddProperty(lexer, wxSTC_H_ATTRIBUTEUNKNOWN, "Unknown HTML Attribute", m_klass.colour, m_background.colour);
        AddProperty(lexer, wxSTC_H_NUMBER, "HTML Number", m_number.colour, m_background.colour);
        AddProperty(lexer, wxSTC_H_DOUBLESTRING, "HTML Double String", m_string.colour, m_background.colour);
        AddProperty(lexer, wxSTC_H_SINGLESTRING, "HTML Single String", m_string.colour, m_background.colour);
        AddProperty(lexer, wxSTC_H_OTHER, "HTML Other", m_foreground.colour, m_background.colour);
        AddProperty(lexer, wxSTC_H_COMMENT, "HTML Comment", m_multiLineComment.colour, m_background.colour);
        AddProperty(lexer, wxSTC_H_ENTITY, "HTML Entity", m_foreground.colour, m_background.colour);

        // Embedded JavaScript
        AddProperty(lexer, wxSTC_HJ_START, "JS Start", m_foreground.colour, m_background.colour);
        AddProperty(lexer, wxSTC_HJ_DEFAULT, "JS Default", m_foreground.colour, m_background.colour);
        AddProperty(lexer, wxSTC_HJ_COMMENT, "JS Comment", m_singleLineComment.colour, m_background.colour);
        AddProperty(lexer, wxSTC_HJ_COMMENTLINE, "JS MultiLine Comment", m_multiLineComment.colour, m_background.colour);
        AddProperty(lexer, wxSTC_HJ_COMMENTDOC, "JS Doc Comment", m_multiLineComment.colour, m_background.colour);
        AddProperty(lexer, wxSTC_HJ_NUMBER, "JS Number", m_number.colour, m_background.colour);
        AddProperty(lexer, wxSTC_HJ_WORD, "JS Word", m_foreground.colour, m_background.colour);
        AddProperty(lexer, wxSTC_HJ_KEYWORD, "JS Keyword", m_keyword.colour, m_background.colour);
        AddProperty(lexer, wxSTC_HJ_DOUBLESTRING, "JS Double String", m_string.colour, m_background.colour);
        AddProperty(lexer, wxSTC_HJ_SINGLESTRING, "JS Single String", m_string.colour, m_background.colour);
        AddProperty(lexer, wxSTC_HJ_SYMBOLS, "JS Symbols", m_foreground.colour, m_background.colour);
        AddProperty(lexer, wxSTC_HJ_STRINGEOL, "JS String EOL", m_string.colour, m_background.colour);
        AddProperty(lexer, wxSTC_HJ_REGEX, "JS Regex", m_klass.colour, m_background.colour);

        // PHP
        AddProperty(lexer, wxSTC_HPHP_COMPLEX_VARIABLE, "PHP Complex Variable", m_variable.colour, m_background.colour);
        AddProperty(lexer, wxSTC_HPHP_DEFAULT, "PHP Default", m_foreground.colour, m_background.colour);
        AddProperty(lexer, wxSTC_HPHP_HSTRING, "PHP Heredoc String", m_string.colour, m_background.colour);
        AddProperty(lexer, wxSTC_HPHP_SIMPLESTRING, "PHP String", m_string.colour, m_background.colour);
        AddProperty(lexer, wxSTC_HPHP_WORD, "PHP Word", m_keyword.colour, m_background.colour);
        AddProperty(lexer, wxSTC_HPHP_NUMBER, "PHP Number", m_number.colour, m_background.colour);
        AddProperty(lexer, wxSTC_HPHP_VARIABLE, "PHP Variable", m_variable.colour, m_background.colour);
        AddProperty(lexer, wxSTC_HPHP_COMMENTLINE, "PHP Comment", m_singleLineComment.colour, m_background.colour);
        AddProperty(lexer, wxSTC_HPHP_COMMENT, "PHP Multiline Comment", m_multiLineComment.colour, m_background.colour);
        AddProperty(lexer, wxSTC_HPHP_HSTRING_VARIABLE, "PHP Heredoc Variable", m_variable.colour, m_background.colour);
        FinalizeImport(lexer);
        return lexer;
    }
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    // EclipsePythonThemeImporter
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    EclipsePythonThemeImporter::EclipsePythonThemeImporter()
    {
        SetKeywords0("and as assert break class continue def del elif else except exec finally for from global if import in "
            "is lambda not or pass print raise return try while with yield");
        SetKeywords1("bool bytes bytearray dict float frozenset int list long set str tuple type unicode");
        SetKeywords2("__import__ abs all any ascii bin callable chr classmethod compile complex delattr dir divmod "
            "enumerate eval filter format getattr globals hasattr hash help hex id input isinstance issubclass "
            "iter len locals map max memoryview min next object oct open ord pow property range raw_input repr "
            "reversed round setattr slice sorted staticmethod sum super vars zip");
        SetKeywords3("True False None");
        SetFileExtensions("*.py;waf;wscript;wscript_build");
        m_langName = "python";
    }

    EclipsePythonThemeImporter::~EclipsePythonThemeImporter() {}

    LexerStyle::Ptr_t EclipsePythonThemeImporter::Import(const wxFileName& eclipseXmlFile)
    {
        LexerStyle::Ptr_t lexer = InitializeImport(eclipseXmlFile, GetLangName(), wxSTC_LEX_PYTHON);
        AddProperty(lexer, wxSTC_P_DEFAULT, "Default", m_foreground.colour, m_background.colour);
        AddProperty(lexer, wxSTC_P_COMMENTLINE, "Comment", m_singleLineComment.colour, m_background.colour);
        AddProperty(lexer, wxSTC_P_COMMENTLINE, "Number", m_number.colour, m_background.colour);
        AddProperty(lexer, wxSTC_P_STRING, "String", m_string.colour, m_background.colour);
        AddProperty(lexer, wxSTC_P_CHARACTER, "Character", m_string.colour, m_background.colour);
        AddProperty(lexer, wxSTC_P_WORD, "Word", m_keyword.colour, m_background.colour);
        AddProperty(lexer, wxSTC_P_TRIPLE, "Triple", m_multiLineComment.colour, m_background.colour);
        AddProperty(lexer, wxSTC_P_TRIPLEDOUBLE, "Double Triple", m_multiLineComment.colour, m_background.colour);
        AddProperty(lexer, wxSTC_P_CLASSNAME, "Class Name", m_klass.colour, m_background.colour);
        AddProperty(lexer, wxSTC_P_DEFNAME, "Def Name", m_variable.colour, m_background.colour);
        AddProperty(lexer, wxSTC_P_OPERATOR, "Operator", m_foreground.colour, m_background.colour);
        AddProperty(lexer, wxSTC_P_IDENTIFIER, "Identifier", m_foreground.colour, m_background.colour);
        AddProperty(lexer, wxSTC_P_COMMENTBLOCK, "Comment block", m_multiLineComment.colour, m_background.colour);
        AddProperty(lexer, wxSTC_P_STRINGEOL, "String EOL", m_string.colour, m_background.colour);
        AddProperty(lexer, wxSTC_P_WORD2, "Word 2", m_keyword.colour, m_background.colour);
        AddProperty(lexer, wxSTC_P_DECORATOR, "Decorator", m_klass.colour, m_background.colour);
        FinalizeImport(lexer);
        return lexer;
    }
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    // EclipseScriptThemeImporter
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    EclipseScriptThemeImporter::EclipseScriptThemeImporter()
    {
        SetKeywords0("echo cp rm ls chmod if then fi done touch for in do while switch else cd pwd  cat mkdir rmdir grep "
            "awk print printf xargs find mv gzip tar bzip zip gunzip");
        SetFileExtensions("*.bash;*.sh;.tcsh;.bashrc;*.ksh;configure");
        m_langName = "script";
    }

    EclipseScriptThemeImporter::~EclipseScriptThemeImporter() {}

    LexerStyle::Ptr_t EclipseScriptThemeImporter::Import(const wxFileName& eclipseXmlFile)
    {
        LexerStyle::Ptr_t lexer = InitializeImport(eclipseXmlFile, GetLangName(), wxSTC_LEX_BASH);
        AddProperty(lexer, wxSTC_SH_DEFAULT, "Default", m_foreground.colour, m_background.colour);

        wxString errorColour;
        if (lexer->IsDark()) {
            errorColour = "PINK";
        }
        else {
            errorColour = "RED";
        }

        AddProperty(lexer, wxSTC_SH_ERROR, "Error", errorColour, m_background.colour);
        AddProperty(lexer, wxSTC_SH_COMMENTLINE, "Line comment", m_singleLineComment.colour, m_background.colour);
        AddProperty(lexer, wxSTC_SH_NUMBER, "Number", m_number.colour, m_background.colour);
        AddProperty(lexer, wxSTC_SH_WORD, "Word (Set 0)", m_keyword.colour, m_background.colour);
        AddProperty(lexer, wxSTC_SH_STRING, "String", m_string.colour, m_background.colour);
        AddProperty(lexer, wxSTC_SH_CHARACTER, "Character", m_string.colour, m_background.colour);
        AddProperty(lexer, wxSTC_SH_OPERATOR, "Operator", m_oper.colour, m_background.colour);
        AddProperty(lexer, wxSTC_SH_IDENTIFIER, "Identifier", m_foreground.colour, m_background.colour);
        AddProperty(lexer, wxSTC_SH_SCALAR, "Scalar", m_foreground.colour, m_background.colour);
        AddProperty(lexer, wxSTC_SH_PARAM, "Param", m_variable.colour, m_background.colour);
        AddProperty(lexer, wxSTC_SH_BACKTICKS, "Backticks", m_string.colour, m_background.colour);
        AddProperty(lexer, wxSTC_SH_HERE_Q, "Here Q", m_klass.colour, m_background.colour);
        AddProperty(lexer, wxSTC_SH_HERE_DELIM, "Here delim", m_klass.colour, m_background.colour);
        FinalizeImport(lexer);
        return lexer;
    }
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    // EclipseSCSSThemeImporter
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    EclipseSCSSThemeImporter::EclipseSCSSThemeImporter()
    {
        SetKeywords0("color background-color background-image background-repeat background-attachment background-position "
            "background font-family"
            " font-style font-variant font-weight font-size font word-spacing letter-spacing text-decoration "
            "vertical-align text-transform"
            " text-align text-indent line-height margin-top margin-right margin-bottom margin-left margin "
            "padding-top padding-right padding-bottom"
            " padding-left padding border-top-width border-right-width border-bottom-width border-left-width "
            "border-width border-top"
            " border-right border-bottom border-left border border-color border-style width height float clear "
            "display white-space"
            " list-style-type list-style-image list-style-position list-style auto none normal italic oblique "
            "small-caps bold bolder"
            " lighter xx-small x-small small medium large x-large xx-large larger smaller transparent repeat "
            "repeat-x repeat-y"
            " no-repeat scroll fixed top bottom left center right justify both underline overline line-through "
            "blink baseline"
            " sub super text-top middle text-bottom capitalize uppercase lowercase thin medium thick dotted "
            "dashed solid double"
            " groove ridge inset outset block inline list-item pre no-wrap inside outside disc circle square "
            "decimal lower-roman"
            " upper-roman lower-alpha upper-alpha aqua black blue fuchsia gray green lime maroon navy olive "
            "purple red silver teal"
            "white yellow ");
        SetKeywords1("      link active visited first-child focus hover lang left right first empty enabled disabled "
            "checked not root target       only-child last-child nth-child nth-last-child       first-of-type "
            "last-of-type nth-of-type nth-last-of-type only-of-type       valid invalid required optional     "
            "indeterminate default");
        SetKeywords2(
            "border-top-color border-right-color border-bottom-color border-left-color border-color border-top-style "
            "border-right-style border-bottom-style border-left-style border-style top right bottom left position z-index "
            "direction unicode-bidi min-width max-width min-height max-height overflow clip visibility content quotes "
            "counter-reset counter-increment marker-offset size marks page-break-before page-break-after page-break-inside "
            "page orphans widows font-stretch font-size-adjust unicode-range units-per-em src panose-1 stemv stemh slope "
            "cap-height x-height ascent descent widths bbox definition-src baseline centerline mathline topline "
            "text-shadow caption-side table-layout border-collapse border-spacing empty-cells speak-header cursor outline "
            "outline-width outline-style outline-color volume speak pause-before pause-after pause cue-before cue-after "
            "cue play-during azimuth elevation speech-rate voice-family pitch pitch-range stress richness "
            "speak-punctuation speak-numeral inherit run-in compact marker table inline-table table-row-group "
            "table-header-group table-footer-group table-row table-column-group table-column table-cell table-caption "
            "static relative absolute fixed ltr rtl embed bidi-override visible hidden scroll collapse open-quote "
            "close-quote no-open-quote no-close-quote decimal-leading-zero lower-greek lower-latin upper-latin hebrew "
            "armenian georgian cjk-ideographic hiragana katakana hiragana-iroha katakana-iroha landscape portrait crop "
            "cross always avoid wider narrower ultra-condensed extra-condensed condensed semi-condensed semi-expanded "
            "expanded extra-expanded ultra-expanded caption icon menu message-box small-caption status-bar separate show "
            "hide once crosshair default pointer move text wait help e-resize ne-resize nw-resize n-resize se-resize "
            "sw-resize s-resize w-resize ActiveBorder ActiveCaption AppWorkspace Background ButtonFace ButtonHighlight "
            "ButtonShadow InactiveCaptionText ButtonText CaptionText GrayText Highlight HighlightText InactiveBorder "
            "InactiveCaption InfoBackground InfoText Menu MenuText Scrollbar ThreeDDarkShadow ThreeDFace ThreeDHighlight "
            "ThreeDLightShadow ThreeDShadow Window WindowFrame WindowText silent x-soft soft medium loud x-loud spell-out "
            "mix left-side far-left center-left center-right far-right right-side behind leftwards rightwards below level "
            "above higher lower x-slow slow medium fast x-fast faster slower male female child x-low low high x-high code "
            "digits continuous");

        SetKeywords3("background-size border-radius border-top-right-radius border-bottom-right-radius "
            "border-bottom-left-radius border-top-left-radius box-shadow columns column-width "
            "column-count column-rule column-gap column-rule-color column-rule-style column-rule-width       "
            "resize opacity word-wrap ");
        SetKeywords4("first-letter first-line before after selection");
        SetFileExtensions("*.scss");
    }

    EclipseSCSSThemeImporter::~EclipseSCSSThemeImporter()
    {
    }

    LexerStyle::Ptr_t EclipseSCSSThemeImporter::Import(const wxFileName& eclipseXmlFile)
    {
        LexerStyle::Ptr_t lexer = InitializeImport(eclipseXmlFile, "scss", wxSTC_LEX_CSS);
        CHECK_PTR_RET_NULL(lexer);

        // Set error colour
        wxColour defaultBg(m_background.colour);
        bool isDark = gsgs::Colour::IsDark(defaultBg);
        wxString errorColour = "RED";
        if (isDark) {
            errorColour = "PINK";
        }
        wxString operatorColour = isDark ? "WHITE" : "BLACK";

        /// Lexical states for SCLEX_CSS
        // Convert to gsgs's XML properties
        AddProperty(lexer, "0", "Default", m_foreground.colour, m_background.colour);
        AddProperty(lexer, "1", "Tag", m_foreground.colour, m_background.colour);
        AddProperty(lexer, "2", "Class", m_klass.colour, m_background.colour);
        AddProperty(lexer, "3", "Pseudo Class", m_klass.colour, m_background.colour);
        AddProperty(lexer, "4", "Unknown Pseudo Class", m_klass.colour, m_background.colour);
        AddProperty(lexer, "5", "Operator", operatorColour, m_background.colour);
        AddProperty(lexer, "6", "Identifier", m_variable.colour, m_background.colour);
        AddProperty(lexer, "7", "Unknown Identifier", m_variable.colour, m_background.colour);
        AddProperty(lexer, "8", "Value", m_foreground.colour, m_background.colour);
        AddProperty(lexer, "9", "Comment", m_multiLineComment.colour, m_background.colour);
        AddProperty(lexer, "10", "ID", m_variable.colour, m_background.colour);
        AddProperty(lexer, "11", "Important", m_foreground.colour, m_background.colour);
        AddProperty(lexer, "12", "Directive", m_klass.colour, m_background.colour);
        AddProperty(lexer, "13", "String", m_string.colour, m_background.colour);
        AddProperty(lexer, "14", "Double String", m_string.colour, m_background.colour);
        AddProperty(lexer, "15", "Identifier 2", m_variable.colour, m_background.colour);
        AddProperty(lexer, "16", "Attribute", m_foreground.colour, m_background.colour);
        AddProperty(lexer, "17", "Identifier 3", m_variable.colour, m_background.colour);
        AddProperty(lexer, "18", "Pseudo Element", m_foreground.colour, m_background.colour);
        AddProperty(lexer, "19", "Extended Identifier", m_variable.colour, m_background.colour);
        AddProperty(lexer, "20", "Extended Pseudo Class", m_foreground.colour, m_background.colour);
        AddProperty(lexer, "21", "Extended Pseudo Element", m_foreground.colour, m_background.colour);
        AddProperty(lexer, "22", "Media", m_klass.colour, m_background.colour);
        AddProperty(lexer, "23", "Variable", m_variable.colour, m_background.colour);
        FinalizeImport(lexer);
        return lexer;
    }
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    // EclipseSQLThemeImporter
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    EclipseSQLThemeImporter::EclipseSQLThemeImporter()
    {
        SetKeywords0("absolute action add admin after aggregate  alias all allocate alter and any are array as asc  "
            "assertion at authorization  before begin binary bit blob boolean both breadth by  call cascade "
            "cascaded case cast catalog char character  check class clob close collate collation column commit  "
            "completion connect connection constraint constraints  constructor continue corresponding create "
            "cross cube current  current_date current_path current_role current_time current_timestamp  "
            "current_user cursor cycle  data date day deallocate dec decimal declare default  deferrable deferred "
            "delete depth deref desc describe descriptor  destroy destructor deterministic dictionary diagnostics "
            "disconnect  distinct domain double drop dynamic  each else end end-exec equals escape every except  "
            "exception exec execute external  false fetch first float for foreign found from free full  function  "
            "general get global go goto grant group grouping  having host hour  identity if ignore immediate in "
            "indicator initialize initially  inner inout input insert int integer intersect interval  into is "
            "isolation iterate  join  key  language large last lateral leading left less level like  limit local "
            "localtime localtimestamp locator  map match minute modifies modify module month  names national "
            "natural nchar nclob new next no none  not null numeric  object of off old on only open operation "
            "option  or order ordinality out outer output  pad parameter parameters partial path postfix "
            "precision prefix  preorder prepare preserve primary  prior privileges procedure public  read reads "
            "real recursive ref references referencing relative  restrict result return returns revoke right  "
            "role rollback rollup routine row rows  savepoint schema scroll scope search second section select  "
            "sequence session session_user set sets size smallint some| space  specific specifictype sql "
            "sqlexception sqlstate sqlwarning start  state statement static structure system_user  table "
            "temporary terminate than then time timestamp  timezone_hour timezone_minute to trailing transaction "
            "translation  treat trigger true  under union unique unknown  unnest update usage user using  value "
            "values varchar variable varying view  when whenever where with without work write  year  zone index "
            "exists replace");
        SetFileExtensions("*.sql;*.sqlite");
        m_langName = "sql";
    }

    EclipseSQLThemeImporter::~EclipseSQLThemeImporter() {}

    LexerStyle::Ptr_t EclipseSQLThemeImporter::Import(const wxFileName& eclipseXmlFile)
    {
        LexerStyle::Ptr_t lexer = InitializeImport(eclipseXmlFile, GetLangName(), wxSTC_LEX_SQL);
        AddProperty(lexer, wxSTC_SQL_DEFAULT, "Default", m_foreground.colour, m_background.colour);
        AddProperty(lexer, wxSTC_SQL_COMMENT, "Comment line", m_multiLineComment.colour, m_background.colour);
        AddProperty(lexer, wxSTC_SQL_COMMENTLINE, "Comment block", m_singleLineComment.colour, m_background.colour);
        AddProperty(lexer, wxSTC_SQL_COMMENTDOC, "Comment doc", m_multiLineComment.colour, m_background.colour);
        AddProperty(lexer, wxSTC_SQL_NUMBER, "Number", m_number.colour, m_background.colour);
        AddProperty(lexer, wxSTC_SQL_WORD, "Word", m_variable.colour, m_background.colour);
        AddProperty(lexer, wxSTC_SQL_STRING, "String", m_string.colour, m_background.colour);
        AddProperty(lexer, wxSTC_SQL_CHARACTER, "Character", m_string.colour, m_background.colour);
        AddProperty(lexer, wxSTC_SQL_SQLPLUS, "SQL Plus", m_foreground.colour, m_background.colour);
        AddProperty(lexer, wxSTC_SQL_SQLPLUS_PROMPT, "SQL Plus prompt", m_foreground.colour, m_background.colour);
        AddProperty(lexer, wxSTC_SQL_OPERATOR, "Operator", m_oper.colour, m_background.colour);
        AddProperty(lexer, wxSTC_SQL_IDENTIFIER, "Identifier", m_foreground.colour, m_background.colour);
        AddProperty(lexer, wxSTC_SQL_SQLPLUS_COMMENT, "SQL Plus comment", m_singleLineComment.colour, m_background.colour);
        AddProperty(lexer, wxSTC_SQL_COMMENTLINEDOC, "Comment line doc", m_singleLineComment.colour, m_background.colour);
        AddProperty(lexer, wxSTC_SQL_WORD2, "Word (Set 2)", m_keyword.colour, m_background.colour);
        AddProperty(
            lexer, wxSTC_SQL_COMMENTDOCKEYWORD, "Comment doc keyword", m_javadocKeyword.colour, m_background.colour);
        AddProperty(lexer,
            wxSTC_SQL_COMMENTDOCKEYWORDERROR,
            "Comment doc keyword error",
            m_javadocKeyword.colour,
            m_background.colour);
        AddProperty(lexer, wxSTC_SQL_USER1, "User 1", m_foreground.colour, m_background.colour);
        AddProperty(lexer, wxSTC_SQL_USER1, "User 2", m_foreground.colour, m_background.colour);
        AddProperty(lexer, wxSTC_SQL_USER1, "User 3", m_foreground.colour, m_background.colour);
        AddProperty(lexer, wxSTC_SQL_USER1, "User 4", m_foreground.colour, m_background.colour);
        AddProperty(lexer, wxSTC_SQL_QUOTEDIDENTIFIER, "Quoted identifier", m_string.colour, m_background.colour);
        FinalizeImport(lexer);
        return lexer;
    }
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    // EclipseTextThemeImporter
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    EclipseTextThemeImporter::EclipseTextThemeImporter() { SetFileExtensions("*.txt;*.log"); }

    EclipseTextThemeImporter::~EclipseTextThemeImporter() {}

    LexerStyle::Ptr_t EclipseTextThemeImporter::Import(const wxFileName& eclipseXmlFile)
    {
        LexerStyle::Ptr_t lexer = InitializeImport(eclipseXmlFile, "text", 1);
        CHECK_PTR_RET_NULL(lexer);

        AddProperty(lexer, "0", "Default", m_foreground.colour, m_background.colour);
        AddCommonProperties(lexer);
        FinalizeImport(lexer);
        return lexer;
    }
    //-------------------------------------------------------------------------------

    EclipseThemeImporterManager::EclipseThemeImporterManager()
    {
        m_importers.push_back(new EclipseCXXThemeImporter());
        m_importers.push_back(new EclipseCMakeThemeImporter());
        m_importers.push_back(new EclipseTextThemeImporter());
        m_importers.push_back(new EclipseMakefileThemeImporter());
        m_importers.push_back(new EclipseDiffThemeImporter());
        m_importers.push_back(new EclipsePHPThemeImporter());
        m_importers.push_back(new EclipseCSSThemeImporter());
        m_importers.push_back(new EclipseXMLThemeImporter());
        m_importers.push_back(new EclipseJavaScriptThemeImporter());
        m_importers.push_back(new EclipseINIThemeImporter());
        m_importers.push_back(new EclipseASMThemeImporter());
        m_importers.push_back(new EclipseBatchThemeImporter());
        m_importers.push_back(new EclipsePythonThemeImporter());
        m_importers.push_back(new EclipseCobraThemeImporter());
        m_importers.push_back(new EclipseCobraAltThemeImporter());
        m_importers.push_back(new EclipseFortranThemeImporter());
        m_importers.push_back(new EclipseInnoSetupThemeImporter());
        m_importers.push_back(new EclipseJavaThemeImporter());
        m_importers.push_back(new EclipseLuaThemeImporter());
        m_importers.push_back(new EclipseScriptThemeImporter());
        m_importers.push_back(new EclipseSQLThemeImporter());
        m_importers.push_back(new EclipseSCSSThemeImporter());
        m_importers.push_back(new EclipseDockerfileThemeImporter());
        m_importers.push_back(new EclipseYAMLThemeImporter());
    }

    EclipseThemeImporterManager::~EclipseThemeImporterManager() {}

    bool EclipseThemeImporterManager::Import(const wxString& eclipseXml)
    {
        EclipseThemeImporterBase::List_t::iterator iter = m_importers.begin();
        for (; iter != m_importers.end(); ++iter) {
            gsgs_Style().AddLexer((*iter)->Import(eclipseXml));
        }
        return true;
    }

    bool EclipseThemeImporterManager::ImportCxxToAll()
    {
        std::vector<wxFileName> files = EclipseCXXThemeImporter::ToEclipseXMLs();
        for (size_t i = 0; i < files.size(); ++i) {
            wxString eclipseXml = files.at(i).GetFullPath();
            EclipseThemeImporterBase::List_t::iterator iter = m_importers.begin();
            for (; iter != m_importers.end(); ++iter) {
                if ((*iter)->GetLangName() != "c++") { gsgs_Style().AddLexer((*iter)->Import(eclipseXml)); }
            }
        }
        gsgs_Style().Save(true);
        return true;
    }
}