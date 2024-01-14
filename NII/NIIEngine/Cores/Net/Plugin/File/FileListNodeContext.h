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

#ifndef __FILE_LIST_NODE_CONTEXT_H
#define __FILE_LIST_NODE_CONTEXT_H

#include "NiiNetSerializer.h"

struct FileListNodeContext
{
	FileListNodeContext() {dataPtr=0; dataLength=0;}
	FileListNodeContext(Nui8 o, Nui32 f1, Nui32 f2, Nui32 f3) : op(o), flnc_extraData1(f1), flnc_extraData2(f2), flnc_extraData3(f3) {dataPtr=0; dataLength=0;}
	~FileListNodeContext() {}

	Nui8 op;
	Nui32 flnc_extraData1;
	Nui32 flnc_extraData2;
	Nui32 flnc_extraData3;
	void *dataPtr;
	Nui32 dataLength;
};

inline NetSerializer& operator<<(NetSerializer & out, FileListNodeContext& in)
{
	out.write(in.op);
	out.write(in.flnc_extraData1);
	out.write(in.flnc_extraData2);
	out.write(in.flnc_extraData3);
	return out;
}
inline NetSerializer& operator>>(NetSerializer & in, FileListNodeContext& out)
{
	in.read(out.op);
	bool success = in.read(out.flnc_extraData1);
	(void) success;
	N_assert(success);
	success = in.read(out.flnc_extraData2);
	(void) success;
	N_assert(success);
	success = in.read(out.flnc_extraData3);
	(void) success;
	N_assert(success);
	return in;
}

#endif
