// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"

#include "Tank.h"
#include "ScreenMessage.h"

#include "BattleBlasterGameMode.generated.h"

/**
 * 
 */
UCLASS()
class BATTLEBLASTER_API ABattleBlasterGameMode : public AGameModeBase
{
	GENERATED_BODY()

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere)
	TSubclassOf<UScreenMessage> ScreenMessageClass;

	UScreenMessage* ScreenMessageWidget;

	UPROPERTY(EditAnywhere)
	float GamOverDelay = 3.0f;

	UPROPERTY(EditAnywhere)
	int32 CountdownDelay = 3;

	bool IsVictory = false;

	ATank* Tank;
	int32 TowerCount;
	int32 CountdownSeconds;

	FTimerHandle CountdownTimeHandle;
	
	void ActorDied(AActor* DeadActor);
	void OnGameOverTimerTimeout();
	void OnCountdowmTimerTimeout();

};
