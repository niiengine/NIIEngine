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

#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <cstdlib>
#include "gsgsPreInclude.h"

namespace gsgs
{
    class _gsgsAPI CxxPreProcessorExpression
    {
    public:
        enum eOperand {
            kNONE,
            kAND,
            kOR,
            kGreaterThan,
            kGreaterThanEqual,
            kLowerThan,
            kLowerThanEqual,
        };

    protected:
        CxxPreProcessorExpression* m_next;
        bool m_isNot;
        eOperand m_operand;
        bool m_defined;
        bool m_valueSet;
        double m_valueLong;
    private:
        /**
         * @brief return the value as 'bool'
         */
        bool DoIsTrue();
        
        /**
         * @brief return the value as 'long'
         */
        double DoGetLong();
        
    public:
        CxxPreProcessorExpression(bool value);
        virtual ~CxxPreProcessorExpression();

        bool IsTrue();

        /**
         * @brief return true if this expression already assigned with a value
         */
        bool IsValueSet() const
        {
            return m_valueSet;
        }
        /**
         * @brief the current expression is from a 'defined' condition?
         */
        void SetDefined(bool defined)
        {
            this->m_defined = defined;
        }
        /**
         * @brief is the current expression is from a 'defined' condition?
         */
        bool IsDefined() const
        {
            return m_defined;
        }
        /**
         * @brief when encountering an expression of type
         * #if !(expression) => SetNot() marks the exclamation mark
         */
        void SetNot();

        /**
         * @brief set the next expression in the chain
         */
        CxxPreProcessorExpression* SetNext(CxxPreProcessorExpression::eOperand operand, CxxPreProcessorExpression* expr);

        /**
         * @brief set value to the current expression
         */
        void SetValue(bool value);
        /**
         * @brief set value to the current expression
         */
        void SetValue(double value);
    };
}
#endif
