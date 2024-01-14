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
#include "NiiSpreadParticle.h"
#include "NiiStrConv.h"

namespace NII
{
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // SpreadParticle
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    class _EngineInner CmdAngle : public PropertyCmd
    {
    public:
        CmdAngle() : PropertyCmd(N_PropertySpreadParticle_Angle, _T("angle"),
            _T("The angle up to which particles may vary in their initial direction "
            "from the emitters direction, in degrees."))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const SpreadParticle *>(own)->getAngle().mValue, out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            Radian temp;
            StrConv::conv(val, temp.mValue);
            static_cast<SpreadParticle *>(own)->setAngle(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            Radian * temp = (Radian *)out;
            *temp = static_cast<const SpreadParticle *>(own)->getAngle();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const void * in)
        {
            Radian * temp = (Radian *)in;
            static_cast<SpreadParticle *>(own)->setAngle(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdAngle();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv(0.0f, out);
        }
    };
    /// Command object for particle emitter  - see PropertyCmd
    class _EngineInner CmdColour : public PropertyCmd
    {
    public:
        CmdColour() : PropertyCmd(N_PropertySpreadParticle_Colour, _T("colour"), _T("The colour of emitted particles."))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const SpreadParticle *>(own)->getColour(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            Colour temp;
            StrConv::conv(val, temp);
            static_cast<SpreadParticle *>(own)->setColour(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            Colour * temp = (Colour *)out;
            *temp = static_cast<const SpreadParticle *>(own)->getColour();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const void * in)
        {
            Colour * temp = (Colour *)in;
            static_cast<SpreadParticle *>(own)->setColour(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdColour();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv(Colour::White, out);
        }
    };

    /// Command object for particle emitter  - see PropertyCmd
    class _EngineInner CmdColourRangeStart : public PropertyCmd
    {
    public:
        CmdColourRangeStart() : PropertyCmd(N_PropertySpreadParticle_MinColour, _T("colour_range_start"),
            _T("The start of a range of colours to be assigned to emitted particles."))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const SpreadParticle *>(own)->getMinColour(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            Colour temp;
            StrConv::conv(val, temp);
            static_cast<SpreadParticle *>(own)->setMinColour(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            Colour * temp = (Colour *)out;
            *temp = static_cast<const SpreadParticle *>(own)->getMinColour();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const void * in)
        {
            Colour * temp = (Colour *)in;
            static_cast<SpreadParticle *>(own)->setMinColour(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdColourRangeStart();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv(Colour::White, out);
        }
    };
    /// Command object for particle emitter  - see PropertyCmd
    class _EngineInner CmdColourRangeEnd : public PropertyCmd
    {
    public:
        CmdColourRangeEnd() : PropertyCmd(N_PropertySpreadParticle_MaxColour, _T("colour_range_end"),
            _T("The end of a range of colours to be assigned to emitted particles."))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const SpreadParticle *>(own)->getMaxColour(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            Colour temp;
            StrConv::conv(val, temp);
            static_cast<SpreadParticle *>(own)->setMaxColour(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            Colour * temp = (Colour *)out;
            *temp = static_cast<const SpreadParticle *>(own)->getMaxColour();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const void * in)
        {
            Colour * temp = (Colour *)in;
            static_cast<SpreadParticle *>(own)->setMaxColour(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdColourRangeEnd();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv(Colour::White, out);
        }
    };

    /// Command object for particle emitter  - see PropertyCmd
    class _EngineInner CmdDirection : public PropertyCmd
    {
    public:
        CmdDirection() : PropertyCmd(N_PropertySpreadParticle_Direction, _T("direction"),
            _T("The base direction of the emitter."))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const SpreadParticle *>(own)->getDirection(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            Vector3f temp;
            StrConv::conv(val, temp);
            static_cast<SpreadParticle *>(own)->setDirection(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            Vector3f * temp = (Vector3f *)out;
            *temp = static_cast<const SpreadParticle *>(own)->getDirection();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const void * in)
        {
            Vector3f * temp = (Vector3f *)in;
            static_cast<SpreadParticle *>(own)->setDirection(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdDirection();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T("0.0 0.0 0.0");
        }
    };

    /// Command object for particle emitter  - see PropertyCmd
    class _EngineInner CmdEmissionRate : public PropertyCmd
    {
    public:
        CmdEmissionRate() : PropertyCmd(N_PropertySpreadParticle_SpreadRate, _T("emission_rate"),
            _T("The number of particles emitted per second."))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const SpreadParticle *>(own)->getSpreadRate(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            NIIf temp;
            StrConv::conv(val, temp);
            static_cast<SpreadParticle *>(own)->setSpreadRate(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            NIIf * temp = (NIIf *)out;
            *temp = static_cast<const SpreadParticle *>(own)->getSpreadRate();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const void * in)
        {
            NIIf * temp = (NIIf *)in;
            static_cast<SpreadParticle *>(own)->setSpreadRate(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdEmissionRate();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv(10.0f, out);
        }
    };
    /// Command object for particle emitter  - see PropertyCmd
    class _EngineInner CmdVelocity : public PropertyCmd
    {
    public:
        CmdVelocity() : PropertyCmd(N_PropertySpreadParticle_SpreadSpeed, _T("velocity"),
            _T("The initial velocity to be assigned to every particle, in world units per second."))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const SpreadParticle *>(own)->getSpreadSpeed(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            NIIf temp;
            StrConv::conv(val, temp);
            static_cast<SpreadParticle *>(own)->setSpreadSpeed(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            NIIf * temp = (NIIf *)out;
            *temp = static_cast<const SpreadParticle *>(own)->getSpreadSpeed();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const void * in)
        {
            NIIf * temp = (NIIf *)in;
            static_cast<SpreadParticle *>(own)->setSpreadSpeed(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdVelocity();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv(1.0f, out);
        }
    };
    /// Command object for particle emitter  - see PropertyCmd
    class _EngineInner CmdMinVelocity : public PropertyCmd
    {
    public:
        CmdMinVelocity() : PropertyCmd(N_PropertySpreadParticle_MinSpreadSpeed, _T("velocity_min"),
            _T("The minimum initial velocity to be assigned to each particle."))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const SpreadParticle *>(own)->getMinSpreadSpeed(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String& val)
        {
            NIIf temp;
            StrConv::conv(val, temp);
            static_cast<SpreadParticle *>(own)->setMinSpreadSpeed(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            NIIf * temp = (NIIf *)out;
            *temp = static_cast<const SpreadParticle *>(own)->getMinSpreadSpeed();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const void * in)
        {
            NIIf * temp = (NIIf *)in;
            static_cast<SpreadParticle *>(own)->setMinSpreadSpeed(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdMinVelocity();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T("1.0");
        }
    };
    /// Command object for particle emitter  - see PropertyCmd
    class _EngineInner CmdMaxVelocity : public PropertyCmd
    {
    public:
        CmdMaxVelocity() : PropertyCmd(N_PropertySpreadParticle_MaxSpreadSpeed, _T("velocity_max"),
            _T("The maximum initial velocity to be assigned to each particle."))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const SpreadParticle *>(own)->getMaxSpreadSpeed(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            NIIf temp;
            StrConv::conv(val, temp);
            static_cast<SpreadParticle *>(own)->setMaxSpreadSpeed(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            NIIf * temp = (NIIf *)out;
            *temp = static_cast<const SpreadParticle *>(own)->getMaxSpreadSpeed();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const void * in)
        {
            NIIf * temp = (NIIf *)in;
            static_cast<SpreadParticle *>(own)->setMaxSpreadSpeed(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdMaxVelocity();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv(1.0f, out);
        }
    };

    /// Command object for particle emitter  - see PropertyCmd
    class _EngineInner CmdMinTTL : public PropertyCmd
    {
    public:
        CmdMinTTL() : PropertyCmd(N_PropertySpreadParticle_MinTTL, _T("time_to_live_min"),
            _T("The minimum lifetime of each particle in seconds.")){}

        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const SpreadParticle *>(own)->getMinTTL(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            Nui32 temp;
            StrConv::conv(val, temp);
            static_cast<SpreadParticle *>(own)->setMinTTL(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            Nui32 * temp = (Nui32 *)out;
            *temp = static_cast<const SpreadParticle *>(own)->getMinTTL();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const void * in)
        {
            Nui32 * temp = (Nui32 *)in;
            static_cast<SpreadParticle *>(own)->setMinTTL(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdMinTTL();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv(5.0f, out);
        }
    };
    /// Command object for particle emitter  - see PropertyCmd
    class _EngineInner CmdMaxTTL : public PropertyCmd
    {
    public:
        CmdMaxTTL() : PropertyCmd(N_PropertySpreadParticle_MaxTTL, _T("time_to_live_max"),
            _T("The maximum lifetime of each particle in seconds."))
        {
        }

        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const SpreadParticle *>(own)->getMaxTTL(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            Nui32 temp;
            StrConv::conv(val, temp);
            static_cast<SpreadParticle *>(own)->setMaxTTL(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            Nui32 * temp = (Nui32 *)out;
            *temp = static_cast<const SpreadParticle *>(own)->getMaxTTL();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const void * in)
        {
            Nui32 * temp = (Nui32 *)in;
            static_cast<SpreadParticle *>(own)->setMaxTTL(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdMaxTTL();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv((TimeDurMS)5, out);
        }
    };
    /// Command object for particle emitter  - see PropertyCmd
    class _EngineInner CmdPosition : public PropertyCmd
    {
    public:
        CmdPosition() : PropertyCmd(N_PropertySpreadParticle_Position, _T("position"),
            _T("The position of the emitter relative to the particle system center."))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const SpreadParticle *>(own)->getPos(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            Vector3f temp;
            StrConv::conv(val, temp);
            static_cast<SpreadParticle *>(own)->setPos(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            Vector3f * temp = (Vector3f *)out;
            *temp = static_cast<const SpreadParticle *>(own)->getPos();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const void * in)
        {
            Vector3f * temp = (Vector3f *)in;
            static_cast<SpreadParticle *>(own)->setPos(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdPosition();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv(Vector3f::ZERO, out);
        }
    };
    /// Command object for particle emitter  - see PropertyCmd
    class _EngineInner CmdDuration : public PropertyCmd
    {
    public:
        CmdDuration() : PropertyCmd(N_PropertySpreadParticle_Duration, _T("duration"),
            _T("The length of time in seconds which an emitter stays enabled for."))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const SpreadParticle *>(own)->getDuration(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            Nui32 temp;
            StrConv::conv(val, temp);
            static_cast<SpreadParticle*>(own)->setDuration(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            Nui32 * temp = (Nui32 *)out;
            *temp = static_cast<const SpreadParticle *>(own)->getDuration();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const void * in)
        {
            Nui32 * temp = (Nui32 *)in;
            static_cast<SpreadParticle*>(own)->setDuration(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdDuration();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T("0");
        }
    };
    /// Command object for particle emitter  - see PropertyCmd
    class _EngineInner CmdMinDuration : public PropertyCmd
    {
    public:
        CmdMinDuration() : PropertyCmd(N_PropertySpreadParticle_MinDuration, _T("duration_min"),
            _T("The minimum length of time in seconds which an emitter stays enabled for."))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const SpreadParticle *>(own)->getMinDuration(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            Nui32 temp;
            StrConv::conv(val, temp);
            static_cast<SpreadParticle *>(own)->setMinDuration(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            Nui32 * temp = (Nui32 *)out;
            *temp = static_cast<const SpreadParticle *>(own)->getMinDuration();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const void * in)
        {
            Nui32 * temp = (Nui32 *)in;
            static_cast<SpreadParticle *>(own)->setMinDuration(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdMinDuration();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T("0");
        }
    };
    /// Command object for particle emitter  - see PropertyCmd
    class _EngineInner CmdMaxDuration : public PropertyCmd
    {
    public:
        CmdMaxDuration() : PropertyCmd(N_PropertySpreadParticle_MaxDuration, _T("duration_max"),
            _T("The maximum length of time in seconds which an emitter stays enabled for."))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const SpreadParticle *>(own)->getMaxDuration(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            Nui32 temp;
            StrConv::conv(val, temp);
            static_cast<SpreadParticle *>(own)->setMaxDuration(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            Nui32 * temp = (Nui32 *)out;
            *temp = static_cast<const SpreadParticle *>(own)->getMaxDuration();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const void * in)
        {
            Nui32 * temp = (Nui32 *)in;
            static_cast<SpreadParticle *>(own)->setMaxDuration(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdMaxDuration();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T("0");
        }
    };
    /// Command object for particle emitter  - see PropertyCmd
    class _EngineInner CmdRepeatDelay : public PropertyCmd
    {
    public:
        CmdRepeatDelay() : PropertyCmd(N_PropertySpreadParticle_RepeatDelay, _T("repeat_delay"),
            _T("If set, after disabling an emitter will repeat (reenable) after this many seconds."))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const SpreadParticle *>(own)->getRepeatDelay(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            Nui32 temp;
            StrConv::conv(val, temp);
            static_cast<SpreadParticle *>(own)->setRepeatDelay(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            Nui32 * temp = (Nui32 *)out;
            *temp = static_cast<const SpreadParticle *>(own)->getRepeatDelay();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const void * in)
        {
            Nui32 * temp = (Nui32 *)in;
            static_cast<SpreadParticle *>(own)->setRepeatDelay(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdRepeatDelay();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T("0");
        }
    };
    /// Command object for particle emitter  - see PropertyCmd
    class _EngineInner CmdMinRepeatDelay : public PropertyCmd
    {
    public:
        CmdMinRepeatDelay() : PropertyCmd(N_PropertySpreadParticle_MinRepeatDelay, _T("repeat_delay_min"),
            _T("If set, after disabling an emitter will repeat (reenable) after this minimum number of seconds."))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const SpreadParticle *>(own)->getMinRepeatDelay(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            Nui32 temp;
            StrConv::conv(val, temp);
            static_cast<SpreadParticle *>(own)->setMinRepeatDelay(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            Nui32 * temp = (Nui32 *)out;
            *temp = static_cast<const SpreadParticle *>(own)->getMinRepeatDelay();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const void * in)
        {
            Nui32 * temp = (Nui32 *)in;
            static_cast<SpreadParticle *>(own)->setMinRepeatDelay(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdMinRepeatDelay();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T("0");
        }
    };
    /// Command object for particle emitter  - see PropertyCmd
    class _EngineInner CmdMaxRepeatDelay : public PropertyCmd
    {
    public:
        CmdMaxRepeatDelay() : PropertyCmd(N_PropertySpreadParticle_MaxRepeatDelay, _T("repeat_delay_max"),
            _T("If set, after disabling an emitter will repeat (reenable) after this maximum number of seconds."))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const SpreadParticle *>(own)->getMaxRepeatDelay(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            Nui32 temp;
            StrConv::conv(val, temp);
            static_cast<SpreadParticle *>(own)->setMaxRepeatDelay(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            Nui32 * temp = (Nui32 *)out;
            *temp = static_cast<const SpreadParticle *>(own)->getMaxRepeatDelay();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const void * in)
        {
            Nui32 * temp = (Nui32 *)in;
            static_cast<SpreadParticle *>(own)->setMaxRepeatDelay(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdMaxRepeatDelay();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T("0");
        }
    };
    /// Command object for particle emitter  - see PropertyCmd
    class _EngineInner CmdName : public PropertyCmd
    {
    public:
        CmdName() : PropertyCmd(N_PropertySpreadParticle_Name, _T("name"), _T("This is the name of the emitter")){}

        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            out = static_cast<const SpreadParticle *>(own)->getName();
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & in)
        {
            static_cast<SpreadParticle *>(own)->setName(in);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            String * temp = (String *)out;
            *temp = static_cast<const SpreadParticle *>(own)->getName();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const void * in)
        {
            String * temp = (String *)in;
            static_cast<SpreadParticle *>(own)->setName(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdName();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = StrUtil::WBLANK;
        }
    };

    /// Command object for particle emitter  - see PropertyCmd
    class _EngineInner CmdEmittedEmitter : public PropertyCmd
    {
    public:
        CmdEmittedEmitter() : PropertyCmd(N_PropertySpreadParticle_SpreadName, _T("emit_emitter"),
            _T("If set, this emitter will emit other emitters instead of visual particles"))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            out = static_cast<const SpreadParticle *>(own)->getSpreadName();
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            static_cast<SpreadParticle *>(own)->setSpreadName(val);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            String * temp = (String *)out;
            *temp = static_cast<const SpreadParticle *>(own)->getSpreadName();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const void * in)
        {
            String * temp = (String *)in;
            static_cast<SpreadParticle *>(own)->setSpreadName(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdEmittedEmitter();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T("");
        }
    };
    /// Command object for particle emitter  - see PropertyCmd
    class _EngineInner CmdUp : public PropertyCmd
    {
    public:
        CmdUp() : PropertyCmd(N_PropertySpreadParticle_Up, _T("up"), _T("The up vector of the emitter."))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const SpreadParticle *>(own)->getUp(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            Vector3f temp;
            StrConv::conv(val, temp);
            static_cast<SpreadParticle *>(own)->setUp(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            Vector3f * temp = (Vector3f *)out;
            *temp = static_cast<const SpreadParticle *>(own)->getUp();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const void * in)
        {
            Vector3f * temp = (Vector3f *)in;
            static_cast<SpreadParticle *>(own)->setUp(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdUp();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T("0.0 0.0 0.0");
        }
    };
    //-----------------------------------------------------------------------
    SpreadParticle::SpreadParticle(ParticleSpace * psys, LangID lid) :
        PropertyCmdObj(N_CmdObj_SpreadParticle, lid),
        mEnableTime(0),
        mMinDur(0),
        mMaxDur(0),
        mRemainDur(0),
        mMinRepeatDelay(0),
        mMaxRepeatDelay(0),
        mRemainDelay(0)
    {
        setupCmd(N_CmdObj_SpreadParticle);
        // Reasonable defaults
        mAngle = 0;
        setDirection(Vector3f::X);
        mSpreadRate = 10;
        mMaxSpeed = mMinSpeed = 1;
        mMaxTTL = mMinTTL = 5;
        mPosition = Vector3f::ZERO;
        mMinColour = mMaxColour = Colour::White;
        mName = StrUtil::WBLANK;
        mSpreadName = StrUtil::WBLANK;
        mEnabled = true;
        mCtrl = psys;
    }
    //-----------------------------------------------------------------------
    SpreadParticle::~SpreadParticle()
    {
    }
    //-----------------------------------------------------------------------
    void SpreadParticle::setPos(const Vector3f & pos)
    {
        mPosition = pos;
    }
    //-----------------------------------------------------------------------
    void SpreadParticle::setDirection(const Vector3f & dir)
    {
        mDirection = dir;
        mDirection.normalise();
        // Generate a default up vector.
        mUp = mDirection.vertical();
        mUp.normalise();
    }
    //-----------------------------------------------------------------------
    void SpreadParticle::setUp(const Vector3f & up)
    {
        mUp = up;
        mUp.normalise();
    }
    //-----------------------------------------------------------------------
    void SpreadParticle::setAngle(const Radian & angle)
    {
        // Store as radians for efficiency
        mAngle = angle;
    }
    //-----------------------------------------------------------------------
    void SpreadParticle::setSpreadSpeed(NIIf speed)
    {
        mMinSpeed = mMaxSpeed = speed;
    }
    //-----------------------------------------------------------------------
    void SpreadParticle::setMinSpreadSpeed(NIIf min)
    {
        mMinSpeed = min;
    }
    //-----------------------------------------------------------------------
    void SpreadParticle::setMaxSpreadSpeed(NIIf max)
    {
        mMaxSpeed = max;
    }
    //-----------------------------------------------------------------------
    void SpreadParticle::setSpreadRate(NIIf rate)
    {
        mSpreadRate = rate;
    }
    //-----------------------------------------------------------------------
    void SpreadParticle::setColour(const Colour & c)
    {
        mMinColour = mMaxColour = c;
    }
    //-----------------------------------------------------------------------
    const String & SpreadParticle::getName() const
    {
        return mName;
    }
    //-----------------------------------------------------------------------
    void SpreadParticle::setName(const String & name)
    {
        mName = name;
    }
    //-----------------------------------------------------------------------
    void SpreadParticle::setSpreadName(const String & name)
    {
        mSpreadName = name;
    }
    //-----------------------------------------------------------------------
    const String & SpreadParticle::getSpreadName() const
    {
        return mSpreadName;
    }
    //-----------------------------------------------------------------------
    void SpreadParticle::initDirectionImpl(Particle * spread)
    {
        if(mAngle != Radian(0))
        {
            Quaternion q;
            Vector3f newUp;
            // Randomise angle
            Radian angle = Math::randomf() * mAngle;
            // Randomise direction
            if(mUp == Vector3f::ZERO)
            {
                // 产生向上
                newUp = mDirection.vertical();
            }
            else
            {
                newUp = mUp;
            }
            q.from(Radian(Math::randomf() * Math::PI2X), mDirection);
            newUp = q * newUp;
            //最后由提供的的角度旋转,实施随机行动
            q.from(angle, newUp);
            spread->mDirection = q * mDirection;
        }
        else
        {
            // Constant angle
            spread->mDirection = mDirection;
        }

        // Don't normalise, we can assume that it will still be a unit vector since
        // both direction and 'up' are.
    }
    //-----------------------------------------------------------------------
    void SpreadParticle::initSpeedImpl(Particle * spread)
    {
        NIIf scalar;
        if(mMinSpeed != mMaxSpeed)
        {
            scalar = mMinSpeed + (Math::randomf() * (mMaxSpeed - mMinSpeed));
        }
        else
        {
            scalar = mMinSpeed;
        }

        spread->mDirection *= scalar;
    }
    //-----------------------------------------------------------------------
    void SpreadParticle::initTTLImpl(Particle * spread)
    {
        if(mMaxTTL != mMinTTL)
        {
            spread->mRemainTTL = spread->mTotalTTL = mMinTTL + (Math::randomi() * (mMaxTTL - mMinTTL));
        }
        else
        {
            spread->mRemainTTL = spread->mTotalTTL = mMinTTL;
        }
    }
    //-----------------------------------------------------------------------
    void SpreadParticle::initColourImpl(Particle * spread)
    {
        if(mMinColour != mMaxColour)
        {
            // Randomise
            //Real t = Math::randomf();
            spread->mColour.r = mMinColour.r + (Math::randomf() * (mMaxColour.r - mMinColour.r));
            spread->mColour.g = mMinColour.g + (Math::randomf() * (mMaxColour.g - mMinColour.g));
            spread->mColour.b = mMinColour.b + (Math::randomf() * (mMaxColour.b - mMinColour.b));
            spread->mColour.a = mMinColour.a + (Math::randomf() * (mMaxColour.a - mMinColour.a));
        }
        else
        {
            spread->mColour = mMinColour;
        }
    }
    //-----------------------------------------------------------------------
    bool SpreadParticle::initCmd(PropertyCmdSet * dest)
    {
        dest->add(N_new CmdAngle());
        dest->add(N_new CmdColour());
        dest->add(N_new CmdColourRangeStart());
        dest->add(N_new CmdColourRangeEnd());
        dest->add(N_new CmdDirection());
        dest->add(N_new CmdUp());
        dest->add(N_new CmdEmissionRate());
        dest->add(N_new CmdMaxTTL());
        dest->add(N_new CmdMaxVelocity());
        dest->add(N_new CmdMinTTL());
        dest->add(N_new CmdMinVelocity());
        dest->add(N_new CmdPosition());
        dest->add(N_new CmdVelocity());
        dest->add(N_new CmdDuration());
        dest->add(N_new CmdMinDuration());
        dest->add(N_new CmdMaxDuration());
        dest->add(N_new CmdRepeatDelay());
        dest->add(N_new CmdMinRepeatDelay());
        dest->add(N_new CmdMaxRepeatDelay());
        dest->add(N_new CmdName());
        dest->add(N_new CmdEmittedEmitter());
        return true;
    }
    //-----------------------------------------------------------------------
    void SpreadParticle::initImpl(Particle * spread)
    {
        // Initialise size in case it's been altered
        spread->inheritSize(true);
    }
    //-----------------------------------------------------------------------
    NCount SpreadParticle::getSpreadImpl(TimeDurMS cost)
    {
        if(mEnabled)
        {
            NCount intRequest = mSpreadRate * (cost / 1000);

            // Check duration
            if(mMaxDur)
            {
                mRemainDur -= cost;
                if(mRemainDur <= 0)
                {
                    // Disable, duration is out (takes effect next time)
                    setEnable(false);
                }
            }
            return intRequest;
        }
        else
        {
            // Check repeat
            if(mMaxRepeatDelay)
            {
                mRemainDelay -= cost;
                if(mRemainDelay <= 0)
                {
                    // Enable, repeat delay is out (takes effect next time)
                    setEnable(true);
                }
            }
            if(mEnableTime)
            {
                mEnableTime -= cost;
                if(mEnableTime <= 0)
                {
                    setEnable(true);
                    mEnableTime = 0;
                }
            }
            return 0;
        }
    }
    //-----------------------------------------------------------------------
    NIIf SpreadParticle::getSpreadSpeed() const
    {
        return mMinSpeed;
    }
    //-----------------------------------------------------------------------
    void SpreadParticle::setMinTTL(TimeDurMS min)
    {
        mMinTTL = min;
    }
    //-----------------------------------------------------------------------
    void SpreadParticle::setMaxTTL(TimeDurMS max)
    {
        mMaxTTL = max;
    }
    //-----------------------------------------------------------------------
    const Colour& SpreadParticle::getColour() const
    {
        return mMinColour;
    }
    //-----------------------------------------------------------------------
    void SpreadParticle::setMinColour(const Colour & val)
    {
        mMinColour = val;
    }
    //-----------------------------------------------------------------------
    void SpreadParticle::setMaxColour(const Colour & val)
    {
        mMaxColour = val;
    }
    //-----------------------------------------------------------------------
    void SpreadParticle::setEnable(bool b)
    {
        mEnabled = b;
        // Reset duration & repeat
        updateCostTime();
    }
    //-----------------------------------------------------------------------
    void SpreadParticle::setEnable(TimeDurMS ms)
    {
        setEnable(false);
        mEnableTime = ms;
    }
    //-----------------------------------------------------------------------
    void SpreadParticle::setDuration(TimeDurMS ms)
    {
        mMinDur = ms;
        mMaxDur = ms;
        updateCostTime();
    }
    //-----------------------------------------------------------------------
    TimeDurMS SpreadParticle::getDuration() const
    {
        return mMinDur;
    }
    //-----------------------------------------------------------------------
    void SpreadParticle::setMinDuration(TimeDurMS min)
    {
        mMinDur = min;
        updateCostTime();
    }
    //-----------------------------------------------------------------------
    void SpreadParticle::setMaxDuration(TimeDurMS max)
    {
        mMaxDur = max;
        updateCostTime();
    }
    //-----------------------------------------------------------------------
    void SpreadParticle::updateCostTime()
    {
        if(mEnabled)
        {
            if(mMinDur == mMaxDur)
            {
                mRemainDur = mMinDur;
            }
            else
            {
                mRemainDur = Math::randomi(mMinDur, mMaxDur);
            }
        }
        else
        {
            // Reset repeat
            if(mMinRepeatDelay == mMaxRepeatDelay)
            {
                mRemainDelay = mMinRepeatDelay;
            }
            else
            {
                mRemainDelay = Math::randomi(mMaxRepeatDelay, mMinRepeatDelay);
            }
        }
    }
    //-----------------------------------------------------------------------
    void SpreadParticle::setRepeatDelay(TimeDurMS delay)
    {
        mMinRepeatDelay = delay;
        mMaxRepeatDelay = delay;
        updateCostTime();
    }
    //-----------------------------------------------------------------------
    TimeDurMS SpreadParticle::getRepeatDelay() const
    {
        return mMinRepeatDelay;
    }
    //-----------------------------------------------------------------------
    void SpreadParticle::setMinRepeatDelay(TimeDurMS min)
    {
        mMinRepeatDelay = min;
        updateCostTime();
    }
    //-----------------------------------------------------------------------
    void SpreadParticle::setMaxRepeatDelay(TimeDurMS max)
    {
        mMaxRepeatDelay = max;
        updateCostTime();
    }
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // SpreadParticleFactory
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    SpreadParticleFactory::~SpreadParticleFactory()
    {
        vector<SpreadParticle *>::type::iterator i, iend = mList.end();
        for(i = mList.begin(); i != iend; ++i)
        {
            N_delete (*i);
        }

        mList.clear();

    }
    //-----------------------------------------------------------------------
    void SpreadParticleFactory::destroy(SpreadParticle * e)
    {
        vector<SpreadParticle*>::type::iterator i, iend = mList.end();
        for(i = mList.begin(); i != iend; ++i)
        {
            if((*i) == e)
            {
                N_delete e;
                mList.erase(i);
                break;
            }
        }
    }
    //------------------------------------------------------------------------
}