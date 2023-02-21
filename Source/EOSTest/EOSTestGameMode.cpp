// Copyright Epic Games, Inc. All Rights Reserved.

#include "EOSTestGameMode.h"
#include "EOSTestCharacter.h"
#include "Online.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "UObject/ConstructorHelpers.h"

AEOSTestGameMode::AEOSTestGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

void AEOSTestGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	if(NewPlayer)
	{
		FUniqueNetIdRepl UniqueNetIdRepl;
		if(NewPlayer->IsLocalController())
		{
			const ULocalPlayer* LocalPlayerRef = NewPlayer->GetLocalPlayer();
			if(LocalPlayerRef)
			{
				UniqueNetIdRepl = LocalPlayerRef->GetPreferredUniqueNetId();
			}
			else
			{
				const UNetConnection* RemoteNetConnectionRef = Cast<UNetConnection>(NewPlayer->Player);
				check(IsValid(RemoteNetConnectionRef));
				UniqueNetIdRepl = RemoteNetConnectionRef->PlayerId;
			}
		}
		else
		{
			const UNetConnection* RemoteNetConnectionRef = Cast<UNetConnection>(NewPlayer->Player);
			check(IsValid(RemoteNetConnectionRef));
			UniqueNetIdRepl = RemoteNetConnectionRef->PlayerId;
		}

		const TSharedPtr<const FUniqueNetId> UniqueNetId = UniqueNetIdRepl.GetUniqueNetId();
		check(UniqueNetId != nullptr);
		const IOnlineSubsystem *SubsystemRef = Online::GetSubsystem(NewPlayer->GetWorld());
		const IOnlineSessionPtr SessionRef = SubsystemRef->GetSessionInterface();
		const bool bRegistrationSuccess = SessionRef->RegisterPlayer(FName("MainSession"),*UniqueNetId,false);
		if(bRegistrationSuccess)
		{
			UE_LOG(LogTemp,Warning,TEXT("Registration Successful"));
		}
	}
}
