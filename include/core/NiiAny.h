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

        template<typename Type> explicit Any(const Type & o)
          : mContent(N_new_t(holder<Type>)(o)){}

        Any(const Any & o) : mContent(o.mContent ? o.mContent->clone() : 0){}

        virtual ~Any()
        { 
            destroy(); 
        }

        Any & swap(Any & o)
        {
            std::swap(mContent, o.mContent);
            return *this;
        }

        template<typename Type> Any & operator=(const Type & o)
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

        template<typename Type> Type operator()() const
        {
            if (mContent)
            {
                 return static_cast<Any::holder<Type> *>(mContent)->held;
            }
            else
            {
                N_EXCEPT(InvalidParam, _I18n("Bad cast from uninitialised Any"));
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

        template<typename Type> class holder : public placeholder
        {
        public:
            holder(const Type & value) : held(value){}
        public:
            virtual const std::type_info & getType() const
            {
                return typeid(Type);
            }

            virtual placeholder * clone() const
            {
                return N_new_t(holder)(held);
            }

            virtual void writeToStream(std::ostream& o)
            {
                o << held;
            }
        public:
            Type held;
        };
    protected:
        placeholder * mContent;

        template<typename Type> friend Type * any_cast(Any *);
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

        template<typename Type> AnyNumeric(const Type & type)
        {
            mContent = N_new_t(numholder<Type>)(type);
        }

        AnyNumeric(const AnyNumeric & other) : Any()
        {
            mContent = other.mContent ? other.mContent->clone() : 0;
        }

        AnyNumeric & operator=(const AnyNumeric & o)
        {
            AnyNumeric(o).swap(*this);
            return * this;
        }
        AnyNumeric operator+(const AnyNumeric & o) const
        {
            return AnyNumeric(static_cast<numplaceholder *>(mContent)->add(o.mContent));
        }
        AnyNumeric operator-(const AnyNumeric & o) const
        {
            return AnyNumeric(static_cast<numplaceholder *>(mContent)->subtract(o.mContent));
        }
        AnyNumeric operator*(const AnyNumeric & o) const
        {
            return AnyNumeric(static_cast<numplaceholder *>(mContent)->multiply(o.mContent));
        }
        AnyNumeric operator*(NIIf factor) const
        {
            return AnyNumeric(static_cast<numplaceholder *>(mContent)->multiply(factor));
        }
        AnyNumeric operator/(const AnyNumeric & o) const
        {
            return AnyNumeric(static_cast<numplaceholder *>(mContent)->divide(o.mContent));
        }
        AnyNumeric & operator+=(const AnyNumeric & o)
        {
            *this = AnyNumeric(static_cast<numplaceholder *>(mContent)->add(o.mContent));
            return *this;
        }
        AnyNumeric & operator-=(const AnyNumeric & o)
        {
            *this = AnyNumeric(static_cast<numplaceholder *>(mContent)->subtract(o.mContent));
            return *this;
        }
        AnyNumeric & operator*=(const AnyNumeric & o)
        {
            *this = AnyNumeric(static_cast<numplaceholder *>(mContent)->multiply(o.mContent));
            return *this;
        }
        AnyNumeric & operator/=(const AnyNumeric & o)
        {
            *this = AnyNumeric(static_cast<numplaceholder *>(mContent)->divide(o.mContent));
            return *this;
        }
    protected:
        AnyNumeric(placeholder * pholder)
        {
            mContent = pholder;
        }

        template<typename Type> class numholder : public numplaceholder
        {
        public: // structors
            numholder(const Type & type) : held(type){}

            virtual const std::type_info & getType() const
            {
                return typeid(Type);
            }

            virtual placeholder * clone() const
            {
                return N_new_t(numholder)(held);
            }

            virtual placeholder * add(placeholder * o)
            {
                return N_new_t(numholder)(held + static_cast<numholder*>(o)->held);
            }
            virtual placeholder * subtract(placeholder * o)
            {
                return N_new_t(numholder)(held - static_cast<numholder*>(o)->held);
            }
            virtual placeholder * multiply(placeholder * o)
            {
                return N_new_t(numholder)(held * static_cast<numholder*>(o)->held);
            }
            virtual placeholder * multiply(NIIf factor)
            {
                return N_new_t(numholder)(held * factor);
            }
            virtual placeholder * divide(placeholder * o)
            {
                return N_new_t(numholder)(held / static_cast<numholder*>(o)->held);
            }
            virtual void writeToStream(std::ostream& o)
            {
                o << held;
            }
        public:
            Type held;
        };
    };

    template<typename Type> Type * any_cast(Any * data)
    {
        return data && (data->getType() == typeid(Type)) ? 
            &static_cast<Any::holder<Type> *>(data->mContent)->held : 0;
    }

    template<typename Type> const Type * any_cast(const Any * data)
    {
        return any_cast<Type>(const_cast<Any *>(data));
    }

    template<typename Type> const Type & any_cast(const Any & data)
    {
        const Type * result = any_cast<Type>(&data);
        if(!result)
        {
            StringStream str;
            str << _I18n("Bad cast from type '") << data.getType().name() << "' "
                << _I18n("to '") << typeid(Type).name() << "'";
            N_EXCEPT(InvalidParam, str.str());
        }
        return *result;
    }
}

#endif