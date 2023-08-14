// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pawn_Basic.h"

#include "Engine/DataTable.h"
#include "Pawn_NPC.generated.h"

UENUM(BlueprintType)
enum class enum_NPCAnimationStateC : uint8
{
	Error = 0,
	Idle,
	Walk,
	Death,
};

UENUM(BlueprintType)
enum class enum_NPCMultiType : uint8
{
	Single = 0,
	Multi,
};


USTRUCT(Atomic, BlueprintType)
struct FNpcCustomizing_Struct : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FVector NpcLocation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName NpcName;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		enum_NPCMultiType Type;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		USkeletalMesh* Boddy;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		USkeletalMesh* Hair;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		USkeletalMesh* Top;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		USkeletalMesh* Bottoms;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		USkeletalMesh* Shoes;
	
};


UCLASS()
class TEST_LIDARSAMPLE_API APawn_NPC : public APawn_Basic
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	APawn_NPC();

	// 캐릭터 스켈레톤 설정

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UWidgetComponent* NameTagComp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UWidgetComponent* LogComp;
	// 채팅 메세지 출력 말풍선
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		class UWidgetComponent* MessageComp;


	// 현재 애니메이션 상태
	UPROPERTY(BlueprintReadWrite)
	enum_NPCAnimationStateC PlayerAnimState;

	// NPC 멀티 타입(온라인인지 아닌지 타입으로 구분)
	UPROPERTY(BlueprintReadWrite)
	enum_NPCMultiType eNPCType = enum_NPCMultiType::Single;

	// 상호작용 가능 상태. false일때 상호작용 가능
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIntercation = false;

	// 닉네임 위젯 공개 상태
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bNametag = false;

	// 회전 논리 값
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bRotationNPC = false;

	// 회전 속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float NPCTurnSpeed = 120.0f;

	//현재 무브타이머의 타임
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float 	fMoveTimeNow = 0.0f;

	//방향 변수 1 or -1
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float 	fRoationDirection = 1.0f;

	//캐릭터 이름
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText CharacterName;

	//캐릭터 ID (서버에서 구분하는데 사용)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString CharacterID;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	bool bCheckOutline = true;


public:	

	UFUNCTION(BlueprintCallable)
		void setOutline();
	UFUNCTION(BlueprintCallable)
		void ClearOutline();

	UFUNCTION(BlueprintCallable)
		void SetName(const FText& Name);

	UFUNCTION(BlueprintCallable)
		void OnNameTag();

	UFUNCTION(BlueprintCallable)
		void SelfDestroy();

	//********상속 가능한 함수 목록***************************//
	// 
	//NPC 상호작용시

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual	void NPCInteraction();

	//멀티포함 NPC 이동함수
	UFUNCTION(BlueprintCallable)
	void NPCMoveToLocation(FVector FLoc);

	// Nav메시기반 이동 함수
	UFUNCTION(BlueprintImplementableEvent)
	void NPCMove(FVector FLoc); 

	//NPC 초기화
	UFUNCTION(BlueprintImplementableEvent)
	void InitNPC(const FString& ID);
};
