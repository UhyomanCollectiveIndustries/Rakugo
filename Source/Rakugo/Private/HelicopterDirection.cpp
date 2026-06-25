#include "HelicopterDirection.h"

// コンストラクタ
AHelicopterDirection::AHelicopterDirection()
{
	PrimaryActorTick.bCanEverTick = true; // Tickを動かすために必要

	//デフォルト値の設定
	UpwardSpeed = 200.0f;			// 1秒間に2m上昇
	RotorRotationSpeed = 720.0f;	// 1秒間に2回転

	// 本体メッシュの作成とルートへの設定
	BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyMesh"));
	RootComponent = BodyMesh;

	// 羽メッシュの作成と本体へのアタッチ
	RotorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RotorMesh"));
	RotorMesh->SetupAttachment(BodyMesh);
}

// BeginPlay
void AHelicopterDirection::BeginPlay()
{
	Super::BeginPlay();
}

// Tick
void AHelicopterDirection::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	// --- 1. 全体を上空へ移動（Actorごと上昇） ---
	// 現在の位置を取得
	FVector NewLocation = GetActorLocation();
	// Z軸（上方向）に移動量を加算 (速度 × フレームの経過時間)
	NewLocation.Z += UpwardSpeed * DeltaTime;
	// 新しい位置を適用
	SetActorLocation(NewLocation);

	// --- 2. 上の羽のみを回転（ローカル空間での回転） ---
	// Z軸（Yaw）を中心に回転させるクォータニオンを作成
	FRotator DeltaRotation = FRotator(0.0f, RotorRotationSpeed * DeltaTime, 0.0f);
	// 子コンポーネントだけをローカル（自分自身）の軸で回転させる
	RotorMesh->AddLocalRotation(DeltaRotation);
}