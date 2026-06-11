#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "RakugoRaketeer.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

UCLASS()
class RAKUGO_API ARakugoRaketeer : public ACharacter
{
	GENERATED_BODY()

public:
	//コンストラクタ
	ARakugoRaketeer();

protected:
	virtual void BeginPlay() override;

    UFUNCTION()
    void OnCapsuleHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

    /* スコアが計算されたらBluePrint側で発火するイベント */
    UFUNCTION(BlueprintImplementableEvent, Category = "Rakugo Score")
    void OnScoreCalculated(int32 FinalScore, const FString& EvaluationMessage, float ElapsedTime, float HitSpeed);

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
    /** コンポーネント */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    USpringArmComponent* CameraBoom;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    UCameraComponent* FollowCamera;

    /** 入力（Enhanced Input） */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputMappingContext* DefaultMappingContext;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputAction* GlideControlAction; // Vector2D: X=ロール(左右), Y=ピッチ(上下)

    /** パラメータ（調整用） */
    UPROPERTY(EditAnywhere, Category = "Rakugo Physics")
    float ForwardSpeed = 1500.0f; // 基本前進速度

    UPROPERTY(EditAnywhere, Category = "Rakugo Physics")
    float FallSpeedGliding = -200.0f; // 滑空時のデフォルト降下速度

    UPROPERTY(EditAnywhere, Category = "Rakugo Physics")
    float PitchInfluence = 500.0f; // ピッチ操作による上昇/下降への影響度

    UPROPERTY(EditAnywhere, Category = "Rakugo Physics")
    float TurnSpeed = 60.0f; // 旋回速度

    /** 内部ステータス */
    bool bIsGliding = true; // 今回はプロトタイプなので最初から展開状態
    float TargetPitch = 0.0f;
    float TargetRoll = 0.0f;

    float GlidingTime = 0.0f;   // 滞空時間を記録する変数
    void CalculateScore(AActor* HitActor, float SpeedAtHit);    // スコア計算関数

    /** 入力ハンドラ */
    void HandleGlideControl(const FInputActionValue& Value);
    void SetPhysicsState(float DeltaTime);
};
