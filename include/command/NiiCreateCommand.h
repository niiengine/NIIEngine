/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2015-5-7

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
#include "NiiCommand.h"
#include "NiiGenerateObjEventArgs.h"

namespace NII
{
namespace NII_COMMAND
{
    /** (�ɽ�������Ķ���)�Ĵ�������
    @version NIIEngine 3.0.0
    */
    class _EngineAPI CreateCommand : public Command
    {
    public:
        CreateCommand();
        ~CreateCommand();

        ///@copydetails Command::main
        void main();
    protected:
        /** ����������ִ�к��ͬʱ�����������
        @version NIIEngine 3.0.0
        */
        void onCreated(const GenerateObjEventArgs & arg);
    private:
        GenerateObjEventArgs * mArg;
    };
}
}