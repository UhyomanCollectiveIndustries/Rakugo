#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
//#include "NiagaraFunctionLibrary.h" 
//#include "NiagaraSystem.h"
#include "RaketeerFloor.generated.h"

UCLASS()
class RAKUGO_API ARaketeerFloor : public AActor
{
	GENERATED_BODY()

public: 
	// コンストラクタ
	ARaketeerFloor();

	// エディタで床ごとに個別に設定するパラメータ郡

	/** 地面判定のログ */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Floor|Settings")
	FString EvaluationMessage = TEXT("普通の地面");

	/** 床に設定された倍率 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Floor|Settings")
	float ScoreMultiplier = 1.0f;

	/** 床に設定されたパーティクル */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Floor|Settings")
	class UNiagaraSystem* LandedFX;

};
