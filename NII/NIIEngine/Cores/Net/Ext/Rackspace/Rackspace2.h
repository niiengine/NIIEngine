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

#if  _RAKNET_SUPPORT_TCPInterface == 1

#include "Export.h"
#include "NiiNetCommon.h"
#include "DS_Queue.h"
#include "RakString.h"
#include "jansson.h"

#ifndef __RACKSPACE_2_H
#define __RACKSPACE_2_H

namespace NII
{
namespace NII_NET
{
	class NetworkObj;
	class HTTP;

	enum Rackspace2ResponseCode
	{
		R2RC_AUTHENTICATED,
		R2RC_GOT_DOMAINS,
		R2RC_GOT_RECORDS,
		R2RC_GOT_SERVERS,
		R2RC_GOT_IMAGES,
		R2RC_NO_CONTENT,
		R2RC_BAD_JSON,
		R2RC_UNAUTHORIZED,
		R2RC_404_NOT_FOUND,
		R2RC_UNKNOWN,
	};

	/// \brief Callback interface to receive the results of operations
	class RAK_DLL_EXPORT Rackspace2EventCallback
	{
	public:
		virtual void OnTCPFailure(void)=0;
		virtual void OnTransmissionFailed(HTTP *httpConnection2, RakString postStr, RakString authURLDomain)=0;
		virtual void OnResponse(Rackspace2ResponseCode r2rc, RakString responseReceived, int contentOffset)=0;
		virtual void OnEmptyResponse(RakString stringTransmitted)=0;
		virtual void OnMessage(const char *message, RakString responseReceived, RakString stringTransmitted, int contentOffset)=0;
	};

	/// \brief Version 2 of the code that uses the NetworkObj class to communicate with the Rackspace API servers
	/// Works with their "next-gen" API as of 2012
	/// API operations are hidden at http://docs.rackspace.com/servers/api/v2/cs-devguide/content/ch_api_operations.html
	/// \pre Compile RakNet with N_OpenSSLSupport set to 1
	/// Maintains its own NetworkObj class
	class RAK_DLL_EXPORT Rackspace2
	{
	public:
		Rackspace2();
		~Rackspace2();

		// Call periodically
		void update();

		/// Sets the callback to be used when an operation completes
		void SetEventCallback(Rackspace2EventCallback *callback);

		int GetCloudAccountNumber(void) const;
		const char *GetAuthToken(void) const;

		/// \brief Authenticate with Rackspace servers, required before executing any commands.
		/// \details All requests to authenticate and operate against Cloud Servers are performed using SSL over HTTP (HTTPS) on TCP port 443.
		/// TODO - Will reauthenticate automatically as needed
		/// \param[in] authenticationURL See http://docs.rackspace.com/cdns/api/v1.0/cdns-devguide/content/Authentication-d1e647.html
		/// \param[in] rackspaceCloudUsername Username you registered with Rackspace on their website
		/// \param[in] apiAccessKey Obtain your API access key from the Rackspace Cloud Control Panel in the Your Account API Access section.
		void Authenticate(const char *authenticationURL, const char *rackspaceCloudUsername, const char *apiAccessKey);

		enum OpType
		{
			OT_POST,
			OT_PUT,
			OT_GET,
			OT_DELETE,
		};

		/// \param[in] URL Path to command, for example https://identity.api.rackspacecloud.com/v2.0/tokens
		/// \param[in] opType Type of operation to perform
		/// \param[in] data If the operation requires data, put it here
		/// \param[in] setAuthToken true to automatically set the auth token for the operation. I believe this should be true for everything except authentication itself
		void AddOperation(RakString URL, OpType opType, json_t *data, bool setAuthToken);

		struct Operation
		{
			RakString URL;
			bool isPost;
			json_t *data;
		};

	protected:
		void AuthenticateInt(const char *authenticationURL, const char *rackspaceCloudUsername, const char *apiAccessKey);
		void Reauthenticate(void);

		char X_Auth_Token[128];
		Rackspace2EventCallback *eventCallback;
		DataStructures::Queue<Operation> operations;
		NetworkObj *tcp;
		HTTP *httpConnection2;
		int cloudAccountNumber;
		RakString lastAuthenticationURL;
		RakString lastRackspaceCloudUsername;
		RakString lastApiAccessKey;
		bool reexecuteLastRequestOnAuth;

		RakString __addOpLast_URL;
		OpType __addOpLast_isPost;
		RakString __addOpLast_dataAsStr;

	};
}
}
#endif
#endif