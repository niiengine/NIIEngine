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

#ifndef _NII_MODEL_EXT_H_
#define _NII_MODEL_EXT_H_

#include "NiiPreInclude.h"

namespace NII
{
namespace ModelExtension
{
    /** 这个类是易用性模型方案方法,其目的是为了简化操作过程,使用户更简单的使用其他
        模型格式加入到NII引擎中,我们知道,每种模型格式个有所不同,不能从成员变量上统
        筹数据,正因为如此,可能会导致结构和思想上不统一,这个类不从成员变量上统筹数据,
        而是从方法上统筹,也就是实现虚方法使更中模型格式有自己的具体实现.
    @remark 
        尤其是在动画模型上,数据统筹很难解决这个问题.
    @par 
        使用这个类要至少要实现,一个模型从文件名字到模型的方法,也要实现渲染这个模型,
        如果有动画的模型,其动画有名字,也要实现更为具体的渲染模型方法.
    @par 
        帧动画是最简单的模型动画,基本上不用处理数据,但会带来内存的开销,而且有不推荐
        使用这个动画思想.
    */
    class _EngineAPI ModelExt : public GeometryObj, public SpaceObj
    {
    public :
        ModelExt(){}
        virtual ~ModelExt(){}
    public :
        /// 获取模型相应的版本
        virtual const String & version() = 0;

        /** 这个方法用于人工,非自动比配具体的模型格式.
        @remark 有的模型是同后缀名字的,通过这函数可以真正获取具体的一种模型格式
        */
        virtual const String & description() = 0;

        /** 是否能重新建立顶点数组映射
        @remark 
            这个方法用于建立顶点数组的映射,也就是说不单一渲染每个顶点,而是调用具体
            的硬件设备数组.
        @par 
            由于涉及了模型的动画问题,或许在帧动画中,这种映射是容易成立,但在变形动
            画中就不好处理,因为我们要通过gpu-cpu然后再修改顶点后再传入到渲染,还有
            一种情况就是既使用了硬件映射,而且又使用了顶点着色程序,来处理变形动画.
        @return 
            如果返回真,说明这个模型是可以建立这种映射的,如果返回假,说明这个模型不
            支持这种映射,而且没有完成任何操作.
        */
        virtual void Built() = 0;
    public :
        DataStream * mDisk;                ///< 从磁盘中更新
    };
}
}
#endif