// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "EOSTestGameMode.generated.h"

UCLASS(minimalapi)
class AEOSTestGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AEOSTestGameMode();

	virtual void PostLogin(APlayerController* NewPlayer) override;
};



