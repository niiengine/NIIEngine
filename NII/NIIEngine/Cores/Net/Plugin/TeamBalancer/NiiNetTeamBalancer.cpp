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
#if _RAKNET_SUPPORT_TeamBalancer==1

#include "TeamBalancer.h"
#include "NiiNetSerializer.h"
#include "NiiNetCommon.h"
#include "NiiNetLinkBase.h"
#include "Rand.h"

namespace NII
{
namespace NII_NET
{
    //------------------------------------------------------------------------
    enum TeamBalancerOperations
    {
        ID_STATUS_UPDATE_TO_NEW_HOST,
        ID_CANCEL_TEAM_REQUEST,
        ID_REQUEST_ANY_TEAM,
        ID_REQUEST_SPECIFIC_TEAM
    };
    //------------------------------------------------------------------------
    TeamBalancer::TeamBalancer()
    {
        defaultAssigmentAlgorithm = SMALLEST_TEAM;
        forceTeamsToBeEven = false;
        lockTeams = false;
        hostGuid = UniqueID::INVALID;
    }
    //------------------------------------------------------------------------
    TeamBalancer::~TeamBalancer()
    {

    }
    //------------------------------------------------------------------------
    void TeamBalancer::SetTeamSizeLimit(TeamId team, Nui16 limit)
    {
        teamLimits.Replace(limit,0,team);
        if (teamLimits.Size() > teamMemberCounts.Size())
            teamMemberCounts.Replace(0,0,teamLimits.Size()-1);
    }
    //------------------------------------------------------------------------
    void TeamBalancer::SetDefaultAssignmentAlgorithm(DefaultAssigmentAlgorithm daa)
    {
        // Just update the default. Currently active teams are not affected.
        defaultAssigmentAlgorithm=daa;
    }
    //------------------------------------------------------------------------
    void TeamBalancer::SetForceEvenTeams(bool force)
    {
        // Set flag to indicate that teams should be even.
        forceTeamsToBeEven=force;

        // If teams are locked, just return.
        if (lockTeams==true)
            return;

        if (forceTeamsToBeEven==true)
        {
            // Run the even team algorithm
            EvenTeams();
        }
    }
    //------------------------------------------------------------------------
    void TeamBalancer::SetLockTeams(bool lock)
    {
        if (lock==lockTeams)
            return;

        // Set flag to indicate that teams can no longer be changed.
        lockTeams=lock;

        // If lock is false, and teams were set to be forced as even, then run through the even team algorithm
        if (lockTeams==false)
        {
            // Process even swaps
            TeamId i,j;
            for (i=0; i < teamMembers.Size(); i++)
            {
                if (teamMembers[i].requestedTeam!=UNASSIGNED_TEAM_ID)
                {
                    for (j=i+1; j < teamMembers.Size(); j++)
                    {
                        if (teamMembers[j].requestedTeam==teamMembers[i].currentTeam &&
                            teamMembers[i].requestedTeam==teamMembers[j].currentTeam)
                        {
                            SwapTeamMembersByRequest(i,j);
                            NotifyTeamAssigment(i);
                            NotifyTeamAssigment(j);
                        }
                    }
                }
            }

            if (forceTeamsToBeEven==true)
            {
                EvenTeams();
            }
            else
            {
                // Process requested team changes
                // Process movement while not full
                for (i=0; i < teamMembers.Size(); i++)
                {
                    TeamId requestedTeam = teamMembers[i].requestedTeam;
                    if (requestedTeam!=UNASSIGNED_TEAM_ID)
                    {
                        if (teamMemberCounts[requestedTeam]<teamLimits[requestedTeam])
                        {
                            SwitchMemberTeam(i,requestedTeam);
                            NotifyTeamAssigment(i);
                        }
                    }
                }
            }
        }
    }
    //------------------------------------------------------------------------
    void TeamBalancer::RequestSpecificTeam(NetworkID memberId, TeamId desiredTeam)
    {
        bool foundMatch=false;
        for (Nui32 i=0; i < myTeamMembers.Size(); i++)
        {
            if (myTeamMembers[i].memberId==memberId)
            {
                foundMatch=true;
                if (myTeamMembers[i].requestedTeam==desiredTeam && myTeamMembers[i].currentTeam==desiredTeam)
                {
                    return;
                }
                else
                {
                    myTeamMembers[i].requestedTeam=desiredTeam;
                }
            }
        }

        if (foundMatch==false)
        {
            MyTeamMembers mtm;
            mtm.currentTeam=UNASSIGNED_TEAM_ID;
            mtm.memberId=memberId;
            mtm.requestedTeam=desiredTeam;
            myTeamMembers.Push(mtm);
        }

        // send desiredTeam to the current host.
        // Also flag that we have requested a team, and record desiredTeam in case the host changes and it needs to be resent.
        NetSerializer bsOut;
        bsOut.write((Token)ID_TEAM_BALANCER_INTERNAL);
        bsOut.write((Token)ID_REQUEST_SPECIFIC_TEAM);
        bsOut.write(memberId);
        bsOut.write(desiredTeam);
        mPrc2->send(&bsOut,PL_HIGH,PT_ORDER_MAKE_SURE,0,hostGuid,false);
    }
    //------------------------------------------------------------------------
    void TeamBalancer::CancelRequestSpecificTeam(NetworkID memberId)
    {
        for (Nui32 i=0; i < myTeamMembers.Size(); i++)
        {
            if (myTeamMembers[i].memberId==memberId)
            {
                myTeamMembers[i].requestedTeam=UNASSIGNED_TEAM_ID;

                // send msg to the host to remove our request flag.
                NetSerializer bsOut;
                bsOut.write((Token)ID_TEAM_BALANCER_INTERNAL);
                bsOut.write((Token)ID_CANCEL_TEAM_REQUEST);
                bsOut.write(memberId);
                mPrc2->send(&bsOut,PL_HIGH,PT_ORDER_MAKE_SURE,0,hostGuid,false);

                return;
            }
        }
    }
    //------------------------------------------------------------------------
    void TeamBalancer::RequestAnyTeam(NetworkID memberId)
    {
        bool foundMatch=false;

        for (Nui32 i=0; i < myTeamMembers.Size(); i++)
        {
            if (myTeamMembers[i].memberId==memberId)
            {
                foundMatch=true;
                if (myTeamMembers[i].currentTeam!=UNASSIGNED_TEAM_ID)
                    return;
                else
                    myTeamMembers[i].requestedTeam=UNASSIGNED_TEAM_ID;
                break;
            }
        }

        if (foundMatch==false)
        {
            MyTeamMembers mtm;
            mtm.currentTeam=UNASSIGNED_TEAM_ID;
            mtm.memberId=memberId;
            mtm.requestedTeam=UNASSIGNED_TEAM_ID;
            myTeamMembers.Push(mtm);
        }

        // Else send to the current host that we need a team.
        NetSerializer bsOut;
        bsOut.write((Token)ID_TEAM_BALANCER_INTERNAL);
        bsOut.write((Token)ID_REQUEST_ANY_TEAM);
        bsOut.write(memberId);
        mPrc2->send(&bsOut,PL_HIGH,PT_ORDER_MAKE_SURE,0,hostGuid,false);
    }
    //------------------------------------------------------------------------
    TeamId TeamBalancer::GetMyTeam(NetworkID memberId) const
    {
        // Return team returned by last ID_TEAM_BALANCER_TEAM_ASSIGNED msg
        for (NCount i = 0; i < myTeamMembers.Size(); ++i)
        {
            if (myTeamMembers[i].memberId == memberId)
            {
                return myTeamMembers[i].currentTeam;
            }
        }

        return UNASSIGNED_TEAM_ID;
    }
    //------------------------------------------------------------------------
    void TeamBalancer::DeleteMember(NetworkID memberId)
    {
        for (NCount i = 0; i < myTeamMembers.Size(); ++i)
        {
            if (myTeamMembers[i].memberId == memberId)
            {
                myTeamMembers.RemoveAtIndexFast(i);
                break;
            }
        }

        for (NCount i = 0; i < teamMembers.Size(); ++i)
        {
            if (teamMembers[i].memberId == memberId)
            {
                RemoveTeamMember(i);
                break;
            }
        }
    }
    //------------------------------------------------------------------------
    ConnectReceiveType TeamBalancer::onMessage(SocketMessage * msg)
    {
        switch(msg->mData[0])
        {
            case ID_FCM2_NEW_HOST:
            {
                hostGuid = msg->mID;

                if (myTeamMembers.Size() > 0)
                {
                    NetSerializer bsOut;
                    bsOut.write((Token)ID_TEAM_BALANCER_INTERNAL);
                    bsOut.write((Token)ID_STATUS_UPDATE_TO_NEW_HOST);

                    bsOut.write((Nui8)myTeamMembers.Size());
                    for (NCount i = 0; i < myTeamMembers.Size(); ++i)
                    {
                        bsOut.write(myTeamMembers[i].memberId);
                        bsOut.write(myTeamMembers[i].currentTeam);
                        bsOut.write(myTeamMembers[i].requestedTeam);
                    }
                    mPrc2->send(&bsOut,PL_HIGH,PT_ORDER_MAKE_SURE,0,hostGuid,false);
                }
            }
            break;
            case ID_TEAM_BALANCER_INTERNAL:
            {
                if (msg->mLength>=2)
                {
                    switch (msg->mData[1])
                    {
                        case ID_STATUS_UPDATE_TO_NEW_HOST:
                            OnStatusUpdateToNewHost(msg);
                            break;
                        case ID_CANCEL_TEAM_REQUEST:
                            OnCancelTeamRequest(msg);
                            break;
                        case ID_REQUEST_ANY_TEAM:
                            OnRequestAnyTeam(msg);
                            break;
                        case ID_REQUEST_SPECIFIC_TEAM:
                            OnRequestSpecificTeam(msg);
                            break;
                    }
                }
            }
            return CRT_Destroy;

            case ID_TEAM_BALANCER_TEAM_ASSIGNED:
            {
                return OnTeamAssigned(msg);
            }

            case ID_TEAM_BALANCER_REQUESTED_TEAM_FULL:
            {
                return OnRequestedTeamChangePending(msg);
            }

            case ID_TEAM_BALANCER_REQUESTED_TEAM_LOCKED:
            {
                return OnTeamsLocked(msg);
            }
        }

        // Got RequestSpecificTeam
        // If teams are locked
        // - If this user already has a team, return ID_TEAM_BALANCER_TEAMS_LOCKED
        // - This user does not already have a team. Assign a team as if the user called RequestAnyTeam(), with a preference for the requested team. Return ID_TEAM_BALANCER_TEAM_ASSIGNED once the team has been assigned.
        // If teams are not locked
        // - If even team balancing is on, only assign this user if this would not cause teams to be unbalanced. If teams WOULD be unbalanced, then flag this user as wanting to join this team. Return ID_TEAM_BALANCER_REQUESTED_TEAM_CHANGE_PENDING
        // - If the destination team is full, flag this user as wanting to join this team. Return ID_TEAM_BALANCER_REQUESTED_TEAM_CHANGE_PENDING
        // - Else, join this team. Return ID_TEAM_BALANCER_TEAM_ASSIGNED

        // Got RequestAnyTeam
        // Put user on a team following the algorithm. No team is set as preferred.

        return CRT_Normal;
    }
    //------------------------------------------------------------------------
    void TeamBalancer::onConnectClose(const Address & address, UniqueID id,
        ConnectCloseType lostConnectionReason)
    {
        (void) address;
        (void) lostConnectionReason;

        RemoveByGuid(id);
    }
    //------------------------------------------------------------------------
    void TeamBalancer::onAttach()
    {
        hostGuid = mPrc2->getGUID((Address *)0);
    }
    //------------------------------------------------------------------------
    void TeamBalancer::RemoveByGuid(UniqueID id)
    {
        // If we are the host, and the closed connection has a team, and teams are not locked:
        if (WeAreHost())
        {
            Nui32 droppedMemberIndex=0;
            while (droppedMemberIndex < teamMembers.Size())
            {
                if (teamMembers[droppedMemberIndex].memberGuid==id)
                {
                    TeamId droppedTeam = teamMembers[droppedMemberIndex].currentTeam;
                    RemoveTeamMember(droppedMemberIndex);
                    if (lockTeams==false)
                    {
                        if (forceTeamsToBeEven)
                        {
                            // - teams were forced to be even, then run the even team algorithm
                            EvenTeams();
                        }
                        else
                        {
                            // - teams were NOT forced to be even, and the team the dropped player on was full, then move users wanting to join that team (if any)
                            if (teamMemberCounts[ droppedTeam ]==teamLimits[ droppedTeam ]-1)
                            {
                                MoveMemberThatWantsToJoinTeam(droppedTeam);
                            }
                        }
                    }
                }
                else
                {
                    droppedMemberIndex++;
                }
            }
        }
    }
    //------------------------------------------------------------------------
    void TeamBalancer::OnStatusUpdateToNewHost(SocketMessage * msg)
    {
        if (WeAreHost()==false)
            return;

        NetSerializer bsIn(msg->mData, msg->mLength, false);
        bsIn.skipRead(2, 0);
        Nui8 requestedTeamChangeListSize;
        bsIn.read(requestedTeamChangeListSize);
        TeamMember tm;
        tm.memberGuid = msg->mID;
        for (Nui8 i=0; i < requestedTeamChangeListSize; ++i)
        {
            bsIn.read(tm.memberId);
            bsIn.read(tm.currentTeam);
            bsIn.read(tm.requestedTeam);

            if (tm.currentTeam!=UNASSIGNED_TEAM_ID && tm.currentTeam>teamLimits.Size())
            {
                N_assert("Current team out of range in TeamBalancer::OnStatusUpdateToNewHost" && 0);
                return;
            }

            if (tm.requestedTeam!=UNASSIGNED_TEAM_ID && tm.requestedTeam>teamLimits.Size())
            {
                N_assert("Requested team out of range in TeamBalancer::OnStatusUpdateToNewHost" && 0);
                return;
            }

            if (tm.currentTeam==UNASSIGNED_TEAM_ID && tm.requestedTeam==UNASSIGNED_TEAM_ID)
                return;

            Nui32 memberIndex = GetMemberIndex(tm.memberId, msg->mID);
            if (memberIndex==(Nui32) -1)
            {
                // Add this system (by UniqueID) to the list of members if he is not already there
                // Also update his requested team flag.
                // Do not process balancing on requested teams, since we don't necessarily have all data from all systems yet and hopefully the state during the host migration was stable.
                if (tm.currentTeam==UNASSIGNED_TEAM_ID)
                {
                    // Assign a default team, then add team member
                    if (tm.requestedTeam==UNASSIGNED_TEAM_ID)
                    {
                        // Assign a default team
                        tm.currentTeam=GetNextDefaultTeam();
                    }
                    else
                    {
                        // Assign to requested team if possible. Otherwise, assign to a default team
                        if (TeamWouldBeOverpopulatedOnAddition(tm.requestedTeam, teamMembers.Size())==false)
                        {
                            tm.currentTeam=tm.requestedTeam;
                        }
                        else
                        {
                            tm.currentTeam=GetNextDefaultTeam();
                        }
                    }
                }

                if (tm.currentTeam==UNASSIGNED_TEAM_ID)
                {
                    N_assert("Too many members asking for teams!" && 0);
                    return;
                }
                NotifyTeamAssigment(AddTeamMember(tm));
            }
        }
    }
    //------------------------------------------------------------------------
    void TeamBalancer::OnCancelTeamRequest(SocketMessage *msg)
    {
        if (WeAreHost()==false)
            return;

        NetSerializer bsIn(msg->mData,msg->mLength,false);
        bsIn.skipRead(2, 0);
        NetworkID memberId;
        bsIn.read(memberId);

        Nui32 memberIndex = GetMemberIndex(memberId, msg->mID);
        if (memberIndex!=(Nui32)-1)
            teamMembers[memberIndex].requestedTeam=UNASSIGNED_TEAM_ID;
    }
    //------------------------------------------------------------------------
    void TeamBalancer::OnRequestAnyTeam(SocketMessage * msg)
    {
        if (WeAreHost() == false)
            return;

        NetSerializer bsIn(msg->mData, msg->mLength, false);
        bsIn.skipRead(2, 0);
        NetworkID memberId;
        bsIn.read(memberId);

        Nui32 memberIndex = GetMemberIndex(memberId, msg->mID);
        if (memberIndex==(Nui32)-1)
        {
            TeamMember tm;
            tm.currentTeam=GetNextDefaultTeam();
            tm.requestedTeam=UNASSIGNED_TEAM_ID;
            tm.memberGuid=msg->mID;
            tm.memberId=memberId;
            if (tm.currentTeam==UNASSIGNED_TEAM_ID)
            {
                N_assert("Too many members asking for teams!" && 0);
                return;
            }
            NotifyTeamAssigment(AddTeamMember(tm));
        }
    }
    //------------------------------------------------------------------------
    void TeamBalancer::OnRequestSpecificTeam(SocketMessage * msg)
    {
        if (WeAreHost() == false)
            return;

        NetSerializer bsIn(msg->mData, msg->mLength, false);
        bsIn.skipRead(2, 0);
        TeamMember tm;
        bsIn.read(tm.memberId);
        bsIn.read(tm.requestedTeam);

        Nui32 memberIndex = GetMemberIndex(tm.memberId, msg->mID);
        if (tm.requestedTeam == UNASSIGNED_TEAM_ID)
        {
            NotifyNoTeam(tm.memberId, msg->mID);
            if (memberIndex != (Nui32) -1)
                RemoveTeamMember(memberIndex);
            return;
        }

        if (tm.requestedTeam>teamLimits.Size())
        {
            N_assert("Requested team out of range in TeamBalancer::OnRequestSpecificTeam" && 0);
            return;
        }
        if (memberIndex==(Nui32) -1)
        {
            tm.memberGuid=msg->mID;

            // Assign to requested team if possible. Otherwise, assign to a default team
            if (TeamWouldBeOverpopulatedOnAddition(tm.requestedTeam, teamMembers.Size())==false)
            {
                tm.currentTeam=tm.requestedTeam;
                tm.requestedTeam=UNASSIGNED_TEAM_ID;
            }
            else
            {
                tm.currentTeam=GetNextDefaultTeam();
            }
            if (tm.currentTeam==UNASSIGNED_TEAM_ID)
            {
                N_assert("Too many members asking for teams!" && 0);
                return;
            }
            NotifyTeamAssigment(AddTeamMember(tm));
        }
        else
        {
            teamMembers[memberIndex].requestedTeam=tm.requestedTeam;
            TeamId oldTeamThisUserWasOn = teamMembers[memberIndex].currentTeam;

            if (lockTeams)
            {
                NotifyTeamsLocked(msg->mID, tm.requestedTeam);
                return;
            }

            // Assign to requested team if possible. Otherwise, assign to a default team
            if (TeamsWouldBeEvenOnSwitch(tm.requestedTeam,oldTeamThisUserWasOn)==true)
            {
                SwitchMemberTeam(memberIndex,tm.requestedTeam);
                NotifyTeamAssigment(memberIndex);
            }
            else
            {
                // If someone wants to join this user's old team, and we want to join their team, they can swap
                Nui32 swappableMemberIndex;
                for (swappableMemberIndex=0; swappableMemberIndex < teamMembers.Size(); swappableMemberIndex++)
                {
                    if (teamMembers[swappableMemberIndex].currentTeam==tm.requestedTeam && teamMembers[swappableMemberIndex].requestedTeam==oldTeamThisUserWasOn)
                        break;
                }

                if (swappableMemberIndex!=teamMembers.Size())
                {
                    SwapTeamMembersByRequest(memberIndex,swappableMemberIndex);
                    NotifyTeamAssigment(memberIndex);
                    NotifyTeamAssigment(swappableMemberIndex);
                }
                else
                {
                    // Full or would not be even
                    NotifyTeamSwitchPending(msg->mID, tm.requestedTeam, tm.memberId);
                }
            }
        }
    }
    //------------------------------------------------------------------------
    Nui32 TeamBalancer::GetMemberIndex(NetworkID memberId, UniqueID id) const
    {
        for (NCount i=0; i < teamMembers.Size(); ++i)
        {
            if (teamMembers[i].memberGuid == id && teamMembers[i].memberId==memberId)
                return i;
        }
        return (Nui32) -1;
    }
    //------------------------------------------------------------------------
    Nui32 TeamBalancer::AddTeamMember(const TeamMember &tm)
    {
        if (tm.currentTeam>teamLimits.Size())
        {
            N_assert("TeamBalancer::AddTeamMember team index out of bounds" && 0);
            return (Nui32) -1;
        }

        N_assert(tm.currentTeam!=UNASSIGNED_TEAM_ID);

        teamMembers.Push(tm);
        if (teamMemberCounts.Size()<tm.currentTeam)
            teamMemberCounts.Replace(1,0,tm.currentTeam);
        else
            teamMemberCounts[tm.currentTeam]=teamMemberCounts[tm.currentTeam]+1;
        return teamMembers.Size()-1;
    }
    //------------------------------------------------------------------------
    void TeamBalancer::RemoveTeamMember(Nui32 index)
    {
        N_assert( teamMemberCounts[ teamMembers[index].currentTeam ] != 0);
        teamMemberCounts[ teamMembers[index].currentTeam ]=teamMemberCounts[ teamMembers[index].currentTeam ]-1;
        teamMembers.RemoveAtIndexFast(index);
    }
    //------------------------------------------------------------------------
    void TeamBalancer::GetMinMaxTeamMembers(int &minMembersOnASingleTeam, int &maxMembersOnASingleTeam)
    {
        minMembersOnASingleTeam = teamMembers.Size()/teamLimits.Size();
        if ((teamMembers.Size() % teamLimits.Size()) == 0)
            maxMembersOnASingleTeam = minMembersOnASingleTeam;
        else
            maxMembersOnASingleTeam = minMembersOnASingleTeam+1;
    }
    //------------------------------------------------------------------------
    void TeamBalancer::EvenTeams(void)
    {
        // Ensure all teams are even. If not, pick players at random from overpopulated teams, and move to underpopulated teams.
        int minMembersOnASingleTeam;
        int maxMembersOnASingleTeam;
        GetMinMaxTeamMembers(minMembersOnASingleTeam,maxMembersOnASingleTeam);

        // First select among players that have requested to switch teams, if any, before choosing players that did not want to switch teams.
        // Players that are moved should be notified of ID_TEAM_BALANCER_TEAM_ASSIGNED
        DataStructures::List<TeamId> overpopulatedTeams;
        TeamId teamMemberCountsIndex;
        Nui32 memberIndexToSwitch;
        for (teamMemberCountsIndex=0; teamMemberCountsIndex<teamMemberCounts.Size(); teamMemberCountsIndex++)
        {
            while (teamMemberCounts[teamMemberCountsIndex]<minMembersOnASingleTeam && teamMemberCounts[teamMemberCountsIndex]<teamLimits[teamMemberCountsIndex])
            {
                GetOverpopulatedTeams(overpopulatedTeams,maxMembersOnASingleTeam);
                N_assert(overpopulatedTeams.Size()>0);
                memberIndexToSwitch=GetMemberIndexToSwitchTeams(overpopulatedTeams,teamMemberCountsIndex);
                N_assert(memberIndexToSwitch!=(Nui32)-1);
                SwitchMemberTeam(memberIndexToSwitch,teamMemberCountsIndex);
                // Tell this member he switched teams
                NotifyTeamAssigment(memberIndexToSwitch);
            }
        }
    }
    //------------------------------------------------------------------------
    Nui32 TeamBalancer::GetMemberIndexToSwitchTeams(const DataStructures::List<TeamId> &sourceTeamNumbers, TeamId targetTeamNumber)
    {
        DataStructures::List<Nui32> preferredSwapIndices;
        DataStructures::List<Nui32> potentialSwapIndices;
        Nui32 i,j;
        for (j=0; j < sourceTeamNumbers.Size(); j++)
        {
            N_assert(sourceTeamNumbers[j]!=targetTeamNumber);
            for (i=0; i < teamMembers.Size(); i++)
            {
                if (teamMembers[i].currentTeam==sourceTeamNumbers[j])
                {
                    if (teamMembers[i].requestedTeam==targetTeamNumber)
                        preferredSwapIndices.Push(i);
                    else
                        potentialSwapIndices.Push(i);
                }
            }
        }

        if (preferredSwapIndices.Size()>0)
        {
            return preferredSwapIndices[ randomMT() % preferredSwapIndices.Size() ];
        }
        else if (potentialSwapIndices.Size()>0)
        {
            return potentialSwapIndices[ randomMT() % potentialSwapIndices.Size() ];
        }
        else
        {
            return (Nui32) -1;
        }
    }
    //------------------------------------------------------------------------
    void TeamBalancer::SwitchMemberTeam(Nui32 teamMemberIndex, TeamId destinationTeam)
    {
        teamMemberCounts[ teamMembers[teamMemberIndex].currentTeam ]=teamMemberCounts[ teamMembers[teamMemberIndex].currentTeam ]-1;
        teamMemberCounts[ destinationTeam ]=teamMemberCounts[ destinationTeam ]+1;
        teamMembers[teamMemberIndex].currentTeam=destinationTeam;
        if (teamMembers[teamMemberIndex].requestedTeam==destinationTeam)
            teamMembers[teamMemberIndex].requestedTeam=UNASSIGNED_TEAM_ID;
    }
    //------------------------------------------------------------------------
    void TeamBalancer::GetOverpopulatedTeams(DataStructures::List<TeamId> &overpopulatedTeams, int maxTeamSize)
    {
        overpopulatedTeams.Clear(true);
        for (TeamId i=0; i < teamMemberCounts.Size(); i++)
        {
            if (teamMemberCounts[i]>=maxTeamSize)
                overpopulatedTeams.Push(i);
        }
    }
    //------------------------------------------------------------------------
    void TeamBalancer::NotifyTeamAssigment(Nui32 teamMemberIndex)
    {
        N_assert(teamMemberIndex < teamMembers.Size());
        if (teamMemberIndex>=teamMembers.Size())
            return;

        NetSerializer bsOut;
        bsOut.write((Token)ID_TEAM_BALANCER_TEAM_ASSIGNED);
        bsOut.write(teamMembers[teamMemberIndex].currentTeam);
        bsOut.write(teamMembers[teamMemberIndex].memberId);
        mPrc2->send(&bsOut,PL_HIGH,PT_ORDER_MAKE_SURE,0,teamMembers[teamMemberIndex].memberGuid,false);
    }
    //------------------------------------------------------------------------
    bool TeamBalancer::WeAreHost() const
    {
        return hostGuid == mPrc2->getGUID((Address *)0);
    }
    //------------------------------------------------------------------------
    ConnectReceiveType TeamBalancer::OnTeamAssigned(SocketMessage * msg)
    {
        if (msg->mID != hostGuid)
            return CRT_Destroy;

        NetSerializer bsIn(msg->mData, msg->mLength, false);
        bsIn.skipRead(1, 0);
        MyTeamMembers mtm;
        bsIn.read(mtm.currentTeam);
        bsIn.read(mtm.memberId);
        mtm.requestedTeam=UNASSIGNED_TEAM_ID;

        bool foundMatch=false;
        for (Nui32 i=0; i < myTeamMembers.Size(); i++)
        {
            if (myTeamMembers[i].memberId==mtm.memberId)
            {
                foundMatch=true;
                if (myTeamMembers[i].requestedTeam==mtm.currentTeam)
                    myTeamMembers[i].requestedTeam=UNASSIGNED_TEAM_ID;
                myTeamMembers[i].currentTeam=mtm.currentTeam;
                break;
            }
        }

        if (foundMatch==false)
        {
            return CRT_Destroy;
        }

        return CRT_Normal;
    }
    //------------------------------------------------------------------------
    ConnectReceiveType TeamBalancer::OnRequestedTeamChangePending(SocketMessage *msg)
    {
        if (msg->mID!=hostGuid)
            return CRT_Destroy;

        return CRT_Normal;
    }
    //------------------------------------------------------------------------
    ConnectReceiveType TeamBalancer::OnTeamsLocked(SocketMessage *msg)
    {
        if (msg->mID!=hostGuid)
            return CRT_Destroy;

        return CRT_Normal;
    }
    //------------------------------------------------------------------------
    TeamId TeamBalancer::GetNextDefaultTeam(void)
    {
        // Accounting for team balancing and team limits, get the team a player should be placed on
        switch (defaultAssigmentAlgorithm)
        {
            case SMALLEST_TEAM:
            {
                return GetSmallestNonFullTeam();
            }

            case FILL_IN_ORDER:
            {
                return GetFirstNonFullTeam();
            }

            default:
            {
                N_assert("TeamBalancer::GetNextDefaultTeam unknown algorithm enumeration" && 0);
                return UNASSIGNED_TEAM_ID;
            }
        }
    }
    //------------------------------------------------------------------------
    bool TeamBalancer::TeamWouldBeOverpopulatedOnAddition(TeamId teamId, Nui32 teamMemberSize)
    {
        // Accounting for team balancing and team limits, would this team be overpopulated if a member was added to it?
        if (teamMemberCounts[teamId]>=teamLimits[teamId])
        {
            return true;
        }

        if (forceTeamsToBeEven)
        {
            int allowedLimit = teamMemberSize/teamLimits.Size() + 1;
            return teamMemberCounts[teamId]>=allowedLimit;
        }

        return false;
    }
    //------------------------------------------------------------------------
    bool TeamBalancer::TeamWouldBeUnderpopulatedOnLeave(TeamId teamId, Nui32 teamMemberSize)
    {
        if (forceTeamsToBeEven)
        {
            Nui32 minMembersOnASingleTeam = (teamMemberSize-1)/teamLimits.Size();
            return teamMemberCounts[teamId]<=minMembersOnASingleTeam;
        }
        return false;
    }
    //------------------------------------------------------------------------
    TeamId TeamBalancer::GetSmallestNonFullTeam(void) const
    {
        TeamId idx;
        Nul smallestTeamCount=MAX_UNSIGNED_LONG;
        TeamId smallestTeamIndex = UNASSIGNED_TEAM_ID;
        for (idx=0; idx < teamMemberCounts.Size(); idx++)
        {
            if (teamMemberCounts[idx]<smallestTeamCount && teamMemberCounts[idx]<teamLimits[idx])
            {
                smallestTeamCount=teamMemberCounts[idx];
                smallestTeamIndex=idx;
            }
        }
        return smallestTeamIndex;
    }
    //------------------------------------------------------------------------
    TeamId TeamBalancer::GetFirstNonFullTeam(void) const
    {
        TeamId idx;
        for (idx=0; idx < teamMemberCounts.Size(); idx++)
        {
            if (teamMemberCounts[idx]<teamLimits[idx])
            {
                return idx;
            }
        }
        return UNASSIGNED_TEAM_ID;
    }
    //------------------------------------------------------------------------
    void TeamBalancer::MoveMemberThatWantsToJoinTeam(TeamId teamId)
    {
        N_assert(forceTeamsToBeEven==false && lockTeams==false);

        do
        {
            teamId = MoveMemberThatWantsToJoinTeamInternal(teamId);
        } while (teamId!=UNASSIGNED_TEAM_ID);
    }
    //------------------------------------------------------------------------
    TeamId TeamBalancer::MoveMemberThatWantsToJoinTeamInternal(TeamId teamId)
    {
        DataStructures::List<TeamId> membersThatWantToJoinTheTeam;
        for (TeamId i=0; i < teamMembers.Size(); ++i)
        {
            if (teamMembers[i].requestedTeam==teamId)
                membersThatWantToJoinTheTeam.Push(i);
        }

        if (membersThatWantToJoinTheTeam.Size()>0)
        {
            TeamId oldTeam;
            Nui32 swappedMemberIndex = membersThatWantToJoinTheTeam[ randomMT() % membersThatWantToJoinTheTeam.Size() ];
            oldTeam=teamMembers[swappedMemberIndex].currentTeam;
            SwitchMemberTeam(swappedMemberIndex,teamId);
            NotifyTeamAssigment(swappedMemberIndex);
            return oldTeam;
        }
        return UNASSIGNED_TEAM_ID;
    }
    //------------------------------------------------------------------------
    void TeamBalancer::NotifyTeamsLocked(UniqueID target, TeamId requestedTeam)
    {
        NetSerializer bsOut;
        bsOut.write((Token)ID_TEAM_BALANCER_REQUESTED_TEAM_LOCKED);
        bsOut.write(requestedTeam);
        mPrc2->send(&bsOut,PL_HIGH,PT_ORDER_MAKE_SURE,0,target,false);
    }
    //------------------------------------------------------------------------
    void TeamBalancer::NotifyTeamSwitchPending(UniqueID target, TeamId requestedTeam, NetworkID memberId)
    {
        NetSerializer bsOut;
        bsOut.write((Token)ID_TEAM_BALANCER_REQUESTED_TEAM_FULL);
        bsOut.write(requestedTeam);
        bsOut.write(memberId);
        mPrc2->send(&bsOut,PL_HIGH,PT_ORDER_MAKE_SURE,0,target,false);
    }
    //------------------------------------------------------------------------
    void TeamBalancer::SwapTeamMembersByRequest(Nui32 memberIndex1, Nui32 memberIndex2)
    {
        TeamId index1Team = teamMembers[memberIndex1].currentTeam;
        teamMembers[memberIndex1].currentTeam=teamMembers[memberIndex2].currentTeam;
        teamMembers[memberIndex2].currentTeam=index1Team;
        teamMembers[memberIndex1].requestedTeam=UNASSIGNED_TEAM_ID;
        teamMembers[memberIndex2].requestedTeam=UNASSIGNED_TEAM_ID;
    }
    //------------------------------------------------------------------------
    void TeamBalancer::NotifyNoTeam(NetworkID memberId, UniqueID target)
    {
        NetSerializer bsOut;
        bsOut.write((Token)ID_TEAM_BALANCER_TEAM_ASSIGNED);
        bsOut.write((Nui8)UNASSIGNED_TEAM_ID);
        bsOut.write(memberId);
        mPrc2->send(&bsOut,PL_HIGH,PT_ORDER_MAKE_SURE,0,target,false);
    }
    //------------------------------------------------------------------------
    bool TeamBalancer::TeamsWouldBeEvenOnSwitch(TeamId t1, TeamId t2)
    {
        N_assert(teamMembers.Size()!=0);
        return TeamWouldBeOverpopulatedOnAddition(t1, teamMembers.Size()-1)==false &&
            TeamWouldBeUnderpopulatedOnLeave(t2, teamMembers.Size()-1)==false;
    }
    //------------------------------------------------------------------------
}
}
#endif // _RAKNET_SUPPORT_*
