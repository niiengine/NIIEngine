/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2014-5-8

�ı�����: utf-8

������˾: �������ǿƼ����޹�˾

�������: ����������

��̷��: ͳ��ʽ

����ģʽ: �ֲ�ʽ

�ڲ��ɷ�: UI���� ������� ��Ƶ���� ������� �¼���������(��ɢ�����)

��Ҫ�ɷ�: c++(80%) c(20%)

��;: ����ϵͳ����(��Χ����ϵͳ�ں�api)
      ��άӦ�����
        �������������������(CAD)
        ������Ϣϵͳ���(GIS)
        ��Ӱ���������ع����
        ������Ϸ���

ƫ����;: ������Ϸ���

��ҳ: www.niiengine.com ��������: niiengine@gmail.com OR niiengine@163.com

��Ȩ��ʽ:��ҵ��Ȩ(www.niiengine.com/license)(3��)
------------------------------------------------------------------------------
*/

#include "NiiPreInclude.h"
#include "NiiSerialItem.h"

namespace NII
{
namespace NII_MEDIA
{
    /**
    @version NIIIEngine 
    */
    class _EngineAPI TouchSerialItemArgs : public SerialItemArgs
    {
    public:
        TouchSerialItemArgs() {}
        ~TouchSerialItemArgs() {}

        /// @copydetails ItemSerialArgs::operator ==
        bool operator == (const EventArgs * other) const 
        {
            const TouchSerialItemArgs * temp = static_cast<const TouchSerialItemArgs *>(o);
            if (temp->dest == dest)
                return true;
            return false;
        }

        /// @copydetails ItemSerialArgs::operator !=
        bool operator != (const EventArgs * other) const 
        {
            const TouchSerialItemArgs * temp = static_cast<const TouchSerialItemArgs *>(o);
            if (temp->dest != dest)
                return true;
            return false;
        }

        Vector3<NIIi> dest;  ///< Ŀ��λ��
    };
}
}