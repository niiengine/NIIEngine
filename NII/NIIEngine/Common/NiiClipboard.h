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

#ifndef _NII_CLIPBOARD_H_
#define _NII_CLIPBOARD_H_

#include "NiiPreInclude.h"
#include "NiiString.h"

namespace NII
{
    /** 剪切数据
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ClipboardData : public DataAlloc
    {
    public:
        virtual ~ClipboardData();

        /** 设置内容
        @version NIIEngine 3.0.0
        */
        virtual void set(const String & mime, void * data, NCount size) = 0;

        /** 获取内容
        @version NIIEngine 3.0.0
        */
        virtual void get(String & mime, void *& data, NCount & size) = 0;
    };

    /** 剪切器
    @version NIIEngine 3.0.0
    */
    class _EngineAPI Clipboard : public DataAlloc
    {
    public:
        Clipboard();
        ~Clipboard();

        /** 设置数据
        @param[in] size 字节大小
        @version NIIEngine 3.0.0
        */
        void setData(const String & mime, const Nui8 * buffer, NCount size);

        /** 获取数据
        @param[out] size 字节大小
        @version NIIEngine 3.0.0
        */
        void getData(String & mime, const Nui8 *& buffer, NCount & size);

        /** 设置剪切内容
        @version NIIEngine 3.0.0
        */
        void set(ClipboardData * src);

        /** 获取剪切内容
        @version NIIEngine 3.0.0
        */
        void get(ClipboardData *& src) const;

        /** 设置文本
        @remark 简易方式 text/plain 数据
        @version NIIEngine 3.0.0
        */
        void setText(const String & text);

        /** 获取文本
        @remark 简易方式 text/plain 数据
        @version NIIEngine 3.0.0
        */
        String getText();
    private:
        Nui8 * mBuffer; 
        String mMimeType;
        NCount mCount;
        ClipboardData * mData;
    };
}
#endif