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

#include "ICodec.h"

namespace NII
{
namespace NII_MEDIA
{
    class CodecFactory
    {
    public:
        CodecFactory() {}
        virtual ~CodecFactory() {}

        static ICodec * CreateCodec(const CStdString & strFileType);
        static ICodec * CreateCodecDemux(const CStdString & strFile, 
        const CStdString & strContent,NCount filecache);
    private:
        static ICodec * CreateOGGCodec(const CStdString & strFile, NCount filecache);
    };
}
}