/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2015-7-8

�ı�����: utf-8

������˾: �������ǿƼ����޹�˾

�������: ����������

��̷��: ͳ��ʽ

����ģʽ: �ֲ�ʽ

�ڲ��ɷ�: UI���� ������� ��Ƶ���� ������� �¼���������(��ɢ�����)

��Ҫ�ɷ�: c++(80%) c(20%)

��;: ����ϵͳ����(��Χ����ϵͳ�ں�api)
      ��άӦ�����
        ������Ϣϵͳ���(GIS)
        ��Ӱ���������ع����
        ������Ϸ���

ƫ����;: ������Ϸ���

��ҳ: www.niiengine.com ��������: niiengine@gmail.com OR niiengine@163.com

��Ȩ��ʽ:��ҵ��Ȩ(www.niiengine.com/license)(3��)
------------------------------------------------------------------------------
*/

#ifndef _NII_TEMPBUFFER_H_
#define _NII_TEMPBUFFER_H_

#include "NiiPreInclude.h"

namespace NII
{
    /** ��̬����
    @version NIIEngine 3.0.0
    */
    template <NCount size> class TempBuffer
    {
    public:
        TempBuffer()
        {
            mValid = 0;
            mPos = 0;
        }

        /** ��������
        @note ֻ���������ڻ����С�����ݡ�
        @version NIIEngine 3.0.0
        */
        NCount cacheData(const void * data, NCount cnt)
        {
            if(cnt < size)
            {
                if(cnt + mValid <= size)
                {
                    memcpy(mBuffer + mValid, data, cnt);
                    mValid += cnt;
                }
                else
                {
                    NCount begOff = cnt - (size - mValid);
                    memmove(mBuffer, mBuffer + begOff, mValid - begOff);
                    memcpy(mBuffer + size - cnt, data, cnt);
                    mValid = size;
                }
                mPos = mValid;
                return cnt;
            }
            else
            {
                memcpy(mBuffer, (const Nui8 *)data + cnt - size, size);
                mValid = mPos = size;
                return size;
            }
        }
        /** �ӻ����ж�ȡ����
        @version NIIEngine 330.0
        */
        NCount read(void * data, NCount cnt) const
        {
            NCount rb = avail();
            rb = (rb < cnt) ? rb : cnt;
            memcpy(data, mBuffer + mPos, rb);
            mPos += rb;
            return rb;
        }

        /** ����
        @version NIIEngine 3.0.0
        */
        bool rewind(NCount cnt)
        {
            if (mPos < cnt)
            {
                clear();
                return false;
            }
            else
            {
                mPos -= cnt;
                return true;
            }
        }

        /** ��ת
        @version NIIEngine 3.0.0
        */
        bool skip(NCount cnt)
        {
            if (avail() < cnt)
            {
                clear();
                return false;
            }
            else
            {
                mPos += cnt;
                return true;
            }
        }

        /** ʣ�໺���С
        @version NIIEngine 3.0.0
        */
        NCount avail() const
        {
            return mValid - mPos;
        }

        /** �������
        @version NIIEngine 3.0.0
        */
        void clear()
        {
            mValid = 0;
            mPos = 0;
        }
    protected:
        Nui8 mBuffer[size];
        NCount mValid;
        mutable NCount mPos;
    };
}
#endif