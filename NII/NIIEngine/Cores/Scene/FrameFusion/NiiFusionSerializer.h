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

#ifndef _NII_FusionSerializer_H_
#define _NII_FusionSerializer_H_

#include "NiiPreInclude.h"
#include "NiiDataStream.h"
#include "NiiFileSerializer.h"
#include "NiiFusion.h"

namespace NII
{
    /** 泛型类,序列化数据to/from二进制流文件
    @remarks
        这个类提供有效方法的数字,为exporting/importing数据从面向流的二进制文件.
    */
    class _EngineAPI FusionSerializer : public FileSerializer
    {
    public:
        FusionSerializer();
        ~FusionSerializer();

        /** 导出一个网格到指定文件
        @param[in] in
        @param[in] file
        @param[in] ver
        @param[in] fromat
        @param[in] emode
        */
        void write(const Fusion * in, const String & file,
            SerialVer ver , SerialFormat sf, Endian emode = E_Native);

        /** 导出一个网格到指定文件
        @param[in] in
        @param[in] stream
        @param[in] emode
        */
        void write(const Fusion * in, DataStream * stream,
            SerialVer ver , SerialFormat sf, Endian emode = E_Native);

        /** 从一个.mesh文件DataStream,导近网格和(可选)网格数据
        @remark
        @param[in] stream
        @param[in] out
        */
        void read(DataStream * stream, Fusion * out);
    protected:
        /// @copydetails FileSerializer::read
        void readUnit(void * out);

        /// @copydetails FileSerializer::write
        void writeUnit(const void * in, SerialVer ver = 30000,
            SerialFormat sf = F_EC_None, Endian emode = E_Native);
    };
}
#endif