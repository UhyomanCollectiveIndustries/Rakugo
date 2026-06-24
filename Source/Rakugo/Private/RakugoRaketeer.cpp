#include "RakugoRaketeer.h"
#include "RaketeerFloor.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Components/CapsuleComponent.h"

#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Notifications/SNotificationList.h"

#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"

//=======================
// LifeCycle
//=======================

// コンストラクタ
ARakugoRaketeer::ARakugoRaketeer()
{
	PrimaryActorTick.bCanEverTick = true;

	// カメラブームの設定（背後からの視点）
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = false; // キャラクターの回転に追従

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// キャラクター移動の初期設定（空中制御をマニュアルで行うため）
	GetCharacterMovement()->GravityScale = 1.0f; // Landedを正しく動かすため、重力はONのままに
	GetCharacterMovement()->AirControl = 1.0f;

	// 空中での慣性を無視して、こちらが指定した速度（Velocity）に強制的に固定する設定
	GetCharacterMovement()->BrakingDecelerationFalling = 0.0f;
}

// BeginPlay
void ARakugoRaketeer::BeginPlay()
{
	Super::BeginPlay();

	GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &ARakugoRaketeer::OnCapsuleHit);
}

void ARakugoRaketeer::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// 滑空状態を開始
	bIsGliding = true;
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->SetMovementMode(MOVE_Falling);
		GetCharacterMovement()->Velocity = GetActorForwardVector() * ForwardSpeed;
	}
}

// Tick
void ARakugoRaketeer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsGliding && !GetCharacterMovement()->IsMovingOnGround())
	{
		GlidingTime += DeltaTime;
		SetPhysicsState(DeltaTime);
	}
}

//=====================
// Input
//=====================

// 入力ハンドラ
void ARakugoRaketeer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// 滑空コントロール（スティック、またはWASD）のバインド
		EnhancedInputComponent->BindAction(GlideControlAction, ETriggerEvent::Triggered, this, &ARakugoRaketeer::HandleGlideControl);
		EnhancedInputComponent->BindAction(GlideControlAction, ETriggerEvent::Completed, this, &ARakugoRaketeer::HandleGlideControl);
	}
}

void ARakugoRaketeer::HandleGlideControl(const FInputActionValue& Value)
{
	FVector2D ControlVector = Value.Get<FVector2D>();

	// X = 左右 (Roll / Yaw に影響), Y = 上下 (Pitch に影響)
	TargetRoll = ControlVector.X;
	TargetPitch = ControlVector.Y;
}

//==================================
// Physics/Movement
//==================================

void ARakugoRaketeer::SetPhysicsState(float DeltaTime)
{
	// 1. 旋回 (Yaw) と 傾き (Roll) の処理
	// 左右入力によって旋回させ、見た目のメッシュを傾けるためにRollを使用
	FRotator CurrentRotation = GetActorRotation();
	float NewYaw = CurrentRotation.Yaw + (TargetRoll * TurnSpeed * DeltaTime);
	float NewRoll = FMath::FInterpTo(CurrentRotation.Roll, TargetRoll * 30.0f, DeltaTime, 5.0f); // 最大30度傾く

	// ピッチ（機首の上下）のビジュアル反映
	float NewPitch = FMath::FInterpTo(CurrentRotation.Pitch, TargetPitch * 25.0f, DeltaTime, 5.0f);

	SetActorRotation(FRotator(NewPitch, NewYaw, NewRoll));

	// 2. 滑空速度の計算
	// 前方ベクトル
	FVector ForwardDirection = GetActorForwardVector();

	// ピッチ入力による速度変化（機首を下げると加速、上げると減速）
	// TargetPitchが負（下入力）のとき、前方速度をブーストする
	float AdjustedForwardSpeed = ForwardSpeed - (TargetPitch * PitchInfluence);

	// 垂直方向の速度（ピッチを上げると降下速度が和らぎ、下げると急降下）
	float AdjustedFallSpeed = FallSpeedGliding + (TargetPitch * PitchInfluence * 1.2f);

	// 最終的なベロシティの合成
	FVector NewVelocity = ForwardDirection * AdjustedForwardSpeed;
	NewVelocity.Z = AdjustedFallSpeed;

	// 一時的にムーブメントモールを「Falling」に
	// これにより、UEの移動コンポーネントが前方の壁や床との衝突を正しく計算するようになる。
	if (GetCharacterMovement()->MovementMode != MOVE_Flying)
	{
		GetCharacterMovement()->SetMovementMode(MOVE_Flying);
	}

	// 物理的に移動させる
	GetCharacterMovement()->Velocity = NewVelocity;
}

//=====================
// GamePlayFlow
//=====================

void ARakugoRaketeer::EndIntroPhase()
{
	UE_LOG(LogTemp, Log, TEXT("エンドフェーズスタート"));

	// プレイヤーの見た目を表示する
	SetActorHiddenInGame(false);
	UE_LOG(LogTemp, Log, TEXT("見た目表示"));

	// 操作のロックを解除
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		PC->SetIgnoreMoveInput(false);
		UE_LOG(LogTemp, Log, TEXT("操作可能"));

		// Enhanced Input Mapping Context の追加
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
			UE_LOG(LogTemp, Log, TEXT("マッピングコンテキスト登録完了！"));
		}

		FInputModeGameOnly InputMode;
		PC->SetInputMode(InputMode);
	}

	UE_LOG(LogTemp, Log, TEXT("エンドフェーズ終了"));
}

//=====================
// Events
//=====================

void ARakugoRaketeer::OnCapsuleHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// 滑空中でない(すでに着地済み)なら処理しない
	if (!bIsGliding) return;

	if (OtherActor)
	{
		// 衝突した瞬間の速度を取得
		float SpeedAtHit = GetCharacterMovement()->Velocity.Size();

		// 先に「着地処理」を完了させてフラグを折ることで、フレーム内の連続ヒットをすべてシャットアウトする
		bIsGliding = false;
		GetCharacterMovement()->GravityScale = 1.0f;
		GetCharacterMovement()->SetMovementMode(MOVE_Falling);

		// 床判定のデフォルト値
		float Multiplier = 1.0f;
		FString Message = TEXT("普通の地面");
		UNiagaraSystem* FXToSpawn = nullptr;

		// 床クラスからスコア設定とエフェクトを取得
		if (ARaketeerFloor* HitFloor = Cast<ARaketeerFloor>(OtherActor))
		{
			// 床が持っている設定値を受け取る
			Multiplier = HitFloor->ScoreMultiplier;
			Message = HitFloor->EvaluationMessage;
			FXToSpawn = HitFloor->LandedFX;
		}

		// パーティクルの生成(床クラスで設定されている場合のみ)
		if (FXToSpawn && GetWorld())
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(
				GetWorld(),
				FXToSpawn,
				Hit.Location,
				FRotator::ZeroRotator
			);
		}

		// スコア計算に変数を渡す
		CalculateScore(OtherActor, SpeedAtHit,Multiplier,Message);
	}
}

//===========================
// Internal Logic
//===========================

void ARakugoRaketeer::CalculateScore(AActor* HitActor, float SpeedAtHit,float ScoreMultiplier,const FString EvaluationMessage)
{
	if (!HitActor)return;

	// 計算
	// 滞空時間 + 速度
	float BaseScore = (GlidingTime * 100.0f) + SpeedAtHit;
	int32 FinalScore = FMath::RoundToInt(BaseScore * ScoreMultiplier);

	// 結果、ログの出力
	UE_LOG(LogTemp, Log, TEXT("========================================="));
	UE_LOG(LogTemp, Log, TEXT("[サゲフェーズ 結果発表]"));
	UE_LOG(LogTemp, Log, TEXT("滞空時間: %.2f 秒"),GlidingTime);
	UE_LOG(LogTemp, Log, TEXT("衝突速度: %.2f"),SpeedAtHit);
	UE_LOG(LogTemp, Log, TEXT("エリア倍率: %.1fx"),ScoreMultiplier);
	UE_LOG(LogTemp, Log, TEXT("獲得スコア: %d 点"),FinalScore);
	UE_LOG(LogTemp, Log, TEXT("評価"),*EvaluationMessage);
	UE_LOG(LogTemp, Log, TEXT("========================================="));

	if (GEngine)
	{
		FString ScreenMsg = FString::Printf(TEXT("SCORE: %d | %s (Time: %.1fs / Speed: %.0f)"),
			FinalScore, *EvaluationMessage, GlidingTime, SpeedAtHit);

		// 画面左上ではなく、エディタの右下に「トースト通知（通知ポップアップ）」としてドンと出す
		FNotificationInfo Info(FText::FromString(ScreenMsg));
		Info.bFireAndForget = true;
		Info.ExpireDuration = 5.0f;
		FSlateNotificationManager::Get().AddNotification(Info);
	}

	// Blueprintへスコア一式を投げる
	OnScoreCalculated(FinalScore, EvaluationMessage, GlidingTime, SpeedAtHit);
}