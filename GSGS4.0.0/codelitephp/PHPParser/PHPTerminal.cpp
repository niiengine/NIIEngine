#include "PHPTerminal.h"
#include "gsgsLexerStyle.h"
#include "gsgsStyleManager.h"
#include "gsgsWindowManager.h"

PHPTerminal::PHPTerminal(wxWindow* parent)
    : TerminalEmulatorFrame(parent)
{
    LexerStyle::Ptr_t lexer = gsgs_Style().GetLexer("php");
    if(lexer) {
        lexer->Apply(GetTerminalUI()->GetTerminalOutputWindow());
    }
    SetSize(300, 300);
    SetName("PHPTerminal");
    gsgs::WindowManager::Load(this);
}

PHPTerminal::~PHPTerminal() {}
