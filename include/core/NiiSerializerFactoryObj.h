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

#include "NiiPreInclude.h"

namespace NII
{
    class _EngineAPI SerializerFactoryObj
    {
    public:
        SerializerFactoryObj();
        virtual ~SerializerFactoryObj();

        /** 获取工厂ID
        @version NIIEngine 3.0.0
        */
        virtual FactoryID getID() const = 0;

        /** 创建对象
        @version NIIEngine 3.0.0
        */
        virtual Serializer * create(const DataStream * src) = 0;

        /** 创建对象
        @version NIIEngine 3.0.0
        */
        virtual Serializer * create(const FileSerializer * src) = 0;

        /** 删除对象
        @version NIIEngine 3.0.0
        */
        virtual void destroy(Serializer * obj) = 0;
    };
}