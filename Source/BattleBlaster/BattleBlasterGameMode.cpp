// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleBlasterGameMode.h"

#include "Kismet/GameplayStatics.h"
#include "Tower.h"
#include "BattleBlasterGameInstance.h"

void ABattleBlasterGameMode::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> Towers;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATower::StaticClass(), Towers);
	TowerCount = Towers.Num();
	UE_LOG(LogTemp, Warning, TEXT("NUmber of Towers: %d"), TowerCount);

	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (PlayerPawn)
	{
		Tank = Cast<ATank>(PlayerPawn);
		if (!Tank)
		{
			UE_LOG(LogTemp, Warning, TEXT("Gamemode: Failed to find the player tank actor!"));
		}
	}

	int32 LoopIndex = 0;
	while (LoopIndex < TowerCount)
	{
		AActor* TowerActor = Towers[LoopIndex];

		if (TowerActor)
		{
			ATower* Tower = Cast<ATower>(TowerActor);
			if (Tower && Tank)
			{
				Tower->Tank = Tank;
			}
		}
		LoopIndex++;
	}

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	if (PlayerController)
	{
		ScreenMessageWidget = CreateWidget<UScreenMessage>(PlayerController, ScreenMessageClass);
		if (ScreenMessageWidget)
		{
			ScreenMessageWidget->AddToPlayerScreen();
			ScreenMessageWidget->SetMessageText("Get Ready!");
		}
	}

	CountdownSeconds = CountdownDelay;
	GetWorldTimerManager().SetTimer(CountdownTimeHandle, this, 
		&ABattleBlasterGameMode::OnCountdowmTimerTimeout, 1.0f, true);
}

void ABattleBlasterGameMode::OnCountdowmTimerTimeout()
{
	CountdownSeconds -= 1;

	if (CountdownSeconds > 0)
	{
		ScreenMessageWidget->SetMessageText(FString::FromInt(CountdownSeconds));
	}
	else if (CountdownSeconds == 0)
	{
		ScreenMessageWidget->SetMessageText("Go!");
		Tank->SetPlayerEnabled(true);
	}
	else
	{
		GetWorldTimerManager().ClearTimer(CountdownTimeHandle);
		ScreenMessageWidget->SetVisibility(ESlateVisibility::Hidden);
	}	
}

void ABattleBlasterGameMode::ActorDied(AActor* DeadActor)
{
	bool IsGameOver = false;

	if (DeadActor == Tank)
	{
		// Tank Died
		Tank->HandleDestruction();
		IsGameOver = true;
	}
	else
	{
		// A Tower Died
		ATower* DeadTower = Cast<ATower>(DeadActor);
		if (DeadTower)
		{
			DeadTower->HandleDestruction();

			DeadTower->Destroy();

			TowerCount--;
			if (TowerCount == 0)
			{
				UE_LOG(LogTemp, Warning, TEXT("VICTORY... All Towers DIED"));
				IsGameOver = true;
				IsVictory = true;
			}
		}
	}
	if (IsGameOver)
	{
		FString GameOverString = IsVictory ? "Victory!" : "Defeat!";

		ScreenMessageWidget->SetMessageText(GameOverString);
		ScreenMessageWidget->SetVisibility(ESlateVisibility::Visible);


		FTimerHandle GameOverTimerHandle;
		GetWorldTimerManager().SetTimer(
			GameOverTimerHandle, this, &ABattleBlasterGameMode::OnGameOverTimerTimeout, 
			GamOverDelay, false);
	}
}

void ABattleBlasterGameMode::OnGameOverTimerTimeout()
{
	UGameInstance* GameInstance = GetGameInstance();
	if (GameInstance)
	{
		UBattleBlasterGameInstance* BattleBlasterGameInstance = Cast<UBattleBlasterGameInstance>
			(GameInstance);
		if (BattleBlasterGameInstance)
		{
			if (IsVictory)
			{
				// next level
				BattleBlasterGameInstance->LoadNextLevel();
			}
			else
			{
				// reload level
				BattleBlasterGameInstance->RestartCurrentLevel();
			}
		}
	}

}

