// Fill out your copyright notice in the Description page of Project Settings.


#include "EOS_GameInstance.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "Kismet/GameplayStatics.h"

void UEOS_GameInstance::LoginWithEOS(const FString& ID, const FString& Token, const FString& LoginType)
{
	const IOnlineSubsystem* SubsystemRef = Online::GetSubsystem(this->GetWorld());
	if(SubsystemRef)
	{
		const IOnlineIdentityPtr IdentityPointerRef = SubsystemRef->GetIdentityInterface();
		if(IdentityPointerRef)
		{
			FOnlineAccountCredentials AccountCredentials;
			AccountCredentials.Id = ID;
			AccountCredentials.Token = Token;
			AccountCredentials.Type = LoginType;
			IdentityPointerRef->OnLoginCompleteDelegates->AddUObject(this, &UEOS_GameInstance::LoginWithEOS_Return);
			IdentityPointerRef->Login(0,AccountCredentials);
		}
	}
}

FString UEOS_GameInstance::GetPlayerUserName()
{
	const IOnlineSubsystem* SubsystemRef = Online::GetSubsystem(this->GetWorld());
	if(SubsystemRef)
	{
		const IOnlineIdentityPtr IdentityPointerRef = SubsystemRef->GetIdentityInterface();
		if(IdentityPointerRef)
		{
			if(IdentityPointerRef->GetLoginStatus(0) == ELoginStatus::LoggedIn)
			{
				return IdentityPointerRef->GetPlayerNickname(0);
			}
		}
	}
	return FString();
}

bool UEOS_GameInstance::IsPlayerLoggedIn()
{
	const IOnlineSubsystem* SubsystemRef = Online::GetSubsystem(this->GetWorld());
	if(SubsystemRef)
	{
		const IOnlineIdentityPtr IdentityPointerRef = SubsystemRef->GetIdentityInterface();
		if(IdentityPointerRef)
		{
			if(IdentityPointerRef->GetLoginStatus(0) == ELoginStatus::LoggedIn)
			{
				return true;
			}
				
		}
	}
	return false;
}

void UEOS_GameInstance::LoginWithEOS_Return(int32 LocalUserNum, bool bWasSuccess, const FUniqueNetId& UserId,
                                            const FString& Error)
{
	if(bWasSuccess)
	{
		UE_LOG(LogTemp,Warning,TEXT("Login Success"));
	}
	else
	{
		UE_LOG(LogTemp,Error,TEXT("Login fail reason : - %s"),*Error);
	}
}

void UEOS_GameInstance::OnCreateSessionCompleted(FName SessionName, bool bWasSuccessful)
{
	if(bWasSuccessful)
	{
		GetWorld()->ServerTravel(OpenLevelText);
		UE_LOG(LogTemp,Warning,TEXT("Create Session Completed Session Name : %s"),*SessionName.ToString());
	}
}

void UEOS_GameInstance::OnDestroySessionCompleted(FName SessionName, bool bWasSuccessful)
{
}

void UEOS_GameInstance::OnFindSessionCompleted(bool bWasSuccess)
{
	if(bWasSuccess)
	{
		const IOnlineSubsystem* SubsystemRef = Online::GetSubsystem(this->GetWorld());
		if(SubsystemRef)
		{
			const IOnlineSessionPtr SessionPtrRef = SubsystemRef->GetSessionInterface();
			if(SessionPtrRef)
			{
				if(SessionSearch->SearchResults.Num() > 0)
				{
					SessionPtrRef->OnJoinSessionCompleteDelegates.AddUObject(this,&UEOS_GameInstance::OnJoinSessionCompleted);
					SessionPtrRef->JoinSession(0,FName("MainSession"),SessionSearch->SearchResults[0]);	
				}
				else
				{
					CreateEOSSession(false,false,10);
				}
			}
		}
	}
	else
	{
		CreateEOSSession(false,false,10);
	}
}

void UEOS_GameInstance::OnJoinSessionCompleted(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if(Result == EOnJoinSessionCompleteResult::Success)
	{
		if(APlayerController* PlayerControllerRef = UGameplayStatics::GetPlayerController(GetWorld(),0))
		{
			FString JoinAddress;
			const IOnlineSubsystem* SubsystemRef = Online::GetSubsystem(this->GetWorld());
			if(SubsystemRef)
			{
				const IOnlineSessionPtr SessionPtrRef = SubsystemRef->GetSessionInterface();
				if(SessionPtrRef)
				{
					SessionPtrRef->GetResolvedConnectString(FName("MainSession"),JoinAddress);
					UE_LOG(LogTemp,Error,TEXT("Join Address is %s"),*JoinAddress)
					if(!JoinAddress.IsEmpty())
					{
						PlayerControllerRef->ClientTravel(JoinAddress,ETravelType::TRAVEL_Absolute);
					}
				}
			}
		}
	}
}

void UEOS_GameInstance::CreateEOSSession(bool bIsDedicatedServer, bool bIsLanServer, int32 NumberOfPublicConnections)
{
	const IOnlineSubsystem* SubsystemRef = Online::GetSubsystem(this->GetWorld());
	if(SubsystemRef)
	{
		const IOnlineSessionPtr SessionPtrRef = SubsystemRef->GetSessionInterface();
		if(SessionPtrRef)
		{
			FOnlineSessionSettings SessionCreationInfo;
			SessionCreationInfo.bIsDedicated = bIsDedicatedServer;
			SessionCreationInfo.bAllowInvites = true;
			SessionCreationInfo.bIsLANMatch = bIsLanServer;
			SessionCreationInfo.NumPublicConnections = NumberOfPublicConnections;
			SessionCreationInfo.bUseLobbiesIfAvailable = false;
			SessionCreationInfo.bUsesPresence = false;
			SessionCreationInfo.bShouldAdvertise = true;
			SessionCreationInfo.Set(SEARCH_KEYWORDS,FString("EOS Test"),EOnlineDataAdvertisementType::ViaOnlineService);
			
			SessionPtrRef->OnCreateSessionCompleteDelegates.AddUObject(this,&UEOS_GameInstance::OnCreateSessionCompleted);
			SessionPtrRef->CreateSession(0,FName("MainSession"),SessionCreationInfo);
		}
	}
}

void UEOS_GameInstance::FindSessionAndJoin()
{
	const IOnlineSubsystem* SubsystemRef = Online::GetSubsystem(this->GetWorld());
	if(SubsystemRef)
	{
		const IOnlineSessionPtr SessionPtrRef = SubsystemRef->GetSessionInterface();
		if(SessionPtrRef)
		{
			SessionSearch = MakeShareable(new FOnlineSessionSearch());
			SessionSearch->bIsLanQuery = false;
			SessionSearch.Get()->MaxSearchResults = 20;
			SessionSearch->QuerySettings.SearchParams.Empty();
			SessionPtrRef->OnFindSessionsCompleteDelegates.AddUObject(this,&UEOS_GameInstance::OnFindSessionCompleted);
			SessionPtrRef->FindSessions(0,SessionSearch.ToSharedRef());
		}
	}
}

void UEOS_GameInstance::JoinSession()
{
	
}

void UEOS_GameInstance::DestroySession()
{
	const IOnlineSubsystem* SubsystemRef = Online::GetSubsystem(this->GetWorld());
	if(SubsystemRef)
	{
		const IOnlineSessionPtr SessionPtrRef = SubsystemRef->GetSessionInterface();
		if(SessionPtrRef)
		{
			SessionPtrRef->OnDestroySessionCompleteDelegates.AddUObject(this,&UEOS_GameInstance::OnDestroySessionCompleted);
			SessionPtrRef->DestroySession(FName("MainSession"));
		}
	}
}
