// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "EOS_GameInstance.generated.h"

/**
 * 
 */
UCLASS()
class EOSTEST_API UEOS_GameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UEOS_GameInstance();

	virtual void Init() override;

	virtual void Shutdown() override;

	UFUNCTION(BlueprintCallable)
	void Login();

	UFUNCTION(BlueprintCallable)
	void CreateSession();

	UFUNCTION(BlueprintCallable)
	void FindSession();
	
	TSharedPtr<FOnlineSessionSearch> SearchSettings;

	UFUNCTION(BlueprintCallable)
	void DestroySession();
	
	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
	void OnFindSessionComplete(bool bWasSuccessful);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);
	void OnLoginComplete(int32 LocalUserNum,bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error);

protected:
	TObjectPtr<class IOnlineSubsystem> OnlineSubsystem;

	bool bIsLoggedIn;
};

	
