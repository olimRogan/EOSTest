// Fill out your copyright notice in the Description page of Project Settings.

#include "EOS_GameInstance.h"

#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Kismet/GameplayStatics.h"

const  FName TestSessionName = FName("Test Session");

UEOS_GameInstance::UEOS_GameInstance()
{
}

void UEOS_GameInstance::Init()
{
	Super::Init();

	OnlineSubsystem = IOnlineSubsystem::Get();
}

void UEOS_GameInstance::Shutdown()
{
	Super::Shutdown();

	DestroySession();
}

void UEOS_GameInstance::Login()
{
	if(OnlineSubsystem)
	{
		if(const IOnlineIdentityPtr Identity = OnlineSubsystem->GetIdentityInterface())
		{
			FOnlineAccountCredentials Credentials;
			Credentials.Id = FString();
			Credentials.Token = FString();
			//Credentials.Type = FString("accountportal");
			Credentials.Type = FString("persistentauth");

			Identity->OnLoginCompleteDelegates->AddUObject(this,&UEOS_GameInstance::OnLoginComplete);
			Identity->Login(0,Credentials);
		}
	}
}

void UEOS_GameInstance::CreateSession()
{
	if(bIsLoggedIn)
	{
		if(OnlineSubsystem)
		{
			if(const IOnlineSessionPtr SessionPtr = OnlineSubsystem->GetSessionInterface())
			{
				FOnlineSessionSettings SessionSettings;
				SessionSettings.bIsDedicated = false;
				SessionSettings.bShouldAdvertise = true;
				SessionSettings.bIsLANMatch = false;
				SessionSettings.NumPublicConnections = 5;
				SessionSettings.bAllowJoinInProgress = true;
				SessionSettings.bAllowJoinViaPresence = true;
				SessionSettings.bUsesPresence = true;
				SessionSettings.bUseLobbiesIfAvailable = true;
				SessionSettings.bAllowInvites = true;
				
				SessionSettings.Set(SEARCH_KEYWORDS,FString("Test Lobby"), EOnlineDataAdvertisementType::ViaOnlineService);

				SessionPtr->OnCreateSessionCompleteDelegates.AddUObject(this,&UEOS_GameInstance::OnCreateSessionComplete);
				SessionPtr->CreateSession(0,TestSessionName,SessionSettings);
			}
		}
	}
	else
	{
		UE_LOG(LogTemp,Error,TEXT("Cannot create session : Not logged In"));
	}
}

void UEOS_GameInstance::FindSession()
{
	if(bIsLoggedIn)
	{
		if(OnlineSubsystem)
		{
			if(const IOnlineSessionPtr SessionPtr = OnlineSubsystem->GetSessionInterface())
			{
				SearchSettings = MakeShareable(new FOnlineSessionSearch());
				SearchSettings->MaxSearchResults = 5000;
				SearchSettings->bIsLanQuery = false;
				SearchSettings->QuerySettings.Set(SEARCH_KEYWORDS,FString("Test Lobby"),EOnlineComparisonOp::Equals);
				SearchSettings->QuerySettings.Set(SEARCH_LOBBIES,true,EOnlineComparisonOp::Equals);
				
				SessionPtr->OnFindSessionsCompleteDelegates.AddUObject(this,&UEOS_GameInstance::OnFindSessionComplete);
				SessionPtr->FindSessions(0,SearchSettings.ToSharedRef());
			}
		}
	}
}

void UEOS_GameInstance::DestroySession()
{
	if(bIsLoggedIn)
	{
		if(OnlineSubsystem)
		{
			if(const IOnlineSessionPtr SessionPtr = OnlineSubsystem->GetSessionInterface())
			{
				SessionPtr->OnDestroySessionCompleteDelegates.AddUObject(this,&UEOS_GameInstance::OnDestroySessionComplete);
				SessionPtr->DestroySession(TestSessionName);
			}
		}
	}
}

void UEOS_GameInstance::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	UE_LOG(LogTemp,Error,TEXT("Success : %d"),bWasSuccessful);

	if(OnlineSubsystem)
	{
		if(const IOnlineSessionPtr SessionPtr = OnlineSubsystem->GetSessionInterface())
		{
			SessionPtr->ClearOnCreateSessionCompleteDelegates(this);
			GetWorld()->ServerTravel(FString("MainSessionMap?listen"),false);
		}
	}
}

void UEOS_GameInstance::OnFindSessionComplete(bool bWasSuccessful)
{
	UE_LOG(LogTemp,Error,TEXT("Find Session : %d"),bWasSuccessful);
	if(bWasSuccessful)
	{
		UE_LOG(LogTemp,Error,TEXT("Found %d Lobbies"),SearchSettings->SearchResults.Num());
		
		if(OnlineSubsystem)
		{
			if(const IOnlineSessionPtr SessionPtr = OnlineSubsystem->GetSessionInterface())
			{
				if(SearchSettings->SearchResults.Num())
				{
					SessionPtr->OnJoinSessionCompleteDelegates.AddUObject(this,&UEOS_GameInstance::OnJoinSessionComplete);
					SessionPtr->JoinSession(0,TestSessionName,SearchSettings->SearchResults[0]);

					SessionPtr->ClearOnFindSessionsCompleteDelegates(this);
				}
			}
		}
	}
}

void UEOS_GameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if(OnlineSubsystem)
	{
		if(const IOnlineSessionPtr SessionPtr = OnlineSubsystem->GetSessionInterface())
		{
			FString ConnectionInfo = FString();
			SessionPtr->GetResolvedConnectString(SessionName, ConnectionInfo);
			if(!ConnectionInfo.IsEmpty())
			{
				if(APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(),0))
				{
					PC->ClientTravel(ConnectionInfo,ETravelType::TRAVEL_Absolute);
				}
			}
		}
	}
}

void UEOS_GameInstance::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
	if(OnlineSubsystem)
	{
		if(const IOnlineSessionPtr SessionPtr = OnlineSubsystem->GetSessionInterface())
		{
			SessionPtr->ClearOnDestroySessionCompleteDelegates(this);
			if(APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(),0))
			{
				PC->ClientTravel("ThirdPersonMap",ETravelType::TRAVEL_Absolute);
				SessionPtr->EndSession(TestSessionName);
			}
		}
	}
}

void UEOS_GameInstance::OnLoginComplete(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId,
                                        const FString& Error)
{
	UE_LOG(LogTemp,Error,TEXT("LoggedIn : %d "),bWasSuccessful);
	bIsLoggedIn = bWasSuccessful;
	
	if(OnlineSubsystem)
	{
		if(const IOnlineIdentityPtr Identity = OnlineSubsystem->GetIdentityInterface())
		{
			Identity->ClearOnLoginCompleteDelegates(0,this);
		}
	}
}
