#ifndef CODE_COMPLETION_API_H
#define CODE_COMPLETION_API_H

#include "gsgsCommon.h"
#include "variable.h"
#include "function.h"
#include "expression_result.h"

extern _gsgsAPI std::string       get_scope_name(const std::string &in, std::vector<std::string > &additionlNS, const std::map<std::string, std::string> &ignoreTokens);
extern _gsgsAPI void              get_variables(const std::string &in, VariableList &li, const std::map<std::string, std::string> &ignoreMap, bool isUsedWithinFunc);
extern _gsgsAPI bool              is_primitive_type(const std::string &in);
extern _gsgsAPI void              get_functions(const std::string &in, FunctionList &li, const std::map<std::string, std::string> &ignoreTokens);
extern _gsgsAPI void              get_typedefs(const std::string &in, gsgs::TypedefList &li);
extern _gsgsAPI ExpressionResult &parse_expression(const std::string &in);
extern _gsgsAPI bool              setLexerInput(const std::string &in, const std::map<std::string, std::string> &ignoreTokens);
extern _gsgsAPI void              cl_scope_lex_clean();

// We dont use _gsgsAPI intentionally to avoid people using these members/functions
// directly
extern int cl_scope_lex();
extern int cl_scope_lineno;
extern std::string cl_scope_text;

class _gsgsAPI CppLexer
{
public:
    typedef std::map<std::string, std::string> StringMap_t;
    
public:
    CppLexer(const std::string& str) {
        StringMap_t dummy;
        ::setLexerInput(str, dummy);
    }
    ~CppLexer() {
        ::cl_scope_lex_clean();
    }
    int lex() const {
        return cl_scope_lex();
    }
    int line_number() const {
        return cl_scope_lineno;
    }
    std::string text() const {
        return cl_scope_text;
    }
};
#endif // CODECOMPLETION_API_H
