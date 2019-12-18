/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2015-8-26

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

#ifndef _NII_UI_STYLEMANAGER_H_
#define _NII_UI_STYLEMANAGER_H_

#include "NiiUIPreInclude.h"
#include "NiiSingleton.h"
#include "NiiUIException.h"
#include "NiiUIStyle.h"

namespace NII
{
    /** UI��������
    @version NIIEngine 3.0.0
    */
    class _EngineAPI UIStyleManager : public Singleton<UIStyleManager>, public UIAlloc
    {
    public:
        typedef map<StyleID, Style *>::type StyleList;
    public:
        UIStyleManager();
        ~UIStyleManager();

        /** �����ļ�
        @version NIIEngine 3.0.0
        */
        void load(const String & file, GroupID gid = GroupUnknow);
        
        /** �����ڴ���
        @version NIIEngine 3.0.0
        */
        void load(const MemDataStream * data);
        
        /** ��ӷ��
        @version NIIEngine 3.0.0
        */
        void add(Style * style);

        /** ��ȥ���
        @version NIIEngine 3.0.0
        */
        void remove(StyleID style);

        /** ��ȥ���з��
        @version NIIEngine 3.0.0
        */
        void removeAll();

        /** �Ƿ���ڷ��
        @version NIIEngine 3.0.0
        */
        bool isExist(StyleID style) const;

        /** ��ȡ���
        @version NIIEngine 3.0.0
        */
        Style * get(StyleID style) const;

        /** 
        @param[out]
        */
        void write(StyleID style, std::ostream & out) const;

        /** 
        @param[out] out 
        */
        void exportSeries(Nui16 Ser, std::ostream & out) const;

        /** 
        @version NIIEngine 3.0.0
        */
		static void setGroup(GroupID gid)
        {
            ResourceGroup = gid;
        }

        /** 
        @version NIIEngine 3.0.0
        */
		static GroupID getGroup()
        {
            return ResourceGroup;
        }
        
		const StyleList & getWidgetList() const;

       	/// @copydetails Singleton::geOnly
        static UIStyleManager & getOnly();

        /// @copydetails Singleton::getOnlyPtr
        static UIStyleManager * getOnlyPtr();
    private:
		static GroupID ResourceGroup;
        StyleList mStyleList;
    };
}
#endif