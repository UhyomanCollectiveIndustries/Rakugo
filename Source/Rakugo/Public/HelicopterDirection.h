#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HelicopterDirection.generated.h"

UCLASS()
class RAKUGO_API AHelicopterDirection : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHelicopterDirection();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// ヘリコプター本体のメッシュ
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Hericopter|Components")
	UStaticMeshComponent* BodyMesh;

	// ヘリコプターの上の羽のメッシュ
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Hericopter|Components")
	UStaticMeshComponent* RotorMesh;

	// 上昇スピード
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Hericopter|Settings")
	float UpwardSpeed;

	// 羽の回転スピード
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Hericopter|Settings")
	float RotorRotationSpeed;
};	
