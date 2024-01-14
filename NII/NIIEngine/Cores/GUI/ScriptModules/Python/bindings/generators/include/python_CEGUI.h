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
#include <vector>
#include <string>
#include <map>
#include <iostream>

#include "NIIUI.h"
#include "NIIXMLEnumHelper.h"

namespace boost
{
    /*
	 * Workaround that allows users to inherit CEGUI classes with methods
	 * that deal with strings (almost all classes)
	 *
	 * The situation:
	 * boost python can't recognize boost::ref(const String &) and doesn't
	 * auto convert it to python unicode string. This works for all types that are
	 * registered normally (String is always converted, the class is unknown
	 * in python).
	 *
	 * The solution:
	 * Make an overloaded version of boost::ref that just returns what it gets.
	 *
	 * Is this horrible?
	 * YES!
	 */
    inline const CEGUI::String & ref(const CEGUI::String & v)
    {
        return v;
    }
}

namespace NII
{
namespace NII_MEDIA
{
namespace UI
{
    class PropertyHelper_wrapper
    {
    public:
        static float stringToFloat(const String & str)
        {
            float temp;
            StrConv::conv(str, temp);
            return temp;
        }
        static unsigned int stringToUint(const String & str)
        {
            Nui32 temp;
            StrConv::conv(str, temp);
            return temp;
        }
        static bool stringToBool(const String & str)
        {
            bool temp;
            StrConv::conv(str, temp);
            return temp;
        }
        static PlaneSizef stringToSize(const String & str)
        {
            PlaneSizef temp;
            StrConv::conv(str, temp);
            return temp;
        }
        static Vector2f stringToVector2(const String & str)
        {
            Vector2f temp;
            StrConv::conv(str, temp);
            return temp;
        }
        static Rectf stringToRect(const String & str)
        {
            Rectf temp;
            StrConv::conv(str, temp);
            return temp;
        }
        static const PixelBuffer * stringToImage(const String & str)
        {
            StringList temp;
            StrUtil::split(str, temp, "|");
            Nui32 rid, pid;
            StrConv(temp[0], rid);
            StrConv(temp[1], pid);
            return N_Only(Pixel)->get(rid, pid);
        }
        static Colour stringToColour(const String & str)
        {
            Colour temp;
            StrConv::conv(str, temp);
            return temp;
        }
        static ColourArea stringToColourArea(const String & str)
        {
            ColourArea temp;
            StrConv::conv(str, temp);
            return temp;
        }
        static RelPos stringToUDim(const String & str)
        {
            RelPos temp;
            StrConv::conv(str, temp);
            return temp;
        }
        static RelVector2 stringToRelVector2(const String & str)
        {
            RelVector2 temp;
            StrConv::conv(str, temp);
            return temp;
        }
        static RelRect stringToRelRect(const String & str)
        {
            RelRect temp;
            StrConv::conv(str, temp);
            return temp;
        }

        static String floatToString(float val)
        {
            return StrConv::conv(val);
        }
        static String uintToString(unsigned int val)
        {
            return StrConv::conv(val);
        }
        static String boolToString(bool val)
        {
            return StrConv::conv(val);
        }
        static String sizeToString(const PlaneSizef & val)
        {
            return StrConv::conv(val);
        }
        static String vector2ToString(const Vector2f & val)
        {
            return StrConv::conv(val);
        }
        static String rectToString(const Rectf & val)
        {
            return StrConv::conv(val);
        }
        static String imageToString(const PixelBuffer * val)
        {
            return val->getGroupID() + "|" + val->getID();
        }
        static String colourToString(const Colour & val)
        {
            return StrConv::conv(val);
        }
        static String colourRectToString(const ColourArea & val)
        {
            return StrConv::conv(val);
        }
        static String udimToString(const RelPos & val)
        {
            return StrConv::conv(val);
        }
        static String uvector2ToString(const RelVector2 & val)
        {
            return StrConv::conv(val);
        }
        static String urectToString(const RelRect & val)
        {
            return StrConv::conv(val);
        }
        static String uboxToString(const RelRect & val)
        {
            return StrConv::conv(val);
        }
    };

    class FalagardXMLHelper_wrapper
    {
    public:
        static VLayout stringToVertFormat(const String & str)
        {
            VLayout temp;
            UIStrConv::conv(str, temp);
            return temp;
        }
        static HLayout stringToHorzFormat(const String & str)
        {
            HLayout temp;
            UIStrConv::conv(str, temp);
            return temp;
        }
        static VAlign stringToVertAlignment(const String & str)
        {
            VAlign temp;
            UIStrConv::conv(str, temp);
            return temp;
        }
        static HAlign stringToHorzAlignment(const String & str)
        {
            HAlign temp;
            UIStrConv::conv(str, temp);
            return temp;
        }
        static SizeType stringToDimensionType(const String & str)
        {
            SizeType temp;
            UIStrConv::conv(str, temp);
            return temp;
        }
        static VTextLayout stringToVertTextFormat(const String & str)
        {
            VTextLayout temp;
            UIStrConv::conv(str, temp);
            return temp;
        }
        static HTextLayout stringToHorzTextFormat(const String & str)
        {
            HTextLayout temp;
            UIStrConv::conv(str, temp);
            return temp;
        }
        static FontMetricType stringToFontMetricType(const String & str)
        {
            FontMetricType temp;
            UIStrConv::conv(str, temp);
            return temp;
        }
        static SizeOpType stringToDimensionOperator(const String & str)
        {
            SizeOpType temp;
            UIStrConv::conv(str, temp);
            return temp;
        }
        static FrameComType stringToFrameImageComponent(const String & str)
        {
            FrameComType temp;
            UIStrConv::conv(str, temp);
            return temp;
        }

        static String vertFormatToString(VLayout val)
        {
            VLayout temp;
            UIStrConv::conv(str, temp);
            return temp;
        }
        static String horzFormatToString(HLayout val)
        {
            HLayout temp;
            UIStrConv::conv(str, temp);
            return temp;
        }
        static String vertAlignmentToString(VAlign val)
        {
            VAlign temp;
            UIStrConv::conv(str, temp);
            return temp;
        }
        static String horzAlignmentToString(HAlign val)
        {
            HAlign temp;
            UIStrConv::conv(str, temp);
            return temp;
        }
        static String dimensionTypeToString(SizeType val)
        {
            SizeType temp;
            UIStrConv::conv(str, temp);
            return temp;
        }
        static String vertTextFormatToString(VTextLayout val)
        {
            VTextLayout temp;
            UIStrConv::conv(str, temp);
            return temp;
        }
        static String horzTextFormatToString(HTextLayout val)
        {
            HTextLayout temp;
            UIStrConv::conv(str, temp);
            return temp;
        }
        static String fontMetricTypeToString(FontMetricType val)
        {
            FontMetricType temp;
            UIStrConv::conv(str, temp);
            return temp;
        }
        static String dimensionOperatorToString(SizeOpType val)
        {
            SizeOpType temp;
            UIStrConv::conv(str, temp);
            return temp;
        }
        static String frameImageComponentToString(FrameComType val)
        {
            FrameComType temp;
            UIStrConv::conv(str, temp);
            return temp;
        }
    };
}
}
}

// First we create a magic namespace to hold all our aliases
namespace pyplusplus
{
    namespace aliases
    {
        typedef ::NII::NII_MEDIA::UI::SharedPtr<NII::NII_MEDIA::UI::PixelLayout> RefCountedFormattedRenderedString;

        typedef ::NII::NII_MEDIA::UI::Vector2<float> Vector2;
        typedef ::NII::NII_MEDIA::UI::Vector3<float> Vector3;
        typedef ::NII::NII_MEDIA::UI::PlaneSize<float> PlaneSize;
        typedef ::NII::NII_MEDIA::UI::TRect<float> Rect;
        typedef ::NII::NII_MEDIA::UI::Vector2<NII::NII_MEDIA::UI::RelPos> RelVector2;
        typedef ::NII::NII_MEDIA::UI::TRect<NII::NII_MEDIA::UI::RelPos> RelRect;
        typedef ::NII::NII_MEDIA::UI::PlaneSize<NII::NII_MEDIA::UI::RelPos> RelPlaneSize;

        typedef std::vector<NII::NII_MEDIA::UI::ListboxItem *> ListBoxItemPtrVector;

        typedef ::NII::NII_MEDIA::UI::SignalPtr Connection;

        typedef ::NII::NII_MEDIA::UI::PixelManager::ImageIterator ImageIterator;

        typedef std::pair<float, float> StdPairFloatFloat;
        typedef std::pair<NII::NII_MEDIA::UI::String, NII::NII_MEDIA::UI::String> StdPairCEGUIStringString;

        typedef std::map<std::string, std::string> StringMap;
        typedef std::vector<std::string> StringList;

        typedef NII::NII_MEDIA::UI::Singleton<NII::NII_MEDIA::UI::AnimationManager> SingletonAnimationManager;
        typedef NII::NII_MEDIA::UI::Singleton<LogManager> SingletonLogger;
        typedef NII::NII_MEDIA::UI::Singleton<NII::NII_MEDIA::UI::FontManager> SingletonFontManager;
        typedef NII::NII_MEDIA::UI::Singleton<NII::NII_MEDIA::UI::PixelManager> SingletonImageManager;
        typedef NII::NII_MEDIA::UI::Singleton<NII::NII_MEDIA::UI::MouseCursor> SingletonMouseCursor;
        typedef NII::NII_MEDIA::UI::Singleton<NII::NII_MEDIA::UI::RenderEffectManager> SingletonRenderEffectManager;
        typedef NII::NII_MEDIA::UI::Singleton<NII::NII_MEDIA::UI::SchemeManager> SingletonSchemeManager;
        typedef NII::NII_MEDIA::UI::Singleton<NII::NII_MEDIA::UI::UIManager> SingletonSystem;
        typedef NII::NII_MEDIA::UI::Singleton<NII::NII_MEDIA::UI::StyleManager> SingletonWidgetLookManager;
        typedef NII::NII_MEDIA::UI::Singleton<NII::NII_MEDIA::UI::WidgetManager> SingletonWindowManager;

        typedef unsigned int Group;
        typedef std::map<NII::NII_MEDIA::UI::String, NII::NII_MEDIA::UI::String, NII::NII_MEDIA::UI::StringFastLessCompare
            > ResourceGroups;
        typedef NII::NII_MEDIA::UI::SharedPtr<NII::NII_MEDIA::UI::EventFunctor> Connection;
        typedef std::multimap<Group, Connection> SlotContainer;
        typedef std::map<NII::NII_MEDIA::UI::String, NII::NII_MEDIA::UI::Event *, NII::NII_MEDIA::UI::StringFastLessCompare
            > EventMap;
        typedef std::map<NII::NII_MEDIA::UI::String, NII::NII_MEDIA::UI::Font *, NII::NII_MEDIA::UI::StringFastLessCompare
            > FontRegistry;
        typedef std::pair<NII::NII_MEDIA::UI::Window *, bool> WindowStackEntry;
        typedef std::vector<WindowStackEntry> WindowStack;
        typedef std::map<NII::NII_MEDIA::UI::String, NII::NII_MEDIA::UI::PropertyCmd*, NII::NII_MEDIA::UI::StringFastLessCompare
            > PropertyRegistry;
        typedef std::map<NII::NII_MEDIA::UI::String, NII::NII_MEDIA::UI::String, NII::NII_MEDIA::UI::StringFastLessCompare
            > UserStringMap;
        typedef std::set<NII::NII_MEDIA::UI::String, NII::NII_MEDIA::UI::StringFastLessCompare> NonSerlProperty;
        typedef std::vector<NII::NII_MEDIA::UI::String> TargetTypeStack;		//!< Type used to implement stack of target type names.
        typedef std::map<NII::NII_MEDIA::UI::String, NII::NII_MEDIA::UI::Window*, NII::NII_MEDIA::UI::StringFastLessCompare
            > WindowRegistry; //!< Type used to implement registry of Window objects
        typedef std::vector<NII::NII_MEDIA::UI::Window *> WindowVector;   //!< Type to use for a collection of Window pointers.
        typedef std::vector<NII::NII_MEDIA::UI::PropertyCmd *> PropertyList;
        typedef std::map<NII::FactoryID, NII::NII_MEDIA::UI::WidgetViewFactory *, NII::NII_MEDIA::UI::StringFastLessCompare
            > WidgetModelList;
        typedef std::map<NII::PropertyID, NII::String> PropertyList;
    }
}

// then we exposed everything needed (and more) to ensure GCCXML makes them visible to Py++
//

// this is a gcc-only workaround, don't do this at build time
#ifdef __GCCXML__

namespace python_CEGUI
{
    namespace details
    {
        inline void instantiate()
        {
            sizeof(::std::pair<float, float>);
            sizeof(::NII::NII_MEDIA::UI::SignalPtr);
            sizeof(::NII::NII_MEDIA::UI::EventFunctor);

            sizeof(::NII::NII_MEDIA::UI::PixelManager::ImageIterator);
            sizeof(::NII::NII_MEDIA::UI::String::iterator);
            sizeof(::NII::NII_MEDIA::UI::Vector2<float>);
            sizeof(::NII::NII_MEDIA::UI::Vector3<float>);
            sizeof(::NII::NII_MEDIA::UI::PlaneSize<float>);
            sizeof(::NII::NII_MEDIA::UI::TRect<float>);
            sizeof(::NII::NII_MEDIA::UI::Vector2<NII::NII_MEDIA::UI::RelPos>);
            sizeof(::NII::NII_MEDIA::UI::TRect<NII::NII_MEDIA::UI::RelPos>);
            sizeof(::NII::NII_MEDIA::UI::PlaneSize<NII::NII_MEDIA::UI::RelPos>);

            sizeof(::std::pair<float, float>);
            sizeof(::std::pair<NII::NII_MEDIA::UI::String, NII::NII_MEDIA::UI::String>);

            // seems that doing an sizeof ( ::NII::NII_MEDIA::UI::CommandObj::Iterator ); doesn't expose the class :(
            sizeof(NII::NII_MEDIA::UI::ConstMapIterator<std::map<NII::NII_MEDIA::UI::String, NII::NII_MEDIA::UI::PropertyCmd *, NII::NII_MEDIA::UI::StringFastLessCompare> > );
            sizeof(NII::NII_MEDIA::UI::ConstMapIterator<std::map<NII::NII_MEDIA::UI::String, NII::NII_MEDIA::UI::Scheme *, NII::NII_MEDIA::UI::StringFastLessCompare> > );
            sizeof(NII::NII_MEDIA::UI::ConstVectorIterator<std::vector<NII::NII_MEDIA::UI::Window *> >);
            sizeof(NII::NII_MEDIA::UI::ConstMapIterator<std::map<NII::NII_MEDIA::UI::String, NII::NII_MEDIA::UI::WidgetManager::WidgetModelDefine, NII::NII_MEDIA::UI::StringFastLessCompare> > );
            sizeof(NII::NII_MEDIA::UI::ConstMapIterator<std::map<NII::NII_MEDIA::UI::String, NII::NII_MEDIA::UI::Event *, NII::NII_MEDIA::UI::StringFastLessCompare> > );
            sizeof(NII::NII_MEDIA::UI::ConstMapIterator<std::map<NII::NII_MEDIA::UI::String, NII::NII_MEDIA::UI::WidgetFactory *, NII::NII_MEDIA::UI::StringFastLessCompare> > );
            sizeof(NII::NII_MEDIA::UI::ConstMapIterator<std::map<NII::NII_MEDIA::UI::String, NII::NII_MEDIA::UI::Font *, NII::NII_MEDIA::UI::StringFastLessCompare> > );
            sizeof(NII::NII_MEDIA::UI::ConstVectorIterator<std::multiset<NII::NII_MEDIA::UI::Layer, std::less<NII::NII_MEDIA::UI::Layer> > > );
            sizeof(NII::NII_MEDIA::UI::ConstVectorIterator<std::vector<NII::NII_MEDIA::UI::StyleSectionImageUnit> > );
            sizeof(NII::NII_MEDIA::UI::ConstVectorIterator<std::set<NII::NII_MEDIA::UI::String, NII::NII_MEDIA::UI::StringFastLessCompare, std::allocator<NII::NII_MEDIA::UI::String> > > );
            sizeof(NII::NII_MEDIA::UI::ConstVectorIterator<std::vector<NII::NII_MEDIA::UI::StyleSectionFrameUnit> > );
            sizeof(NII::NII_MEDIA::UI::ConstMapIterator<std::map<NII::NII_MEDIA::UI::String, NII::NII_MEDIA::UI::StyleSection, NII::NII_MEDIA::UI::StringFastLessCompare> > );
            sizeof(NII::NII_MEDIA::UI::ConstMapIterator<std::map<NII::NII_MEDIA::UI::String, NII::NII_MEDIA::UI::StyleSpecial, NII::NII_MEDIA::UI::StringFastLessCompare> > );
            sizeof(NII::NII_MEDIA::UI::ConstMapIterator<std::map<NII::NII_MEDIA::UI::String, NII::NII_MEDIA::UI::Style, NII::NII_MEDIA::UI::StringFastLessCompare> > );
            sizeof(NII::NII_MEDIA::UI::ConstMapIterator<std::map<NII::NII_MEDIA::UI::String, NII::NII_MEDIA::UI::StyleLayout, NII::NII_MEDIA::UI::StringFastLessCompare> > );
            sizeof(NII::NII_MEDIA::UI::ConstVectorIterator<std::vector<NII::NII_MEDIA::UI::Section> > );
            sizeof(NII::NII_MEDIA::UI::ConstVectorIterator<std::vector<NII::NII_MEDIA::UI::StyleSectionTextUnit> > );
        }
    }
}

#endif