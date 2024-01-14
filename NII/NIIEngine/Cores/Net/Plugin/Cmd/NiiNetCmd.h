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
#if _RAKNET_SUPPORT_RakNetCommandParser == 1

#ifndef __RAKNET_COMMAND_PARSER
#define __RAKNET_COMMAND_PARSER

#include "NiiNetPreInclude.h"
#include "NiiNetCmdBase.h"

namespace NII
{
namespace NII_NET
{
    /// \brief This allows a console client to call most of the functions in NetworkSys
    class _EngineAPI Cmd : public CmdBase
    {
    public:
        Cmd();
        ~Cmd();

        /// Given \a command with parameters \a parameterList , do whatever processing you wish.
        /// \param[in] command The command to process
        /// \param[in] numParameters How many parameters were passed along with the command
        /// \param[in] parameterList The list of parameters.  parameterList[0] is the first parameter and so on.
        /// \param[in] transport The transport interface we can use to write to
        /// \param[in] address The player that sent this command.
        /// \param[in] originalString The string that was actually sent over the network, in case you want to do your own parsing
        bool OnCommand(const char * command, Nui32 numParameters, char ** parameterList,
            TransportInterface * transport, const Address & address, const char * originalString);

        /// You are responsible for overriding this function and returning a static string, which will identifier your parser.
        /// This should return a static string
        /// \return The name that you return.
        const char * GetName(void) const;

        /// A callback for when you are expected to send a brief description of your parser to \a address
        /// \param[in] transport The transport interface we can use to write to
        /// \param[in] address The player that requested help.
        void SendHelp(TransportInterface * transport, const Address & address);

        /// Records the instance of NetworkSys to perform the desired commands on
        /// \param[in] link The NetworkSys instance, or a derived class (e.g. NetworkSys or NetworkSys)
        void setPrc(LinkBase * link);
    protected:
        /// Which instance of NetworkSys we are working on.  Set from setPrc()
        LinkBase * mPeer;
    };
}
}
#endif
#endif