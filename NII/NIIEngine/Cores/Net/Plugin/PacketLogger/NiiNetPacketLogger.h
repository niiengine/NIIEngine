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

#include "NativeFeatureIncludes.h"
#if _RAKNET_SUPPORT_PacketLogger==1

#ifndef __PACKET_LOGGER_H
#define __PACKET_LOGGER_H

#include "NiiNetPreInclude.h"
#include "NiiNetCommon.h"
#include "NiiNetPlugin.h"

namespace NII
{
namespace NII_NET
{
    /// \defgroup PACKETLOGGER_GROUP PacketLogger
    /// \brief Print out incoming messages to a target destination
    /// \details
    /// \ingroup PLUGINS_GROUP

    /// \brief Writes incoming and outgoing messages to the screen.
    /// This will write all incoming and outgoing messages to the console window, or to a file if you override it and give it this functionality.
    /// \ingroup PACKETLOGGER_GROUP
    class _EngineAPI PacketLogger : public Plugin : public PacketListener
    {
    public:
        PacketLogger();
        virtual ~PacketLogger();

        // Translate the supplied parameters into an output line - overloaded version that takes a MessageIdentifier
        // and translates it into a string (numeric or textual representation based on printId); this calls the
        // second version which takes a const char* argument for the messageIdentifier
        virtual void FormatLine(char * into, const char * dir, const char * type, Nui32 makesureMark, Nui32 frame, Nui8 messageIdentifier, 
            NBitCount bitLen, Nui64 time, const Address & local, const Address & remote, Nui32 splitid, Nui32 splitindex, Nui32 splitcount, Nui32 order);

        virtual void FormatLine(char * into, const char * dir, const char * type, Nui32 makesureMark, Nui32 frame, const char * idToPrint, 
            NBitCount bitLen, Nui64 time, const Address & local, const Address & remote, Nui32 splitid, Nui32 splitindex, Nui32 splitcount, Nui32 order);

        /// Events on low level sends and receives.  These functions may be called from different threads at the same time.
        //virtual void onRawSend(const Nui8 * data, NBitCount count, Address * remote);
        //virtual void onRawReceive(const Nui8 * data, NBitCount count, Address * remote);
        //virtual void onReceiveLocal(const Nui8 * data, NBitCount count, Address * remote);

        virtual void onReport(const char * error, NBitCount count, Address * remote, bool isError);
        //virtual void onReceivePacket(Packet * packet, Nui32 frameNumber, Address * remote, TimeDurMS time, int isSend);
        //virtual void onReceiveAck(Nui32 ackmark, Address * remote, TimeDurMS time);

        /// Logs out a header for all the data
        virtual void LogHeader();

        /// Override this to log strings to wherever.  Log should be threadsafe
        virtual void WriteLog(const char * str);

        // Write informational messages
        virtual void WriteMiscellaneous(const char * type, const char * msg);

        // Set to true to print ID_* instead of numbers
        virtual void SetPrintID(bool print);
        // Print or hide acks (clears up the screen not to print them but is worse for debugging)
        virtual void SetPrintAcks(bool print);

        /// Prepend this string to output logs.
        virtual void SetPrefix(const char * _prefix);

        /// Append this string to output logs. (newline is useful here)
        virtual void SetSuffix(const char * _suffix);
        static const char * BaseIDTOString(Nui8 Id);

        /// Log the direct sends and receives or not. Default true
        void SetLogDirectMessages(bool send);
    protected:
        /// @copydetails Plugin::isRawProcess
        //virtual bool isRawProcess() const {return true;}
        
        const char * IDTOString(Nui8 Id);
        
        virtual void AddToLog(const char * str);
        
        // Users should override this
        virtual const char * UserIDTOString(Nui8 Id);

        void GetLocalTime(char buffer[128]);
	protected:
        bool logDirectMessages;
        bool printId, printAcks;
        char prefix[256];
        char suffix[256];
    };
}
}
#endif
#endif