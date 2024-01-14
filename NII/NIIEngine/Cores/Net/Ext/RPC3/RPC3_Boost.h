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
#ifndef __RPC3_BOOST_H
#define __RPC3_BOOST_H

// Fixes
// error C2504: 'boost::fusion::detail::invoke_impl<Function,Sequence,N,CBI,RandomAccess>' : base class undefined
// This defines the maximum number of parameters you can have
#ifndef BOOST_FUSION_INVOKE_MAX_ARITY
#define BOOST_FUSION_INVOKE_MAX_ARITY 10
#endif

// Boost dependencies
// Boost is assumed to be at C:\boost_1_43_0 based on the project settings
// If this is not where you downloaded boost, change the project settings Configuration Properties / C/C++ / General / Additional Include Directories
// If you don't have boost, get it from http://www.boost.org/users/download/
// If you don't want to use boost, use RPC instead which relies on assembly but has fewer features
#include "boost/type_traits.hpp"
#include "boost/function.hpp"
#include "boost/bind.hpp"
#include "boost/mpl/if.hpp"
#include "boost/mpl/apply.hpp"
#include "boost/function_types/parameter_types.hpp"
#include "boost/fusion/container/list/cons.hpp" // boost::fusion::nil
#include "boost/fusion/include/push_back.hpp"
#include "boost/fusion/include/invoke.hpp"
#include "boost/fusion/tuple/tuple.hpp"
#include "boost/fusion/tuple/make_tuple.hpp"
#include "boost/fusion/functional/invocation/invoke.hpp"
#include "boost/type_traits/is_array.hpp"

// Not needed?
//#include <boost/fusion/container/generation/make_vector.hpp>

#include "NiiNetPreInclude.h"
#include "NetworkIDManager.h"
#include "NetworkIDObject.h"
#include "NiiNetSerializer.h"

namespace NII
{
namespace NII_NET
{
    class RPC3;

    namespace _RPC3
    {

    enum InvokeResultCodes
    {
        IRC_SUCCESS,
        IRC_NEED_BITSTREAM,
        IRC_NEED_NETWORK_ID_MANAGER,
        IRC_NEED_NETWORK_ID,
        IRC_NEED_CLASS_OBJECT,
    };

    struct InvokeArgs
    {
        // Bitstream to use to deserialize
        NetSerializer * bitStream;

        // NetworkIDManager to use to lookup objects
        NetworkIDManager * networkIDManager;

        // C++ class member object
        NetworkID classMemberObjectId;

        // The calling plugin
        RPC3 * caller;

        // The this pointer for C++
        NetworkIDObject *thisPtr;
    };

    typedef boost::fusion::tuple<bool, boost::function<InvokeResultCodes (InvokeArgs)> > FunctionPointer;

    struct StrWithDestructor
    {
        char * c;
        ~StrWithDestructor() {if (c) delete c;}
    };

    enum RPC3TagFlag
    {
        RPC3_TAG_FLAG_DEREF=1,
        RPC3_TAG_FLAG_ARRAY=2,
    };

    struct RPC3Tag
    {
        RPC3Tag() {}
        RPC3Tag(void *_v, unsigned int _count, RPC3TagFlag _flag) : v(_v), count(_count), flag((unsigned char)_flag) {}
        void * v;
        unsigned int count;
        unsigned char flag;
    };

    // Track the pointers tagged with _RPC3::Deref
    static RPC3Tag __RPC3TagPtrs[BOOST_FUSION_INVOKE_MAX_ARITY+1];
    static int __RPC3TagHead=0;
    static int __RPC3TagTail=0;

    // If this assert hits, then _RPC3::Deref was called more times than the argument was passed to the function
    static void __RPC3_Tag_AddHead(const RPC3Tag &p)
    {
        // update tag if already in array
        int i;
        for (i=__RPC3TagTail; i!=__RPC3TagHead; i=(i+1)%BOOST_FUSION_INVOKE_MAX_ARITY)
        {
            if (__RPC3TagPtrs[i].v==p.v)
            {
                if (p.flag==RPC3_TAG_FLAG_ARRAY)
                {
                    __RPC3TagPtrs[i].count=p.count;
                }
                __RPC3TagPtrs[i].flag|=p.flag;

                return;
            }
        }

        __RPC3TagPtrs[__RPC3TagHead]=p;
        __RPC3TagHead = (__RPC3TagHead + 1) % BOOST_FUSION_INVOKE_MAX_ARITY;
        assert(__RPC3TagHead!=__RPC3TagTail);
    }
    static void __RPC3ClearTail(void) {
        while (__RPC3TagTail!=__RPC3TagHead)
        {
            if (__RPC3TagPtrs[__RPC3TagTail].v==0)
                __RPC3TagTail = (__RPC3TagTail+1) % BOOST_FUSION_INVOKE_MAX_ARITY;
            else
                return;
        }
    }
    static bool __RPC3ClearPtr(void * p, RPC3Tag * tag)
    {
        int i;
        for (i = __RPC3TagTail; i != __RPC3TagHead; i = (i+1)%BOOST_FUSION_INVOKE_MAX_ARITY)
        {
            if (__RPC3TagPtrs[i].v == p)
            {
                *tag = __RPC3TagPtrs[i];
                __RPC3TagPtrs[i].v = 0;
                __RPC3ClearTail();
                return true;
            }
        }
        tag->flag = 0;
        tag->count = 1;
        return false;
    }

    template <class templateType>
    inline const templateType & Deref(const templateType & t)
    {
        __RPC3_Tag_AddHead(RPC3Tag((void*)t, 1, RPC3_TAG_FLAG_DEREF));
        return t;
    }

    template <class templateType>
    inline const templateType & PtrToArray(unsigned int count, const templateType & t)
    {
        __RPC3_Tag_AddHead(RPC3Tag((void *)t, count, RPC3_TAG_FLAG_ARRAY));
        return t;
    }

    struct ReadBitstream
    {
        static void applyArray(NetSerializer & out, NetSerializer * t)
        {
            apply(out, t);
        }

        static void apply(NetSerializer & out, NetSerializer * t)
        {
            NBitCount numBitsUsed;
            out.readCompress(numBitsUsed);
            out.read(t, numBitsUsed);
        }
    };

    //template <typename T>
    struct ReadPtr
    {
        template <typename T2>
        static inline void applyArray(NetSerializer & out, T2 *t)
        {
            out.read(*t);
        }
        template <typename T2>
        static inline void apply(NetSerializer & out, T2 *t)
        {
            out.read(*t);
        }

        static inline void apply(NetSerializer & out, char *&t)
        {
            applyStr(out, (char *&)t);
        }
        static inline void apply(NetSerializer & out, unsigned char *&t)
        {
            applyStr(out, (char *&)t);
        }
        static inline void apply(NetSerializer & out, const char *&t)
        {
            applyStr(out, (char *&)t);
        }
        static inline void apply(NetSerializer & out, const unsigned char *&t)
        {
            applyStr(out, (char *&)t);
        }
        static inline void applyStr(NetSerializer & out, char *&t)
        {
            RakString rs;
            out.read(rs);
            size_t len = rs.GetLength()+1;

            // The caller should have already allocated memory, so we need to free
            // it and allocate a new buffer.
            N_assert("Expected allocated array, got NULL" && (NULL != t));
            delete [] t;

            t = new char [len];
            memcpy(t,rs.C_String(),len);
        }
    };


    template< typename T >
    struct DoRead
    {
        typedef typename boost::mpl::if_<
            boost::is_convertible<T*,NetSerializer*>,
            ReadBitstream,
            ReadPtr >::type type;
    };


    template< typename T >
    struct ReadWithoutNetworkIDNoPtr
    {
        static InvokeResultCodes apply(InvokeArgs &args, T &t)
        {
            DoRead< typename boost::remove_pointer<T>::type >::type::apply(* (args.bitStream),&t);

            return IRC_SUCCESS;
        }
        template< typename T2 >
        static void Cleanup(T2 &t) {}
    };

    template< typename T >
    struct ReadWithNetworkIDPtr
    {
        static InvokeResultCodes apply(InvokeArgs & args, T & t)
        {
            bool isNull;
            args.bitStream->read(isNull);
            if (isNull)
            {
                t = 0;
                return IRC_SUCCESS;
            }

            bool deref, isArray;
            args.bitStream->read(deref);
            args.bitStream->read(isArray);
            NCount count;
            if (isArray)
                args.bitStream->readCompress(count);
            else
                count = 1;
            NetworkID networkId;
            for (NCount i = 0; i < count; ++i)
            {
                args.bitStream->read(networkId);
                t = args.networkIDManager->GET_OBJECT_FROM_ID< T >(networkId);
                if(deref)
                {
                    NCount bU;
                    NBitCount BU;
                    args.bitStream->readAlign();
                    args.bitStream->read(bU);
                    args.bitStream->read(BU);

                    if(t)
                    {
                        DoRead<typename boost::remove_pointer<T>::type>::type::apply(* (args.bitStream),t);
                    }
                    else
                    {
                        // Skip data!
                        args.bitStream->skipWrite(bU, BU);
                    }
                }
            }

            return IRC_SUCCESS;
        }

        template< typename T2 >
        static void Cleanup(T2 & t) {}
    };

    template< typename T >
    struct ReadWithoutNetworkIDPtr
    {
        template <typename T2>
        static InvokeResultCodes apply(InvokeArgs & args, T2 & t)
        {
            bool isNull = false;
            args.bitStream->read(isNull);
            if (isNull)
            {
                t=0;
                return IRC_SUCCESS;
            }

            typedef typename boost::remove_pointer< T >::type ActualObjectType;

            bool isArray = false;
            NCount count;
            args.bitStream->read(isArray);
            if (isArray)
                args.bitStream->readCompress(count);
            else
                count = 1;

            t = new ActualObjectType[count]();
            if (isArray)
            {
                for (NCount i = 0; i < count; ++i)
                {
                    DoRead< typename boost::remove_pointer<T>::type >::type::applyArray(* (args.bitStream),t+i);
                }
            }
            else
            {
                DoRead< typename boost::remove_pointer<T>::type >::type::apply(* (args.bitStream),t);
            }

            return IRC_SUCCESS;
        }

        template< typename T2 >
        static void Cleanup(T2 &t) {
            if (t)
                delete [] t;
        }
    };

    template< typename T >
    struct SetRPC3Ptr
    {
        static InvokeResultCodes apply(InvokeArgs &args, T &obj)
        {
            obj=args.caller;
            return IRC_SUCCESS;
        }

        template< typename T2 >
        static void Cleanup(T2 &t) {}
    };
    template< typename T >
    struct ReadWithoutNetworkID
    {
        typedef typename boost::mpl::if_<
            boost::is_pointer<T>
            , ReadWithoutNetworkIDPtr<T> // true
            , ReadWithoutNetworkIDNoPtr<T>
        >::type type;
    };

    template< typename T >
    struct identity
    {
        typedef T type;
    };

    template< typename T >
    struct IsRPC3Ptr
    {
        typedef typename boost::mpl::if_<
            boost::is_convertible<T,RPC3*>,
            boost::mpl::true_,
            boost::mpl::false_>::type type;
    };

    template< typename T >
    struct ShouldReadNetworkID
    {
        typedef typename boost::mpl::if_<
            boost::is_convertible<T,NetworkIDObject*>,
            boost::mpl::true_,
            boost::mpl::false_>::type type;
    };

    template< typename T >
    struct GetReadFunction
    {
        typedef typename boost::mpl::if_<
            typename ShouldReadNetworkID<T>::type
            , ReadWithNetworkIDPtr<T>
            , typename ReadWithoutNetworkID<T>::type
        >::type type;
    };

    template< typename T >
    struct ProcessArgType
    {
        typedef typename boost::mpl::if_<
            typename IsRPC3Ptr<T>::type
            , SetRPC3Ptr<T>
            , typename GetReadFunction<T>::type
        >::type type;
    };

    template< typename Function
        , class From = typename boost::mpl::begin< boost::function_types::parameter_types<Function> >::type
        , class To   = typename boost::mpl::end< boost::function_types::parameter_types<Function> >::type
    >
    struct BoostRPCInvoker
    {
        // add an argument to a Fusion cons-list for each parameter type
        template<typename Args>
        static inline
            InvokeResultCodes apply(Function func, InvokeArgs &functionArgs, Args const &args)
        {
            typedef typename boost::mpl::deref<From>::type arg_type;
            typedef typename boost::mpl::next<From>::type next_iter_type;
            typedef typename boost::remove_reference<arg_type>::type arg_type_no_ref;

            arg_type_no_ref argType;
            ProcessArgType< arg_type_no_ref >::type::apply(functionArgs, argType);

            InvokeResultCodes irc = BoostRPCInvoker<Function, next_iter_type, To>::apply
                ( func, functionArgs, boost::fusion::push_back(args, boost::ref(argType) ) );

            ProcessArgType< arg_type_no_ref >::type::Cleanup(argType);
            return irc;
        }
    };

    template< typename Function
    , class From = typename boost::mpl::begin< boost::function_types::parameter_types<Function> >::type
    , class To   = typename boost::mpl::end< boost::function_types::parameter_types<Function> >::type
    >
    struct BoostRPCInvoker_ThisPtr
    {
        // add an argument to a Fusion cons-list for each parameter type
        template<typename Args>
        static inline
            InvokeResultCodes apply(Function func, InvokeArgs &functionArgs, Args const &args)
        {
            typedef typename boost::mpl::deref<From>::type arg_type;
            typedef typename boost::mpl::next<From>::type next_iter_type;

            arg_type argType = (arg_type) *(functionArgs.thisPtr);

            return BoostRPCInvoker<Function, next_iter_type, To>::apply
                ( func, functionArgs, boost::fusion::push_back(args, boost::ref(argType) ) );
        }
    };

    template<typename Function, class To>
    struct BoostRPCInvoker<Function,To,To>
    {
        // the argument list is complete, now call the function
        template<typename Args>
        static inline
            InvokeResultCodes apply(Function func, InvokeArgs&, Args const &args)
        {
            boost::fusion::invoke(func,args);

            return IRC_SUCCESS;
        }
    };

    template <typename T>
    struct DoNothing
    {
        static void apply(NetSerializer &out, T& t)
        {
            (void) out;
            (void) t;
        }
    };

    struct WriteBitstream
    {
        static void applyArray(NetSerializer & out, NetSerializer * t)
        {
            apply(out, t);
        }
        static void apply(NetSerializer & out, NetSerializer * t)
        {
            NBitCount oldReadOffset = t->getReadOffset();
            t->seekRead(0);
            out.writeCompress(t->getWriteOffset());
            out.write(t);
            t->seekRead(oldReadOffset);
        }
    };

    struct WritePtr
    {
        template <typename T2>
        static inline void applyArray(NetSerializer & out, T2 * t)
        {
            out.wirte(*t);
        }
        template <typename T2>
        static inline void apply(NetSerializer & out, T2 * t)
        {
            out.wirte(*t);
        }
        static inline void apply(NetSerializer & out, char *t)
        {
            out.wirte(t);
        }
        static inline void apply(NetSerializer & out, unsigned char * t)
        {
            out.wirte(t);
        }
        static inline void apply(NetSerializer & out, const char * t)
        {
            out.wirte(t);
        }
        static inline void apply(NetSerializer & out, const unsigned char * t)
        {
            out.wirte(t);
        }
    };

    template< typename T >
    struct DoWrite
    {
        typedef typename boost::mpl::if_<
            boost::is_convertible<T *, NetSerializer *>,
            WriteBitstream,
            WritePtr >::type type;
    };

    template <typename T>
    struct WriteWithNetworkIDPtr
    {
        static void apply(NetSerializer & out, T & t)
        {
            bool isNull;
            isNull = (t == 0);
            out.write(isNull);
            if (isNull)
                return;
            RPC3Tag tag;
            __RPC3ClearPtr(t, &tag);
            bool deref = (tag.flag & RPC3_TAG_FLAG_DEREF) !=0;
            bool isArray = (tag.flag & RPC3_TAG_FLAG_ARRAY) !=0;
            out.write(deref);
            out.write(isArray);
            if (isArray)
            {
                out.writeCompress(tag.count);
            }
            for (NCount i = 0; i < tag.count; ++i)
            {
                NetworkID inNetworkID = t->GetNetworkID();
                out.wirte(inNetworkID);
                if(deref)
                {
                    // skip bytes, write data, go back, write number of bits written, reset cursor
                    out.writeAlign();
                    NBitCount wBO1 = out.getWriteOffset();
                    NBitCount BU1 = out.getWriteOffset(); // ¡Ÿ ±
                    out.write(BU1);
                    BU1 = out.getWriteOffset();
                    DoWrite < typename boost::remove_pointer<T>::type >::type::apply(out, t);
                    NBitCount wBO2 = out.getWriteOffset();
                    NBitCount BU2 = out.getWriteOffset();
                    out.seekWrite(wBO1);
                    out.write(BU2 - BU1);
                    out.seekWrite(wBO2);
                }
            }
        }
    };

    template <typename T>
    struct WriteWithoutNetworkIDNoPtr
    {
        static void apply(NetSerializer &out, T& t)
        {
            DoWrite< typename boost::remove_pointer<T>::type >::type::apply(out,&t);
        }
    };

    template <typename T>
    struct WriteWithoutNetworkIDPtr
    {
        static void apply(NetSerializer &out, T& t)
        {
            bool isNull;
            isNull=(t==0);
            out.write(isNull);
            if (isNull)
                return;

            RPC3Tag tag;
            __RPC3ClearPtr((void*) t, &tag);
            bool isArray = (tag.flag & RPC3_TAG_FLAG_ARRAY) !=0;
            out.write(isArray);
            if (isArray)
            {
                out.writeCompress(tag.count);
            }
            if (isArray)
            {
                for (NCount i = 0; i < tag.count; ++i)
                    DoWrite< typename boost::remove_pointer<T>::type >::type::applyArray(out,t+i);
            }
            else
            {
                DoWrite< typename boost::remove_pointer<T>::type >::type::apply(out,t);
            }

        }
    };

    template <typename T>
    struct SerializeCallParameterBranch
    {
        typedef typename boost::mpl::if_<
            typename IsRPC3Ptr<T>::type
            , DoNothing<T>
            , WriteWithoutNetworkIDPtr<T>
        >::type typeCheck1;

        typedef typename boost::mpl::if_<
            boost::is_pointer<T>
            , typeCheck1
            , WriteWithoutNetworkIDNoPtr<T>
        >::type typeCheck2;

        typedef typename boost::mpl::if_<
            typename ShouldReadNetworkID<T>::type
            , WriteWithNetworkIDPtr<T>
            , typeCheck2
        >::type type;
    };
    template<typename Function>
    struct GetBoundPointer_C
    {
        static FunctionPointer GetBoundPointer(Function f)
        {
            return FunctionPointer(false, boost::bind( & BoostRPCInvoker<Function>::template apply<boost::fusion::nil>, f, _1, boost::fusion::nil() ));
        }
    };

    template<typename Function>
    struct GetBoundPointer_CPP
    {
        static FunctionPointer GetBoundPointer(Function f)
        {
            return FunctionPointer(true, boost::bind(& BoostRPCInvoker_ThisPtr<Function>::template apply<boost::fusion::nil>, f, _1, boost::fusion::nil() ));
        }
    };
    template<typename Function>
    FunctionPointer GetBoundPointer(Function f)
    {
        return boost::mpl::if_<
        boost::is_member_function_pointer<Function>
        , GetBoundPointer_CPP<Function>
        , GetBoundPointer_C<Function>
        >::type::GetBoundPointer(f);
    }

    }
}
}
#endif