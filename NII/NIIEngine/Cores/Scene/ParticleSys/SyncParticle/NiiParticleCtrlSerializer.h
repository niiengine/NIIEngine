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

#ifndef _NII_Billboard_Serializer_H_
#define _NII_Billboard_Serializer_H_

#include "NiiPreInclude.h"
#include "NiiFileSerializer.h"
#include "NiiString.h"

namespace NII
{
    class _EngineAPI ParticleCtrlSerializer : public FileSerializer
    {
    public:
        ParticleCtrlSerializer();
        virtual ~ParticleCtrlSerializer();

        /** 导出粒子控制到指定文件
        @param[in] in
        @param[in] file
        @param[in] emode
        */
        void write(const ParticleCtrl * in, const String & file, SerialVer ver,
            SerialFormat sf, Endian emode = E_Native);

        /** 导出粒子控制到指定文件
        @param[in] in
        @param[in] stream
        @param[in] emode
        */
        void write(const ParticleCtrl * in, DataStream * stream, SerialVer ver,
            SerialFormat sf, Endian emode = E_Native);

        /** 从一个.particlectrl文件DataStream,导近粒子控制数据
        @remark
        @param[in] stream
        @param[in] out
        */
        void read(DataStream * stream, ParticleCtrl * out);
    protected:
        /// @copydetails Serializer::read
        void readUnit(void * out);

        /// @copydetails Serializer::write
        void writeUnit(const void * in, SerialVer ver = 30000,
            SerialFormat sf = F_EC_None, Endian emode = E_Native);
    };
}
#endif