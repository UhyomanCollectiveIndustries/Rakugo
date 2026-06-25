#pragma once
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "RakugoDataType.generated.h"

/**
* なぞかけの正解組み合わせデータ
*/
USTRUCT(BlueprintType)
struct FRakugoNazokakeAnswer : public FTableRowBase
{
	GENERATED_BODY()

public:
	/** フェーズ1のアイテム名 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Nazokake|ItemA")
	FString ItemA;

	/** フェーズ2のアイテム名 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Nazokake|ItemB")
	FString ItemB;

	/** フェーズ3の床タグ */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Nazokake|Floor")
	FString FloorTag;

	/** 正解時のメッセージ*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Nazokake|Answer")
	FString AnswerMessage;
};