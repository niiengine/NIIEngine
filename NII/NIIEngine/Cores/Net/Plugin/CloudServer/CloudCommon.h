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
#if _RAKNET_SUPPORT_CloudClient==1 || _RAKNET_SUPPORT_CloudServer==1

#ifndef __CLOUD_COMMON_H
#define __CLOUD_COMMON_H

#include "NiiPreInclude.h"
#include "NiiNetCommon.h"
#include "RakString.h"

namespace NII
{
namespace NII_NET
{
    struct CloudQueryRow;

    /// Allocates CloudQueryRow and the row data. Override to use derived classes or different allocators
    /// \ingroup CLOUD_GROUP
    class _EngineAPI CloudAllocator
    {
    public:
        CloudAllocator()
        {
        }
        virtual ~CloudAllocator()
        {
        }

        /// \brief Allocate a row
        virtual CloudQueryRow * AllocateCloudQueryRow(void);
        /// \brief Free a row
        virtual void DeallocateCloudQueryRow(CloudQueryRow * row);
        /// \brief Allocate CloudQueryRow::mData
        virtual Nui8 * AllocateRowData(Nui32 bytesNeededForData);
        /// \brief Free CloudQueryRow::mData
        virtual void DeallocateRowData(void * data);
    };

    /// Serves as a key to identify data uploaded to or queried from the server.
    /// \ingroup CLOUD_GROUP
    struct _EngineAPI CloudKey
    {
        CloudKey()
        {
        }
        CloudKey(RakString _primaryKey, Nui32 _secondaryKey) :
            primaryKey(_primaryKey),
            secondaryKey(_secondaryKey)
        {
        }
        ~CloudKey()
        {
        }

        /**
        @version NIIEngine 3.0.0
        */
        void write(NetSerializer * in);

        /**
        @version NIIEngine 3.0.0
        */
        void read(NetSerializer * out);

        /// Identifies the primary key. This is intended to be a major category, such as the name of the application
        /// Must be non-empty
        RakString primaryKey;

        /// Identifies the secondary key. This is intended to be a subcategory enumeration, such as PLAYER_LIST or RUNNING_SCORES
        Nui32 secondaryKey;
    };

    /// \internal
    int CloudKeyComp(const CloudKey & key, const CloudKey & data);

    /// Data members used to query the cloud
    /// \ingroup CLOUD_GROUP
    struct _EngineAPI CloudQuery
    {
        CloudQuery()
        {
            startingRowIndex = 0;
            maxRowsToReturn = 0;
            subscribeToResults = false;
        }

        /// List of keys to query. Must be at least of length 1.
        /// This query is run on uploads from all clients, and those that match the combination of primaryKey and secondaryKey are potentially returned
        /// If you pass more than one key at a time, the results are concatenated so if you need to differentiate between queries then send two different queries
        DataStructures::List<CloudKey> keys;

        /// If limiting the number of rows to return, this is the starting offset into the list. Has no effect unless maxRowsToReturn is > 0
        Nui32 startingRowIndex;

        /// Maximum number of rows to return. Actual number may still be less than this. Pass 0 to mean no-limit.
        Nui32 maxRowsToReturn;

        /// If true, automatically get updates as the results returned to you change. Unsubscribe with CloudMemoryClient::Unsubscribe()
        bool subscribeToResults;

        /// \internal
        void write(NetSerializer * in);

        void read(NetSerializer * out);
    };

    /// \ingroup CLOUD_GROUP
    struct _EngineAPI CloudQueryRow
    {
        /// Key used to identify this data
        CloudKey key;

        /// Data uploaded
        Nui8 * mData;

        /// Length of data uploaded
        Nui32 mLength;

        /// System address of server that is holding this data, and the client is connected to
        Address * serverSystemAddress;

        /// System address of client that uploaded this data
        Address * clientSystemAddress;

        /// UniqueID of server that is holding this data, and the client is connected to
        UniqueID serverGUID;

        /// UniqueID of client that uploaded this data
        UniqueID clientGUID;

        /// \internal
        void write(NetSerializer * in, CloudAllocator * allocator);

        void read(NetSerializer * out, CloudAllocator * allocator);
    };

    /// \ingroup CLOUD_GROUP
    struct _EngineAPI CloudQueryResult
    {
        /// Query originally passed to Download()
        CloudQuery cloudQuery;

        /// Results returned from query. If there were multiple keys in CloudQuery::keys then see resultKeyIndices
        DataStructures::List<CloudQueryRow *> rowsReturned;

        /// If there were multiple keys in CloudQuery::keys, then each key is processed in order and the result concatenated to rowsReturned
        /// The starting index of each query is written to resultKeyIndices
        /// For example, if CloudQuery::keys had 4 keys, returning 3 rows, 0, rows, 5 rows, and 12 rows then
        /// resultKeyIndices would be 0, 3, 3, 8
        DataStructures::List<Nui32> resultKeyIndices;

        /// Whatever was passed to CloudClient::Get() as CloudQuery::subscribeToResults
        bool subscribeToResults;

        /// \internal
        void write(NetSerializer * in, CloudAllocator * allocator);
        void read(NetSerializer * out, CloudAllocator * allocator);

        /// \internal
        void writeHeader(NetSerializer * in);
        void readHeader(NetSerializer * out);
        /// \internal
        void writeNumRows(Nui32 & numRows, NetSerializer * in);
        void readNumRows(Nui32 & numRows, NetSerializer * out);
        /// \internal
        void writeCloudQueryRows(Nui32 & numRows, NetSerializer * in, CloudAllocator * allocator);
        void readCloudQueryRows(Nui32 & numRows, NetSerializer * out, CloudAllocator * allocator);
    };
}
}
#endif
#endif // #if _RAKNET_SUPPORT_CloudClient==1 || _RAKNET_SUPPORT_CloudServer==1