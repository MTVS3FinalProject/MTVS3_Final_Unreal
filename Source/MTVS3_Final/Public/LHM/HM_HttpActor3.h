// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/IHttpRequest.h"
#include "HM_HttpActor3.generated.h"

UENUM(BlueprintType)
enum class EImageType : uint8
{
	StickerImage,
	TicketImage,
	BackgroundImage,
};
USTRUCT()
struct FTitles
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, Category = "Default|Titles")
	int32 titleId;
    
	UPROPERTY(VisibleAnywhere, Category = "Default|Titles")
	FString titleName;
	
	UPROPERTY(VisibleAnywhere, Category = "Default|Titles")
	FString titleScript;
	
	UPROPERTY(VisibleAnywhere, Category = "Default|Titles")
	FString titleRarity;
	
	UPROPERTY(VisibleAnywhere, Category = "Default|Titles")
	bool bIsEquipped;
	
	FTitles()
		: titleId(0)
		, titleName(TEXT(""))
		, titleScript(TEXT(""))
		, titleRarity(TEXT(""))
		, bIsEquipped(false)
	{}
	// 매개 변수를 받는 생성자
	FTitles(int32 InId, const FString& InName, const FString& InScript, const FString& InRarity, bool InEquipped)
		: titleId(InId)
		, titleName(InName)
		, titleScript(InScript)
		, titleRarity(InRarity)
		, bIsEquipped(InEquipped)
	{}
};

USTRUCT()
struct FStickers
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, Category = "Default|Stickers")
	int32 stickerId;

	UPROPERTY(VisibleAnywhere, Category = "Default|Stickers")
	FString stickerName;
	
	UPROPERTY(VisibleAnywhere, Category = "Default|Stickers")
	FString stickerScript;

	UPROPERTY(VisibleAnywhere, Category = "Default|Stickers")
	FString stickerRarity;
	
	UPROPERTY(VisibleAnywhere, Category = "Default|Stickers")
	FString stickerImage;
	
	FStickers()
		: stickerId(0)
		, stickerName(TEXT(""))
		, stickerScript(TEXT(""))
		, stickerRarity(TEXT(""))
		, stickerImage(TEXT(""))
	{}
	// 매개 변수를 받는 생성자
	FStickers(int32 InId, const FString& InName, const FString& InScript, const FString& InRarity, const FString& InImage)
		: stickerId(InId)
		, stickerName(InName)
		, stickerScript(InScript)
		, stickerRarity(InRarity)
		, stickerImage(InImage)
	{}
};

USTRUCT()
struct FTickets
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, Category = "Default|Stickers")
	int32 ticketId;
	
	UPROPERTY(VisibleAnywhere, Category = "Default|Stickers")
	FString concertName;
	
	UPROPERTY(VisibleAnywhere, Category = "Default|Stickers")
	FString seatInfo;

	UPROPERTY(VisibleAnywhere, Category = "Default|Stickers")
	FString ticketImage;
	
	FTickets()
		: ticketId(0)
		, concertName(TEXT(""))
		, seatInfo(TEXT(""))
		, ticketImage(TEXT(""))
	{}
	// 매개 변수를 받는 생성자
	FTickets(int32 InId, const FString& InConcertNam, const FString& InSeatInfo, const FString& InImage)
		: ticketId(InId)
		, concertName(InConcertNam)
		, seatInfo(InSeatInfo)
		, ticketImage(InImage)
	{}
};

UCLASS()
class MTVS3_FINAL_API AHM_HttpActor3 : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHM_HttpActor3();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
#pragma region UI & URL
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	class UMainWidget* MainUI;
	void SetMainUI(UMainWidget* InMainUI);
	
	UPROPERTY()
	class UMH_TicketingWidget* TicketingUI;
	void SetTicketingUI(UMH_TicketingWidget* InTicketingUI);

	const FString _url = "https://ticketaka.shop/api";
	//const FString _url = "http://125.132.216.190:7878/api";
#pragma endregion

#pragma region FTitles/FStickers/FTickets/BackgroundId Getter & Setter Methods
	TArray<FTitles> TitleItems;
	TArray<FStickers> StickerItems;
	TArray<FTickets> TicketItems;
	
	// 인벤토리 데이터를 가져오는 함수
	TArray<FTitles>& GetTitleItems() { return TitleItems; }
	void SetTitleItems(TArray<FTitles>& NewTitleItems ) { TitleItems = NewTitleItems; };
	TArray<FStickers>& GetStickerItems() { return StickerItems; }
	void SetStickerItems(TArray<FStickers>& NewStickerItems ) { StickerItems = NewStickerItems; };
	TArray<FTickets>& GetTicketItems() { return TicketItems; }
	void SetTicketItems(TArray<FTickets>& NewTicketItems ) { TicketItems = NewTicketItems; };

	// 스티커 아이디 리스트를 가져오는 함수
	TArray<int32> GetStickerIds() const
	{
		TArray<int32> StickerIds;
		for (const FStickers& Sticker : StickerItems)
		{
			StickerIds.Add(Sticker.stickerId);
		}
		return StickerIds;
	}

	UPROPERTY(VisibleAnywhere, Category = "Default|params")
	int32 BackgroundId;
	int32 GetBackgroundId() const { return BackgroundId; }
	void SetBackgroundId(int32 _BackgroundId) { BackgroundId = _BackgroundId; }

	// 현재 다운로드 중인 이미지의 인덱스를 추적
	TMap<FString, int32> ImageUrlToIndex;

	// 시연용 ticketId
	UPROPERTY(VisibleAnywhere, Category = "Default|params")
	int32 CheatTicketID;
	int32 GetTicketId() const { return CheatTicketID; }
	void SetTicketId(int32 _TicketId) { CheatTicketID = _TicketId; }
	
#pragma endregion
	
#pragma region HTTP Methods
	// 인벤토리 정보 요청
	void ReqGetInventoryData(FString AccessToken);
	void OnResGetInventoryData(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful);

	// Puzzle 결과, Sticker 획득 요청
	void ReqPostPuzzleResultAndGetSticker(int32 Rank, FString AccessToken);
	void OnResPostPuzzleResultAndGetSticker(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful);
	
	// 커스텀 티켓 저장 요청
	void ReqPostSaveCustomTicket(const TArray<uint8>& ImageData, TArray<int32> StickerList, int32 BackGroundId, FString AccessToken);
	void OnResPostSaveCustomTicket(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful);
	
	// 배경 생성 요청
	void ReqPostBackground(FString AccessToken);
	void OnResPostBackground(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful);
	
	// My 커스텀 티켓 목록 조회 요청
	void ReqGetCustomTicketList(FString AccessToken);
	void OnResGetCustomTicketList(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful);
	
	// 티켓 커스텀 제작 입장 요청
	void ReqGetEnterTicketCustomization(FString AccessToken);
	void OnResGetEnterTicketCustomization(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful);

	// 타이틀 장착 요청
	void ReqGetEquipTheTitle(int32 TitleID, FString AccessToken);
	void OnResGetEquipTheTitle(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful);

	// 타이틀 해제 요청
	void ReqGetNotEquipTheTitle(FString AccessToken);
	void OnResGetNotEquipTheTitle(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful);
	
#pragma endregion
};
