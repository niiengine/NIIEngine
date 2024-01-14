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
#if _RAKNET_SUPPORT_StatisticsHistory == 1

#ifndef __STATISTICS_HISTORY_H
#define __STATISTICS_HISTORY_H

#include "NiiNetPreInclude.h"
#include "NiiNetPlugin.h"
#include "DS_List.h"
#include "NiiNetCommon.h"
#include "DS_OrderedList.h"
#include "RakString.h"
#include "DS_Queue.h"
#include "DS_Hash.h"

namespace NII
{
namespace NII_NET
{
    // Type used to track values. If needed, change to NIId and recompile
    typedef NIId SHValueType;
    #define SH_TYPE_MAX DBL_MAX

    /// \brief Input numerical values over time. Get sum, average, highest, lowest, standard deviation on recent or all-time values
    class _EngineAPI StatisticsHistory
    {
    public:
        enum SHErrorCode
        {
            SH_OK,
            SH_UKNOWN_OBJECT,
            SH_UKNOWN_KEY,
            SH_INVALID_PARAMETER,
        };

        enum SHSortOperation
        {
            SH_DO_NOT_SORT,

            SH_SORT_BY_RECENT_SUM_ASCENDING,
            SH_SORT_BY_RECENT_SUM_DESCENDING,
            SH_SORT_BY_LONG_TERM_SUM_ASCENDING,
            SH_SORT_BY_LONG_TERM_SUM_DESCENDING,
            SH_SORT_BY_RECENT_SUM_OF_SQUARES_ASCENDING,
            SH_SORT_BY_RECENT_SUM_OF_SQUARES_DESCENDING,
            SH_SORT_BY_RECENT_AVERAGE_ASCENDING,
            SH_SORT_BY_RECENT_AVERAGE_DESCENDING,
            SH_SORT_BY_LONG_TERM_AVERAGE_ASCENDING,
            SH_SORT_BY_LONG_TERM_AVERAGE_DESCENDING,
            SH_SORT_BY_RECENT_HIGHEST_ASCENDING,
            SH_SORT_BY_RECENT_HIGHEST_DESCENDING,
            SH_SORT_BY_RECENT_LOWEST_ASCENDING,
            SH_SORT_BY_RECENT_LOWEST_DESCENDING,
            SH_SORT_BY_LONG_TERM_HIGHEST_ASCENDING,
            SH_SORT_BY_LONG_TERM_HIGHEST_DESCENDING,
            SH_SORT_BY_LONG_TERM_LOWEST_ASCENDING,
            SH_SORT_BY_LONG_TERM_LOWEST_DESCENDING,
        };

        enum SHDataCategory
        {
            /// Insert values from one set into the other set, in time order
            /// Values at the same time end up in the final set twice
            /// Use when you have additional data points to add to a graph
            DC_DISCRETE,

            /// Add values from one set to values from the other set, at corresponding times
            /// If value at time t does not exist in the other set, linearly extrapolate value for other set based on nearest two data points
            /// longTerm* values are unknown using this method
            /// Use to add two graphs together
            DC_CONTINUOUS
        };

        struct TimeAndValue;
        struct TimeAndValueQueue;

        struct TrackedObjectData
        {
            TrackedObjectData();
            TrackedObjectData(Nui64 _objectId, int _objectType, void *_userData);
            Nui64 objectId;
            int objectType;
            void *userData;
        };

        StatisticsHistory();
        virtual ~StatisticsHistory();
        void SetDefaultTimeToTrack(TimeDurMS defaultTimeToTrack);
        TimeDurMS GetDefaultTimeToTrack(void) const;
        bool AddObject(TrackedObjectData tod);
        bool RemoveObject(Nui64 objectId, void **userData);
        void RemoveObjectAtIndex(Nui32 index);
        void Clear(void);
        Nui32 GetObjectCount(void) const;
        StatisticsHistory::TrackedObjectData * GetObjectAtIndex(Nui32 index) const;
        Nui32 GetObjectIndex(Nui64 objectId) const;
        bool AddValueByObjectID(Nui64 objectId, RakString key, SHValueType val, TimeDurMS curTime, bool combineEqualTimes);
        void AddValueByIndex(Nui32 index, RakString key, SHValueType val, TimeDurMS curTime, bool combineEqualTimes);
        SHErrorCode GetHistoryForKey(Nui64 objectId, RakString key, TimeAndValueQueue **values, TimeDurMS curTime) const;
        bool GetHistorySorted(Nui64 objectId, SHSortOperation sortType, DataStructures::List<TimeAndValueQueue *> &values) const;
        void MergeAllObjectsOnKey(RakString key, TimeAndValueQueue *tavqOutput, SHDataCategory dataCategory) const;
        void GetUniqueKeyList(DataStructures::List<RakString> &keys);

        struct TimeAndValue
        {
            TimeDurMS time;
            SHValueType val;
        };

        struct TimeAndValueQueue
        {
            TimeAndValueQueue();
            ~TimeAndValueQueue();

            DataStructures::Queue<TimeAndValue> values;

            TimeDurMS timeToTrackValues;
            RakString key;

            SHValueType recentSum;
            SHValueType recentSumOfSquares;
            SHValueType longTermSum;
            SHValueType longTermCount;
            SHValueType longTermLowest;
            SHValueType longTermHighest;

            void SetTimeToTrackValues(TimeDurMS t);
            TimeDurMS GetTimeToTrackValues(void) const;
            SHValueType GetRecentSum(void) const;
            SHValueType GetRecentSumOfSquares(void) const;
            SHValueType GetLongTermSum(void) const;
            SHValueType GetRecentAverage(void) const;
            SHValueType GetRecentLowest(void) const;
            SHValueType GetRecentHighest(void) const;
            SHValueType GetRecentStandardDeviation(void) const;
            SHValueType GetLongTermAverage(void) const;
            SHValueType GetLongTermLowest(void) const;
            SHValueType GetLongTermHighest(void) const;
            SHValueType GetSumSinceTime(TimeDurMS t) const;
            TimeDurMS GetTimeRange(void) const;

            // Merge two sets to output
            static void MergeSets( const TimeAndValueQueue *lhs, SHDataCategory lhsDataCategory, const TimeAndValueQueue *rhs, SHDataCategory rhsDataCategory, TimeAndValueQueue *output );

            // Shrink or expand a sample set to the approximate number given
            // DC_DISCRETE will produce a histogram (sum) while DC_CONTINUOUS will produce an average
            void ResizeSampleSet( int approximateSamples, DataStructures::Queue<StatisticsHistory::TimeAndValue> &blendedSamples, SHDataCategory dataCategory, TimeDurMS timeClipStart=0, TimeDurMS timeClipEnd=0 );

            // Clear out all values
            void Clear(void);

            TimeAndValueQueue& operator = ( const TimeAndValueQueue& input );

            /// \internal
            void CullExpiredValues(TimeDurMS curTime);
            /// \internal
            static SHValueType Interpolate(TimeAndValue t1, TimeAndValue t2, TimeDurMS time);
            /// \internal
            SHValueType sortValue;
        };

    protected:
        struct TrackedObject;
    public:
        static int TrackedObjectComp( const Nui64 &key, TrackedObject* const &data );
    protected:

        struct TrackedObject
        {
            TrackedObject();
            ~TrackedObject();
            TrackedObjectData trackedObjectData;
            DataStructures::Hash<RakString, TimeAndValueQueue*, 32, RakString::ToInteger> dataQueues;
        };

        DataStructures::OrderedList<Nui64, TrackedObject*,TrackedObjectComp> objects;

        TimeDurMS timeToTrack;
    };

    /// \brief Input numerical values over time. Get sum, average, highest, lowest, standard deviation on recent or all-time values
    /// \ingroup PLUGINS_GROUP
    class _EngineAPI StatisticsHistoryPlugin : public Plugin
    {
    public:
        StatisticsHistory statistics;

        StatisticsHistoryPlugin();
        virtual ~StatisticsHistoryPlugin();
        void SetTrackConnections(bool _addNewConnections, int newConnectionsObjectType, bool _removeLostConnections);
    protected:
        virtual void update();
        virtual void onConnectClose(const Address & address, UniqueID id, ConnectCloseType lostConnectionReason);
        virtual void onConnect(const Address & address, UniqueID id);

        bool addNewConnections;
        bool removeLostConnections;
        int newConnectionsObjectType;
    };
}
}
#endif
#endif