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
#include "NiiKeyboardControlPattern.h"
#include "NiiInputManager.h"

namespace NII
{
namespace NII_MEDIA
{
    //------------------------------------------------------------------------
    const KeyValue Key::Unknow = 0;
    static KeyboardMethod _nii_KB_[124] = { 0 };
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // KeyboardArgs
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    KeyboardArgs::KeyboardArgs(KeyValue k, bool state) : mKey(k), mState(state) {}
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // KeyboardSerialItemArgs
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    KeyboardSerialItemArgs::KeyboardSerialItemArgs(KeyValue k) :
        mKey(k){}
    //------------------------------------------------------------------------
    KeyboardSerialItemArgs::~KeyboardSerialItemArgs(){}
    //------------------------------------------------------------------------
    bool KeyboardSerialItemArgs::operator == (const EventArgs * o) const
    {
        KeyValue src = static_cast<const KeyboardArgs *>(o)->mKey; 
        if(mKey == src)
            return true;
        return false;
    }
    //------------------------------------------------------------------------
    bool KeyboardSerialItemArgs::operator != (const EventArgs * o) const
    {
        KeyValue src = static_cast<const KeyboardArgs *>(o)->mKey; 
        if(mKey == src)
            return false;
        return true;        
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // KeyboardControlItem
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    KeyboardControlItem::KeyboardControlItem() :
        ControlItem(),
        mModifiers(0)
    {
    }
    //------------------------------------------------------------------------
    KeyboardControlItem::~KeyboardControlItem()
    {
    }
    //------------------------------------------------------------------------
    bool KeyboardControlItem::isEnable(KeyboardAlterLockType type)
    {
        return mModifiers & type;
    }
    //------------------------------------------------------------------------
    ControlDevType KeyboardControlItem::getType() const
    {
        return CDT_Keyboard;
    }
    //------------------------------------------------------------------------
    void KeyboardControlItem::init()
    {
        for (NCount i = 0; i < 124; ++i)
        {
            _nii_KB_[i] = &OnDummyMethod;
        }
        //press
        _nii_KB_[Key::Esc] = &KeyboardControlItem::onEsc;
        _nii_KB_[Key::NO1] = &KeyboardControlItem::on1;
        _nii_KB_[Key::NO2] = &KeyboardControlItem::on2;
        _nii_KB_[Key::NO3] = &KeyboardControlItem::on3;
        _nii_KB_[Key::NO4] = &KeyboardControlItem::on4;
        _nii_KB_[Key::NO5] = &KeyboardControlItem::on5;
        _nii_KB_[Key::NO6] = &KeyboardControlItem::on6;
        _nii_KB_[Key::NO7] = &KeyboardControlItem::on7;
        _nii_KB_[Key::NO8] = &KeyboardControlItem::on8;
        _nii_KB_[Key::NO9] = &KeyboardControlItem::on9;
        _nii_KB_[Key::NO0] = &KeyboardControlItem::on0;
        _nii_KB_[Key::Minus] = &KeyboardControlItem::onMinus;
        _nii_KB_[Key::Equals] = &KeyboardControlItem::onEqual;
        _nii_KB_[Key::BK] = &KeyboardControlItem::onBK;
        _nii_KB_[Key::TAB] = &KeyboardControlItem::onTAB;
        _nii_KB_[Key::Q] = &KeyboardControlItem::onQ;
        _nii_KB_[Key::W] = &KeyboardControlItem::onW;
        _nii_KB_[Key::E] = &KeyboardControlItem::onE;
        _nii_KB_[Key::R] = &KeyboardControlItem::onR;
        _nii_KB_[Key::T] = &KeyboardControlItem::onT;
        _nii_KB_[Key::Y] = &KeyboardControlItem::onY;
        _nii_KB_[Key::U] = &KeyboardControlItem::onU;
        _nii_KB_[Key::I] = &KeyboardControlItem::onI;
        _nii_KB_[Key::O] = &KeyboardControlItem::onO;
        _nii_KB_[Key::P] = &KeyboardControlItem::onP;
        _nii_KB_[Key::LBracket] = &KeyboardControlItem::onLBracket;
        _nii_KB_[Key::RBracket] = &KeyboardControlItem::onRBracket;
        _nii_KB_[Key::Enter] = &KeyboardControlItem::onEnter;
        _nii_KB_[Key::LCtrl] = &KeyboardControlItem::onLCtrl;
        _nii_KB_[Key::A] = &KeyboardControlItem::onA;
        _nii_KB_[Key::S] = &KeyboardControlItem::onS;
        _nii_KB_[Key::D] = &KeyboardControlItem::onD;
        _nii_KB_[Key::F] = &KeyboardControlItem::onF;
        _nii_KB_[Key::G] = &KeyboardControlItem::onG;
        _nii_KB_[Key::H] = &KeyboardControlItem::onH;
        _nii_KB_[Key::J] = &KeyboardControlItem::onJ;
        _nii_KB_[Key::K] = &KeyboardControlItem::onK;
        _nii_KB_[Key::L] = &KeyboardControlItem::onL;
        _nii_KB_[Key::Semicolon] = &KeyboardControlItem::onSemicolon;
        _nii_KB_[Key::Apostrophe] = &KeyboardControlItem::onApostrophe;
        _nii_KB_[Key::Grave] = &KeyboardControlItem::onGrave;
        _nii_KB_[Key::LShift] = &KeyboardControlItem::onLShift;
        _nii_KB_[Key::BackSlash] = &KeyboardControlItem::onBackSlash;
        _nii_KB_[Key::Z] = &KeyboardControlItem::onZ;
        _nii_KB_[Key::X] = &KeyboardControlItem::onX;
        _nii_KB_[Key::C] = &KeyboardControlItem::onC;
        _nii_KB_[Key::V] = &KeyboardControlItem::onV;
        _nii_KB_[Key::B] = &KeyboardControlItem::onB;
        _nii_KB_[Key::N] = &KeyboardControlItem::onN;
        _nii_KB_[Key::M] = &KeyboardControlItem::onM;
        _nii_KB_[Key::Comma] = &KeyboardControlItem::onComma;
        _nii_KB_[Key::Period] = &KeyboardControlItem::onPeriod;
        _nii_KB_[Key::Slash] = &KeyboardControlItem::onSlash;
        _nii_KB_[Key::RShift] = &KeyboardControlItem::onRShift;
        _nii_KB_[Key::N_Mul] = &KeyboardControlItem::onNumMul;
        _nii_KB_[Key::LAlt] = &KeyboardControlItem::onLAlt;
        _nii_KB_[Key::Space] = &KeyboardControlItem::onSpace;
        _nii_KB_[Key::CapsLock] = &KeyboardControlItem::onCapsLock;
        _nii_KB_[Key::F1] = &KeyboardControlItem::onF1;
        _nii_KB_[Key::F2] = &KeyboardControlItem::onF2;
        _nii_KB_[Key::F3] = &KeyboardControlItem::onF3;
        _nii_KB_[Key::F4] = &KeyboardControlItem::onF4;
        _nii_KB_[Key::F5] = &KeyboardControlItem::onF5;
        _nii_KB_[Key::F6] = &KeyboardControlItem::onF6;
        _nii_KB_[Key::F7] = &KeyboardControlItem::onF7;
        _nii_KB_[Key::F8] = &KeyboardControlItem::onF8;
        _nii_KB_[Key::F9] = &KeyboardControlItem::onF9;
        _nii_KB_[Key::F10] = &KeyboardControlItem::onF10;
        _nii_KB_[Key::N_Lock] = &KeyboardControlItem::onNumLock;
        _nii_KB_[Key::M_ScrLock] = &KeyboardControlItem::onMidScrLock;
        _nii_KB_[Key::N_NO7] = &KeyboardControlItem::onNum7;
        _nii_KB_[Key::N_NO8] = &KeyboardControlItem::onNum8;
        _nii_KB_[Key::N_NO9] = &KeyboardControlItem::onNum9;
        _nii_KB_[Key::N_Subtract] = &KeyboardControlItem::onNumSubtract;
        _nii_KB_[Key::N_NO4] = &KeyboardControlItem::onNum4;
        _nii_KB_[Key::N_NO5] = &KeyboardControlItem::onNum5;
        _nii_KB_[Key::N_NO6] = &KeyboardControlItem::onNum6;
        _nii_KB_[Key::N_Add] = &KeyboardControlItem::onNumAdd;
        _nii_KB_[Key::N_NO1] = &KeyboardControlItem::onNum1;
        _nii_KB_[Key::N_NO2] = &KeyboardControlItem::onNum2;
        _nii_KB_[Key::N_NO3] = &KeyboardControlItem::onNum3;
        _nii_KB_[Key::N_NO0] = &KeyboardControlItem::onNum0;
        _nii_KB_[Key::N_Dot] = &KeyboardControlItem::onNumDot;
        _nii_KB_[Key::F11] = &KeyboardControlItem::onF11;
        _nii_KB_[Key::F12] = &KeyboardControlItem::onF12;
        _nii_KB_[Key::N_Enter] = &KeyboardControlItem::onNumEnter;
        _nii_KB_[Key::RCtrl] = &KeyboardControlItem::onRCtrl;
        _nii_KB_[Key::RAlt] = &KeyboardControlItem::onRAlt;
        _nii_KB_[Key::LWin] = &KeyboardControlItem::onLWin;
        _nii_KB_[Key::RWin] = &KeyboardControlItem::onRWin;
        _nii_KB_[Key::Compose] = &KeyboardControlItem::onCompose;
        _nii_KB_[Key::N_Divide] = &KeyboardControlItem::onNumDivide;
        _nii_KB_[Key::M_SysRQ] = &KeyboardControlItem::onMidSysRQ;
        _nii_KB_[Key::M_Pause] = &KeyboardControlItem::onMidPause;
        _nii_KB_[Key::M_Home] = &KeyboardControlItem::onMidHome;
        _nii_KB_[Key::M_Up] = &KeyboardControlItem::onMidUp;
        _nii_KB_[Key::M_PgUp] = &KeyboardControlItem::onMidPgUp;
        _nii_KB_[Key::M_Left] = &KeyboardControlItem::onMidLeft;
        _nii_KB_[Key::M_Right] = &KeyboardControlItem::onMidRight;
        _nii_KB_[Key::M_End] = &KeyboardControlItem::onMidEnd;
        _nii_KB_[Key::M_Down] = &KeyboardControlItem::onMidDown;
        _nii_KB_[Key::M_PgDown] = &KeyboardControlItem::onMidPgDown;
        _nii_KB_[Key::M_Insert] = &KeyboardControlItem::onMidInsert;
        _nii_KB_[Key::M_Delete] = &KeyboardControlItem::onMidDelete;
    }
    //------------------------------------------------------------------------
    void KeyboardControlItem::onEsc(const KeyboardArgs * arg)
    {

    }
    //------------------------------------------------------------------------
    void KeyboardControlItem::on1(const KeyboardArgs * arg)
    {

    }
    //------------------------------------------------------------------------
    void KeyboardControlItem::on2(const KeyboardArgs * arg)
    {

    }
    //------------------------------------------------------------------------
    void KeyboardControlItem::on3(const KeyboardArgs * arg)
    {

    }
    //------------------------------------------------------------------------
    void KeyboardControlItem::on4(const KeyboardArgs * arg)
    {

    }
    //------------------------------------------------------------------------
    void KeyboardControlItem::on5(const KeyboardArgs * arg)
    {

    }
    //------------------------------------------------------------------------
    void KeyboardControlItem::on6(const KeyboardArgs * arg)
    {

    }
    //------------------------------------------------------------------------
    void KeyboardControlItem::on7(const KeyboardArgs * arg)
    {

    }
    //------------------------------------------------------------------------
    void KeyboardControlItem::on8(const KeyboardArgs * arg)
    {

    }
    //------------------------------------------------------------------------
    void KeyboardControlItem::on9(const KeyboardArgs * arg)
    {

    }
    //------------------------------------------------------------------------
    void KeyboardControlItem::on0(const KeyboardArgs * arg)
    {

    }
    //------------------------------------------------------------------------
    void KeyboardControlItem::onMinus(const KeyboardArgs * arg)
    {

    }
    //------------------------------------------------------------------------
    void KeyboardControlItem::onEqual(const KeyboardArgs * arg)
    {

    }
    //------------------------------------------------------------------------
    void KeyboardControlItem::onBK(const KeyboardArgs * arg)
    {

    }
    //------------------------------------------------------------------------
    void KeyboardControlItem::onTAB(const KeyboardArgs * arg)
    {

    }
    //------------------------------------------------------------------------
    void KeyboardControlItem::onQ(const KeyboardArgs * arg)
    {

    }
    //------------------------------------------------------------------------
    void KeyboardControlItem::onW(const KeyboardArgs * arg)
    {

    }
    //------------------------------------------------------------------------
    void KeyboardControlItem::onE(const KeyboardArgs * arg)
    {

    }
    //------------------------------------------------------------------------
    void KeyboardControlItem::onR(const KeyboardArgs * arg)
    {

    }
    //------------------------------------------------------------------------
    void KeyboardControlItem::onT(const KeyboardArgs * arg)
    {
    }
    //------------------------------------------------------------------------
    void KeyboardControlItem::onY(const KeyboardArgs * arg)
    {
    }
    //------------------------------------------------------------------------
    void KeyboardControlItem::onU(const KeyboardArgs * arg)
    {
    }
    //------------------------------------------------------------------------
    void KeyboardControlItem::onI(const KeyboardArgs * arg)
    {
    }
    //------------------------------------------------------------------------
    void KeyboardControlItem::onO(const KeyboardArgs * arg)
    {

    }
    //------------------------------------------------------------------------
    void KeyboardControlItem::onP(const KeyboardArgs * arg)
    {

    }
    //------------------------------------------------------------------------
    void KeyboardControlItem::onLBracket(const KeyboardArgs * arg)
    {

    }
    //------------------------------------------------------------------------
    void KeyboardControlItem::onRBracket(const KeyboardArgs * arg)
    {

    }
    //------------------------------------------------------------------------
    void KeyboardControlItem::onEnter(const KeyboardArgs * arg)
    {

    }
    //------------------------------------------------------------------------
    void KeyboardControlItem::onLCtrl(const KeyboardArgs * arg)
    {
        if(arg->mState)
            static_cast<KeyboardControlPattern*>(mInfluence)->on(KT_A_Ctrl);
        else
            static_cast<KeyboardControlPattern*>(mInfluence)->off(KT_A_Ctrl);
    }
    //------------------------------------------------------------------------
    void KeyboardControlItem::onA(const KeyboardArgs * arg)
    {

    }
    //------------------------------------------------------------------------
    void KeyboardControlItem::onS(const KeyboardArgs * arg)
    {

    }
    //------------------------------------------------------------------------
    void KeyboardControlItem::onD(const KeyboardArgs * arg)
    {

    }
    //------------------------------------------------------------------------
    void KeyboardControlItem::onF(const KeyboardArgs * arg)
    {

    }
    //------------------------------------------------------------------------
    void KeyboardControlItem::onG(const KeyboardArgs * arg)
    {

    }
    //------------------------------------------------------------------------
    void KeyboardControlItem::onH(const KeyboardArgs * arg)
    {

    }
    //------------------------------------------------------------------------
    void KeyboardControlItem::onJ(const KeyboardArgs * arg)
    {

    }
    //------------------------------------------------------------------------
    void KeyboardControlItem::onK(const KeyboardArgs * arg)
    {

    }
    //------------------------------------------------------------------------
    void KeyboardControlItem::onL(const KeyboardArgs * arg)
    {

    }
    //------------------------------------------------------------------------
    void KeyboardControlItem::onSemicolon(const KeyboardArgs * arg)
    {

    }
    //------------------------------------------------------------------------
    void KeyboardControlItem::onApostrophe(const KeyboardArgs * arg)
    {

    }
    //------------------------------------------------------------------------
    void KeyboardControlItem::onGrave(const KeyboardArgs * arg)
    {

    }
    //------------------------------------------------------------------------
    void KeyboardControlItem::onLShift(const KeyboardArgs * arg)
    {
        if(arg->mState)
            static_cast<KeyboardControlPattern*>(mInfluence)->on(KT_A_Shift);
        else
            static_cast<KeyboardControlPattern*>(mInfluence)->off(KT_A_Shift);
    }
    //------------------------------------------------------------------------
    void KeyboardControlItem::onBackSlash(const KeyboardArgs * arg)
    {

    }
    //------------------------------------------------------------------------
    void KeyboardControlItem::onZ(const KeyboardArgs * arg)
    {

    }
    //------------------------------------------------------------------------
    void KeyboardControlItem::onX(const KeyboardArgs * arg)
    {

    }
    //------------------------------------------------------------------------
    void KeyboardControlItem::onC(const KeyboardArgs * arg)
    {

    }
    //------------------------------------------------------------------------
    void KeyboardControlItem::onV(const KeyboardArgs * arg)
    {

    }
    //------------------------------------------------------------------------
    void KeyboardControlItem::onB(const KeyboardArgs * arg)
    {

    }
    //------------------------------------------------------------------------
    void KeyboardControlItem::onN(const KeyboardArgs * arg)
    {

    }
    //------------------------------------------------------------------------
    void KeyboardControlItem::onM(const KeyboardArgs * arg)
    {

    }
    //------------------------------------------------------------------------
    void KeyboardControlItem::onComma(const KeyboardArgs * arg)
    {

    }
    //------------------------------------------------------------------------
    void KeyboardControlItem::onPeriod(const KeyboardArgs * arg)
    {

    }
    //------------------------------------------------------------------------
    void KeyboardControlItem::onSlash(const KeyboardArgs * arg)
    {

    }
    //------------------------------------------------------------------------
    void KeyboardControlItem::onRShift(const KeyboardArgs * arg)
    {
        if(arg->mState)
            static_cast<KeyboardControlPattern*>(mInfluence)->on(KT_A_Shift);
        else
            static_cast<KeyboardControlPattern*>(mInfluence)->off(KT_A_Shift);
    }
    //------------------------------------------------------------------------
    void KeyboardControlItem::onNumMul(const KeyboardArgs * arg)
    {

    }
    //------------------------------------------------------------------------
    void KeyboardControlItem::onLAlt(const KeyboardArgs * arg)
    {
        if(arg->mState)
            static_cast<KeyboardControlPattern*>(mInfluence)->on(KT_A_Alt);
        else
            static_cast<KeyboardControlPattern*>(mInfluence)->off(KT_A_Alt);
    }
    //------------------------------------------------------------------------
    void KeyboardControlItem::onSpace(const KeyboardArgs * arg)
    {

    }
    //------------------------------------------------------------------------
    void KeyboardControlItem::onCapsLock(const KeyboardArgs * arg)
    {
        static_cast<KeyboardControlPattern*>(mInfluence)->syn(KT_L_CapsLock);
    }
    //------------------------------------------------------------------------
    void KeyboardControlItem::onF1(const KeyboardArgs * arg)
    {

    }
    //------------------------------------------------------------------------
    void KeyboardControlItem::onF2(const KeyboardArgs * arg)
    {

    }
    //------------------------------------------------------------------------
    void KeyboardControlItem::onF3(const KeyboardArgs * arg)
    {

    }
    //------------------------------------------------------------------------
    void KeyboardControlItem::onF4(const KeyboardArgs * arg)
    {

    }
    //------------------------------------------------------------------------
    void KeyboardControlItem::onF5(const KeyboardArgs * arg)
    {

    }
    //------------------------------------------------------------------------
    void KeyboardControlItem::onF6(const KeyboardArgs * arg)
    {

    }
    //------------------------------------------------------------------------
    void KeyboardControlItem::onF7(const KeyboardArgs * arg)
    {

    }
    //------------------------------------------------------------------------
    void KeyboardControlItem::onF8(const KeyboardArgs * arg)
    {

    }
    //------------------------------------------------------------------------
    void KeyboardControlItem::onF9(const KeyboardArgs * arg)
    {

    }
    //------------------------------------------------------------------------
    void KeyboardControlItem::onF10(const KeyboardArgs * arg)
    {

    }
    //------------------------------------------------------------------------
    void KeyboardControlItem::onNumLock(const KeyboardArgs * arg)
    {
        if(!arg->mState)
            static_cast<KeyboardControlPattern*>(mInfluence)->syn(KT_L_NumLock);
    }
    //------------------------------------------------------------------------
    void KeyboardControlItem::onMidScrLock(const KeyboardArgs * arg)
    {
        if(arg->mState)
            static_cast<KeyboardControlPattern*>(mInfluence)->syn(KT_L_ScrLock);
    }
    //------------------------------------------------------------------------
    void KeyboardControlItem::onNum7(const KeyboardArgs * arg)
    {

    }
    //------------------------------------------------------------------------
    void KeyboardControlItem::onNum8(const KeyboardArgs * arg)
    {

    }
    //------------------------------------------------------------------------
    void KeyboardControlItem::onNum9(const KeyboardArgs * arg)
    {
    }
    //------------------------------------------------------------------------
    void KeyboardControlItem::onNumSubtract(const KeyboardArgs * arg)
    {

    }
    //------------------------------------------------------------------------
    void KeyboardControlItem::onNum4(const KeyboardArgs * arg)
    {

    }
    //------------------------------------------------------------------------
    void KeyboardControlItem::onNum5(const KeyboardArgs * arg)
    {

    }
    //------------------------------------------------------------------------
    void KeyboardControlItem::onNum6(const KeyboardArgs * arg)
    {

    }
    //------------------------------------------------------------------------
    void KeyboardControlItem::onNumAdd(const KeyboardArgs * arg)
    {

    }
    //------------------------------------------------------------------------
    void KeyboardControlItem::onNum1(const KeyboardArgs * arg)
    {

    }
    //------------------------------------------------------------------------
    void KeyboardControlItem::onNum2(const KeyboardArgs * arg)
    {

    }
    //------------------------------------------------------------------------
    void KeyboardControlItem::onNum3(const KeyboardArgs * arg)
    {

    }
    //------------------------------------------------------------------------
    void KeyboardControlItem::onNum0(const KeyboardArgs * arg)
    {

    }
    //------------------------------------------------------------------------
    void KeyboardControlItem::onNumDot(const KeyboardArgs * arg)
    {

    }
    //------------------------------------------------------------------------
    void KeyboardControlItem::onF11(const KeyboardArgs * arg)
    {

    }
    //------------------------------------------------------------------------
    void KeyboardControlItem::onF12(const KeyboardArgs * arg)
    {

    }
    //------------------------------------------------------------------------
    void KeyboardControlItem::onNumEnter(const KeyboardArgs * arg)
    {

    }
    //------------------------------------------------------------------------
    void KeyboardControlItem::onRCtrl(const KeyboardArgs * arg)
    {
        if(arg->mState)
            static_cast<KeyboardControlPattern*>(mInfluence)->on(KT_A_Ctrl);
        else
            static_cast<KeyboardControlPattern*>(mInfluence)->off(KT_A_Ctrl);
    }
    //------------------------------------------------------------------------
    void KeyboardControlItem::onRAlt(const KeyboardArgs * arg)
    {
        if(arg->mState)
            static_cast<KeyboardControlPattern*>(mInfluence)->on(KT_A_Alt);
        else
            static_cast<KeyboardControlPattern*>(mInfluence)->off(KT_A_Alt);
    }
    //------------------------------------------------------------------------
    void KeyboardControlItem::onLWin(const KeyboardArgs * arg)
    {
        if(arg->mState)
            static_cast<KeyboardControlPattern*>(mInfluence)->on(KT_A_Meta);
        else
            static_cast<KeyboardControlPattern*>(mInfluence)->off(KT_A_Meta);
    }
    //------------------------------------------------------------------------
    void KeyboardControlItem::onRWin(const KeyboardArgs * arg)
    {
        if(arg->mState)
            static_cast<KeyboardControlPattern*>(mInfluence)->on(KT_A_Meta);
        else
            static_cast<KeyboardControlPattern*>(mInfluence)->off(KT_A_Meta);
    }
    //------------------------------------------------------------------------
    void KeyboardControlItem::onCompose(const KeyboardArgs * arg)
    {

    }
    //------------------------------------------------------------------------
    void KeyboardControlItem::onNumDivide(const KeyboardArgs * arg)
    {

    }
    //------------------------------------------------------------------------
    void KeyboardControlItem::onMidSysRQ(const KeyboardArgs * arg)
    {

    }
    //------------------------------------------------------------------------
    void KeyboardControlItem::onMidPause(const KeyboardArgs * arg)
    {

    }
    //------------------------------------------------------------------------
    void KeyboardControlItem::onMidHome(const KeyboardArgs * arg)
    {

    }
    //------------------------------------------------------------------------
    void KeyboardControlItem::onMidUp(const KeyboardArgs * arg)
    {

    }
    //------------------------------------------------------------------------
    void KeyboardControlItem::onMidPgUp(const KeyboardArgs * arg)
    {

    }
    //------------------------------------------------------------------------
    void KeyboardControlItem::onMidLeft(const KeyboardArgs * arg)
    {

    }
    //------------------------------------------------------------------------
    void KeyboardControlItem::onMidRight(const KeyboardArgs * arg)
    {

    }
    //------------------------------------------------------------------------
    void KeyboardControlItem::onMidEnd(const KeyboardArgs * arg)
    {

    }
    //------------------------------------------------------------------------
    void KeyboardControlItem::onMidDown(const KeyboardArgs * arg)
    {

    }
    //------------------------------------------------------------------------
    void KeyboardControlItem::onMidPgDown(const KeyboardArgs * arg)
    {

    }
    //------------------------------------------------------------------------
    void KeyboardControlItem::onMidInsert(const KeyboardArgs * arg)
    {

    }
    //------------------------------------------------------------------------
    void KeyboardControlItem::onMidDelete(const KeyboardArgs * arg)
    {

    }
    //------------------------------------------------------------------------
    void KeyboardControlItem::onPress(KeyValue key)
    {
        KeyboardArgs args(key, true);
        raise(&args);
        // 引发普通按键事件
        (this->*_nii_KB_[key])(&args);
    }
    //------------------------------------------------------------------------
    void KeyboardControlItem::onRelease(KeyValue key)
    {
        KeyboardArgs args(key, false);
        // 引发普通按键事件
        (this->*_nii_KB_[key])(&args);
    }
    //------------------------------------------------------------------------
    void KeyboardControlItem::OnDummyMethod(const KeyboardArgs *)
    {
        // do nothing
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // ListenerKeyboardControlItem
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    ListenerKeyboardControlItem::ListenerKeyboardControlItem() :
        mChar(0),
        mWChar(0)
    {

    }
    //------------------------------------------------------------------------
    ListenerKeyboardControlItem::~ListenerKeyboardControlItem()
    {

    }
    //------------------------------------------------------------------------
    void ListenerKeyboardControlItem::onEsc(const KeyboardArgs * arg)
    {
        mChar = 27;
        mWChar = 0x1b;
        mDescrip = _I18n("Esc key");
    }
    //------------------------------------------------------------------------
    void ListenerKeyboardControlItem::on1(const KeyboardArgs * arg)
    {
        if (mModifiers & KT_A_Shift)
        {
            mChar = 33;
            mWChar = 0x21;
            mDescrip = _I18n("! key");
        }
        else
        {
            mChar = 49;
            mWChar = 0x31;
            mDescrip = _I18n("1 key in mainpad");
        }
    }
    //------------------------------------------------------------------------
    void ListenerKeyboardControlItem::on2(const KeyboardArgs * arg)
    {
        if (mModifiers & KT_A_Shift)
        {
            mChar = 64;
            mWChar = 0x40;
            mDescrip = _I18n("@ key");
        }
        else
        {
            mChar = 50;
            mWChar = 0x32;
            mDescrip = _I18n("2 key in mainpad");
        }
    }
    //------------------------------------------------------------------------
    void ListenerKeyboardControlItem::on3(const KeyboardArgs * arg)
    {
        if (mModifiers & KT_A_Shift)
        {
            mChar = 35;
            mWChar = 0x23;
            mDescrip = _I18n("# key");
        }
        else
        {
            mChar = 51;
            mWChar = 0x33;
            mDescrip = _I18n("3 key in mianpad");
        }
    }
    //------------------------------------------------------------------------
    void ListenerKeyboardControlItem::on4(const KeyboardArgs * arg)
    {
        if (mModifiers & KT_A_Shift)
        {
            mChar = 36;
            mWChar = 0x24;
            mDescrip = _I18n("$ key");
        }
        else
        {
            mChar = 52;
            mWChar = 0x34;
            mDescrip = _I18n("4 key in mianpad");
        }
    }
    //------------------------------------------------------------------------
    void ListenerKeyboardControlItem::on5(const KeyboardArgs * arg)
    {
        if (mModifiers & KT_A_Shift)
        {
            mChar = 37;
            mWChar = 0x25;
            mDescrip = _I18n("% key");
        }
        else
        {
            mChar = 53;
            mWChar = 0x35;
            mDescrip = _I18n("5 key in mainpad");
        }
    }
    //------------------------------------------------------------------------
    void ListenerKeyboardControlItem::on6(const KeyboardArgs * arg)
    {
        if (mModifiers & KT_A_Shift)
        {
            mChar = 94;
            mWChar = 0x51;
            mDescrip = _I18n("^ key");
        }
        else
        {
            mChar = 54;
            mWChar = 0x36;
            mDescrip = _I18n("6 key in mainpad");
        }
    }
    //------------------------------------------------------------------------
    void ListenerKeyboardControlItem::on7(const KeyboardArgs * arg)
    {
        if (mModifiers & KT_A_Shift)
        {
            mChar = 38;
            mWChar = 0x26;
            mDescrip = _I18n("& key");
        }
        else
        {
            mChar = 55;
            mWChar = 0x37;
            mDescrip = _I18n("7 key in mainpad");
        }
    }
    //------------------------------------------------------------------------
    void ListenerKeyboardControlItem::on8(const KeyboardArgs * arg)
    {
        if (mModifiers & KT_A_Shift)
        {
            mChar = 42;
            mWChar = 0x2a;
            mDescrip = _I18n("* key");
        }
        else
        {
            mChar = 56;
            mWChar = 0x38;
            mDescrip = _I18n("8 key in mainpad");
        }
    }
    //------------------------------------------------------------------------
    void ListenerKeyboardControlItem::on9(const KeyboardArgs * arg)
    {
        if (mModifiers & KT_A_Shift)
        {
            mChar = 40;
            mWChar = 0x28;
            mDescrip = _I18n("( key");
        }
        else
        {
            mChar = 57;
            mWChar = 0x39;
            mDescrip = _I18n("9 key in mainpad");
        }
    }
    //------------------------------------------------------------------------
    void ListenerKeyboardControlItem::on0(const KeyboardArgs * arg)
    {
        if (mModifiers & KT_A_Shift)
        {
            mChar = 41;
            mWChar = 0x29;
            mDescrip = _I18n(") key");
        }
        else
        {
            mChar = 48;
            mWChar = 0x30;
            mDescrip = _I18n("0 key in mainpad");
        }
    }
    //------------------------------------------------------------------------
    void ListenerKeyboardControlItem::onMinus(const KeyboardArgs * arg)
    {
        if (mModifiers & KT_A_Shift)
        {
            mChar = 95;
            mWChar = 0x5f;
            mDescrip = _I18n("_ key");
        }
        else
        {
            mChar = 45;
            mWChar = 0x2d;
            mDescrip = _I18n("- key");
        }
    }
    //------------------------------------------------------------------------
    void ListenerKeyboardControlItem::onEqual(const KeyboardArgs * arg)
    {
        if (mModifiers & KT_A_Shift)
        {
            mChar = 43;
            mWChar = 0x2b;
            mDescrip = _I18n("+ key");
        }
        else
        {
            mChar = 61;
            mWChar = 0x3d;
            mDescrip = _I18n("= key");
        }
    }
    //------------------------------------------------------------------------
    void ListenerKeyboardControlItem::onBK(const KeyboardArgs * arg)
    {
        mChar = 8;
        mWChar = 0x08;
        mDescrip = _I18n("backspace key");
    }
    //------------------------------------------------------------------------
    void ListenerKeyboardControlItem::onTAB(const KeyboardArgs * arg)
    {
        mChar = 9;
        mWChar = 0x09;
        mDescrip = _I18n("tab key");
    }
    //------------------------------------------------------------------------
    void ListenerKeyboardControlItem::onQ(const KeyboardArgs * arg)
    {
        if ((mModifiers & KT_A_Shift & ~KT_L_CapsLock) ||
            (mModifiers & ~KT_A_Shift & KT_L_CapsLock))
        {
            mChar = 81;
            mWChar = 0x51;
            mDescrip = _I18n("Q key");
        }
        else
        {
            mChar = 113;
            mWChar = 0x71;
            mDescrip = _I18n("q key");
        }
    }
    //------------------------------------------------------------------------
    void ListenerKeyboardControlItem::onW(const KeyboardArgs * arg)
    {
        if ((mModifiers & KT_A_Shift & ~KT_L_CapsLock) ||
            (mModifiers & ~KT_A_Shift & KT_L_CapsLock))
        {
            mChar = 87;
            mWChar = 0x57;
            mDescrip = _I18n("W key");
        }
        else
        {
            mChar = 119;
            mWChar = 0x77;
            mDescrip = _I18n("w key");
        }
    }
    //------------------------------------------------------------------------
    void ListenerKeyboardControlItem::onE(const KeyboardArgs * arg)
    {
        if ((mModifiers & KT_A_Shift & ~KT_L_CapsLock) ||
            (mModifiers & ~KT_A_Shift & KT_L_CapsLock))
        {
            mChar = 69;
            mWChar = 0x45;
            mDescrip = _I18n("E key");
        }
        else
        {
            mChar = 101;
            mWChar = 0x65;
            mDescrip = _I18n("e key");
        }
    }
    //------------------------------------------------------------------------
    void ListenerKeyboardControlItem::onR(const KeyboardArgs * arg)
    {
        if ((mModifiers & KT_A_Shift & ~KT_L_CapsLock) ||
            (mModifiers & ~KT_A_Shift & KT_L_CapsLock))
        {
            mChar = 82;
            mWChar = 0x52;
            mDescrip = _I18n("R key");
        }
        else
        {
            mChar = 114;
            mWChar = 0x72;
            mDescrip = _I18n("r key");
        }
    }
    //------------------------------------------------------------------------
    void ListenerKeyboardControlItem::onT(const KeyboardArgs * arg)
    {
        if ((mModifiers & KT_A_Shift & ~KT_L_CapsLock) ||
            (mModifiers & ~KT_A_Shift & KT_L_CapsLock))
        {
            mChar = 84;
            mWChar = 0x54;
            mDescrip = _I18n("T key");
        }
        else
        {
            mChar = 116;
            mWChar = 0x74;
            mDescrip = _I18n("t key");
        }
    }
    //------------------------------------------------------------------------
    void ListenerKeyboardControlItem::onY(const KeyboardArgs * arg)
    {
        if ((mModifiers & KT_A_Shift & ~KT_L_CapsLock) ||
            (mModifiers & ~KT_A_Shift & KT_L_CapsLock))
        {
            mChar = 89;
            mWChar = 0x59;
            mDescrip = _I18n("Y key");
        }
        else
        {
            mChar = 121;
            mWChar = 0x79;
            mDescrip = _I18n("y key");
        }
    }
    //------------------------------------------------------------------------
    void ListenerKeyboardControlItem::onU(const KeyboardArgs * arg)
    {
        if ((mModifiers & KT_A_Shift & ~KT_L_CapsLock) ||
            (mModifiers & ~KT_A_Shift & KT_L_CapsLock))
        {
            mChar = 85;
            mWChar = 0x55;
            mDescrip = _I18n("U key");
        }
        else
        {
            mChar = 117;
            mWChar = 0x75;
            mDescrip = _I18n("u key");
        }
    }
    //------------------------------------------------------------------------
    void ListenerKeyboardControlItem::onI(const KeyboardArgs * arg)
    {
        if ((mModifiers & KT_A_Shift & ~KT_L_CapsLock) ||
            (mModifiers & ~KT_A_Shift & KT_L_CapsLock))
        {
            mChar = 73;
            mWChar = 0x49;
            mDescrip = _I18n("I key");
        }
        else
        {
            mChar = 105;
            mWChar = 0x69;
            mDescrip = _I18n("i key");
        }
    }
    //------------------------------------------------------------------------
    void ListenerKeyboardControlItem::onO(const KeyboardArgs * arg)
    {
        if ((mModifiers & KT_A_Shift & ~KT_L_CapsLock) ||
            (mModifiers & ~KT_A_Shift & KT_L_CapsLock))
        {
            mChar = 79;
            mWChar = 0x4f;
            mDescrip = _I18n("O key");
        }
        else
        {
            mChar = 111;
            mWChar = 0x6f;
            mDescrip = _I18n("o key");
        }
    }
    //------------------------------------------------------------------------
    void ListenerKeyboardControlItem::onP(const KeyboardArgs * arg)
    {
        if ((mModifiers & KT_A_Shift & ~KT_L_CapsLock) ||
            (mModifiers & ~KT_A_Shift & KT_L_CapsLock))
        {
            mChar = 80;
            mWChar = 0x50;
            mDescrip = _I18n("P key");
        }
        else
        {
            mChar = 112;
            mWChar = 0x70;
            mDescrip = _I18n("p key");
        }
    }
    //------------------------------------------------------------------------
    void ListenerKeyboardControlItem::onLBracket(const KeyboardArgs * arg)
    {
        if (mModifiers & KT_A_Shift)
        {
            mChar = 123;
            mWChar = 0x7b;
            mDescrip = _I18n("{ key");
        }
        else
        {
            mChar = 91;
            mWChar = 0x5b;
            mDescrip = _I18n("[ key");
        }
    }
    //------------------------------------------------------------------------
    void ListenerKeyboardControlItem::onRBracket(const KeyboardArgs * arg)
    {
        if (mModifiers & KT_A_Shift)
        {
            mChar = 125;
            mWChar = 0x7d;
            mDescrip = _I18n("} key");
        }
        else
        {
            mChar = 93;
            mWChar = 0x5d;
            mDescrip = _I18n("] key");
        }
    }
    //------------------------------------------------------------------------
    void ListenerKeyboardControlItem::onEnter(const KeyboardArgs * arg)
    {
        mChar = 13;
        mWChar = 0x0d;
        mDescrip = _I18n("enter key");
    }
    //------------------------------------------------------------------------
    void ListenerKeyboardControlItem::onLCtrl(const KeyboardArgs * arg)
    {
        mModifiers |= KT_A_Ctrl;
        mChar = 0;
        mWChar = 0;
        mDescrip = _I18n("left ctrl key");
    }
    //------------------------------------------------------------------------
    void ListenerKeyboardControlItem::onA(const KeyboardArgs * arg)
    {
        if ((mModifiers & KT_A_Shift & ~KT_L_CapsLock) ||
            (mModifiers & ~KT_A_Shift & KT_L_CapsLock))
        {
            mChar = 65;
            mWChar = 0x41;
            mDescrip = _I18n("A key");
        }
        else
        {
            mChar = 97;
            mWChar = 0x61;
            mDescrip = _I18n("a key");
        }
    }
    //------------------------------------------------------------------------
    void ListenerKeyboardControlItem::onS(const KeyboardArgs * arg)
    {
        if ((mModifiers & KT_A_Shift & ~KT_L_CapsLock) ||
            (mModifiers & ~KT_A_Shift & KT_L_CapsLock))
        {
            mChar = 83;
            mWChar = 0x53;
            mDescrip = _I18n("S key");
        }
        else
        {
            mChar = 115;
            mWChar = 0x73;
            mDescrip = _I18n("s key");
        }
    }
    //------------------------------------------------------------------------
    void ListenerKeyboardControlItem::onD(const KeyboardArgs * arg)
    {
        if ((mModifiers & KT_A_Shift & ~KT_L_CapsLock) ||
            (mModifiers & ~KT_A_Shift & KT_L_CapsLock))
        {
            mChar = 68;
            mWChar = 0x44;
            mDescrip = _I18n("D key");
        }
        else
        {
            mChar = 100;
            mWChar = 0x64;
            mDescrip = _I18n("d key");
        }
    }
    //------------------------------------------------------------------------
    void ListenerKeyboardControlItem::onF(const KeyboardArgs * arg)
    {
        if ((mModifiers & KT_A_Shift & ~KT_L_CapsLock) ||
            (mModifiers & ~KT_A_Shift & KT_L_CapsLock))
        {
            mChar = 70;
            mWChar = 0x46;
            mDescrip = _I18n("F key");
        }
        else
        {
            mChar = 102;
            mWChar = 0x66;
            mDescrip = _I18n("f key");
        }
    }
    //------------------------------------------------------------------------
    void ListenerKeyboardControlItem::onG(const KeyboardArgs * arg)
    {
        if ((mModifiers & KT_A_Shift & ~KT_L_CapsLock) ||
            (mModifiers & ~KT_A_Shift & KT_L_CapsLock))
        {
            mChar = 71;
            mWChar = 0x47;
            mDescrip = _I18n("G key");
        }
        else
        {
            mChar = 103;
            mWChar = 0x67;
            mDescrip = _I18n("g key");
        }
    }
    //------------------------------------------------------------------------
    void ListenerKeyboardControlItem::onH(const KeyboardArgs * arg)
    {
        if ((mModifiers & KT_A_Shift & ~KT_L_CapsLock) ||
            (mModifiers & ~KT_A_Shift & KT_L_CapsLock))
        {
            mChar = 72;
            mWChar = 0x48;
            mDescrip = _I18n("H key");
        }
        else
        {
            mChar = 104;
            mWChar = 0x68;
            mDescrip = _I18n("h key");
        }
    }
    //------------------------------------------------------------------------
    void ListenerKeyboardControlItem::onJ(const KeyboardArgs * arg)
    {
        if ((mModifiers & KT_A_Shift & ~KT_L_CapsLock) ||
            (mModifiers & ~KT_A_Shift & KT_L_CapsLock))
        {
            mChar = 74;
            mWChar = 0x4a;
            mDescrip = _I18n("J key");
        }
        else
        {
            mChar = 106;
            mWChar = 0x6a;
            mDescrip = _I18n("j key");
        }
    }
    //------------------------------------------------------------------------
    void ListenerKeyboardControlItem::onK(const KeyboardArgs * arg)
    {
        if ((mModifiers & KT_A_Shift & ~KT_L_CapsLock) ||
            (mModifiers & ~KT_A_Shift & KT_L_CapsLock))
        {
            mChar = 75;
            mWChar = 0x4b;
            mDescrip = _I18n("K key");
        }
        else
        {
            mChar = 107;
            mWChar = 0x6b;
            mDescrip = _I18n("k key");
        }
    }
    //------------------------------------------------------------------------
    void ListenerKeyboardControlItem::onL(const KeyboardArgs * arg)
    {
        if ((mModifiers & KT_A_Shift & ~KT_L_CapsLock) ||
            (mModifiers & ~KT_A_Shift & KT_L_CapsLock))
        {
            mChar = 76;
            mWChar = 0x4c;
            mDescrip = _I18n("L key");
        }
        else
        {
            mChar = 108;
            mWChar = 0x6c;
            mDescrip = _I18n("l key");
        }
    }
    //------------------------------------------------------------------------
    void ListenerKeyboardControlItem::onSemicolon(const KeyboardArgs * arg)
    {
        if (mModifiers & KT_A_Shift)
        {
            mChar = 58;
            mWChar = 0x3a;
            mDescrip = _I18n(": key");
        }
        else
        {
            mChar = 59;
            mWChar = 0x3b;
            mDescrip = _I18n("; key");
        }
    }
    //------------------------------------------------------------------------
    void ListenerKeyboardControlItem::onApostrophe(const KeyboardArgs * arg)
    {
        if (mModifiers & KT_A_Shift)
        {
            mChar = 34;
            mWChar = 0x22;
            mDescrip = _I18n("\" key");
        }
        else
        {
            mChar = 39;
            mWChar = 0x27;
            mDescrip = _I18n("' key");
        }
    }
    //------------------------------------------------------------------------
    void ListenerKeyboardControlItem::onGrave(const KeyboardArgs * arg)
    {
        if (mModifiers & KT_A_Shift)
        {
            mChar = 126;
            mWChar = 0x7e;
            mDescrip = _I18n("~ key");
        }
        else
        {
            mChar = 96;
            mWChar = 0x60;
            mDescrip = _I18n("` key");
        }
    }
    //------------------------------------------------------------------------
    void ListenerKeyboardControlItem::onLShift(const KeyboardArgs * arg)
    {
        mModifiers |= KT_A_Shift;
        mChar = 0;
        mWChar = 0;
        mDescrip = _I18n("left shift key");
    }
    //------------------------------------------------------------------------
    void ListenerKeyboardControlItem::onBackSlash(const KeyboardArgs * arg)
    {
        if (mModifiers & KT_A_Shift)
        {
            mChar = 124;
            mWChar = 0x7c;
            mDescrip = _I18n("| key");
        }
        else
        {
            mChar = 92;
            mWChar = 0x5c;
            mDescrip = _I18n("\\ key");

        }
    }
    //------------------------------------------------------------------------
    void ListenerKeyboardControlItem::onZ(const KeyboardArgs * arg)
    {
        if ((mModifiers & KT_A_Shift & ~KT_L_CapsLock) ||
            (mModifiers & ~KT_A_Shift & KT_L_CapsLock))
        {
            mChar = 90;
            mWChar = 0x5a;
            mDescrip = _I18n("Z key");
        }
        else
        {
            mChar = 122;
            mWChar = 0x7a;
            mDescrip = _I18n("z key");
        }
    }
    //------------------------------------------------------------------------
    void ListenerKeyboardControlItem::onX(const KeyboardArgs * arg)
    {
        if ((mModifiers & KT_A_Shift & ~KT_L_CapsLock) ||
            (mModifiers & ~KT_A_Shift & KT_L_CapsLock))
        {
            mChar = 88;
            mWChar = 0x58;
            mDescrip = _I18n("X key");
        }
        else
        {
            mChar = 120;
            mWChar = 0x78;
            mDescrip = _I18n("x key");
        }
    }
    //------------------------------------------------------------------------
    void ListenerKeyboardControlItem::onC(const KeyboardArgs * arg)
    {
        if ((mModifiers & KT_A_Shift & ~KT_L_CapsLock) ||
            (mModifiers & ~KT_A_Shift & KT_L_CapsLock))
        {
            mChar = 67;
            mWChar = 0x43;
            mDescrip = _I18n("C key");
        }
        else
        {
            mChar = 99;
            mWChar = 0x63;
            mDescrip = _I18n("c key");
        }
    }
    //------------------------------------------------------------------------
    void ListenerKeyboardControlItem::onV(const KeyboardArgs * arg)
    {
        if ((mModifiers & KT_A_Shift & ~KT_L_CapsLock) ||
            (mModifiers & ~KT_A_Shift & KT_L_CapsLock))
        {
            mChar = 86;
            mWChar = 0x56;
            mDescrip = _I18n("V key");
        }
        else
        {
            mChar = 118;
            mWChar = 0x76;
            mDescrip = _I18n("v key");
        }
    }
    //------------------------------------------------------------------------
    void ListenerKeyboardControlItem::onB(const KeyboardArgs * arg)
    {
        if ((mModifiers & KT_A_Shift & ~KT_L_CapsLock) ||
            (mModifiers & ~KT_A_Shift & KT_L_CapsLock))
        {
            mChar = 66;
            mWChar = 0x42;
            mDescrip = _I18n("B key");
        }
        else
        {
            mChar = 98;
            mWChar = 0x62;
            mDescrip = _I18n("b key");
        }
    }
    //------------------------------------------------------------------------
    void ListenerKeyboardControlItem::onN(const KeyboardArgs * arg)
    {
        if ((mModifiers & KT_A_Shift & ~KT_L_CapsLock) ||
            (mModifiers & ~KT_A_Shift & KT_L_CapsLock))
        {
            mChar = 78;
            mWChar = 0x4e;
            mDescrip = _I18n("N key");
        }
        else
        {
            mChar = 110;
            mWChar = 0x6e;
            mDescrip = _I18n("n key");
        }
    }
    //------------------------------------------------------------------------
    void ListenerKeyboardControlItem::onM(const KeyboardArgs * arg)
    {
        if ((mModifiers & KT_A_Shift & ~KT_L_CapsLock) ||
            (mModifiers & ~KT_A_Shift & KT_L_CapsLock))
        {
            mChar = 77;
            mWChar = 0x4d;
            mDescrip = _I18n("M key");
        }
        else
        {
            mChar = 109;
            mWChar = 0x6d;
            mDescrip = _I18n("m key");
        }
    }
    //------------------------------------------------------------------------
    void ListenerKeyboardControlItem::onComma(const KeyboardArgs * arg)
    {
        if (mModifiers & KT_A_Shift)
        {
            mChar = 60;
            mWChar = 0x3c;
            mDescrip = _I18n("< key");
        }
        else
        {
            mChar = 44;
            mWChar = 0x2c;
            mDescrip = _I18n(", key");

        }
    }
    //------------------------------------------------------------------------
    void ListenerKeyboardControlItem::onPeriod(const KeyboardArgs * arg)
    {
        if (mModifiers & KT_A_Shift)
        {
            mChar = 62;
            mWChar = 0x3e;
            mDescrip = _I18n("> key");
        }
        else
        {
            mChar = 46;
            mWChar = 0x2e;
            mDescrip = _I18n(". key");

        }
    }
    //------------------------------------------------------------------------
    void ListenerKeyboardControlItem::onSlash(const KeyboardArgs * arg)
    {
        if (mModifiers & KT_A_Shift)
        {
            mChar = 63;
            mWChar = 0x3f;
            mDescrip = _I18n("? key");
        }
        else
        {
            mChar = 47;
            mWChar = 0x2f;
            mDescrip = _I18n("/ key");

        }
    }
    //------------------------------------------------------------------------
    void ListenerKeyboardControlItem::onRShift(const KeyboardArgs * arg)
    {
        mModifiers |= KT_A_Shift;
        mChar = 0;
        mWChar = 0;
        mDescrip = _I18n("right shift key");
    }
    //------------------------------------------------------------------------
    void ListenerKeyboardControlItem::onNumMul(const KeyboardArgs * arg)
    {
        mChar = 42;
        mWChar = 0x2a;
        mDescrip = _I18n("* key in numpad");
    }
    //------------------------------------------------------------------------
    void ListenerKeyboardControlItem::onLAlt(const KeyboardArgs * arg)
    {
        mModifiers |= KT_A_Alt;
        mChar = 0;
        mWChar = 0;
        mDescrip = _I18n("left alt key");
    }
    //------------------------------------------------------------------------
    void ListenerKeyboardControlItem::onSpace(const KeyboardArgs * arg)
    {
        mChar = 32;
        mWChar = 0x20;
        mDescrip = _I18n("space key");
    }
    //------------------------------------------------------------------------
    void ListenerKeyboardControlItem::onCapsLock(const KeyboardArgs * arg)
    {
        static_cast<KeyboardControlPattern*>(mInfluence)->syn(KT_L_CapsLock);
        mChar = 0;
        mWChar = 0;
        mDescrip = _I18n("capslock key");
    }
    //------------------------------------------------------------------------
    void ListenerKeyboardControlItem::onF1(const KeyboardArgs * arg)
    {
        mChar = 0;
        mWChar = 0;
        mDescrip = _I18n("F1 key");
    }
    //------------------------------------------------------------------------
    void ListenerKeyboardControlItem::onF2(const KeyboardArgs * arg)
    {
        mChar = 0;
        mWChar = 0;
        mDescrip = _I18n("F2 key");
    }
    //------------------------------------------------------------------------
    void ListenerKeyboardControlItem::onF3(const KeyboardArgs * arg)
    {
        mChar = 0;
        mWChar = 0;
        mDescrip = _I18n("F3 key");
    }
    //------------------------------------------------------------------------
    void ListenerKeyboardControlItem::onF4(const KeyboardArgs * arg)
    {
        mChar = 0;
        mWChar = 0;
        mDescrip = _I18n("F4 key");
    }
    //------------------------------------------------------------------------
    void ListenerKeyboardControlItem::onF5(const KeyboardArgs * arg)
    {
        mChar = 0;
        mWChar = 0;
        mDescrip = _I18n("F5 key");
    }
    //------------------------------------------------------------------------
    void ListenerKeyboardControlItem::onF6(const KeyboardArgs * arg)
    {
        mChar = 0;
        mWChar = 0;
        mDescrip = _I18n("F6 key");
    }
    //------------------------------------------------------------------------
    void ListenerKeyboardControlItem::onF7(const KeyboardArgs * arg)
    {
        mChar = 0;
        mWChar = 0;
        mDescrip = _I18n("F7 key");
    }
    //------------------------------------------------------------------------
    void ListenerKeyboardControlItem::onF8(const KeyboardArgs * arg)
    {
        mChar = 0;
        mWChar = 0;
        mDescrip = _I18n("F8 key");
    }
    //------------------------------------------------------------------------
    void ListenerKeyboardControlItem::onF9(const KeyboardArgs * arg)
    {
        mChar = 0;
        mWChar = 0;
        mDescrip = _I18n("F9 key");
    }
    //------------------------------------------------------------------------
    void ListenerKeyboardControlItem::onF10(const KeyboardArgs * arg)
    {
        mChar = 0;
        mWChar = 0;
        mDescrip = _I18n("F10 key");
    }
    //------------------------------------------------------------------------
    void ListenerKeyboardControlItem::onNumLock(const KeyboardArgs * arg)
    {
        static_cast<KeyboardControlPattern*>(mInfluence)->syn(KT_L_NumLock);
        mChar = 0;
        mWChar = 0;
        mDescrip = _I18n("numlock key");
    }
    //------------------------------------------------------------------------
    void ListenerKeyboardControlItem::onMidScrLock(const KeyboardArgs * arg)
    {
        static_cast<KeyboardControlPattern*>(mInfluence)->syn(KT_L_ScrLock);
        mChar = 0;
        mWChar = 0;
        mDescrip = _I18n("scrlock key");
    }
    //------------------------------------------------------------------------
    void ListenerKeyboardControlItem::onNum7(const KeyboardArgs * arg)
    {
        if (mModifiers & KT_L_NumLock)
        {
            mChar = 55;
            mWChar = 0x37;
            mDescrip = _I18n("num7 key in numpad");
        }
        else
        {
            mChar = 0;
            mWChar = 0;
            mDescrip = _I18n("home key in numpad");
        }
    }
    //------------------------------------------------------------------------
    void ListenerKeyboardControlItem::onNum8(const KeyboardArgs * arg)
    {
        if (mModifiers & KT_L_NumLock)
        {
            mChar = 56;
            mWChar = 0x38;
            mDescrip = _I18n("num8 key in numpad");
        }
        else
        {
            mChar = 0;
            mWChar = 0;
            mDescrip = _I18n("up key in numpad");
        }
    }
    //------------------------------------------------------------------------
    void ListenerKeyboardControlItem::onNum9(const KeyboardArgs * arg)
    {
        if (mModifiers & KT_L_NumLock)
        {
            mChar = 57;
            mWChar = 0x39;
            mDescrip = _I18n("num9 key in numpad");
        }
        else
        {
            mChar = 0;
            mWChar = 0;
            mDescrip = _I18n("pgup key in numpad");
        }
    }
    //------------------------------------------------------------------------
    void ListenerKeyboardControlItem::onNumSubtract(const KeyboardArgs * arg)
    {
        mChar = 45;
        mWChar = 0x2d;
        mDescrip = _I18n("- key in numpad");
    }
    //------------------------------------------------------------------------
    void ListenerKeyboardControlItem::onNum4(const KeyboardArgs * arg)
    {
        if (mModifiers & KT_L_NumLock)
        {
            mChar = 52;
            mWChar = 0x34;
            mDescrip = _I18n("num4 key in numpad");
        }
        else
        {
            mChar = 0;
            mWChar = 0;
            mDescrip = _I18n("left key in numpad");
        }
    }
    //------------------------------------------------------------------------
    void ListenerKeyboardControlItem::onNum5(const KeyboardArgs * arg)
    {
        if (mModifiers & KT_L_NumLock)
        {
            mChar = 53;
            mWChar = 0x35;
            mDescrip = _I18n("num5 key in numpad");
        }
        else
        {
            mChar = 0;
            mWChar = 0;
            mDescrip = _I18n("");
        }
    }
    //------------------------------------------------------------------------
    void ListenerKeyboardControlItem::onNum6(const KeyboardArgs * arg)
    {
        if (mModifiers & KT_L_NumLock)
        {
            mChar = 54;
            mWChar = 0x36;
            mDescrip = _I18n("num6 key in numpad");
        }
        else
        {
            mChar = 0;
            mWChar = 0;
            mDescrip = _I18n("right key in numpad");
        }
    }
    //------------------------------------------------------------------------
    void ListenerKeyboardControlItem::onNumAdd(const KeyboardArgs * arg)
    {
        mChar = 43;
        mWChar = 0x2b;
        mDescrip = _I18n("+ key in numpad");
    }
    //------------------------------------------------------------------------
    void ListenerKeyboardControlItem::onNum1(const KeyboardArgs * arg)
    {
        if (mModifiers & KT_L_NumLock)
        {
            mChar = 49;
            mWChar = 0x31;
            mDescrip = _I18n("1 key in numpad");
        }
        else
        {
            mChar = 0;
            mWChar = 0;
            mDescrip = _I18n("end key in numpad");
        }
    }
    //------------------------------------------------------------------------
    void ListenerKeyboardControlItem::onNum2(const KeyboardArgs * arg)
    {
        if (mModifiers & KT_L_NumLock)
        {
            mChar = 50;
            mWChar = 0x32;
            mDescrip = _I18n("2 key in numpad");
        }
        else
        {
            mChar = 0;
            mWChar = 0;
            mDescrip = _I18n("down key in numpad");
        }
    }
    //------------------------------------------------------------------------
    void ListenerKeyboardControlItem::onNum3(const KeyboardArgs * arg)
    {
        if (mModifiers & KT_L_NumLock)
        {
            mChar = 51;
            mWChar = 0x33;
            mDescrip = _I18n("3 key in numpad");
        }
        else
        {
            mChar = 0;
            mWChar = 0;
            mDescrip = _I18n("pgdn key in numpad");
        }
    }
    //------------------------------------------------------------------------
    void ListenerKeyboardControlItem::onNum0(const KeyboardArgs * arg)
    {
        if (mModifiers & KT_L_NumLock)
        {
            mChar = 48;
            mWChar = 0x30;
            mDescrip = _I18n("0 key in numpad");
            mDescrip = _I18n("insert key in numpad");
        }
        else
        {
            mChar = 0;
            mWChar = 0;
            mDescrip = _I18n("insert key in numpad");
        }
    }
    //------------------------------------------------------------------------
    void ListenerKeyboardControlItem::onNumDot(const KeyboardArgs * arg)
    {
        if (mModifiers & KT_L_NumLock)
        {
            mChar = 46;
            mWChar = 0x2e;
            mDescrip = _I18n(". key in numpad");
        }
        else
        {
            mChar = 0;
            mWChar = 0;
            mDescrip = _I18n("delete in numpad");
        }
    }
    //------------------------------------------------------------------------
    void ListenerKeyboardControlItem::onF11(const KeyboardArgs * arg)
    {
        mChar = 0;
        mWChar = 0;
        mDescrip = _I18n("F11 key");
    }
    //------------------------------------------------------------------------
    void ListenerKeyboardControlItem::onF12(const KeyboardArgs * arg)
    {
        mChar = 0;
        mWChar = 0;
        mDescrip = _I18n("F12 key");
    }
    //------------------------------------------------------------------------
    void ListenerKeyboardControlItem::onNumEnter(const KeyboardArgs * arg)
    {
        mChar = 13;
        mWChar = 0x0d;
        mDescrip = _I18n("Enter key in numpad");
    }
    //------------------------------------------------------------------------
    void ListenerKeyboardControlItem::onRCtrl(const KeyboardArgs * arg)
    {
        mModifiers |= KT_A_Ctrl;
        mChar = 0;
        mWChar = 0;
        mDescrip = _I18n("right ctrl key");
    }
    //------------------------------------------------------------------------
    void ListenerKeyboardControlItem::onRAlt(const KeyboardArgs * arg)
    {
        mModifiers |= KT_A_Alt;
        mChar = 0;
        mWChar = 0;
        mDescrip = _I18n("right alt key");
    }
    //------------------------------------------------------------------------
    void ListenerKeyboardControlItem::onLWin(const KeyboardArgs * arg)
    {
        mChar = 0;
        mWChar = 0;
        mDescrip = _I18n("left win key");
    }
    //------------------------------------------------------------------------
    void ListenerKeyboardControlItem::onRWin(const KeyboardArgs * arg)
    {
        mChar = 0;
        mWChar = 0;
        mDescrip = _I18n("right win key");
    }
    //------------------------------------------------------------------------
    void ListenerKeyboardControlItem::onCompose(const KeyboardArgs * arg)
    {
        mChar = 0;
        mWChar = 0;
        mDescrip = _I18n("compose key");
    }
    //------------------------------------------------------------------------
    void ListenerKeyboardControlItem::onNumDivide(const KeyboardArgs * arg)
    {
        mChar = 47;
        mWChar = 0x2f;
        mDescrip = _I18n("/ key in numpad");
    }
    //------------------------------------------------------------------------
    void ListenerKeyboardControlItem::onMidSysRQ(const KeyboardArgs * arg)
    {
        mChar = 0;
        mWChar = 0;
        mDescrip = _I18n("sysrq key");
    }
    //------------------------------------------------------------------------
    void ListenerKeyboardControlItem::onMidPause(const KeyboardArgs * arg)
    {
        mChar = 0;
        mWChar = 0;
        mDescrip = _I18n("pause key");
    }
    //------------------------------------------------------------------------
    void ListenerKeyboardControlItem::onMidHome(const KeyboardArgs * arg)
    {
        mChar = 0;
        mWChar = 0;
        mDescrip = _I18n("home key");
    }
    //------------------------------------------------------------------------
    void ListenerKeyboardControlItem::onMidUp(const KeyboardArgs * arg)
    {
        mChar = 0;
        mWChar = 0;
        mDescrip = _I18n("up key");
    }
    //------------------------------------------------------------------------
    void ListenerKeyboardControlItem::onMidPgUp(const KeyboardArgs * arg)
    {
        mChar = 0;
        mWChar = 0;
        mDescrip = _I18n("pgup key");
    }
    //------------------------------------------------------------------------
    void ListenerKeyboardControlItem::onMidLeft(const KeyboardArgs * arg)
    {
        mChar = 0;
        mWChar = 0;
        mDescrip = _I18n("left key");
    }
    //------------------------------------------------------------------------
    void ListenerKeyboardControlItem::onMidRight(const KeyboardArgs * arg)
    {
        mChar = 0;
        mWChar = 0;
        mDescrip = _I18n("right key");
    }
    //------------------------------------------------------------------------
    void ListenerKeyboardControlItem::onMidEnd(const KeyboardArgs * arg)
    {
        mChar = 0;
        mWChar = 0;
        mDescrip = _I18n("end key");
    }
    //------------------------------------------------------------------------
    void ListenerKeyboardControlItem::onMidDown(const KeyboardArgs * arg)
    {
        mChar = 0;
        mWChar = 0;
        mDescrip = _I18n("down key");
    }
    //------------------------------------------------------------------------
    void ListenerKeyboardControlItem::onMidPgDown(const KeyboardArgs * arg)
    {
        mChar = 0;
        mWChar = 0;
        mDescrip = _I18n("pgdown key");
    }
    //------------------------------------------------------------------------
    void ListenerKeyboardControlItem::onMidInsert(const KeyboardArgs * arg)
    {
        mChar = 0;
        mWChar = 0;
        mDescrip = _I18n("insert key");
    }
    //------------------------------------------------------------------------
    void ListenerKeyboardControlItem::onMidDelete(const KeyboardArgs * arg)
    {
        mChar = 0;
        mWChar = 0;
        mDescrip = _I18n("delete key");
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // SignalKeyboardControlItem
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    N_EVENT(CommandObj, SignalKeyboardControlItem, KeyEscEvent, 0);
    N_EVENT(CommandObj, SignalKeyboardControlItem, Key1Event, 1);
    N_EVENT(CommandObj, SignalKeyboardControlItem, Key2Event, 2);
    N_EVENT(CommandObj, SignalKeyboardControlItem, Key3Event, 3);
    N_EVENT(CommandObj, SignalKeyboardControlItem, Key4Event, 4);
    N_EVENT(CommandObj, SignalKeyboardControlItem, Key5Event, 5);
    N_EVENT(CommandObj, SignalKeyboardControlItem, Key6Event, 6);
    N_EVENT(CommandObj, SignalKeyboardControlItem, Key7Event, 7);
    N_EVENT(CommandObj, SignalKeyboardControlItem, Key8Event, 8);
    N_EVENT(CommandObj, SignalKeyboardControlItem, Key9Event, 9);
    N_EVENT(CommandObj, SignalKeyboardControlItem, Key0Event, 10);
    N_EVENT(CommandObj, SignalKeyboardControlItem, KeyMinusEvent, 11);
    N_EVENT(CommandObj, SignalKeyboardControlItem, KeyEqualEvent, 12);
    N_EVENT(CommandObj, SignalKeyboardControlItem, KeyBKEvent, 13);
    N_EVENT(CommandObj, SignalKeyboardControlItem, KeyTABEvent, 14);
    N_EVENT(CommandObj, SignalKeyboardControlItem, KeyQEvent, 15);
    N_EVENT(CommandObj, SignalKeyboardControlItem, KeyWEvent, 16);
    N_EVENT(CommandObj, SignalKeyboardControlItem, KeyEEvent, 17);
    N_EVENT(CommandObj, SignalKeyboardControlItem, KeyREvent, 18);
    N_EVENT(CommandObj, SignalKeyboardControlItem, KeyTEvent, 19);
    N_EVENT(CommandObj, SignalKeyboardControlItem, KeyYEvent, 20);
    N_EVENT(CommandObj, SignalKeyboardControlItem, KeyUEvent, 21);
    N_EVENT(CommandObj, SignalKeyboardControlItem, KeyIEvent, 22);
    N_EVENT(CommandObj, SignalKeyboardControlItem, KeyOEvent, 23);
    N_EVENT(CommandObj, SignalKeyboardControlItem, KeyPEvent, 24);
    N_EVENT(CommandObj, SignalKeyboardControlItem, KeyLBracketEvent, 25);
    N_EVENT(CommandObj, SignalKeyboardControlItem, KeyRBracketEvent, 26);
    N_EVENT(CommandObj, SignalKeyboardControlItem, KeyEnterEvent, 27);
    N_EVENT(CommandObj, SignalKeyboardControlItem, KeyLCtrlEvent, 28);
    N_EVENT(CommandObj, SignalKeyboardControlItem, KeyAEvent, 29);
    N_EVENT(CommandObj, SignalKeyboardControlItem, KeySEvent, 30);
    N_EVENT(CommandObj, SignalKeyboardControlItem, KeyDEvent, 31);
    N_EVENT(CommandObj, SignalKeyboardControlItem, KeyFEvent, 32);
    N_EVENT(CommandObj, SignalKeyboardControlItem, KeyGEvent, 33);
    N_EVENT(CommandObj, SignalKeyboardControlItem, KeyHEvent, 34);
    N_EVENT(CommandObj, SignalKeyboardControlItem, KeyJEvent, 35);
    N_EVENT(CommandObj, SignalKeyboardControlItem, KeyKEvent, 36);
    N_EVENT(CommandObj, SignalKeyboardControlItem, KeyLEvent, 37);
    N_EVENT(CommandObj, SignalKeyboardControlItem, KeySemicolonEvent, 38);
    N_EVENT(CommandObj, SignalKeyboardControlItem, KeyApostropheEvent, 39);
    N_EVENT(CommandObj, SignalKeyboardControlItem, KeyGraveEvent, 40);
    N_EVENT(CommandObj, SignalKeyboardControlItem, KeyLShiftEvent, 41);
    N_EVENT(CommandObj, SignalKeyboardControlItem, KeyBackSlashEvent, 42);
    N_EVENT(CommandObj, SignalKeyboardControlItem, KeyZEvent, 43);
    N_EVENT(CommandObj, SignalKeyboardControlItem, KeyXEvent, 44);
    N_EVENT(CommandObj, SignalKeyboardControlItem, KeyCEvent, 45);
    N_EVENT(CommandObj, SignalKeyboardControlItem, KeyVEvent, 46);
    N_EVENT(CommandObj, SignalKeyboardControlItem, KeyBEvent, 47);
    N_EVENT(CommandObj, SignalKeyboardControlItem, KeyNEvent, 48);
    N_EVENT(CommandObj, SignalKeyboardControlItem, KeyMEvent, 49);
    N_EVENT(CommandObj, SignalKeyboardControlItem, KeyCommaEvent, 50);
    N_EVENT(CommandObj, SignalKeyboardControlItem, KeyPeriodEvent, 51);
    N_EVENT(CommandObj, SignalKeyboardControlItem, KeySlashEvent, 52);
    N_EVENT(CommandObj, SignalKeyboardControlItem, KeyRShiftEvent, 53);
    N_EVENT(CommandObj, SignalKeyboardControlItem, KeyNumMulEvent, 54);
    N_EVENT(CommandObj, SignalKeyboardControlItem, KeyLAltEvent, 55);
    N_EVENT(CommandObj, SignalKeyboardControlItem, KeySpaceEvent, 56);
    N_EVENT(CommandObj, SignalKeyboardControlItem, KeyCapsLockEvent, 57);
    N_EVENT(CommandObj, SignalKeyboardControlItem, KeyF1Event, 58);
    N_EVENT(CommandObj, SignalKeyboardControlItem, KeyF2Event, 59);
    N_EVENT(CommandObj, SignalKeyboardControlItem, KeyF3Event, 60);
    N_EVENT(CommandObj, SignalKeyboardControlItem, KeyF4Event, 61);
    N_EVENT(CommandObj, SignalKeyboardControlItem, KeyF5Event, 62);
    N_EVENT(CommandObj, SignalKeyboardControlItem, KeyF6Event, 63);
    N_EVENT(CommandObj, SignalKeyboardControlItem, KeyF7Event, 64);
    N_EVENT(CommandObj, SignalKeyboardControlItem, KeyF8Event, 65);
    N_EVENT(CommandObj, SignalKeyboardControlItem, KeyF9Event, 66);
    N_EVENT(CommandObj, SignalKeyboardControlItem, KeyF10Event, 67);
    N_EVENT(CommandObj, SignalKeyboardControlItem, KeyNumLockEvent, 68);
    N_EVENT(CommandObj, SignalKeyboardControlItem, KeyMidScrLockEvent, 69);
    N_EVENT(CommandObj, SignalKeyboardControlItem, KeyNum7Event, 70);
    N_EVENT(CommandObj, SignalKeyboardControlItem, KeyNum8Event, 71);
    N_EVENT(CommandObj, SignalKeyboardControlItem, KeyNum9Event, 72);
    N_EVENT(CommandObj, SignalKeyboardControlItem, KeyNumSubtractEvent, 73);
    N_EVENT(CommandObj, SignalKeyboardControlItem, KeyNum4Event, 74);
    N_EVENT(CommandObj, SignalKeyboardControlItem, KeyNum5Event, 75);
    N_EVENT(CommandObj, SignalKeyboardControlItem, KeyNum6Event, 76);
    N_EVENT(CommandObj, SignalKeyboardControlItem, KeyNumAddEvent, 77);
    N_EVENT(CommandObj, SignalKeyboardControlItem, KeyNum1Event, 78);
    N_EVENT(CommandObj, SignalKeyboardControlItem, KeyNum2Event, 79);
    N_EVENT(CommandObj, SignalKeyboardControlItem, KeyNum3Event, 80);
    N_EVENT(CommandObj, SignalKeyboardControlItem, KeyNum0Event, 81);
    N_EVENT(CommandObj, SignalKeyboardControlItem, KeyNumDotEvent, 82);
    N_EVENT(CommandObj, SignalKeyboardControlItem, KeyF11Event, 83);
    N_EVENT(CommandObj, SignalKeyboardControlItem, KeyF12Event, 84);
    N_EVENT(CommandObj, SignalKeyboardControlItem, KeyNumEnterEvent, 85);
    N_EVENT(CommandObj, SignalKeyboardControlItem, KeyRCtrlEvent, 86);
    N_EVENT(CommandObj, SignalKeyboardControlItem, KeyRAltEvent, 87);
    N_EVENT(CommandObj, SignalKeyboardControlItem, KeyLWinEvent, 88);
    N_EVENT(CommandObj, SignalKeyboardControlItem, KeyRWinEvent, 89);
    N_EVENT(CommandObj, SignalKeyboardControlItem, KeyComposeEvent, 90);
    N_EVENT(CommandObj, SignalKeyboardControlItem, KeyNumDivideEvent, 91);
    N_EVENT(CommandObj, SignalKeyboardControlItem, KeyMidSysRQEvent, 92);
    N_EVENT(CommandObj, SignalKeyboardControlItem, KeyMidPauseEvent, 93);
    N_EVENT(CommandObj, SignalKeyboardControlItem, KeyMidHomeEvent, 94);
    N_EVENT(CommandObj, SignalKeyboardControlItem, KeyMidUpEvent, 95);
    N_EVENT(CommandObj, SignalKeyboardControlItem, KeyMidPgUpEvent, 96);
    N_EVENT(CommandObj, SignalKeyboardControlItem, KeyMidLeftEvent, 97);
    N_EVENT(CommandObj, SignalKeyboardControlItem, KeyMidRightEvent, 98);
    N_EVENT(CommandObj, SignalKeyboardControlItem, KeyMidEndEvent, 99);
    N_EVENT(CommandObj, SignalKeyboardControlItem, KeyMidDownEvent, 100);
    N_EVENT(CommandObj, SignalKeyboardControlItem, KeyMidPgDownEvent, 101);
    N_EVENT(CommandObj, SignalKeyboardControlItem, KeyMidInsertEvent, 102);
    N_EVENT(CommandObj, SignalKeyboardControlItem, KeyMidDeleteEvent, 103);
    N_EVENT(CommandObj, SignalKeyboardControlItem, EventCount, 208);
    //------------------------------------------------------------------------
    SignalKeyboardControlItem::SignalKeyboardControlItem()
    {
        notifySignal(SignalKeyboardControlItem::EventCount);
    }
    //------------------------------------------------------------------------
    SignalKeyboardControlItem::~SignalKeyboardControlItem()
    {
    }
    //------------------------------------------------------------------------
    void SignalKeyboardControlItem::onEsc(const KeyboardArgs * arg)
    {
        signal(PressEscEvent, arg);
    }
    //------------------------------------------------------------------------
    void SignalKeyboardControlItem::on1(const KeyboardArgs * arg)
    {
        signal(Press1Event, arg);
    }
    //------------------------------------------------------------------------
    void SignalKeyboardControlItem::on2(const KeyboardArgs * arg)
    {
        signal(Press2Event, arg);
    }
    //------------------------------------------------------------------------
    void SignalKeyboardControlItem::on3(const KeyboardArgs * arg)
    {
        signal(Press3Event, arg);
    }
    //------------------------------------------------------------------------
    void SignalKeyboardControlItem::on4(const KeyboardArgs * arg)
    {
        signal(Press4Event, arg);
    }
    //------------------------------------------------------------------------
    void SignalKeyboardControlItem::on5(const KeyboardArgs * arg)
    {
        signal(Press5Event, arg);
    }
    //------------------------------------------------------------------------
    void SignalKeyboardControlItem::on6(const KeyboardArgs * arg)
    {
        signal(Press6Event, arg);
    }
    //------------------------------------------------------------------------
    void SignalKeyboardControlItem::on7(const KeyboardArgs * arg)
    {
        signal(Press7Event, arg);
    }
    //------------------------------------------------------------------------
    void SignalKeyboardControlItem::on8(const KeyboardArgs * arg)
    {
        signal(Press8Event, arg);
    }
    //------------------------------------------------------------------------
    void SignalKeyboardControlItem::on9(const KeyboardArgs * arg)
    {
        signal(Press9Event, arg);
    }
    //------------------------------------------------------------------------
    void SignalKeyboardControlItem::on0(const KeyboardArgs * arg)
    {
        signal(Press0Event, arg);
    }
    //------------------------------------------------------------------------
    void SignalKeyboardControlItem::onMinus(const KeyboardArgs * arg)
    {
        signal(PressMinusEvent, arg);
    }
    //------------------------------------------------------------------------
    void SignalKeyboardControlItem::onEqual(const KeyboardArgs * arg)
    {
        signal(PressEqualEvent, arg);
    }
    //------------------------------------------------------------------------
    void SignalKeyboardControlItem::onBK(const KeyboardArgs * arg)
    {
        signal(PressBKEvent, arg);
    }
    //------------------------------------------------------------------------
    void SignalKeyboardControlItem::onTAB(const KeyboardArgs * arg)
    {
        signal(PressTABEvent, arg);
    }
    //------------------------------------------------------------------------
    void SignalKeyboardControlItem::onQ(const KeyboardArgs * arg)
    {
        signal(PressQEvent, arg);
    }
    //------------------------------------------------------------------------
    void SignalKeyboardControlItem::onW(const KeyboardArgs * arg)
    {
        signal(PressWEvent, arg);
    }
    //------------------------------------------------------------------------
    void SignalKeyboardControlItem::onE(const KeyboardArgs * arg)
    {
        signal(PressEEvent, arg);
    }
    //------------------------------------------------------------------------
    void SignalKeyboardControlItem::onR(const KeyboardArgs * arg)
    {
        signal(PressREvent, arg);
    }
    //------------------------------------------------------------------------
    void SignalKeyboardControlItem::onT(const KeyboardArgs * arg)
    {
        signal(PressTEvent, arg);
    }
    //------------------------------------------------------------------------
    void SignalKeyboardControlItem::onY(const KeyboardArgs * arg)
    {
        signal(PressYEvent, arg);
    }
    //------------------------------------------------------------------------
    void SignalKeyboardControlItem::onU(const KeyboardArgs * arg)
    {
        signal(PressUEvent, arg);
    }
    //------------------------------------------------------------------------
    void SignalKeyboardControlItem::onI(const KeyboardArgs * arg)
    {
        signal(PressIEvent, arg);
    }
    //------------------------------------------------------------------------
    void SignalKeyboardControlItem::onO(const KeyboardArgs * arg)
    {
        signal(PressOEvent, arg);
    }
    //------------------------------------------------------------------------
    void SignalKeyboardControlItem::onP(const KeyboardArgs * arg)
    {
        signal(PressPEvent, arg);
    }
    //------------------------------------------------------------------------
    void SignalKeyboardControlItem::onLBracket(const KeyboardArgs * arg)
    {
        signal(PressLBracketEvent, arg);
    }
    //------------------------------------------------------------------------
    void SignalKeyboardControlItem::onRBracket(const KeyboardArgs * arg)
    {
        signal(PressRBracketEvent, arg);
    }
    //------------------------------------------------------------------------
    void SignalKeyboardControlItem::onEnter(const KeyboardArgs * arg)
    {
        signal(PressEnterEvent, arg);
    }
    //------------------------------------------------------------------------
    void SignalKeyboardControlItem::onLCtrl(const KeyboardArgs * arg)
    {
        KeyboardControlItem::onLCtrl(arg);
        signal(PressLCtrlEvent, arg);
    }
    //------------------------------------------------------------------------
    void SignalKeyboardControlItem::onA(const KeyboardArgs * arg)
    {
        signal(PressAEvent, arg);
    }
    //------------------------------------------------------------------------
    void SignalKeyboardControlItem::onS(const KeyboardArgs * arg)
    {
        signal(PressSEvent, arg);
    }
    //------------------------------------------------------------------------
    void SignalKeyboardControlItem::onD(const KeyboardArgs * arg)
    {
        signal(PressDEvent, arg);
    }
    //------------------------------------------------------------------------
    void SignalKeyboardControlItem::onF(const KeyboardArgs * arg)
    {
        signal(PressFEvent, arg);
    }
    //------------------------------------------------------------------------
    void SignalKeyboardControlItem::onG(const KeyboardArgs * arg)
    {
        signal(PressGEvent, arg);
    }
    //------------------------------------------------------------------------
    void SignalKeyboardControlItem::onH(const KeyboardArgs * arg)
    {
        signal(PressHEvent, arg);
    }
    //------------------------------------------------------------------------
    void SignalKeyboardControlItem::onJ(const KeyboardArgs * arg)
    {
        signal(PressJEvent, arg);
    }
    //------------------------------------------------------------------------
    void SignalKeyboardControlItem::onK(const KeyboardArgs * arg)
    {
        signal(PressKEvent, arg);
    }
    //------------------------------------------------------------------------
    void SignalKeyboardControlItem::onL(const KeyboardArgs * arg)
    {
        signal(PressLEvent, arg);
    }
    //------------------------------------------------------------------------
    void SignalKeyboardControlItem::onSemicolon(const KeyboardArgs * arg)
    {
        signal(PressSemicolonEvent, arg);
    }
    //------------------------------------------------------------------------
    void SignalKeyboardControlItem::onApostrophe(const KeyboardArgs * arg)
    {
        signal(PressApostropheEvent, arg);
    }
    //------------------------------------------------------------------------
    void SignalKeyboardControlItem::onGrave(const KeyboardArgs * arg)
    {
        signal(PressGraveEvent, arg);
    }
    //------------------------------------------------------------------------
    void SignalKeyboardControlItem::onLShift(const KeyboardArgs * arg)
    {
        KeyboardControlItem::onLShift(arg);
        signal(PressLShiftEvent, arg);
    }
    //------------------------------------------------------------------------
    void SignalKeyboardControlItem::onBackSlash(const KeyboardArgs * arg)
    {
        signal(PressBackSlashEvent, arg);
    }
    //------------------------------------------------------------------------
    void SignalKeyboardControlItem::onZ(const KeyboardArgs * arg)
    {
        signal(PressZEvent, arg);
    }
    //------------------------------------------------------------------------
    void SignalKeyboardControlItem::onX(const KeyboardArgs * arg)
    {
        signal(PressXEvent, arg);
    }
    //------------------------------------------------------------------------
    void SignalKeyboardControlItem::onC(const KeyboardArgs * arg)
    {
        signal(PressCEvent, arg);
    }
    //------------------------------------------------------------------------
    void SignalKeyboardControlItem::onV(const KeyboardArgs * arg)
    {
        signal(PressVEvent, arg);
    }
    //------------------------------------------------------------------------
    void SignalKeyboardControlItem::onB(const KeyboardArgs * arg)
    {
        signal(PressBEvent, arg);
    }
    //------------------------------------------------------------------------
    void SignalKeyboardControlItem::onN(const KeyboardArgs * arg)
    {
        signal(PressNEvent, arg);
    }
    //------------------------------------------------------------------------
    void SignalKeyboardControlItem::onM(const KeyboardArgs * arg)
    {
        signal(PressMEvent, arg);
    }
    //------------------------------------------------------------------------
    void SignalKeyboardControlItem::onComma(const KeyboardArgs * arg)
    {
        signal(PressCommaEvent, arg);
    }
    //------------------------------------------------------------------------
    void SignalKeyboardControlItem::onPeriod(const KeyboardArgs * arg)
    {
        signal(PressPeriodEvent, arg);
    }
    //------------------------------------------------------------------------
    void SignalKeyboardControlItem::onSlash(const KeyboardArgs * arg)
    {
        signal(PressSlashEvent, arg);
    }
    //------------------------------------------------------------------------
    void SignalKeyboardControlItem::onRShift(const KeyboardArgs * arg)
    {
        KeyboardControlItem::onRShift(arg);
        signal(PressRShiftEvent, arg);
    }
    //------------------------------------------------------------------------
    void SignalKeyboardControlItem::onNumMul(const KeyboardArgs * arg)
    {
        signal(PressNumMulEvent, arg);
    }
    //------------------------------------------------------------------------
    void SignalKeyboardControlItem::onLAlt(const KeyboardArgs * arg)
    {
        KeyboardControlItem::onLAlt(arg);
        signal(PressLAltEvent, arg);
    }
    //------------------------------------------------------------------------
    void SignalKeyboardControlItem::onSpace(const KeyboardArgs * arg)
    {
        signal(PressSpaceEvent, arg);
    }
    //------------------------------------------------------------------------
    void SignalKeyboardControlItem::onCapsLock(const KeyboardArgs * arg)
    {
        KeyboardControlItem::onCapsLock(arg);
        signal(PressCapsLockEvent, arg);
    }
    //------------------------------------------------------------------------
    void SignalKeyboardControlItem::onF1(const KeyboardArgs * arg)
    {
        signal(PressF1Event, arg);
    }
    //------------------------------------------------------------------------
    void SignalKeyboardControlItem::onF2(const KeyboardArgs * arg)
    {
        signal(PressF2Event, arg);
    }
    //------------------------------------------------------------------------
    void SignalKeyboardControlItem::onF3(const KeyboardArgs * arg)
    {
        signal(PressF3Event, arg);
    }
    //------------------------------------------------------------------------
    void SignalKeyboardControlItem::onF4(const KeyboardArgs * arg)
    {
        signal(PressF4Event, arg);
    }
    //------------------------------------------------------------------------
    void SignalKeyboardControlItem::onF5(const KeyboardArgs * arg)
    {
        signal(PressF5Event, arg);
    }
    //------------------------------------------------------------------------
    void SignalKeyboardControlItem::onF6(const KeyboardArgs * arg)
    {
        signal(PressF6Event, arg);
    }
    //------------------------------------------------------------------------
    void SignalKeyboardControlItem::onF7(const KeyboardArgs * arg)
    {
        signal(PressF7Event, arg);
    }
    //------------------------------------------------------------------------
    void SignalKeyboardControlItem::onF8(const KeyboardArgs * arg)
    {
        signal(PressF8Event, arg);
    }
    //------------------------------------------------------------------------
    void SignalKeyboardControlItem::onF9(const KeyboardArgs * arg)
    {
        signal(PressF9Event, arg);
    }
    //------------------------------------------------------------------------
    void SignalKeyboardControlItem::onF10(const KeyboardArgs * arg)
    {
        signal(PressF10Event, arg);
    }
    //------------------------------------------------------------------------
    void SignalKeyboardControlItem::onNumLock(const KeyboardArgs * arg)
    {
        signal(PressNumLockEvent, arg);
    }
    //------------------------------------------------------------------------
    void SignalKeyboardControlItem::onMidScrLock(const KeyboardArgs * arg)
    {
        KeyboardControlItem::onMidScrLock(arg);
        signal(PressMidScrLockEvent, arg);
    }
    //------------------------------------------------------------------------
    void SignalKeyboardControlItem::onNum7(const KeyboardArgs * arg)
    {
        signal(PressNum7Event, arg);
    }
    //------------------------------------------------------------------------
    void SignalKeyboardControlItem::onNum8(const KeyboardArgs * arg)
    {
        signal(PressNum8Event, arg);
    }
    //------------------------------------------------------------------------
    void SignalKeyboardControlItem::onNum9(const KeyboardArgs * arg)
    {
        signal(PressNum9Event, arg);
    }
    //------------------------------------------------------------------------
    void SignalKeyboardControlItem::onNumSubtract(const KeyboardArgs * arg)
    {
        signal(PressNumSubtractEvent, arg);
    }
    //------------------------------------------------------------------------
    void SignalKeyboardControlItem::onNum4(const KeyboardArgs * arg)
    {
        signal(PressNum4Event, arg);
    }
    //------------------------------------------------------------------------
    void SignalKeyboardControlItem::onNum5(const KeyboardArgs * arg)
    {
        signal(PressNum5Event, arg);
    }
    //------------------------------------------------------------------------
    void SignalKeyboardControlItem::onNum6(const KeyboardArgs * arg)
    {
        signal(PressNum6Event, arg);
    }
    //------------------------------------------------------------------------
    void SignalKeyboardControlItem::onNumAdd(const KeyboardArgs * arg)
    {
        signal(PressNumAddEvent, arg);
    }
    //------------------------------------------------------------------------
    void SignalKeyboardControlItem::onNum1(const KeyboardArgs * arg)
    {
        signal(PressNum1Event, arg);
    }
    //------------------------------------------------------------------------
    void SignalKeyboardControlItem::onNum2(const KeyboardArgs * arg)
    {
        signal(PressNum2Event, arg);
    }
    //------------------------------------------------------------------------
    void SignalKeyboardControlItem::onNum3(const KeyboardArgs * arg)
    {
        signal(PressNum3Event, arg);
    }
    //------------------------------------------------------------------------
    void SignalKeyboardControlItem::onNum0(const KeyboardArgs * arg)
    {
        signal(PressNum0Event, arg);
    }
    //------------------------------------------------------------------------
    void SignalKeyboardControlItem::onNumDot(const KeyboardArgs * arg)
    {
        signal(PressNumDotEvent, arg);
    }
    //------------------------------------------------------------------------
    void SignalKeyboardControlItem::onF11(const KeyboardArgs * arg)
    {
        signal(PressF11Event, arg);
    }
    //------------------------------------------------------------------------
    void SignalKeyboardControlItem::onF12(const KeyboardArgs * arg)
    {
        signal(PressF12Event, arg);
    }
    //------------------------------------------------------------------------
    void SignalKeyboardControlItem::onNumEnter(const KeyboardArgs * arg)
    {
        signal(PressNumEnterEvent, arg);
    }
    //------------------------------------------------------------------------
    void SignalKeyboardControlItem::onRCtrl(const KeyboardArgs * arg)
    {
        KeyboardControlItem::onRCtrl(arg);
        signal(PressRCtrlEvent, arg);
    }
    //------------------------------------------------------------------------
    void SignalKeyboardControlItem::onRAlt(const KeyboardArgs * arg)
    {
        KeyboardControlItem::onRAlt(arg);
        signal(PressRAltEvent, arg);
    }
    //------------------------------------------------------------------------
    void SignalKeyboardControlItem::onLWin(const KeyboardArgs * arg)
    {
        KeyboardControlItem::onLWin(arg);
        signal(PressLWinEvent, arg);
    }
    //------------------------------------------------------------------------
    void SignalKeyboardControlItem::onRWin(const KeyboardArgs * arg)
    {
        KeyboardControlItem::onRWin(arg);
        signal(PressRWinEvent, arg);
    }
    //------------------------------------------------------------------------
    void SignalKeyboardControlItem::onCompose(const KeyboardArgs * arg)
    {
        signal(PressComposeEvent, arg);
    }
    //------------------------------------------------------------------------
    void SignalKeyboardControlItem::onNumDivide(const KeyboardArgs * arg)
    {
        signal(PressNumDivideEvent, arg);
    }
    //------------------------------------------------------------------------
    void SignalKeyboardControlItem::onMidSysRQ(const KeyboardArgs * arg)
    {
        signal(PressMidSysRQEvent, arg);
    }
    //------------------------------------------------------------------------
    void SignalKeyboardControlItem::onMidPause(const KeyboardArgs * arg)
    {
        signal(PressMidPauseEvent, arg);
    }
    //------------------------------------------------------------------------
    void SignalKeyboardControlItem::onMidHome(const KeyboardArgs * arg)
    {
        signal(PressMidHomeEvent, arg);
    }
    //------------------------------------------------------------------------
    void SignalKeyboardControlItem::onMidUp(const KeyboardArgs * arg)
    {
        signal(PressMidUpEvent, arg);
    }
    //------------------------------------------------------------------------
    void SignalKeyboardControlItem::onMidPgUp(const KeyboardArgs * arg)
    {
        signal(PressMidPgUpEvent, arg);
    }
    //------------------------------------------------------------------------
    void SignalKeyboardControlItem::onMidLeft(const KeyboardArgs * arg)
    {
        signal(PressMidLeftEvent, arg);
    }
    //------------------------------------------------------------------------
    void SignalKeyboardControlItem::onMidRight(const KeyboardArgs * arg)
    {
        signal(PressMidRightEvent, arg);
    }
    //------------------------------------------------------------------------
    void SignalKeyboardControlItem::onMidEnd(const KeyboardArgs * arg)
    {
        signal(PressMidEndEvent, arg);
    }
    //------------------------------------------------------------------------
    void SignalKeyboardControlItem::onMidDown(const KeyboardArgs * arg)
    {
        signal(PressMidDownEvent, arg);
    }
    //------------------------------------------------------------------------
    void SignalKeyboardControlItem::onMidPgDown(const KeyboardArgs * arg)
    {
        signal(PressMidPgDownEvent, arg);
    }
    //------------------------------------------------------------------------
    void SignalKeyboardControlItem::onMidInsert(const KeyboardArgs * arg)
    {
        signal(PressMidInsertEvent, arg);
    }
    //------------------------------------------------------------------------
    void SignalKeyboardControlItem::onMidDelete(const KeyboardArgs * arg)
    {
        signal(PressMidDeleteEvent, arg);
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // KeyboardControlPattern
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    KeyboardControlPattern::KeyboardControlPattern(){}
    //------------------------------------------------------------------------
    KeyboardControlPattern::KeyboardControlPattern(Nid id, const ViewWindow * own) :
        ControlPattern(id, own),
        mModifiers(0)
    {
        mBuffer[0] = 0;
        mBuffer[1] = 0;
        mBuffer[2] = 0;
        mBuffer[3] = 0;
        mBuffer[4] = 0;
        mBuffer[5] = 0;
        mBuffer[6] = 0;
        mBuffer[7] = 0;
        mTarget = N_Only(Input).getDummyKeyboard();
    }
    //------------------------------------------------------------------------
    KeyboardControlPattern::~KeyboardControlPattern()
    {
        detach();
    }
    //------------------------------------------------------------------------
    void KeyboardControlPattern::on(KeyboardAlterLockType altype)
    {
        mModifiers |= altype;
        mTarget->mModifiers = mModifiers;
    }
    //------------------------------------------------------------------------
    void KeyboardControlPattern::off(KeyboardAlterLockType altype)
    {
        mModifiers &= ~altype;
        mTarget->mModifiers = mModifiers;
    }
    //------------------------------------------------------------------------
    void KeyboardControlPattern::syn(KeyboardAlterLockType type)
    {
        (void)type;
    }
    //------------------------------------------------------------------------
    bool KeyboardControlPattern::isPress(KeyValue key)
    {
        return ((mBuffer[key >> 5] & N32_1OrMark[key % 32]) == 0) ? false : true;
    }
    //------------------------------------------------------------------------
    bool KeyboardControlPattern::isRelease(KeyValue key)
    {
        return ((mBuffer[key >> 5] & N32_1OrMark[key % 32]) == 0) ? true : false;
    }
    
    //------------------------------------------------------------------------
    Nui32 KeyboardControlPattern::getKeyCode(KeyValue key)
    {
        return 0;
    }
    //------------------------------------------------------------------------
    String KeyboardControlPattern::getKeyText(KeyValue key)
    {
        return _T("");
    }
    //------------------------------------------------------------------------
    void KeyboardControlPattern::press(const KeyValue key)
    {
        mBuffer[key >> 5] |= N32_1OrMark[key % 32];
        mTarget->onPress(key);
    }
    //------------------------------------------------------------------------
    void KeyboardControlPattern::release(const KeyValue key)
    {
        mBuffer[key >> 5] &= N32_1NotMark[key % 32];
        mTarget->onRelease(key);
    }
    //------------------------------------------------------------------------
    ControlDevType KeyboardControlPattern::getType() const
    {
        return CDT_Keyboard;
    }
    //------------------------------------------------------------------------
    void KeyboardControlPattern::apply(ControlItem * item)
    {
        item->setInfluence(this);
        mTarget = static_cast<KeyboardControlItem *>(item);
    }
    //------------------------------------------------------------------------
    void KeyboardControlPattern::detach()
    {
        mTarget->setInfluence(this);
        mTarget = N_Only(Input).getDummyKeyboard();
    }
    //------------------------------------------------------------------------
}
}