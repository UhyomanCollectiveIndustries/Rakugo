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

//==========
// 変数
//==========
private:

    /** コンポーネント */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rakugo|Camera", meta = (AllowPrivateAccess = "true"))
    USpringArmComponent* CameraBoom;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rakugo|Camera", meta = (AllowPrivateAccess = "true"))
    UCameraComponent* FollowCamera;

    /** 入力（Enhanced Input） */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rakugo|Input", meta = (AllowPrivateAccess = "true"))
    UInputMappingContext* DefaultMappingContext;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rakugo|Input", meta = (AllowPrivateAccess = "true"))
    UInputAction* GlideControlAction; // Vector2D: X=ロール(左右), Y=ピッチ(上下)

    /** 物理パラメータ */
    UPROPERTY(EditAnywhere, Category = "Rakugo|Physics")
    float ForwardSpeed = 1500.0f; // 基本前進速度

    UPROPERTY(EditAnywhere, Category = "Rakugo|Physics")
    float FallSpeedGliding = -200.0f; // 滑空時のデフォルト降下速度

    UPROPERTY(EditAnywhere, Category = "Rakugo|Physics")
    float PitchInfluence = 500.0f; // ピッチ操作による上昇/下降への影響度

    UPROPERTY(EditAnywhere, Category = "Rakugo|Physics")
    float TurnSpeed = 60.0f; // 旋回速度

    //---------
    // 状態
    //---------
    /** 滑空中か */
    bool bIsGliding = true; // 今回はプロトタイプなので最初から展開状態
    /** 滞空時間時間(デバック用) */
    float GlidingTime = 0.0f;

    //----------------
    // 内部ステータス
    //----------------
    /** 入力ターゲット値(-1.0 ～ 1.0) */
    float TargetPitch = 0.0f;
    float TargetRoll = 0.0f;

//==============
// 関数
//==============
public:
	/** コンストラクタ */
	ARakugoRaketeer();

protected:
    /** ゲーム開始時の初期化処理 */
	virtual void BeginPlay() override;

public:
    /** Tick */
    virtual void Tick(float DeltaTime) override;

    /** 入力ハンドラ */ 
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
    /** 入力ハンドラ */
    void HandleGlideControl(const FInputActionValue& Value);

    void SetPhysicsState(float DeltaTime);


//---------------------
// Internal Functions
//---------------------
public:
    /** 演出開始時に呼ばれる(入力を禁止し、滑空時間をリセット */
    UFUNCTION(BlueprintCallable, Category = "Rakugo|Event")
    void StartIntroPhase();

    /** 演出終了(ヘリから落ちる瞬間)に呼ばれる(入力を開始) */
    UFUNCTION(BlueprintCallable, Category = "Rakugo|Event")
    void EndIntroPhase();

protected:
    /**  Playerのカプセルコンポーネントが衝突を検知したら発火 */
    UFUNCTION()
    void OnCapsuleHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

    /** スコアが計算されたらBluePrint側で発火するイベント */
    UFUNCTION(BlueprintImplementableEvent, Category = "Rakugo|Score")
    void OnScoreCalculated(int32 FinalScore, const FString& EvaluationMessage, float ElapsedTime, float HitSpeed);

private:
    /** スコア計算関数 */
    void CalculateScore(AActor* HitActor, float SpeedAtHit,float Multiplier,FString EvaluationMessage);

};
