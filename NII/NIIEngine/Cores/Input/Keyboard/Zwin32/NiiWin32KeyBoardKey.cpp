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

#include "NiiPreProcess.h"
#include "NiiKeyBoardControlPattern.h"

namespace NII
{
namespace NII_MEDIA
{
    //------------------------------------------------------------------------
    const KeyValue Key::Esc             = 0x01; ///< ESC����(��������)    //1
    const KeyValue Key::NO1             = 0x02; ///< ����1��(��������)    //2
    const KeyValue Key::NO2             = 0x03; ///< ����2��(��������)    //3
    const KeyValue Key::NO3             = 0x04; ///< ����3��(��������)    //4
    const KeyValue Key::NO4             = 0x05; ///< ����4��(��������)    //5
    const KeyValue Key::NO5             = 0x06; ///< ����5��(��������)    //6
    const KeyValue Key::NO6             = 0x07; ///< ����6��(��������)    //7
    const KeyValue Key::NO7             = 0x08; ///< ����7��(��������)    //8
    const KeyValue Key::NO8             = 0x09; ///< ����8��(��������)    //9
    const KeyValue Key::NO9             = 0x0A; ///< ����9��(��������)    //10
    const KeyValue Key::NO0             = 0x0B; ///< ����0��(��������)    //11
    const KeyValue Key::Minus           = 0x0C; ///< - �� _ ��(��������)  //12
    const KeyValue Key::Equals            = 0x0D; ///< = �� + ��(��������)  //13
    const KeyValue Key::BK              = 0x0E; ///< �ո��(��������)     //14
    const KeyValue Key::TAB             = 0x0F; ///< tab��(��������)      //15
    const KeyValue Key::Q               = 0x10; ///< q��                 //16
    const KeyValue Key::W               = 0x11; ///< w��                 //17
    const KeyValue Key::E               = 0x12; ///< e��                 //18
    const KeyValue Key::R               = 0x13; ///< r��                 //19
    const KeyValue Key::T               = 0x14; ///< t��                 //20
    const KeyValue Key::Y               = 0x15; ///< y��                 //21
    const KeyValue Key::U               = 0x16; ///< u��                 //22
    const KeyValue Key::I               = 0x17; ///< i��                 //23
    const KeyValue Key::O               = 0x18; ///< o��                 //24
    const KeyValue Key::P               = 0x19; ///< p��                 //25
    const KeyValue Key::LBracket        = 0x1A; ///< [ �� { ��(��������)  //26
    const KeyValue Key::RBracket        = 0x1B; ///< ] �� } ��(��������)  //27
    const KeyValue Key::Enter           = 0x1C; ///< Enter��(��������)    //28
    const KeyValue Key::LCtrl           = 0x1D; ///< ��ctrl(��������)     //29
    const KeyValue Key::A               = 0x1E; ///< a��                 //30
    const KeyValue Key::S               = 0x1F; ///< s��                 //31
    const KeyValue Key::D               = 0x20; ///< d��                 //32
    const KeyValue Key::F               = 0x21; ///< f��                 //33
    const KeyValue Key::G               = 0x22; ///< g��                 //34
    const KeyValue Key::H               = 0x23; ///< h��                 //35
    const KeyValue Key::J               = 0x24; ///< j��                 //36
    const KeyValue Key::K               = 0x25; ///< k��                 //37
    const KeyValue Key::L               = 0x26; ///< l��                 //38
    const KeyValue Key::Semicolon       = 0x27; ///< ; �� : ��(��������)  //39
    const KeyValue Key::Apostrophe      = 0x28; ///< ' �� " ��(��������)  //40
    const KeyValue Key::Grave           = 0x29; ///< ` �� ~ ��(��������)  //41
    const KeyValue Key::LShift          = 0x2A; ///< ��shift��(��������)  //42
    const KeyValue Key::BackSlash       = 0x2B; ///< \��|��(102��������)  //43
    const KeyValue Key::Z               = 0x2C; ///< z��                 //44
    const KeyValue Key::X               = 0x2D; ///< x��                 //45
    const KeyValue Key::C               = 0x2E; ///< c��                 //46
    const KeyValue Key::V               = 0x2F; ///< v��                 //47
    const KeyValue Key::B               = 0x30; ///< b��                 //48
    const KeyValue Key::N               = 0x31; ///< n��                 //49
    const KeyValue Key::M               = 0x32; ///< m��                 //50
    const KeyValue Key::Comma           = 0x33; ///< , �� < ��(��������)  //51
    const KeyValue Key::Period          = 0x34; ///< . �� > ��(��������)  //52
    const KeyValue Key::Slash           = 0x35; ///< / �� ? ��(��������)  //53
    const KeyValue Key::RShift          = 0x36; ///< ��shift��(��������)  //54
    const KeyValue Key::N_Mul           = 0x37; ///< *��(���ּ���)        //55
    const KeyValue Key::LAlt            = 0x38; ///< ��alt��(��������)    //56
    const KeyValue Key::Space           = 0x39; ///< Space��             //57
    const KeyValue Key::CapsLock        = 0x3A; ///< CapsLock��          //58
    const KeyValue Key::F1              = 0x3B; ///< F1��                //59
    const KeyValue Key::F2              = 0x3C; ///< F2��                //60
    const KeyValue Key::F3              = 0x3D; ///< F3��                //61
    const KeyValue Key::F4              = 0x3E; ///< F4��                //62
    const KeyValue Key::F5              = 0x3F; ///< F5��                //63
    const KeyValue Key::F6              = 0x40; ///< F6��                //64
    const KeyValue Key::F7              = 0x41; ///< F7��                //65
    const KeyValue Key::F8              = 0x42; ///< F8��                //66
    const KeyValue Key::F9              = 0x43; ///< F9��                //67
    const KeyValue Key::F10             = 0x44; ///< F10��               //68
    const KeyValue Key::N_Lock          = 0x45; ///< NumLock��           //69
    const KeyValue Key::M_ScrLock       = 0x46; ///< ScrollLock��        //70
    const KeyValue Key::N_NO7           = 0x47; ///< ����7��(��������)    //71
    const KeyValue Key::N_NO8           = 0x48; ///< ����8��(��������)    //72
    const KeyValue Key::N_NO9           = 0x49; ///< ����9��(��������)    //73
    const KeyValue Key::N_Subtract      = 0x4A; ///< -��(��������)        //74
    const KeyValue Key::N_NO4           = 0x4B; ///< ����4��(��������)    //75
    const KeyValue Key::N_NO5           = 0x4C; ///< ����5��(��������)    //76
    const KeyValue Key::N_NO6           = 0x4D; ///< ����6��(��������)    //77
    const KeyValue Key::N_Add           = 0x4E; ///< +��(��������)        //78
    const KeyValue Key::N_NO1           = 0x4F; ///< ����1��(��������)    //79
    const KeyValue Key::N_NO2           = 0x50; ///< ����2��(��������)    //80
    const KeyValue Key::N_NO3           = 0x51; ///< ����3��(��������)    //81
    const KeyValue Key::N_NO0           = 0x52; ///< Ins������0��(��������)//82
    const KeyValue Key::N_Dot           = 0x53; ///< . �� Del ��(��������)//83
    const KeyValue Key::F11             = 0x57; ///< F11��(101+��������)  //84
    const KeyValue Key::F12             = 0x58; ///< F12��(101+��������)  //85
    const KeyValue Key::N_Enter         = 0x9C; ///< Enter��(��������)    //86
    const KeyValue Key::RCtrl           = 0x9D; ///< ��ctrl��(104��������)//87
    const KeyValue Key::N_Divide        = 0xB5; ///< /��(��������)         //88
    const KeyValue Key::M_SysRQ         = 0xB7; ///< ������Ļͼ��(��������) //89
    const KeyValue Key::RAlt            = 0xB8; ///< ��alt��(104��������)  //90
    const KeyValue Key::M_Pause         = 0xC5; ///< ��ͣ��(��������)      //91
    const KeyValue Key::M_Home          = 0xC7; ///< Home��(��������)      //92
    const KeyValue Key::M_Up            = 0xC8; ///< UpArrow��(��������)   //93
    const KeyValue Key::M_PgUp          = 0xC9; ///< PgUp��(��������)      //94
    const KeyValue Key::M_Left          = 0xCB; ///< LeftArrow��(��������) //95
    const KeyValue Key::M_Right         = 0xCD; ///< RightArrow��(��������)//96
    const KeyValue Key::M_End           = 0xCF; ///< End��(��������)       //97
    const KeyValue Key::M_Down          = 0xD0; ///< DownArrow��(��������) //98
    const KeyValue Key::M_PgDown        = 0xD1; ///< PgDn��(��������)      //99
    const KeyValue Key::M_Insert        = 0xD2; ///< Insert��(��������)    //100
    const KeyValue Key::M_Delete        = 0xD3; ///< Delete��(��������)    //101
    const KeyValue Key::LWin            = 0xDB; ///< ��Win��(��������)     //102
    const KeyValue Key::RWin            = 0xDC; ///< ��Win��(��������)     //103
    const KeyValue Key::Compose         = 0xDD; ///< AppMenu��(��������)   //104
    const KeyValue Key::Max             = 0xDE;
}
}