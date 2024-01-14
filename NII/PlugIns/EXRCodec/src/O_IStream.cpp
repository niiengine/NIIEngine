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
#include "O_IStream.h"
#include <Iex.h>
using namespace Imf;


namespace Ogre {

bool O_IStream::read(char c[], int n) {
    size_t s = _stream.read(c, n);
    if(s != n)
        throw Iex::InputExc ("Unexpected end of file.");
    return _stream.eof();
}

Int64 O_IStream::tellg() {
    return _stream.getCurrentPtr() - _stream.getPtr();
}

void O_IStream::seekg(Int64 pos) {
    _stream.seek(pos);
}

void O_IStream::clear() {
    /* Clear error flags -- there are no error flags */
}


};
