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
#if _RAKNET_SUPPORT_TeamManager==1

#include "TeamManager.h"
#include "NiiNetSerializer.h"
#include "NiiNetCommon.h"

namespace NII
{
namespace NII_NET
{
    //------------------------------------------------------------------------
    enum TeamManagerOperations
    {
        ID_RUN_UpdateListsToNoTeam,
        ID_RUN_UpdateTeamsRequestedToAny,
        ID_RUN_JoinAnyTeam,
        ID_RUN_JoinRequestedTeam,
        ID_RUN_UpdateTeamsRequestedToNoneAndAddTeam,
        ID_RUN_RemoveFromTeamsRequestedAndAddTeam,
        ID_RUN_AddToRequestedTeams,
        ID_RUN_LeaveTeam,
        ID_RUN_SetMemberLimit,
        ID_RUN_SetJoinPermissions,
        ID_RUN_SetBalanceTeams,
        ID_RUN_SetBalanceTeamsInitial,
        ID_RUN_SerializeWorld,
    };
    //------------------------------------------------------------------------
    int TM_World::JoinRequestHelperComp(const TM_World::JoinRequestHelper & key,
        const TM_World::JoinRequestHelper & data)
    {
        if (key.whenRequestMade < data.whenRequestMade)
            return -1;
        if (key.whenRequestMade > data.whenRequestMade)
            return 1;
        if (key.requestIndex < data.requestIndex)
            return -1;
        if (key.requestIndex > data.requestIndex)
            return 1;
        return 0;
    }
    //------------------------------------------------------------------------
    TeamSelection::TeamSelection() {}
    //------------------------------------------------------------------------
    TeamSelection::TeamSelection(JoinTeamType itt) : joinTeamType(itt) {}
    //------------------------------------------------------------------------
    TeamSelection::TeamSelection(JoinTeamType itt, TM_Team *param) : joinTeamType(itt) {teamParameter.specificTeamToJoin=param;}
    //------------------------------------------------------------------------
    TeamSelection::TeamSelection(JoinTeamType itt, NoTeamId param) : joinTeamType(itt) {teamParameter.noTeamSubcategory=param;}
    //------------------------------------------------------------------------
    TeamSelection TeamSelection::AnyAvailable(void) {return TeamSelection(JOIN_ANY_AVAILABLE_TEAM);}
    //------------------------------------------------------------------------
    TeamSelection TeamSelection::SpecificTeam(TM_Team *specificTeamToJoin) {return TeamSelection(JOIN_SPECIFIC_TEAM, specificTeamToJoin);}
    //------------------------------------------------------------------------
    TeamSelection TeamSelection::NoTeam(NoTeamId noTeamSubcategory) {return TeamSelection(JOIN_NO_TEAM, noTeamSubcategory);}
    //------------------------------------------------------------------------
    TM_TeamMember::TM_TeamMember()
    {
        networkId=0;
        world=0;
        joinTeamType=JOIN_NO_TEAM;
        noTeamSubcategory=0;
    }
    //------------------------------------------------------------------------
    TM_TeamMember::~TM_TeamMember()
    {
        if (world)
        {
            world->DereferenceTeamMember(this);
        }
    }
    //------------------------------------------------------------------------
    bool TM_TeamMember::RequestTeam(TeamSelection teamSelection)
    {
        if (teamSelection.joinTeamType==JOIN_NO_TEAM)
        {
            // If joining no team:
            // - If already no team, and no team category is the same, return false.
            // - Execute JoinNoTeam() locally. Return ID_TEAM_BALANCER_TEAM_ASSIGNED locally.
            // - If we are host, broadcast event. Done.
            // - send to remote host event to call JoinNoTeam()
            // - remote Host executes JoinNoTeam() and broadcasts event. This may cause may cause rebalance if team balancing is on.
            // - - JoinNoTeam(): Remove from all current and requested teams. Set no-team category.

            if (teams.Size()==0 && noTeamSubcategory==teamSelection.teamParameter.noTeamSubcategory)
            {
                // No change
                return false;
            }

            NetSerializer out;
            out.write((Nui8)ID_TEAM_BALANCER_INTERNAL);
            out.write((Nui8)ID_RUN_UpdateListsToNoTeam);
            out.write(world->GetWorldId());
            out.write(networkId);
            out.write(teamSelection.teamParameter.noTeamSubcategory);
            world->BroadcastToParticipants(&out, UniqueID::INVALID);

            StoreLastTeams();

            UpdateListsToNoTeam(teamSelection.teamParameter.noTeamSubcategory);

            world->GetTeamManager()->PushTeamAssigned(this);
            if (world->GetHost()==world->GetTeamManager()->getID())
            {
                world->FillRequestedSlots();
                world->EnforceTeamBalance(teamSelection.teamParameter.noTeamSubcategory);
            }
        }
        else if (teamSelection.joinTeamType==JOIN_ANY_AVAILABLE_TEAM)
        {
            // If joining any team
            // Execute JoinAnyTeamCheck()
            // - JoinAnyTeamCheck():
            // - - If already on a team, return false
            // - - If any team is already in requested teams, return false.
            // On local, call UpdateTeamsRequestedToAny(). send event to also execute this to remote host
            // If we are host, execute JoinAnyTeam(myguid).
            // - JoinAnyTeam(requesterGuid): Attempt to join any team immediately.  If fails, send to all except requestGuid UpdateTeamsRequestedToAny(). Else sends out new team, including to caller.
            // On remote host, execute JoinAnyTeamCheck(). If fails, this was because you were added to a team simultaneously on host. This is OK, just ignore the call.
            // Assuming JoinAnyTeamCheck() passed on remote host, call UpdateTeamsRequestedToAny() for this player. execute JoinAnyTeam(msg->mID).

            if (JoinAnyTeamCheck()==false)
                return false;

            UpdateTeamsRequestedToAny();

            // send request to host to execute JoinAnyTeam()
            NetSerializer out;
            out.write((Nui8)ID_TEAM_BALANCER_INTERNAL);
            out.write((Nui8)ID_RUN_JoinAnyTeam);
            out.write(world->GetWorldId());
            out.write(networkId);
            world->GetTeamManager()->send(&out,PL_HIGH, PT_ORDER_MAKE_SURE, 0, world->GetHost(), false);
        }
        else
        {
            N_assert(teamSelection.joinTeamType==JOIN_SPECIFIC_TEAM);

            // If joining specific team
            // Execute JoinSpecificTeamCheck()
            // JoinSpecificTeamCheck():
            // - If already on specific team, return false
            // - If specific team is in requested list, return false
            // On local, call AddToRequestedTeams(). send event to also execute this to remote host
            // If we are host, execute JoinSpecificTeam(myguid)
            // - JoinSpecificTeam(requesterGuid): Attempt to join specific team immediately. If fails, send to all except requesterGuid to execute AddSpecificToRequested(). Else sends out new team, including to caller.
            // On remote host, execute JoinSpecificTeamCheck(). If fails, just ignore.
            // Assuming JoinSpecificTeamCheck() passed on host, call AddSpecificToRequestedList(). Execute JoinSpecificTeam(msg->mID)

            if (JoinSpecificTeamCheck(teamSelection.teamParameter.specificTeamToJoin,false)==false)
                return false;

            AddToRequestedTeams(teamSelection.teamParameter.specificTeamToJoin);

            // send request to host to execute JoinRequestedTeam()
            NetSerializer out;
            out.write((Nui8)ID_TEAM_BALANCER_INTERNAL);
            out.write((Nui8)ID_RUN_JoinRequestedTeam);
            out.write(world->GetWorldId());
            out.write(networkId);
            out.write(teamSelection.teamParameter.specificTeamToJoin->GetNetworkID());
            out.write0();
            world->GetTeamManager()->send(&out,PL_HIGH, PT_ORDER_MAKE_SURE, 0, world->GetHost(), false);
        }

        return true;
    }
    //------------------------------------------------------------------------
    bool TM_TeamMember::RequestTeamSwitch(TM_Team *teamToJoin, TM_Team *teamToLeave)
    {
        if (SwitchSpecificTeamCheck(teamToJoin,teamToLeave,false)==false)
            return false;

        AddToRequestedTeams(teamToJoin, teamToLeave);

        // send request to host to execute JoinRequestedTeam()
        NetSerializer out;
        out.write((Nui8)ID_TEAM_BALANCER_INTERNAL);
        out.write((Nui8)ID_RUN_JoinRequestedTeam);
        out.write(world->GetWorldId());
        out.write(networkId);
        out.write(teamToJoin->GetNetworkID());
        out.write1();
        if (teamToLeave)
        {
            out.write1();
            out.write(teamToLeave->GetNetworkID());
        }
        else
        {
            out.write0();
        }
        world->GetTeamManager()->send(&out,PL_HIGH, PT_ORDER_MAKE_SURE, 0, world->GetHost(), false);

        return true;
    }
    //------------------------------------------------------------------------
    TeamSelection TM_TeamMember::GetRequestedTeam(void) const
    {
        if (teamsRequested.Size()>0)
            return TeamSelection::SpecificTeam(teamsRequested[0].requested);
        else if (joinTeamType==JOIN_NO_TEAM)
            return TeamSelection::NoTeam(noTeamSubcategory);
        else
            return TeamSelection::AnyAvailable();
    }
    //------------------------------------------------------------------------
    void TM_TeamMember::GetRequestedSpecificTeams(DataStructures::List<TM_Team*> &requestedTeams) const
    {
        requestedTeams.Clear(true);
        for (Nui32 i=0; i < teamsRequested.Size(); i++)
            requestedTeams.Push(teamsRequested[i].requested);
    }
    //------------------------------------------------------------------------
    bool TM_TeamMember::HasRequestedTeam(TM_Team * team) const
    {
        Nui32 i = GetRequestedTeamIndex(team);
        if (i==(Nui32)-1)
            return false;
        return true;
    }
    //------------------------------------------------------------------------
    Nui32 TM_TeamMember::GetRequestedTeamIndex(TM_Team * team) const
    {
        Nui32 i;
        for (i=0; i < teamsRequested.Size(); i++)
        {
            if (teamsRequested[i].requested==team)
                return i;
        }
        return (Nui32) -1;
    }
    //------------------------------------------------------------------------
    Nui32 TM_TeamMember::GetRequestedTeamCount(void) const
    {
        return teamsRequested.Size();
    }
    //------------------------------------------------------------------------
    bool TM_TeamMember::CancelTeamRequest(TM_Team *specificTeamToCancel)
    {
        if (RemoveFromRequestedTeams(specificTeamToCancel)==false)
            return false;

        // send request to host to execute JoinRequestedTeam()
        NetSerializer out;
        out.write((Nui8)ID_TEAM_BALANCER_TEAM_REQUESTED_CANCELLED);
        out.write(world->GetWorldId());
        out.write(networkId);
        if (specificTeamToCancel)
        {
            out.write1();
            out.write(specificTeamToCancel->GetNetworkID());
        }
        else
        {
            out.write0();
        }
        world->BroadcastToParticipants(&out, UniqueID::INVALID);

        world->GetTeamManager()->PushBitStream(&out);

        return true;
    }
    //------------------------------------------------------------------------
    bool TM_TeamMember::LeaveTeam(TM_Team* team, NoTeamId _noTeamSubcategory)
    {
        if (LeaveTeamCheck(team)==false)
            return false;

        RemoveFromSpecificTeamInternal(team);
        if (teams.Size()==0)
        {
            noTeamSubcategory=_noTeamSubcategory;
            joinTeamType=JOIN_NO_TEAM;
        }

        // Execute LeaveTeamCheck()
        // - LeaveTeamCheck():
        // - - If not on this team, return false
        // On local, call RemoteFromTeamsList(). send event to also execute this to remote host
        // If we are host, execute OnLeaveTeamEvent(mLocalHostID)
        // - OnLeaveTeamEvent(requesterGuid):
        // - - If rebalancing is active, rebalance
        // - - If someone else wants to join this team, let them.
        // - - send leave team event notification to all except requesterGuid-
        // On remote host, execute LeaveTeamCheck(). If fails, ignore.
        // On remote host, execute RemoteFromTeamsList() followed by OnLeaveTeamEvent(msg->mID)

        // Pattern:
        // Execute local check, if fails return false
        // Locally execute non-host guaranteed changes
        // If local system is also host, execute host changes. Relay to all but local
        // On remote host, execute check. If check passes, execute non-host changes, followed by host changes. Relay to all but sender.
        NetSerializer out;
        out.write((Nui8)ID_TEAM_BALANCER_INTERNAL);
        out.write((Nui8)ID_RUN_LeaveTeam);
        out.write(world->GetWorldId());
        out.write(networkId);
        out.write(team->GetNetworkID());
        out.write(noTeamSubcategory);
        world->BroadcastToParticipants(&out, UniqueID::INVALID);

        if (world->GetHost()==world->GetTeamManager()->getID())
        {
            // Rebalance teams
            world->FillRequestedSlots();
            world->EnforceTeamBalance(noTeamSubcategory);
        }
        return true;
    }
    //------------------------------------------------------------------------
    bool TM_TeamMember::LeaveAllTeams(NoTeamId noTeamSubcategory)
    {
        return RequestTeam(TeamSelection::NoTeam(noTeamSubcategory));
    }
    //------------------------------------------------------------------------
    TM_Team * TM_TeamMember::GetCurrentTeam(void) const
    {
        if (teams.Size()>0)
            return teams[0];
        return 0;
    }
    //------------------------------------------------------------------------
    Nui32 TM_TeamMember::GetCurrentTeamCount(void) const
    {
        return teams.Size();
    }
    //------------------------------------------------------------------------
    TM_Team * TM_TeamMember::GetCurrentTeamByIndex(Nui32 index)
    {
        return teams[index];
    }
    //------------------------------------------------------------------------
    void TM_TeamMember::GetCurrentTeams(DataStructures::List<TM_Team*> &_teams) const
    {
        _teams=teams;
    }
    //------------------------------------------------------------------------
    void TM_TeamMember::GetLastTeams(DataStructures::List<TM_Team*> &_teams) const
    {
        _teams=lastTeams;
    }
    //------------------------------------------------------------------------
    bool TM_TeamMember::IsOnTeam(TM_Team *team) const
    {
        Nui32 i;
        for (i=0; i < teams.Size(); i++)
        {
            if (teams[i]==team)
                return true;
        }
        return false;
    }
    //------------------------------------------------------------------------
    NetworkID TM_TeamMember::GetNetworkID(void) const
    {
        return networkId;
    }
    //------------------------------------------------------------------------
    TM_World * TM_TeamMember::GetTM_World(void) const
    {
        return world;
    }
    //------------------------------------------------------------------------
    void TM_TeamMember::SerializeConstruction(NetSerializer *constructionBitstream)
    {
        // write requested teams
        constructionBitstream->write(world->GetWorldId());
        constructionBitstream->write(networkId);
        constructionBitstream->write((Nui16)teamsRequested.Size());
        for (Nui32 i=0; i < teamsRequested.Size(); i++)
        {
            constructionBitstream->write(teamsRequested[i].isTeamSwitch);
            if (teamsRequested[i].teamToLeave)
            {
                constructionBitstream->write1();
                constructionBitstream->write(teamsRequested[i].teamToLeave->GetNetworkID());
            }
            else
            {
                constructionBitstream->write0();
            }
            if (teamsRequested[i].requested)
            {
                constructionBitstream->write1();
                constructionBitstream->write(teamsRequested[i].requested->GetNetworkID());
            }
            else
            {
                constructionBitstream->write0();
            }
        }

        world->teamManager->EncodeTeamAssigned(constructionBitstream, this);
    }
    //------------------------------------------------------------------------
    bool TM_TeamMember::DeserializeConstruction(TeamManager *teamManager, NetSerializer *constructionBitstream)
    {
        // read requested teams
        bool success;
        Nui16 teamsRequestedSize;

        WorldId worldId;
        constructionBitstream->read(worldId);
        TM_World *world = teamManager->GetWorldWithId(worldId);
        N_assert(world);
        constructionBitstream->read(networkId);
        world->ReferenceTeamMember(this,networkId);

        success=constructionBitstream->read(teamsRequestedSize);
        for (Nui32 i=0; i < teamsRequestedSize; i++)
        {
            RequestedTeam rt;
            rt.isTeamSwitch=false;
            rt.requested=0;
            rt.whenRequested=0;
            constructionBitstream->read(rt.isTeamSwitch);
            bool hasTeamToLeave=false;
            constructionBitstream->read(hasTeamToLeave);
            NetworkID teamToLeaveId;
            if (hasTeamToLeave)
            {
                constructionBitstream->read(teamToLeaveId);
                rt.teamToLeave = world->GetTeamByNetworkID(teamToLeaveId);
                N_assert(rt.teamToLeave);
            }
            else
                rt.teamToLeave=0;
            bool hasTeamRequested=false;
            success=constructionBitstream->read(hasTeamRequested);
            NetworkID teamRequestedId;
            if (hasTeamRequested)
            {
                success=constructionBitstream->read(teamRequestedId);
                rt.requested = world->GetTeamByNetworkID(teamRequestedId);
                N_assert(rt.requested);
            }
            rt.whenRequested = N_Engine().getTimer().getMS();
            rt.requestIndex = world->teamRequestIndex++; // In case whenRequested is the same between two teams when sorting team requests
            if (
                (hasTeamToLeave==false || (hasTeamToLeave==true && rt.teamToLeave!=0)) &&
                (hasTeamRequested==false || (hasTeamRequested==true && rt.requested!=0))
                )
            {
                teamsRequested.Push(rt);
            }
        }

        if (success)
            world->teamManager->ProcessTeamAssigned(constructionBitstream);
        return success;
    }
    //------------------------------------------------------------------------
    void * TM_TeamMember::GetOwner(void) const
    {
        return owner;
    }
    //------------------------------------------------------------------------
    void TM_TeamMember::SetOwner(void * o)
    {
        owner=o;
    }
    //------------------------------------------------------------------------
    NoTeamId TM_TeamMember::GetNoTeamId(void) const
    {
        return noTeamSubcategory;
    }
    //------------------------------------------------------------------------
    Nui32 TM_TeamMember::GetWorldIndex(void) const
    {
        return world->GetTeamMemberIndex(this);
    }
    //------------------------------------------------------------------------
    Nul TM_TeamMember::ToUint32(const NetworkID & g)
    {
        return g & 0xFFFFFFFF;
    }
    //------------------------------------------------------------------------
    void TM_TeamMember::UpdateListsToNoTeam(NoTeamId nti)
    {
        teamsRequested.Clear(true);
        for (Nui32 i=0; i < teams.Size(); i++)
        {
            teams[i]->RemoveFromTeamMemberList(this);
        }
        teams.Clear(true);
        noTeamSubcategory=nti;
        joinTeamType=JOIN_NO_TEAM;
    }
    //------------------------------------------------------------------------
    bool TM_TeamMember::JoinAnyTeamCheck(void) const
    {
        // - - If already on a team, return false
        if (teams.Size() > 0)
            return false;

        // - - If any team is already in requested teams, return false.
        if (teamsRequested.Size()==0 && joinTeamType==JOIN_ANY_AVAILABLE_TEAM)
            return false;

        return true;
    }
    //------------------------------------------------------------------------
    bool TM_TeamMember::JoinSpecificTeamCheck(TM_Team *specificTeamToJoin,
        bool ignoreRequested) const
    {
        // - If already on specific team, return false
        if (IsOnTeam(specificTeamToJoin))
            return false;

        if (ignoreRequested)
            return true;

        Nui32 i;
        for (i=0; i < teamsRequested.Size(); i++)
        {
            if (teamsRequested[i].requested==specificTeamToJoin)
            {
                if (teamsRequested[i].isTeamSwitch==true)
                    return true; // Turn off team switch

                // Same thing
                return false;
            }
        }

        // Not in teams requested
        return true;
    }
    //------------------------------------------------------------------------
    bool TM_TeamMember::SwitchSpecificTeamCheck(TM_Team *teamToJoin, TM_Team *teamToLeave, bool ignoreRequested) const
    {
        N_assert(teamToJoin!=0);

        // - If already on specific team, return false
        if (IsOnTeam(teamToJoin))
            return false;

        if (teamToLeave!=0 && IsOnTeam(teamToLeave)==false)
            return false;

        if (teamToJoin==teamToLeave)
            return false;

        if (ignoreRequested)
            return true;

        Nui32 i;
        for (i=0; i < teamsRequested.Size(); i++)
        {
            if (teamsRequested[i].requested==teamToJoin)
            {
                if (teamsRequested[i].isTeamSwitch==false)
                    return true; // Different - leave team was off, turn on

                if (teamsRequested[i].teamToLeave==teamToLeave)
                    return false; // Same thing - leave all or a specific team

                // Change leave team
                return true;
            }
        }

        // Not in teams requested
        return true;
    }
    //------------------------------------------------------------------------
    bool TM_TeamMember::LeaveTeamCheck(TM_Team *team) const
    {
        if (IsOnTeam(team)==false)
            return false;
        return true;
    }
    //------------------------------------------------------------------------
    void TM_TeamMember::UpdateTeamsRequestedToAny(void)
    {
        teamsRequested.Clear(true);
        joinTeamType = JOIN_ANY_AVAILABLE_TEAM;
        whenJoinAnyRequested = N_Engine().getTimer().getMS();
        joinAnyRequestIndex = world->teamRequestIndex++; // In case whenRequested is the same between two teams when sorting team requests
    }
    //------------------------------------------------------------------------
    void TM_TeamMember::UpdateTeamsRequestedToNone(void)
    {
        teamsRequested.Clear(true);
        joinTeamType=JOIN_NO_TEAM;
    }
    //------------------------------------------------------------------------
    void TM_TeamMember::AddToRequestedTeams(TM_Team *teamToJoin)
    {
        RemoveFromRequestedTeams(teamToJoin);

        RequestedTeam rt;
        rt.isTeamSwitch = false;
        rt.requested = teamToJoin;
        rt.teamToLeave = 0;
        rt.whenRequested = N_Engine().getTimer().getMS();
        rt.requestIndex = world->teamRequestIndex++; // In case whenRequested is the same between two teams when sorting team requests
        teamsRequested.Push(rt );
    }
    //------------------------------------------------------------------------
    void TM_TeamMember::AddToRequestedTeams(TM_Team *teamToJoin, TM_Team *teamToLeave)
    {
        RemoveFromRequestedTeams(teamToJoin);

        RequestedTeam rt;
        rt.isTeamSwitch=true;
        rt.requested=teamToJoin;
        rt.teamToLeave=teamToLeave;
        rt.whenRequested=N_Engine().getTimer().getMS();
        rt.requestIndex=world->teamRequestIndex++; // In case whenRequested is the same between two teams when sorting team requests
        teamsRequested.Push(rt);
    }
    //------------------------------------------------------------------------
    bool TM_TeamMember::RemoveFromRequestedTeams(TM_Team *team)
    {
        if (team==0)
        {
            teamsRequested.Clear(true);
            joinTeamType=JOIN_NO_TEAM;
            return true;
        }
        else
        {
            Nui32 i;
            for (i=0; i < teamsRequested.Size(); i++)
            {
                if (teamsRequested[i].requested==team)
                {
                    teamsRequested.RemoveAtIndex(i);
                    if (teamsRequested.Size()==0)
                    {
                        joinTeamType=JOIN_NO_TEAM;
                    }
                    return true;
                }
            }
        }
        return false;
    }
    //------------------------------------------------------------------------
    void TM_TeamMember::AddToTeamList(TM_Team * team)
    {
        team->teamMembers.Push(this);
        teams.Push(team);
    }
    //------------------------------------------------------------------------
    void TM_TeamMember::RemoveFromSpecificTeamInternal(TM_Team * team)
    {
        Nui32 i,j;
        for (i=0; i < teams.Size(); i++)
        {
            if (teams[i]==team)
            {
                for (j=0; j < team->teamMembers.Size(); j++)
                {
                    if (team->teamMembers[j]==this)
                    {
                        team->teamMembers.RemoveAtIndex(j);
                        break;
                    }
                }
                teams.RemoveAtIndex(i);
                break;
            }
        }
    }
    //------------------------------------------------------------------------
    void TM_TeamMember::RemoveFromAllTeamsInternal(void)
    {
        TM_Team *team;
        Nui32 i,j;
        for (i=0; i < teams.Size(); i++)
        {
            team = teams[i];

            for (j=0; j < team->teamMembers.Size(); j++)
            {
                if (team->teamMembers[j]==this)
                {
                    team->teamMembers.RemoveAtIndex(j);
                    break;
                }
            }
        }
        teams.Clear(true);
    }
    //------------------------------------------------------------------------
    void TM_TeamMember::StoreLastTeams(void)
    {
        lastTeams=teams;
    }
    //------------------------------------------------------------------------
    TM_Team::TM_Team()
    {
        ID=0;
        world=0;
        joinPermissions=ALLOW_JOIN_ANY_AVAILABLE_TEAM|ALLOW_JOIN_SPECIFIC_TEAM|ALLOW_JOIN_REBALANCING;
        balancingApplies=true;
        teamMemberLimit=65535;
        owner=0;
    }
    //------------------------------------------------------------------------
    TM_Team::~TM_Team()
    {
        if (world)
            world->DereferenceTeam(this, 0);
    }
    //------------------------------------------------------------------------
    bool TM_Team::SetMemberLimit(TeamMemberLimit _teamMemberLimit, NoTeamId noTeamId)
    {
        if (teamMemberLimit==_teamMemberLimit)
            return false;

        teamMemberLimit=_teamMemberLimit;
        // Network this as request to host
        NetSerializer out;
        out.write((Nui8)ID_TEAM_BALANCER_INTERNAL);
        out.write((Nui8)ID_RUN_SetMemberLimit);
        out.write(world->GetWorldId());
        out.write(GetNetworkID());
        out.write(teamMemberLimit);
        out.write(noTeamId);
        world->GetTeamManager()->send(&out, world->GetHost(), false);

        return true;
    }
    //------------------------------------------------------------------------
    TeamMemberLimit TM_Team::GetMemberLimit(void) const
    {
        if (world->GetBalanceTeams()==false)
        {
            return teamMemberLimit;
        }
        else
        {
            TeamMemberLimit limitWithBalancing=world->GetBalancedTeamLimit();
            if (limitWithBalancing < teamMemberLimit)
                return limitWithBalancing;
            return teamMemberLimit;
        }
    }
    //------------------------------------------------------------------------
    TeamMemberLimit TM_Team::GetMemberLimitSetting(void) const
    {
        return teamMemberLimit;
    }
    //------------------------------------------------------------------------
    bool TM_Team::SetJoinPermissions(JoinPermissions _joinPermissions)
    {
        if (joinPermissions==_joinPermissions)
            return false;

        joinPermissions=_joinPermissions;

        // Network this as request to host
        NetSerializer out;
        out.write((Nui8)ID_TEAM_BALANCER_INTERNAL);
        out.write((Nui8)ID_RUN_SetJoinPermissions);
        out.write(world->GetWorldId());
        out.write(GetNetworkID());
        out.write(_joinPermissions);
        world->GetTeamManager()->send(&out,world->GetHost(), false);

        return true;
    }
    //------------------------------------------------------------------------
    JoinPermissions TM_Team::GetJoinPermissions(void) const
    {
        return joinPermissions;
    }
    //------------------------------------------------------------------------
    void TM_Team::LeaveTeam(TM_TeamMember* teamMember, NoTeamId noTeamSubcategory)
    {
        teamMember->LeaveTeam(this, noTeamSubcategory);
    }
    //------------------------------------------------------------------------
    bool TM_Team::GetBalancingApplies(void) const
    {
        return balancingApplies;
    }
    //------------------------------------------------------------------------
    void TM_Team::GetTeamMembers(DataStructures::List<TM_TeamMember*> &_teamMembers) const
    {
        _teamMembers=teamMembers;
    }
    //------------------------------------------------------------------------
    Nui32 TM_Team::GetTeamMembersCount(void) const
    {
        return teamMembers.Size();
    }
    //------------------------------------------------------------------------
    TM_TeamMember *TM_Team::GetTeamMemberByIndex(Nui32 index) const
    {
        return teamMembers[index];
    }
    //------------------------------------------------------------------------
    NetworkID TM_Team::GetNetworkID(void) const
    {
        return ID;
    }
    //------------------------------------------------------------------------
    TM_World* TM_Team::GetTM_World(void) const
    {
        return world;
    }
    //------------------------------------------------------------------------
    void TM_Team::SerializeConstruction(NetSerializer *constructionBitstream)
    {
        // Do not need to serialize member lists, the team members do this
        constructionBitstream->write(world->GetWorldId());
        constructionBitstream->write(ID);
        constructionBitstream->write(joinPermissions);
        constructionBitstream->write(balancingApplies);
        constructionBitstream->write(teamMemberLimit);

    }
    //------------------------------------------------------------------------
    bool TM_Team::DeserializeConstruction(TeamManager *teamManager, NetSerializer *constructionBitstream)
    {
        WorldId worldId;
        constructionBitstream->read(worldId);
        TM_World *world = teamManager->GetWorldWithId(worldId);
        N_assert(world);
        constructionBitstream->read(ID);
        constructionBitstream->read(joinPermissions);
        constructionBitstream->read(balancingApplies);
        bool b = constructionBitstream->read(teamMemberLimit);
        N_assert(b);
        if (b)
        {
            world->ReferenceTeam(this,ID,balancingApplies);
        }
        return b;
    }
    //------------------------------------------------------------------------
    Nul TM_Team::ToUint32(const NetworkID & g)
    {
        return g & 0xFFFFFFFF;
    }
    //------------------------------------------------------------------------
    void *TM_Team::GetOwner(void) const
    {
        return owner;
    }
    //------------------------------------------------------------------------
    Nui32 TM_Team::GetWorldIndex(void) const
    {
        return world->GetTeamIndex(this);
    }
    //------------------------------------------------------------------------
    void TM_Team::SetOwner(void *o)
    {
        owner=o;
    }
    //------------------------------------------------------------------------
    void TM_Team::RemoveFromTeamMemberList(TM_TeamMember *teamMember)
    {
        Nui32 index = teamMembers.GetIndexOf(teamMember);
        N_assert(index != (Nui32) -1);
        teamMembers.RemoveAtIndex(index);
    }
    //------------------------------------------------------------------------
    Nui32 TM_Team::GetMemberWithRequestedSingleTeamSwitch(TM_Team *team)
    {
        Nui32 i;
        for (i=0; i < teamMembers.Size(); i++)
        {
            if (teamMembers[i]->GetCurrentTeamCount()==1)
            {
                Nui32 j = teamMembers[i]->GetRequestedTeamIndex(team);
                if (j!=(Nui32)-1)
                {
                    if (teamMembers[i]->teamsRequested[j].isTeamSwitch &&
                        (teamMembers[i]->teamsRequested[j].teamToLeave==0 ||
                        teamMembers[i]->teamsRequested[j].teamToLeave==teamMembers[i]->teams[0])
                    )
                    return i;
                }
            }
        }
        return (Nui32) -1;
    }
    //------------------------------------------------------------------------
    TM_World::TM_World()
    {
        teamManager=0;
        balanceTeamsIsActive=false;
        hostGuid=UniqueID::INVALID;
        worldId=0;
        autoAddParticipants=true;
        teamRequestIndex=0;
    }
    //------------------------------------------------------------------------
    TM_World::~TM_World()
    {
        Clear();
    }
    //------------------------------------------------------------------------
    TeamManager *TM_World::GetTeamManager(void) const
    {
        return teamManager;
    }
    //------------------------------------------------------------------------
    void TM_World::AddParticipant(UniqueID id)
    {
        participants.Push(id);

        // send to remote system status of balanceTeamsIsActive

        if (GetTeamManager()->getID()==GetHost())
        {
            // Actually just transmitting initial value of balanceTeamsIsActive
            NetSerializer out;
            out.write((Nui8)ID_TEAM_BALANCER_INTERNAL);
            out.write((Nui8)ID_RUN_SetBalanceTeamsInitial);
            out.write(GetWorldId());
            out.write(balanceTeamsIsActive);
            teamManager->send(&out, PL_HIGH, PT_ORDER_MAKE_SURE, 0,id, false);
        }
    }
    //------------------------------------------------------------------------
    void TM_World::RemoveParticipant(UniqueID id)
    {
        Nui32 i;
        i = participants.GetIndexOf(id);
        if (i!=(Nui32)-1)
            participants.RemoveAtIndex(i);
    }
    //------------------------------------------------------------------------
    void TM_World::SetAutoManageConnections(bool autoAdd)
    {
        autoAddParticipants=autoAdd;
    }
    //------------------------------------------------------------------------
    void TM_World::GetParticipantList(DataStructures::List<UniqueID> &participantList)
    {
        participantList = participants;
    }
    //------------------------------------------------------------------------
    void TM_World::ReferenceTeam(TM_Team *team, NetworkID networkId, bool applyBalancing)
    {
        Nui32 i;
        for (i=0; i < teams.Size(); i++)
        {
            if (teams[i]==team)
                return;
        }

        team->ID=networkId;
        team->balancingApplies=applyBalancing;
        team->world=this;

        // Add this team to the list of teams
        teams.Push(team);

        teamsHash.Push(networkId, team);

        // If autobalancing is on, and the team lock state supports it, then call EnforceTeamBalancing()
        if (applyBalancing && balanceTeamsIsActive)
        {
            EnforceTeamBalance(0);
        }
    }
    //------------------------------------------------------------------------
    void TM_World::DereferenceTeam(TM_Team *team, NoTeamId noTeamSubcategory)
    {
        Nui32 i;
        for (i=0; i < teams.Size(); i++)
        {
            if (teams[i]==team)
            {
                TM_Team *team = teams[i];
                while (team->teamMembers.Size())
                {
                    team->teamMembers[team->teamMembers.Size()-1]->LeaveTeam(team, noTeamSubcategory);
                }
                teams.RemoveAtIndex(i);

                teamsHash.Remove(team->GetNetworkID());

                break;
            }
        }
    }
    //------------------------------------------------------------------------
    Nui32 TM_World::GetTeamCount(void) const
    {
        return teams.Size();
    }
    //------------------------------------------------------------------------
    TM_Team * TM_World::GetTeamByIndex(Nui32 index) const
    {
        return teams[index];
    }
    //------------------------------------------------------------------------
    TM_Team * TM_World::GetTeamByNetworkID(NetworkID teamId)
    {
        DataStructures::HashIndex hi = teamsHash.GetIndexOf(teamId);
        if (hi.IsInvalid())
            return 0;
        return teamsHash.ItemAtIndex(hi);
    }
    //------------------------------------------------------------------------
    Nui32 TM_World::GetTeamIndex(const TM_Team * team) const
    {
        Nui32 i;
        for (i=0; i < teams.Size(); i++)
        {
            if (teams[i]==team)
                return i;
        }
        return (Nui32) -1;
    }
    //------------------------------------------------------------------------
    void TM_World::ReferenceTeamMember(TM_TeamMember *teamMember, NetworkID networkId)
    {
        Nui32 i;
        for (i=0; i < teamMembers.Size(); i++)
        {
            if (teamMembers[i]==teamMember)
                return;
        }

        teamMember->world=this;
        teamMember->networkId=networkId;

        teamMembers.Push(teamMember);

        teamMembersHash.Push(networkId, teamMember);
    }
    //------------------------------------------------------------------------
    void TM_World::DereferenceTeamMember(TM_TeamMember *teamMember)
    {
        Nui32 i;
        for (i=0; i < teamMembers.Size(); i++)
        {
            if (teamMembers[i]==teamMember)
            {
                teamMembers[i]->UpdateListsToNoTeam(0);
                teamMembersHash.Remove(teamMembers[i]->GetNetworkID());
                teamMembers.RemoveAtIndex(i);
                break;
            }
        }
    }
    //------------------------------------------------------------------------
    Nui32 TM_World::GetTeamMemberCount(void) const
    {
        return teamMembers.Size();
    }
    //------------------------------------------------------------------------
    TM_TeamMember *TM_World::GetTeamMemberByIndex(Nui32 index) const
    {
        return teamMembers[index];
    }
    //------------------------------------------------------------------------
    NetworkID TM_World::GetTeamMemberIDByIndex(Nui32 index) const
    {
        return teamMembers[index]->GetNetworkID();
    }
    //------------------------------------------------------------------------
    TM_TeamMember *TM_World::GetTeamMemberByNetworkID(NetworkID teamMemberId)
    {
        DataStructures::HashIndex hi = teamMembersHash.GetIndexOf(teamMemberId);
        if (hi.IsInvalid())
            return 0;
        return teamMembersHash.ItemAtIndex(hi);
    }
    //------------------------------------------------------------------------
    Nui32 TM_World::GetTeamMemberIndex(const TM_TeamMember *teamMember) const
    {
        Nui32 i;
        for (i=0; i < teamMembers.Size(); i++)
        {
            if (teamMembers[i]==teamMember)
                return i;
        }
        return (Nui32) -1;
    }
    //------------------------------------------------------------------------
    bool TM_World::SetBalanceTeams(bool balanceTeams, NoTeamId noTeamId)
    {
        if (balanceTeams==balanceTeamsIsActive)
            return false;

        balanceTeamsIsActive=balanceTeams;

        // Network this as request to host
        NetSerializer out;
        out.write((Nui8)ID_TEAM_BALANCER_INTERNAL);
        out.write((Nui8)ID_RUN_SetBalanceTeams);
        out.write(GetWorldId());
        out.write(balanceTeams);
        out.write(noTeamId);
        GetTeamManager()->send(&out,PL_HIGH, PT_ORDER_MAKE_SURE, 0, GetHost(), false);

        return true;
    }
    //------------------------------------------------------------------------
    bool TM_World::GetBalanceTeams(void) const
    {
        return balanceTeamsIsActive;
    }
    //------------------------------------------------------------------------
    void TM_World::SetHost(UniqueID _hostGuid)
    {
        if (hostGuid==_hostGuid)
            return;

        N_assert(_hostGuid!=UniqueID::INVALID);

        hostGuid=_hostGuid;

        if (GetHost()==GetTeamManager()->getID())
            FillRequestedSlots();
    }
    //------------------------------------------------------------------------
    UniqueID TM_World::GetHost(void) const
    {
        return hostGuid;
    }
    //------------------------------------------------------------------------
    WorldId TM_World::GetWorldId(void) const
    {
        return worldId;
    }
    //------------------------------------------------------------------------
    void TM_World::Clear(void)
    {
        for (Nui32 i=0; i < teams.Size(); i++)
        {
            teams[i]->world=0;
        }
        for (Nui32 i=0; i < teamMembers.Size(); i++)
        {
            teamMembers[i]->world=0;
        }
        participants.Clear(true);
        teams.Clear(true);
        teamMembers.Clear(true);
    }
    //------------------------------------------------------------------------
    void TM_World::onConnectClose(const Address & address, UniqueID id,
        ConnectCloseType lostConnectionReason )
    {
        (void) lostConnectionReason;
        (void) address;

        RemoveParticipant(id);
    }
    //------------------------------------------------------------------------
    void TM_World::onConnect(const Address & address, UniqueID id)
    {
        (void) address;
        if (autoAddParticipants)
            AddParticipant(id);
    }
    //------------------------------------------------------------------------
    void TM_World::EnforceTeamBalance(NoTeamId noTeamId)
    {
        // Host only function
        N_assert(GetHost()==GetTeamManager()->getID());

        KickExcessMembers(noTeamId);
    }
    //------------------------------------------------------------------------
    void TM_World::KickExcessMembers(NoTeamId noTeamId)
    {
        // Host only function
        N_assert(GetHost()==GetTeamManager()->getID());

        // For each team that applies balancing, if the team is overfull, put on a team that is not overfull if the team has ALLOW_JOIN_REBALANCING set
        // If cannot move the player to another team, just take the player off the team and set to noTeamId if they have no team at that point

        TeamMemberLimit balancedTeamLimit;
        if (balanceTeamsIsActive)
            balancedTeamLimit = GetBalancedTeamLimit();
        else
            balancedTeamLimit = (TeamMemberLimit) -1;

        TM_Team *team, *teamToJoin;
        Nui32 i, teamIndex;
        for (i=0; i < teams.Size(); i++)
        {
            team = teams[i];
            while (team->GetMemberLimitSetting() < team->GetTeamMembersCount() ||
                (balancedTeamLimit < team->GetTeamMembersCount() && team->GetBalancingApplies()) )
            {
                TM_TeamMember *teamMember = team->teamMembers[team->teamMembers.Size()-1];

                teamIndex = GetAvailableTeamIndexWithFewestMembers(balancedTeamLimit, ALLOW_JOIN_REBALANCING);
                if (teamIndex == (Nui32)-1)
                {
                    // Move this member to no team
                    teamMember->LeaveTeam(team, noTeamId);
                    teamManager->PushTeamAssigned(teamMember);
                }
                else
                {
                    teamToJoin = teams[teamIndex];

                    // Move this member
                    teamMember->StoreLastTeams();
                    teamManager->RemoveFromTeamsRequestedAndAddTeam(teamMember, teamToJoin, true, team);

                    NetSerializer out;
                    out.write((Nui8)ID_TEAM_BALANCER_INTERNAL);
                    out.write((Nui8)ID_RUN_RemoveFromTeamsRequestedAndAddTeam);
                    out.write(GetWorldId());
                    out.write(teamMember->GetNetworkID());
                    out.write(teamToJoin->GetNetworkID());
                    out.write1();
                    out.write1();
                    out.write(team->GetNetworkID());
                    BroadcastToParticipants(&out, UniqueID::INVALID);
                }

            }
        }
    }
    //------------------------------------------------------------------------
    void TM_World::FillRequestedSlots(void)
    {
        // Host only function
        N_assert(GetHost()==GetTeamManager()->getID());


        TeamMemberLimit balancedTeamLimit;
        if (balanceTeamsIsActive)
            balancedTeamLimit = GetBalancedTeamLimit();
        else
            balancedTeamLimit = (TeamMemberLimit) -1;

        Nui32 teamIndex, indexIntoTeamsRequested = (Nui32)-1;
        TM_Team *team;
        TM_TeamMember *teamMember;
        DataStructures::OrderedList<TM_World::JoinRequestHelper, TM_World::JoinRequestHelper, JoinRequestHelperComp> joinRequests;
        GetSortedJoinRequests(joinRequests);
        Nui32 joinRequestIndex;

        for (joinRequestIndex=0; joinRequestIndex < joinRequests.Size(); joinRequestIndex++)
        {
            teamMember = teamMembers[joinRequests[joinRequestIndex].teamMemberIndex];
            if (teamMember->teamsRequested.Size()==0)
            {
                if (teamMember->joinTeamType==JOIN_ANY_AVAILABLE_TEAM)
                    teamIndex = GetAvailableTeamIndexWithFewestMembers(balancedTeamLimit, ALLOW_JOIN_ANY_AVAILABLE_TEAM);
                else
                    teamIndex=(Nui32)-1;
            }
            else
            {
                indexIntoTeamsRequested = joinRequests[joinRequestIndex].indexIntoTeamsRequested;

                team = teamMember->teamsRequested[indexIntoTeamsRequested].requested;
                if (team->GetTeamMembersCount() < balancedTeamLimit &&
                    team->GetTeamMembersCount() < team->GetMemberLimitSetting() &&
                    (ALLOW_JOIN_SPECIFIC_TEAM & team->GetJoinPermissions())!=0)
                {
                    teamIndex=teams.GetIndexOf(team);
                }
                else
                {
                    teamIndex=(Nui32)-1;
                }
            }

            if (teamIndex != (Nui32)-1)
            {
                team = teams[teamIndex];

                if (teamMember->teamsRequested.Size()==0)
                {
                    if (teamMember->joinTeamType==JOIN_ANY_AVAILABLE_TEAM)
                    {
                        // Join any
                        teamMember->StoreLastTeams();
                        teamMember->UpdateTeamsRequestedToNone();
                        teamMember->AddToTeamList(teams[teamIndex]);
                        teamManager->PushTeamAssigned(teamMember);

                        NetSerializer out;
                        out.write((Nui8)ID_TEAM_BALANCER_INTERNAL);
                        out.write((Nui8)ID_RUN_UpdateTeamsRequestedToNoneAndAddTeam);
                        out.write(GetWorldId());
                        out.write(teamMember->GetNetworkID());
                        out.write(team->GetNetworkID());
                        BroadcastToParticipants(&out, UniqueID::INVALID);
                    }
                }
                else
                {
                    // Switch or join specific
                    DataStructures::List<TM_Team*> teamsWeAreLeaving;
                    bool isSwitch = teamMember->teamsRequested[indexIntoTeamsRequested].isTeamSwitch;
                    TM_Team *teamToLeave;
                    if (isSwitch)
                    {
                        teamToLeave=teamMember->teamsRequested[indexIntoTeamsRequested].teamToLeave;
                        if (teamToLeave)
                        {
                            if (teamMember->IsOnTeam(teamToLeave))
                            {
                                teamsWeAreLeaving.Push(teamToLeave);
                            }
                            else
                            {
                                teamToLeave=0;
                                isSwitch=false;
                            }
                        }
                        else
                        {
                            teamsWeAreLeaving=teamMember->teams;
                        }
                    }
                    else
                        teamToLeave=0;

                    int teamJoined = JoinSpecificTeam(teamMember, team, isSwitch, teamToLeave, teamsWeAreLeaving);

                    if(teamJoined == 1)
                    {
                        NetSerializer out;
                        out.write((Nui8)ID_TEAM_BALANCER_INTERNAL);
                        out.write((Nui8)ID_RUN_RemoveFromTeamsRequestedAndAddTeam);
                        out.write(GetWorldId());
                        out.write(teamMember->GetNetworkID());
                        out.write(team->GetNetworkID());
                        out.write(isSwitch);
                        if(teamToLeave != 0)
                        {
                            out.write1();
                            out.write(teamToLeave->GetNetworkID());
                        }
                        else
                            out.write0();
                        BroadcastToParticipants(&out, UniqueID::INVALID);
                    }
                }
            }
        }
    }
    //------------------------------------------------------------------------
    Nui32 TM_World::GetAvailableTeamIndexWithFewestMembers(TeamMemberLimit secondaryLimit, JoinPermissions joinPermissions)
    {
        Nui32 teamIndex;

        Nui32 lowestTeamMembers = (Nui32) -1;
        Nui32 lowestIndex = (Nui32) -1;

        for (teamIndex=0; teamIndex < teams.Size(); teamIndex++)
        {
            if (teams[teamIndex]->GetTeamMembersCount() < secondaryLimit &&
                teams[teamIndex]->GetTeamMembersCount() < teams[teamIndex]->GetMemberLimitSetting() &&
                teams[teamIndex]->GetTeamMembersCount() < lowestTeamMembers &&
                (joinPermissions & teams[teamIndex]->GetJoinPermissions())!=0)
            {
                lowestTeamMembers = teams[teamIndex]->GetTeamMembersCount();
                lowestIndex = teamIndex;
            }
        }

        return lowestIndex;
    }
    //------------------------------------------------------------------------
    void TM_World::GetSortedJoinRequests(DataStructures::OrderedList<TM_World::JoinRequestHelper, TM_World::JoinRequestHelper, JoinRequestHelperComp> &joinRequests)
    {
        Nui32 i;

        for (i=0; i < teamMembers.Size(); i++)
        {
            TM_TeamMember *teamMember = teamMembers[i];
            if (teamMember->teamsRequested.Size()==0)
            {
                if (teamMember->joinTeamType==JOIN_ANY_AVAILABLE_TEAM)
                {
                    TM_World::JoinRequestHelper jrh;
                    jrh.whenRequestMade=teamMember->whenJoinAnyRequested;
                    jrh.teamMemberIndex=i;
                    jrh.requestIndex=teamMember->joinAnyRequestIndex;
                    joinRequests.Insert(jrh, jrh, true);
                }
            }
            else
            {
                Nui32 j;
                for (j=0; j < teamMember->teamsRequested.Size(); j++)
                {
                    TM_World::JoinRequestHelper jrh;
                    jrh.whenRequestMade=teamMember->teamsRequested[j].whenRequested;
                    jrh.teamMemberIndex=i;
                    jrh.indexIntoTeamsRequested=j;
                    jrh.requestIndex=teamMember->teamsRequested[j].requestIndex;
                    joinRequests.Insert(jrh, jrh, true);
                }

            }
        }
    }
    //------------------------------------------------------------------------
    void TM_World::BroadcastToParticipants(NetSerializer * out, UniqueID exclusionGuid)
    {
        for (NCount i = 0; i < participants.Size(); ++i)
        {
            if (participants[i]==exclusionGuid)
                continue;
            teamManager->send(out, PL_HIGH, PT_ORDER_MAKE_SURE, 0, participants[i], false);
        }
    }
    //------------------------------------------------------------------------
    void TM_World::BroadcastToParticipants(Nui8 * data, int length, UniqueID exclusionGuid)
    {
        for (NCount i = 0; i < participants.Size(); ++i)
        {
            if (participants[i] == exclusionGuid)
                continue;
            teamManager->send(data, length, PL_HIGH, PT_ORDER_MAKE_SURE, 0, participants[i], false);
        }
    }
    //------------------------------------------------------------------------
    TM_Team* TM_World::JoinAnyTeam(TM_TeamMember *teamMember, int *resultCode)
    {
        TeamMemberLimit balancedLimit = GetBalancedTeamLimit();

        Nui32 idx = GetAvailableTeamIndexWithFewestMembers(balancedLimit, ALLOW_JOIN_ANY_AVAILABLE_TEAM);
        if (idx == (Nui32 ) -1)
        {
            // If any team is joinable but full, return full. Otherwise return locked
            for (idx=0; idx < teams.Size(); idx++)
            {
                if ((teams[idx]->GetTeamMembersCount() >= balancedLimit ||
                    teams[idx]->GetTeamMembersCount()  >= teams[idx]->GetMemberLimitSetting()) &&
                    teams[idx]->GetMemberLimitSetting() != 0 &&
                    (ALLOW_JOIN_ANY_AVAILABLE_TEAM & teams[idx]->GetJoinPermissions())!=0)
                {
                    // Full
                    *resultCode=-2;
                    return teams[idx];
                }
            }

            // Locked
            *resultCode=-1;
            return 0;
        }

        TM_Team* lowestMemberTeam = teams[idx];

        teamMember->StoreLastTeams();
        teamMember->UpdateTeamsRequestedToNone();
        teamMember->AddToTeamList(lowestMemberTeam);
        teamManager->PushTeamAssigned(teamMember);

        *resultCode=1;
        return lowestMemberTeam;
    }
    //------------------------------------------------------------------------
    int TM_World::JoinSpecificTeam(TM_TeamMember *teamMember, TM_Team *team, bool isTeamSwitch, TM_Team *teamToLeave, DataStructures::List<TM_Team*> &teamsWeAreLeaving)
    {
        if (team->GetJoinPermissions() & ALLOW_JOIN_SPECIFIC_TEAM)
        {
            if (balanceTeamsIsActive==false || teamsWeAreLeaving.Size()==0)
            {
                if (team->GetMemberLimit() > team->GetTeamMembersCount())
                {
                    // Can join normally
                    teamMember->StoreLastTeams();
                    teamManager->RemoveFromTeamsRequestedAndAddTeam(teamMember, team, isTeamSwitch, teamToLeave);
                    return 1;
                }
                else
                {
                    // Full
                    return -2;
                }
            }
            else
            {
                // Note: balanceTeamsIsActive==true && isTeamSwitch==true

                // Do limited team swap
                // We must be on one team, target must be on one team, and we want to exchange teams
                if (teamsWeAreLeaving.Size()==1)
                {
                    Nui32 j = team->GetMemberWithRequestedSingleTeamSwitch(teamsWeAreLeaving[0]);
                    if (j!=(Nui32)-1)
                    {
                        TM_TeamMember *swappingMember = team->teamMembers[j];
                        teamMember->StoreLastTeams();
                        swappingMember->StoreLastTeams();
                        teamManager->RemoveFromTeamsRequestedAndAddTeam(teamMember, team, true, 0);
                        teamManager->RemoveFromTeamsRequestedAndAddTeam(swappingMember, teamsWeAreLeaving[0], true, 0);

                        // send ID_TEAM_BALANCER_TEAM_ASSIGNED to all, for swapped member
                        // Calling function sends ID_RUN_RemoveFromTeamsRequestedAndAddTeam which pushes ID_TEAM_BALANCER_TEAM_ASSIGNED for teamMember
                        NetSerializer out;
                        out.write((Nui8)ID_TEAM_BALANCER_TEAM_ASSIGNED);
                        teamManager->EncodeTeamAssigned(&out, swappingMember);
                        BroadcastToParticipants(&out, UniqueID::INVALID);

                        return 1;
                    }
                }

                // Full
                return -2;
            }
        }
        else
        {
            // Locked
            return -1;
        }
    }
    //------------------------------------------------------------------------
    TeamMemberLimit TM_World::GetBalancedTeamLimit(void) const
    {
        if (teams.Size()==0)
            return 0;

        if (balanceTeamsIsActive==false)
            return (TeamMemberLimit) -1;

        Nui32 i;
        bool additionalTeamsExcluded;
        TeamMemberLimit balancedLimit;
        Nui32 teamsCount=teams.Size();
        Nui32 membersCount=teamMembers.Size();
        DataStructures::List<TM_Team*> consideredTeams = teams;

        do
        {
            additionalTeamsExcluded=false;
            balancedLimit = (TeamMemberLimit) ((membersCount+(teamsCount-1))/(teamsCount));
            i=0;
            while (i < consideredTeams.Size())
            {
                if (consideredTeams[i]->GetMemberLimitSetting() < balancedLimit)
                {
                    additionalTeamsExcluded=true;
                    membersCount-=consideredTeams[i]->GetMemberLimitSetting();
                    teamsCount--;
                    consideredTeams.RemoveAtIndexFast(i);
                }
                else
                {
                    i++;
                }
            }

        } while (additionalTeamsExcluded==true && teamsCount>0);

        return balancedLimit;
    }
    //------------------------------------------------------------------------
    TeamManager::TeamManager()
    {
        for (Nui32 i=0; i < 255; i++)
            worldsArray[i]=0;
        autoAddParticipants=true;
        topology=TM_PEER_TO_PEER;
    }
    //------------------------------------------------------------------------
    TeamManager::~TeamManager()
    {
        Clear();
    }
    //------------------------------------------------------------------------
    TM_World* TeamManager::AddWorld(WorldId worldId)
    {
        N_assert(worldsArray[worldId]==0 && "World already in use");

        TM_World * newWorld = N_new TM_World();
        newWorld->worldId=worldId;
        newWorld->teamManager=this;
        newWorld->hostGuid=getID();
        worldsArray[worldId]=newWorld;
        worldsList.Push(newWorld);
        return newWorld;
    }
    //------------------------------------------------------------------------
    void TeamManager::RemoveWorld(WorldId worldId)
    {
        N_assert(worldsArray[worldId]!=0 && "World not in use");
        for (Nui32 i=0; i < worldsList.Size(); i++)
        {
            if (worldsList[i]==worldsArray[worldId])
            {
                N_delete worldsList[i];
                worldsList.RemoveAtIndexFast(i);
                break;
            }
        }
        worldsArray[worldId]=0;
    }
    //------------------------------------------------------------------------
    Nui32 TeamManager::GetWorldCount(void) const
    {
        return worldsList.Size();
    }
    //------------------------------------------------------------------------
    TM_World * TeamManager::GetWorldAtIndex(Nui32 index) const
    {
        return worldsList[index];
    }
    //------------------------------------------------------------------------
    TM_World * TeamManager::GetWorldWithId(WorldId worldId) const
    {
        return worldsArray[worldId];
    }
    //------------------------------------------------------------------------
    void TeamManager::SetAutoManageConnections(bool autoAdd)
    {
        autoAddParticipants=autoAdd;

        for (Nui32 i=0; i < worldsList.Size(); i++)
        {
            worldsList[i]->SetAutoManageConnections(autoAdd);
        }
    }
    //------------------------------------------------------------------------
    void TeamManager::SetTopology(TMTopology _topology)
    {
        topology=_topology;
    }
    //------------------------------------------------------------------------
    void TeamManager::EncodeTeamFull(NetSerializer *out, TM_TeamMember *teamMember, TM_Team *team)
    {
        out->write((Nui8)ID_TEAM_BALANCER_REQUESTED_TEAM_FULL);
        EncodeTeamFullOrLocked(out, teamMember, team);
    }
    //------------------------------------------------------------------------
    void TeamManager::DecomposeTeamFull(SocketMessage *msg,
        TM_World **world, TM_TeamMember **teamMember, TM_Team **team,
            Nui16 &currentMembers, Nui16 &memberLimitIncludingBalancing,
                bool &balancingIsActive, JoinPermissions &joinPermissions)
    {
        NetSerializer bsIn(msg->mData, msg->mLength, false);
        bsIn.skipRead(1, 0);
        DecomposeTeamFullOrLocked(&bsIn, world, teamMember, team, currentMembers, memberLimitIncludingBalancing, balancingIsActive, joinPermissions);
    }
    //------------------------------------------------------------------------
    void TeamManager::EncodeTeamLocked(NetSerializer *out, TM_TeamMember *teamMember, TM_Team *team)
    {
        out->write((Nui8)ID_TEAM_BALANCER_REQUESTED_TEAM_LOCKED);
        EncodeTeamFullOrLocked(out, teamMember, team);
    }
    //------------------------------------------------------------------------
    void TeamManager::EncodeTeamFullOrLocked(NetSerializer *out, TM_TeamMember *teamMember, TM_Team *team)
    {
        out->write(teamMember->world->GetWorldId());
        out->write(teamMember->GetNetworkID());
        out->write(team->GetNetworkID());
        out->write((Nui16)team->GetTeamMembersCount());
        out->write(team->GetMemberLimit());
        out->write(team->GetBalancingApplies());
        out->write(team->GetJoinPermissions());
    }
    //------------------------------------------------------------------------
    void TeamManager::DecomposeTeamFullOrLocked(NetSerializer *bsIn, TM_World **world, TM_TeamMember **teamMember, TM_Team **team,
        Nui16 &currentMembers, Nui16 &memberLimitIncludingBalancing, bool &balancingIsActive, JoinPermissions &joinPermissions)
    {
        WorldId worldId;
        NetworkID teamMemberId;
        NetworkID teamId;

        *teamMember=0;
        *team=0;
        *world=0;

        bsIn->read(worldId);
        bsIn->read(teamMemberId);
        bsIn->read(teamId);
        bsIn->read(currentMembers);
        bsIn->read(memberLimitIncludingBalancing);
        bsIn->read(balancingIsActive);
        bsIn->read(joinPermissions);

        *world = GetWorldWithId(worldId);
        if (*world)
        {
            *teamMember = (*world)->GetTeamMemberByNetworkID(teamMemberId);
            *team = (*world)->GetTeamByNetworkID(teamId);
        }
    }
    //------------------------------------------------------------------------
    void TeamManager::DecomposeTeamLocked(SocketMessage * msg,
        TM_World ** world, TM_TeamMember ** teamMember, TM_Team ** team,
            Nui16 & currentMembers, Nui16 & memberLimitIncludingBalancing,
                bool & balancingIsActive, JoinPermissions & joinPermissions)
    {
        NetSerializer bsIn(msg->mData, msg->mLength, false);
        bsIn.skipRead(1, 0);
        DecomposeTeamFullOrLocked(&bsIn, world, teamMember, team, currentMembers, memberLimitIncludingBalancing, balancingIsActive, joinPermissions);
    }
    //------------------------------------------------------------------------
    void TeamManager::EncodeTeamAssigned(NetSerializer * out, TM_TeamMember * teamMember)
    {
        out->write(teamMember->world->GetWorldId());
        out->write(teamMember->GetNetworkID());
        out->write((Nui16)teamMember->teams.Size());
        for(Nui32 i = 0; i < teamMember->teams.Size(); ++i)
        {
            out->write(teamMember->teams[i]->GetNetworkID());
        }
        out->write(teamMember->noTeamSubcategory);
        out->write(teamMember->joinTeamType);
    }
    //------------------------------------------------------------------------
    void TeamManager::ProcessTeamAssigned(NetSerializer *bsIn)
    {
        TM_World *world;
        TM_TeamMember *teamMember;
        NoTeamId noTeamId;
        JoinTeamType joinTeamType;
        DataStructures::List<TM_Team *> newTeam;
        DataStructures::List<TM_Team *> teamsLeft;
        DataStructures::List<TM_Team *> teamsJoined;
        DecodeTeamAssigned(bsIn, &world, &teamMember, noTeamId, joinTeamType, newTeam, teamsLeft, teamsJoined);
        if (teamMember)
        {
            teamMember->StoreLastTeams();
            for (Nui32 i=0; i < teamsLeft.Size(); i++)
            {
                teamMember->RemoveFromSpecificTeamInternal(teamsLeft[i]);
            }
            for (Nui32 i=0; i < teamsJoined.Size(); i++)
            {
                if (teamMember->IsOnTeam(teamsJoined[i])==false)
                {
                    teamMember->RemoveFromRequestedTeams(teamsJoined[i]);
                    teamMember->AddToTeamList(teamsJoined[i]);
                }
            }
            teamMember->noTeamSubcategory=noTeamId;
            teamMember->joinTeamType=joinTeamType;
        }
    }
    //------------------------------------------------------------------------
    void TeamManager::DecodeTeamAssigned(SocketMessage * msg, TM_World ** world,
        TM_TeamMember ** teamMember)
    {
        WorldId worldId;
        NetworkID teamMemberId;

        NetSerializer bsIn(msg->mData, msg->mLength, false);
        bsIn.skipRead(1, 0);
        bsIn.read(worldId);
        bsIn.read(teamMemberId);
        *world = GetWorldWithId(worldId);
        if (*world)
        {
            *teamMember = (*world)->GetTeamMemberByNetworkID(teamMemberId);
        }
        else
        {
            *teamMember=0;
        }
    }
    //------------------------------------------------------------------------
    void TeamManager::DecodeTeamCancelled(SocketMessage * msg, TM_World ** world,
        TM_TeamMember ** teamMember, TM_Team ** teamCancelled)
    {
        WorldId worldId;
        NetworkID teamMemberId;

        NetSerializer bsIn(msg->mData, msg->mLength, false);
        bsIn.skipRead(1, 0);
        bsIn.read(worldId);
        bsIn.read(teamMemberId);
        bool sp=false;
        *world = GetWorldWithId(worldId);
        if (*world)
        {
            *teamMember = (*world)->GetTeamMemberByNetworkID(teamMemberId);
        }
        else
        {
            *teamMember=0;
        }

        bsIn.read(sp);
        if (sp)
        {
            NetworkID nid;
            bsIn.read(nid);
            *teamCancelled = (*world)->GetTeamByNetworkID(nid);
        }
        else
        {
            *teamCancelled = 0;
        }
    }
    //------------------------------------------------------------------------
    void TeamManager::DecodeTeamAssigned(NetSerializer *bsIn, TM_World **world, TM_TeamMember **teamMember, NoTeamId &noTeamId,
                                            JoinTeamType &joinTeamType, DataStructures::List<TM_Team *> &newTeam,
                                            DataStructures::List<TM_Team *> &teamsLeft, DataStructures::List<TM_Team *> &teamsJoined
                                            )
    {
        newTeam.Clear(true);
        teamsLeft.Clear(true);
        teamsJoined.Clear(true);

        WorldId worldId;
        NetworkID teamMemberId;
        NetworkID teamId;

        bsIn->read(worldId);
        bsIn->read(teamMemberId);
        *world = GetWorldWithId(worldId);
        if (*world)
        {
            *teamMember = (*world)->GetTeamMemberByNetworkID(teamMemberId);
            Nui16 teamsCount;
            bsIn->read(teamsCount);

            for (Nui32 i=0; i < teamsCount; i++)
            {
                bsIn->read(teamId);
                TM_Team * team = (*world)->GetTeamByNetworkID(teamId);
                N_assert(team);
                if (team)
                    newTeam.Push(team);
                // else probably didn't reference team first
            }

            if (*teamMember)
            {
                for (Nui32 i=0; i < (*teamMember)->teams.Size(); i++)
                {
                    TM_Team *team = (*teamMember)->teams[i];
                    if (newTeam.GetIndexOf(team)==(Nui32)-1)
                        teamsLeft.Push(team);
                }
            }

            for (Nui32 i=0; i < newTeam.Size(); i++)
            {
                TM_Team *team = newTeam[i];
                if ((*teamMember)->teams.GetIndexOf(team)==(Nui32)-1)
                    teamsJoined.Push(team);
            }

            bsIn->read(noTeamId);
            bsIn->read(joinTeamType);
        }
        else
        {
            *teamMember=0;
        }
    }
    //------------------------------------------------------------------------
    void TeamManager::Clear(void)
    {
        for (Nui32 i=0; i < worldsList.Size(); i++)
        {
            worldsArray[worldsList[i]->worldId]=0;
            worldsList[i]->Clear();
            delete worldsList[i];
        }
        worldsList.Clear(false);
    }
    //------------------------------------------------------------------------
    void TeamManager::update()
    {
    }
    //------------------------------------------------------------------------
    ConnectReceiveType TeamManager::onMessage(SocketMessage * msg)
    {
        switch (msg->mData[0])
        {
        case ID_FCM2_NEW_HOST:
            {
                NCount i;
                for (i = 0; i < worldsList.Size(); ++i)
                    worldsList[i]->SetHost(msg->mID);
            }
            break;
        case ID_TEAM_BALANCER_TEAM_ASSIGNED:
            {
                NetSerializer bsIn(msg->mData, msg->mLength, false);
                bsIn.skipRead(1, 0);
                ProcessTeamAssigned(&bsIn);
            }
            break;
        case ID_TEAM_BALANCER_TEAM_REQUESTED_CANCELLED:
            {
                NetSerializer bsIn(msg->mData, msg->mLength, false);
                bsIn.skipRead(1, 0);
                WorldId worldId;
                bsIn.read(worldId);
                TM_World *world = GetWorldWithId(worldId);
                if (world==0)
                    return CRT_Destroy;
                bool validPacket = OnRemoveFromRequestedTeams(msg, world);
                if (validPacket==false)
                    return CRT_Destroy;
                break;
            }
        case ID_TEAM_BALANCER_INTERNAL:
            {
                if (msg->mLength >= 2)
                {
                    NetSerializer bsIn(msg->mData, msg->mLength, false);
                    bsIn.skipRead(2, 0);
                    WorldId worldId;
                    bsIn.read(worldId);
                    TM_World * world = GetWorldWithId(worldId);
                    if (world==0)
                        return CRT_Destroy;

                    switch (msg->mData[1])
                    {
                    case ID_RUN_UpdateListsToNoTeam:
                        OnUpdateListsToNoTeam(msg, world);
                        break;
                    case ID_RUN_UpdateTeamsRequestedToAny:
                        OnUpdateTeamsRequestedToAny(msg, world);
                        break;
                    case ID_RUN_JoinAnyTeam:
                        OnJoinAnyTeam(msg, world);
                        break;
                    case ID_RUN_JoinRequestedTeam:
                        OnJoinRequestedTeam(msg, world);
                        break;
                    case ID_RUN_UpdateTeamsRequestedToNoneAndAddTeam:
                        OnUpdateTeamsRequestedToNoneAndAddTeam(msg, world);
                        break;
                    case ID_RUN_RemoveFromTeamsRequestedAndAddTeam:
                        OnRemoveFromTeamsRequestedAndAddTeam(msg, world);
                        break;
                    case ID_RUN_AddToRequestedTeams:
                        OnAddToRequestedTeams(msg, world);
                        break;
                    case ID_RUN_LeaveTeam:
                        OnLeaveTeam(msg, world);
                        break;
                    case ID_RUN_SetMemberLimit:
                        OnSetMemberLimit(msg, world);
                        break;
                    case ID_RUN_SetJoinPermissions:
                        OnSetJoinPermissions(msg, world);
                        break;
                    case ID_RUN_SetBalanceTeams:
                        OnSetBalanceTeams(msg, world);
                        break;
                    case ID_RUN_SetBalanceTeamsInitial:
                        OnSetBalanceTeamsInitial(msg, world);
                        break;
                    }
                }
            }
            return CRT_Destroy;
        }
        return CRT_Normal;
    }
    //------------------------------------------------------------------------
    void TeamManager::onConnectClose(const Address & address, UniqueID id,
        ConnectCloseType lostConnectionReason)
    {
        for (NCount i = 0; i < worldsList.Size(); ++i)
        {
            worldsList[i]->onConnectClose(address, id, lostConnectionReason);
        }
    }
    //------------------------------------------------------------------------
    void TeamManager::onConnect(const Address & address, UniqueID id)
    {
        for(NCount i = 0; i < worldsList.Size(); ++i)
        {
            worldsList[i]->onConnect(address, id);
        }
    }
    //------------------------------------------------------------------------
    void TeamManager::send(const NetSerializer * out, const UniqueAddress usys, bool broadcast )
    {
        send(out,PL_HIGH, PT_ORDER_MAKE_SURE, 0, usys, broadcast);
    }
    //------------------------------------------------------------------------
    void TeamManager::RemoveFromTeamsRequestedAndAddTeam(TM_TeamMember *teamMember, TM_Team *team, bool isTeamSwitch, TM_Team *teamToLeave)
    {
        teamMember->RemoveFromRequestedTeams(team);
        if (isTeamSwitch)
        {
            if (teamToLeave==0)
            {
                // Leave all teams
                teamMember->RemoveFromAllTeamsInternal();
            }
            else
            {
                // Leave specific team if it exists
                teamMember->RemoveFromSpecificTeamInternal(teamToLeave);
            }
        }
        teamMember->AddToTeamList(team);
        PushTeamAssigned(teamMember);
    }
    //------------------------------------------------------------------------
    void TeamManager::PushTeamAssigned(TM_TeamMember * teamMember)
    {
        // Push ID_TEAM_BALANCER_TEAM_ASSIGNED locally
        NetSerializer out;
        out.write((Nui8)ID_TEAM_BALANCER_TEAM_ASSIGNED);
        EncodeTeamAssigned(&out, teamMember);

        PushBitStream(&out);
    }
    //------------------------------------------------------------------------
    void TeamManager::PushBitStream(NetSerializer * out)
    {
        SocketMessage * p = createMessage(out->getSize());
        memcpy(p->mData, out->getData(), out->getSize());
        p->mAddress = Address::invalid;
        p->mAddress.mIndex = (Nindex)-1;
        p->mID = UniqueID::INVALID;
        p->mLocal = true;
        addMessage(p, true);
    }
    //------------------------------------------------------------------------
    void TeamManager::OnUpdateListsToNoTeam(SocketMessage * msg, TM_World * world)
    {
        NetSerializer bsIn(msg->mData,msg->mLength, false);
        bsIn.skipRead(2 + sizeof(WorldId), 0);
        NetworkID networkId;
        bsIn.read(networkId);
        TM_TeamMember *teamMember = world->GetTeamMemberByNetworkID(networkId);
        NoTeamId noTeamId;
        bsIn.read(noTeamId);
        if (teamMember)
        {
            teamMember->StoreLastTeams();
            teamMember->UpdateListsToNoTeam(noTeamId);
            PushTeamAssigned(teamMember);

            if (world->GetHost()==world->GetTeamManager()->getID())
            {
                world->FillRequestedSlots();
                world->EnforceTeamBalance(noTeamId);

                if (topology == TM_CLIENT_SERVER)
                {
                    // Relay
                    world->BroadcastToParticipants(msg->mData, msg->mLength, msg->mID);
                }
            }
        }
    }
    //------------------------------------------------------------------------
    void TeamManager::OnUpdateTeamsRequestedToAny(SocketMessage * msg, TM_World * world)
    {
        NetSerializer bsIn(msg->mData, msg->mLength, false);
        bsIn.skipRead(2 + sizeof(WorldId), 0);
        NetworkID networkId;
        bsIn.read(networkId);
        TM_TeamMember *teamMember = world->GetTeamMemberByNetworkID(networkId);
        if (teamMember)
        {
            teamMember->UpdateTeamsRequestedToAny();
        }
    }
    //------------------------------------------------------------------------
    void TeamManager::OnJoinAnyTeam(SocketMessage * msg, TM_World * world)
    {
        NetSerializer bsIn(msg->mData, msg->mLength, false);
        bsIn.skipRead(2 + sizeof(WorldId), 0);
        NetworkID networkId;
        bsIn.read(networkId);
        TM_TeamMember * teamMember = world->GetTeamMemberByNetworkID(networkId);
        if (teamMember)
        {
            // This is a host-only operation
            N_assert(world->GetHost() == world->GetTeamManager()->getID());

            teamMember->UpdateTeamsRequestedToAny();

            int resultCode;
            TM_Team * newTeam = world->JoinAnyTeam(teamMember, &resultCode);

            if (resultCode == 1)
            {
                // Broadcast msg - remote systems should clear requested teams to none, and add the team we joined.
                // Broadcast includes non-host sender (all participants)
                NetSerializer out;
                out.write((Nui8)ID_TEAM_BALANCER_INTERNAL);
                out.write((Nui8)ID_RUN_UpdateTeamsRequestedToNoneAndAddTeam);
                out.write(world->GetWorldId());
                out.write(networkId);
                out.write(newTeam->GetNetworkID());
                world->BroadcastToParticipants(&out, msg->mID);

                // send to sender ID_TEAM_BALANCER_TEAM_ASSIGNED
                if (msg->mID!=getID())
                {
                    NetSerializer out;
                    out.write((Nui8)ID_TEAM_BALANCER_TEAM_ASSIGNED);
                    EncodeTeamAssigned(&out, teamMember);
                    send(&out, PL_HIGH, PT_ORDER_MAKE_SURE, 0, msg->mID, false);
                }
            }
            else
            {
                // Relay msg to set requested teams to any
                NetSerializer out;
                out.write((Nui8)ID_TEAM_BALANCER_INTERNAL);
                out.write((Nui8)ID_RUN_UpdateTeamsRequestedToAny);
                out.write(world->GetWorldId());
                out.write(networkId);
                world->BroadcastToParticipants(&out, msg->mID);

                out.seekWrite(0);
                out.seekRead(0);
                if (resultCode == -2)
                {
                    EncodeTeamFull(&out, teamMember, newTeam);
                }
                else if (resultCode == -1)
                {
                    EncodeTeamLocked(&out, teamMember, newTeam);
                }
                world->BroadcastToParticipants(&out, UniqueID::INVALID);
                if (msg->mID != getID())
                    PushBitStream(&out);
            }
        }
    }
    //------------------------------------------------------------------------
    void TeamManager::OnJoinRequestedTeam(SocketMessage * msg, TM_World * world)
    {
        NetSerializer bsIn(msg->mData, msg->mLength, false);
        bsIn.skipRead(2 + sizeof(WorldId), 0);
        NetworkID networkId;
        bsIn.read(networkId);
        TM_TeamMember * teamMember = world->GetTeamMemberByNetworkID(networkId);
        NetworkID teamToJoinNetworkId;
        bsIn.read(teamToJoinNetworkId);
        TM_Team * teamToJoin = world->GetTeamByNetworkID(teamToJoinNetworkId);
        bool isTeamSwitch = false;
        bool switchSpecificTeam = false;
        NetworkID teamToLeaveNetworkId = UNASSIGNED_NETWORK_ID;
        TM_Team * teamToLeave = 0;
        bsIn.read(isTeamSwitch);
        if (isTeamSwitch)
        {
            bsIn.read(switchSpecificTeam);
            if (switchSpecificTeam)
            {
                bsIn.read(teamToLeaveNetworkId);
                teamToLeave = world->GetTeamByNetworkID(teamToLeaveNetworkId);
                if (teamToLeave==0)
                    isTeamSwitch=false;
            }
        }
        if (teamToJoin && teamMember)
        {
            if (isTeamSwitch)
            {
                if (teamMember->SwitchSpecificTeamCheck(teamToJoin, teamToLeave, msg->mID==getID())==false)
                    return;

                teamMember->AddToRequestedTeams(teamToJoin, teamToLeave);
            }
            else
            {
                if (teamMember->JoinSpecificTeamCheck(teamToJoin, msg->mID==getID())==false)
                    return;

                teamMember->AddToRequestedTeams(teamToJoin);
            }

            DataStructures::List<TM_Team*> teamsWeAreLeaving;
            if (isTeamSwitch)
            {
                if (teamToLeave==0)
                {
                    teamsWeAreLeaving=teamMember->teams;
                }
                else
                {
                    if (teamMember->IsOnTeam(teamToLeave))
                        teamsWeAreLeaving.Push(teamToLeave);
                }

                if (teamsWeAreLeaving.Size()==0)
                    isTeamSwitch=false;
            }

            int resultCode = world->JoinSpecificTeam(teamMember, teamToJoin, isTeamSwitch, teamToLeave, teamsWeAreLeaving);

            if (resultCode==1)
            {
                // Broadcast msg - remote systems should remove from requested teams and add the team we joined.
                // Broadcast includes non-host sender (all participants)
                NetSerializer out;
                out.write((Nui8)ID_TEAM_BALANCER_INTERNAL);
                out.write((Nui8)ID_RUN_RemoveFromTeamsRequestedAndAddTeam);
                out.write(world->GetWorldId());
                out.write(networkId);
                out.write(teamToJoin->GetNetworkID());
                out.write(isTeamSwitch);
                if (isTeamSwitch)
                {
                    out.write(switchSpecificTeam);
                    if (switchSpecificTeam)
                        out.write(teamToLeaveNetworkId);
                }
                world->BroadcastToParticipants(&out, msg->mID);

                // send to sender ID_TEAM_BALANCER_TEAM_ASSIGNED
                if (msg->mID != getID())
                {
                    NetSerializer out;
                    out.write((Nui8)ID_TEAM_BALANCER_TEAM_ASSIGNED);
                    EncodeTeamAssigned(&out, teamMember);
                    send(&out, PL_HIGH, PT_ORDER_MAKE_SURE, 0, msg->mID, false);
                }
            }
            else
            {
                // Relay msg to set requested teams to any
                NetSerializer out;
                out.write((Nui8)ID_TEAM_BALANCER_INTERNAL);
                out.write((Nui8)ID_RUN_AddToRequestedTeams);
                out.write(world->GetWorldId());
                out.write(networkId);
                out.write(teamToJoin->GetNetworkID());
                out.write(isTeamSwitch);
                if (isTeamSwitch)
                {
                    out.write(switchSpecificTeam);
                    if (switchSpecificTeam)
                        out.write(teamToLeaveNetworkId);
                }
                world->BroadcastToParticipants(&out, msg->mID);

                out.seekWrite(0);
                out.seekRead(0);
                if (resultCode == -2)
                {
                    EncodeTeamFull(&out, teamMember, teamToJoin);
                }
                else if (resultCode == -1)
                {
                    EncodeTeamLocked(&out, teamMember, teamToJoin);
                }

                world->BroadcastToParticipants(&out, UniqueID::INVALID);
                if (msg->mID != getID())
                    PushBitStream(&out);
            }
        }
    }
    //------------------------------------------------------------------------
    void TeamManager::OnUpdateTeamsRequestedToNoneAndAddTeam(SocketMessage * msg,
        TM_World * world)
    {
        NetSerializer bsIn(msg->mData, msg->mLength, false);
        bsIn.skipRead(2 + sizeof(WorldId), 0);
        NetworkID networkId;
        bsIn.read(networkId);
        TM_TeamMember * teamMember = world->GetTeamMemberByNetworkID(networkId);
        NetworkID teamNetworkId;
        bsIn.read(teamNetworkId);
        TM_Team * team = world->GetTeamByNetworkID(teamNetworkId);

        if (team && teamMember)
        {
            teamMember->StoreLastTeams();
            teamMember->UpdateTeamsRequestedToNone();
            teamMember->AddToTeamList(team);
            world->GetTeamManager()->PushTeamAssigned(teamMember);
        }
    }
    //------------------------------------------------------------------------
    void TeamManager::OnRemoveFromTeamsRequestedAndAddTeam(SocketMessage * msg, TM_World * world)
    {
        NetSerializer bsIn(msg->mData, msg->mLength, false);
        bsIn.skipRead(2 + sizeof(WorldId), 0);
        NetworkID networkId;
        bsIn.read(networkId);
        TM_TeamMember * teamMember = world->GetTeamMemberByNetworkID(networkId);
        NetworkID teamNetworkId;
        bsIn.read(teamNetworkId);
        bool isTeamSwitch = false, switchSpecificTeam=false;
        NetworkID teamToLeaveNetworkId;
        TM_Team * teamToLeave = 0;
        bsIn.read(isTeamSwitch);
        if (isTeamSwitch)
        {
            bsIn.read(switchSpecificTeam);
            if (switchSpecificTeam)
            {
                bsIn.read(teamToLeaveNetworkId);
                teamToLeave = world->GetTeamByNetworkID(teamToLeaveNetworkId);
            }
        }

        TM_Team *team = world->GetTeamByNetworkID(teamNetworkId);
        if (team && teamMember)
        {
            teamMember->StoreLastTeams();
            if (teamToLeave)
                teamMember->RemoveFromSpecificTeamInternal(teamToLeave);
            else if (isTeamSwitch==true && switchSpecificTeam==false)
                teamMember->RemoveFromAllTeamsInternal();
            RemoveFromTeamsRequestedAndAddTeam(teamMember, team, false, 0);
        }
    }
    //------------------------------------------------------------------------
    void TeamManager::OnAddToRequestedTeams(SocketMessage * msg, TM_World * world)
    {
        NetSerializer bsIn(msg->mData, msg->mLength, false);
        bsIn.skipRead(2 + sizeof(WorldId), 0);
        NetworkID networkId;
        bsIn.read(networkId);
        TM_TeamMember * teamMember = world->GetTeamMemberByNetworkID(networkId);
        NetworkID teamNetworkId;
        bsIn.read(teamNetworkId);
        TM_Team * team = world->GetTeamByNetworkID(teamNetworkId);

        bool isTeamSwitch = false;
        bool switchSpecificTeam = false;
        NetworkID teamToLeaveNetworkId = UNASSIGNED_NETWORK_ID;
        TM_Team * teamToLeave = 0;
        bsIn.read(isTeamSwitch);
        if (isTeamSwitch)
        {
            bsIn.read(switchSpecificTeam);
            if (switchSpecificTeam)
            {
                bsIn.read(teamToLeaveNetworkId);
                teamToLeave = world->GetTeamByNetworkID(teamToLeaveNetworkId);
                if (teamToLeave == 0)
                    isTeamSwitch=false;
            }
        }

        if (team && teamMember)
        {
            if (isTeamSwitch)
                teamMember->AddToRequestedTeams(team, teamToLeave);
            else
                teamMember->AddToRequestedTeams(team);
        }
    }
    //------------------------------------------------------------------------
    bool TeamManager::OnRemoveFromRequestedTeams(SocketMessage * msg, TM_World * world)
    {
        NetSerializer bsIn(msg->mData,msg->mLength,false);
        bsIn.skipRead(1 + sizeof(WorldId), 0);
        NetworkID networkId;
        bsIn.read(networkId);
        TM_TeamMember *teamMember = world->GetTeamMemberByNetworkID(networkId);
        bool hasSpecificTeam=false;
        NetworkID teamNetworkId;
        TM_Team *team;
        bsIn.read(hasSpecificTeam);
        if (hasSpecificTeam)
        {
            bsIn.read(teamNetworkId);
            team = world->GetTeamByNetworkID(teamNetworkId);
            if (team==0)
                return false;
        }
        else
        {
            team=0;
        }

        if (teamMember)
        {
            teamMember->RemoveFromRequestedTeams(team);

            // Relay as host
            if (world->GetHost()==world->GetTeamManager()->getID() && topology==TM_CLIENT_SERVER)
            {
                world->BroadcastToParticipants(msg->mData, msg->mLength, msg->mID);
            }
            return true;
        }
        else
        {
            return false;
        }
    }
    //------------------------------------------------------------------------
    void TeamManager::OnLeaveTeam(SocketMessage * msg, TM_World * world)
    {
        NetSerializer bsIn(msg->mData,msg->mLength,false);
        bsIn.skipRead(2 + sizeof(WorldId), 0);
        NetworkID networkId;
        bsIn.read(networkId);
        TM_TeamMember *teamMember = world->GetTeamMemberByNetworkID(networkId);
        NetworkID teamNetworkId;
        bsIn.read(teamNetworkId);
        TM_Team *team = world->GetTeamByNetworkID(teamNetworkId);
        NoTeamId noTeamId;
        bsIn.read(noTeamId);

        if (team && teamMember)
        {
            if (teamMember->LeaveTeamCheck(team)==false)
                return;

            teamMember->StoreLastTeams();
            teamMember->RemoveFromSpecificTeamInternal(team);
            if (teamMember->GetCurrentTeamCount()==0)
            {
                teamMember->noTeamSubcategory=noTeamId;
                teamMember->joinTeamType=JOIN_NO_TEAM;
            }
            PushTeamAssigned(teamMember);

            if (world->GetHost()==world->GetTeamManager()->getID())
            {
                // Rebalance teams
                world->FillRequestedSlots();
                world->EnforceTeamBalance(noTeamId);

                // Relay as host
                if (topology==TM_CLIENT_SERVER)
                    world->BroadcastToParticipants(msg->mData, msg->mLength, msg->mID);
            }
        }
    }
    //------------------------------------------------------------------------
    void TeamManager::OnSetMemberLimit(SocketMessage * msg, TM_World * world)
    {
        NetSerializer bsIn(msg->mData,msg->mLength,false);
        bsIn.skipRead(2 + sizeof(WorldId), 0);
        NetworkID teamNetworkId;
        bsIn.read(teamNetworkId);
        TeamMemberLimit teamMemberLimit;
        NoTeamId noTeamId;
        bsIn.read(teamMemberLimit);
        bsIn.read(noTeamId);

        TM_Team *team = world->GetTeamByNetworkID(teamNetworkId);
        if (team)
        {
            team->teamMemberLimit=teamMemberLimit;

            if (world->GetHost()==world->GetTeamManager()->getID())
            {
                if (msg->mID==getID())
                    world->BroadcastToParticipants(msg->mData, msg->mLength, msg->mID);
                else
                    world->BroadcastToParticipants(msg->mData, msg->mLength, UniqueID::INVALID);
                world->FillRequestedSlots();
                world->KickExcessMembers(noTeamId);
            }
        }
    }
    //------------------------------------------------------------------------
    void TeamManager::OnSetJoinPermissions(SocketMessage * msg, TM_World * world)
    {
        NetSerializer bsIn(msg->mData,msg->mLength,false);
        bsIn.skipRead(2 + sizeof(WorldId), 0);
        NetworkID teamNetworkId;
        bsIn.read(teamNetworkId);
        JoinPermissions joinPermissions;
        bsIn.read(joinPermissions);

        TM_Team *team = world->GetTeamByNetworkID(teamNetworkId);
        if (team)
        {
            team->joinPermissions=joinPermissions;

            if (world->GetHost()==world->GetTeamManager()->getID())
            {
                if (msg->mID==getID())
                    world->BroadcastToParticipants(msg->mData, msg->mLength, msg->mID);
                else
                    world->BroadcastToParticipants(msg->mData, msg->mLength, UniqueID::INVALID);
                world->FillRequestedSlots();
            }
        }
    }
    //------------------------------------------------------------------------
    void TeamManager::OnSetBalanceTeams(SocketMessage * msg, TM_World * world)
    {
        NetSerializer bsIn(msg->mData, msg->mLength, false);
        bsIn.skipRead(2 + sizeof(WorldId), 0);
        bool balanceTeams=false;
        bsIn.read(balanceTeams);
        NoTeamId noTeamId;
        bsIn.read(noTeamId);

        world->balanceTeamsIsActive=balanceTeams;
        if (world->GetHost()==world->GetTeamManager()->getID())
        {
            if (msg->mID==getID())
                world->BroadcastToParticipants(msg->mData, msg->mLength, msg->mID);
            else
                world->BroadcastToParticipants(msg->mData, msg->mLength, UniqueID::INVALID);

            if (balanceTeams)
                world->EnforceTeamBalance(noTeamId);
            else
                world->FillRequestedSlots();
        }
    }
    //------------------------------------------------------------------------
    void TeamManager::OnSetBalanceTeamsInitial(SocketMessage * msg, TM_World * world)
    {
        NetSerializer bsIn(msg->mData, msg->mLength, false);
        bsIn.skipRead(2 + sizeof(WorldId), 0);
        bool balanceTeams=false;
        bsIn.read(balanceTeams);
        world->balanceTeamsIsActive=balanceTeams;
    }
    //------------------------------------------------------------------------
}
}
#endif // _RAKNET_SUPPORT_TeamManager==1