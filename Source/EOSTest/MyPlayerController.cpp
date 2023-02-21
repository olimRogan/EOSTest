// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerController.h"

#include "EOS_GameInstance.h"

void AMyPlayerController::OnNetCleanup(UNetConnection* Connection)
{
	Super::OnNetCleanup(Connection);
}
