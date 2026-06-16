#include "RaketeerFloor.h"
#include "NiagaraFunctionLibrary.h" 
#include "NiagaraSystem.h"

// コンストラクタ
ARaketeerFloor::ARaketeerFloor()
{
	PrimaryActorTick.bCanEverTick = false;

	// Initialize variables
	EvaluationMessage = TEXT("Normal Floor");
	ScoreMultiplier = 1.0f;

	// ポインタ変数を完全に初期化
	LandedFX = nullptr;
}
