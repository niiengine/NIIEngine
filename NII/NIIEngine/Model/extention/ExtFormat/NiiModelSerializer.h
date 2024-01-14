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

#ifndef _NII_MODEL_SERIALIZER_
#define _NII_MODEL_SERIALIZER_

#include "NiiPererq.h"
#include "NiiSerializer.h"

namespace NII
{
namespace ModelExtension
{
    class ModelExtSerializer : public FileSerializer
    {
    public:
        ModelExtSerializer();
        virtual ~ModelExtSerializer();

        /** 导出一个网格到指定文件
        @param[in] in
        @param[in] filename
        @param[in] ver
        @param[in] format
        @param[in] emode
        */
        void export(const ModelExt * in, const String & file, SerialVer ver, SerialFormat sf,
            Endian emode = E_Native);

        /** 导出一个网格到指定文件
        @param[in] in
        @param[in] stream
        @param[in] ver
        @param[in] format
        @param[in] emode
        */
        void export(const ModelExt * in, DataStream * stream, SerialVer ver, SerialFormat sf,
            Endian emode = E_Native);

        /** 从一个.mesh文件DataStream,导近网格和(可选)网格数据
        @remark
        @param[in] stream
        @param[in] out
        */
        void import(DataStream * stream, ModelExt * out);
    protected:
        /// @copydetails FileSerializer::read
        void read(void * out);

        /// @copydetails FileSerializer::write
        void write(const void * in, SerialVer ver = 30000, SerialFormat sf = F_EC_None,
            Endian emode = E_Native);
    };
}
}
#endif