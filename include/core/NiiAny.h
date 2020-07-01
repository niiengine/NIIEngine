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

#ifndef _NII_ANY_H_
#define _NII_ANY_H_

#include "NiiPreInclude.h"
#include "NiiException.h"
#include <algorithm>
#include <typeinfo>

// -- Based on boost::any, original copyright information follows --
// Copyright Kevlin Henney, 2000, 2001, 2002. All rights reserved.
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompAnying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
// -- End original copyright --

namespace NII
{
    /** 变型类型,可以表示其他任何类型.
    */
    class Any
    {
    public:
        Any() : mContent(0){}

        template<typename ValueType> explicit Any(const ValueType & value)
          : mContent(N_new_t(holder<ValueType>)(value)){}

        Any(const Any & other) : mContent(other.mContent ? other.mContent->clone() : 0){}

        virtual ~Any()
        { 
            destroy(); 
        }

        Any & swap(Any & o)
        {
            std::swap(mContent, o.mContent);
            return *this;
        }

        template<typename ValueType> Any & operator=(const ValueType & o)
        {
            Any(o).swap(*this);
            return *this;
        }

        Any & operator=(const Any & o)
        {
            Any(o).swap(*this);
            return *this;
        }

        bool isEmpty() const{ return !mContent; }

        const std::type_info & getType() const
        {
            return mContent ? mContent->getType() : typeid(void);
        }

        inline friend std::ostream & operator <<( std::ostream & o, const Any & v )
        {
            if (v.mContent)
                v.mContent->writeToStream(o);
            return o;
        }

        void destroy()
        {
            N_delete_t(mContent, placeholder);
            mContent = NULL;
        }

        template<typename ValueType> ValueType operator()() const
        {
            if (!mContent)
            {
                N_EXCEPT(InvalidParam, _I18n("Bad cast from uninitialised Any"));
            }
            else if(getType() == typeid(ValueType))
            {
                 return static_cast<Any::holder<ValueType> *>(mContent)->held;
            }
            else
            {
                StringStream str;
                str << _I18n("Bad cast from type '") << getType().name() << "' "
                    << _I18n("to '") << typeid(ValueType).name() << "'";
                N_EXCEPT(InvalidParam, str.str());
            }
        }
    protected: // types
        class placeholder
        {
        public:
            virtual ~placeholder(){}
        public:
            virtual const std::type_info & getType() const = 0;

            virtual placeholder * clone() const = 0;

            virtual void writeToStream(std::ostream& o) = 0;
        };

        template<typename ValueType> class holder : public placeholder
        {
        public:
            holder(const ValueType & value) : held(value){}
        public:
            virtual const std::type_info & getType() const
            {
                return typeid(ValueType);
            }

            virtual placeholder * clone() const
            {
                return N_new_t(holder)(held);
            }

            virtual void writeToStream(std::ostream& o)
            {
                o << held;
            }
        public: // 描述
            ValueType held;
        };
    protected: // 描述
        placeholder * mContent;

        template<typename ValueType> friend ValueType * any_cast(Any *);
    };

    /// 专门在Any基础上建立的算术运算符类,而且能表示其他任何支持+,-,*and/的类型.
    class AnyNumeric : public Any
    {
    public:
        class numplaceholder : public Any::placeholder
        {
        public:
            ~numplaceholder() {}
            virtual placeholder * add(placeholder * o) = 0;
            virtual placeholder * subtract(placeholder * o) = 0;
            virtual placeholder * multiply(placeholder * o) = 0;
            virtual placeholder * multiply(NIIf factor) = 0;
            virtual placeholder * divide(placeholder * o) = 0;
        };
    public:
        AnyNumeric() : Any(){}

        template<typename ValueType> AnyNumeric(const ValueType & value)
        {
            mContent = N_new_t(numholder<ValueType>)(value);
        }

        AnyNumeric(const AnyNumeric & other) : Any()
        {
            mContent = other.mContent ? other.mContent->clone() : 0;
        }

        AnyNumeric & operator=(const AnyNumeric & rhs)
        {
            AnyNumeric(rhs).swap(*this);
            return * this;
        }
        AnyNumeric operator+(const AnyNumeric & rhs) const
        {
            return AnyNumeric(static_cast<numplaceholder *>(mContent)->add(rhs.mContent));
        }
        AnyNumeric operator-(const AnyNumeric & rhs) const
        {
            return AnyNumeric(static_cast<numplaceholder *>(mContent)->subtract(rhs.mContent));
        }
        AnyNumeric operator*(const AnyNumeric & rhs) const
        {
            return AnyNumeric(static_cast<numplaceholder *>(mContent)->multiply(rhs.mContent));
        }
        AnyNumeric operator*(NIIf factor) const
        {
            return AnyNumeric(static_cast<numplaceholder *>(mContent)->multiply(factor));
        }
        AnyNumeric operator/(const AnyNumeric & rhs) const
        {
            return AnyNumeric(static_cast<numplaceholder *>(mContent)->divide(rhs.mContent));
        }
        AnyNumeric & operator+=(const AnyNumeric & rhs)
        {
            *this = AnyNumeric(static_cast<numplaceholder *>(mContent)->add(rhs.mContent));
            return *this;
        }
        AnyNumeric & operator-=(const AnyNumeric & rhs)
        {
            *this = AnyNumeric(static_cast<numplaceholder *>(mContent)->subtract(rhs.mContent));
            return *this;
        }
        AnyNumeric & operator*=(const AnyNumeric & rhs)
        {
            *this = AnyNumeric(static_cast<numplaceholder *>(mContent)->multiply(rhs.mContent));
            return *this;
        }
        AnyNumeric & operator/=(const AnyNumeric & rhs)
        {
            *this = AnyNumeric(static_cast<numplaceholder *>(mContent)->divide(rhs.mContent));
            return *this;
        }
    protected:
        AnyNumeric(placeholder * pholder)
        {
            mContent = pholder;
        }

        template<typename ValueType> class numholder : public numplaceholder
        {
        public: // structors
            numholder(const ValueType & value) : held(value){}

            virtual const std::type_info & getType() const
            {
                return typeid(ValueType);
            }

            virtual placeholder * clone() const
            {
                return N_new_t(numholder)(held);
            }

            virtual placeholder * add(placeholder * rhs)
            {
                return N_new_t(numholder)(held + static_cast<numholder*>(rhs)->held);
            }
            virtual placeholder * subtract(placeholder * rhs)
            {
                return N_new_t(numholder)(held - static_cast<numholder*>(rhs)->held);
            }
            virtual placeholder * multiply(placeholder * rhs)
            {
                return N_new_t(numholder)(held * static_cast<numholder*>(rhs)->held);
            }
            virtual placeholder * multiply(NIIf factor)
            {
                return N_new_t(numholder)(held * factor);
            }
            virtual placeholder * divide(placeholder * rhs)
            {
                return N_new_t(numholder)(held / static_cast<numholder*>(rhs)->held);
            }
            virtual void writeToStream(std::ostream& o)
            {
                o << held;
            }
        public:
            ValueType held;
        };
    };

    template<typename ValueType> ValueType * any_cast(Any * operand)
    {
        return operand && (std::strcmp(operand->getType().name(), typeid(ValueType).name()) == 0)
            ? &static_cast<Any::holder<ValueType> *>(operand->mContent)->held : 0;
    }

    template<typename ValueType> const ValueType * any_cast(const Any * operand)
    {
        return any_cast<ValueType>(const_cast<Any *>(operand));
    }

    template<typename ValueType> ValueType any_cast(const Any & operand)
    {
        const ValueType * result = any_cast<ValueType>(&operand);
        if(!result)
        {
            StringStream str;
            str << _I18n("Bad cast from type '") << operand.getType().name() << "' "
                << _I18n("to '") << typeid(ValueType).name() << "'";
            N_EXCEPT(InvalidParam, str.str());
        }
        return *result;
    }
}

#endif