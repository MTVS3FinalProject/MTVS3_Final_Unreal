// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/IHttpRequest.h"
#include "HM_HttpActor3.generated.h"

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
	UTexture2D* stickerImage;
	
	FStickers()
		: stickerId(0)
		, stickerName(TEXT(""))
		, stickerScript(TEXT(""))
		, stickerRarity(TEXT(""))
		, stickerImage(nullptr)
	{}
	// 매개 변수를 받는 생성자
	FStickers(int32 InId, const FString& InName, const FString& InScript, const FString& InRarity, UTexture2D* InImage)
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
	UTexture2D* ticketImage;
	
	FTickets()
		: ticketId(0)
		, concertName(TEXT(""))
		, seatInfo(TEXT(""))
		, ticketImage(nullptr)
	{}
	// 매개 변수를 받는 생성자
	FTickets(int32 InId, const FString& InConcertNam, const FString& InSeatInfo, UTexture2D* InImage)
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<class UHM_TicketCustom> TicketCustomWidget;
	class UHM_TicketCustom* TicketCustomUI;

	const FString _url = "https://ticketaka.shop/api";
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

	UPROPERTY(VisibleAnywhere, Category = "Default|params")
	int32 BackgroundId;
	int32 GetBackgroundId() const { return BackgroundId; }
	void SetBackgroundId(int32 _BackgroundId) { BackgroundId = _BackgroundId; }
	
#pragma endregion
	
	// 인벤토리 정보 요청
	void ReqGetInventoryData(FString AccessToken);
	void OnResGetInventoryData(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful);

	// Puzzle 결과, Sticker 획득 요청
	void ReqPostPuzzleResultAndGetSticker(int32 Rank, FString AccessToken);
	void OnResPostPuzzleResultAndGetSticker(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful);
	
	// 커스텀 티켓 저장 요청
	void ReqPostSaveCustomTicket(UTexture2D* CustomTicket, TArray<int32> StickerList, int32 BackGroundId, FString AccessToken);
	void OnReqPostSaveCustomTicket(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful);
	
	// 배경 생성 요청
	void ReqGetBackground(FString AccessToken);
	void OnReqGetBackground(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful);
	
	// My 커스텀 티켓 목록 조회 요청
	void ReqGetCustomTicketList(FString AccessToken);
	void OnReqGetCustomTicketList(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful);
	
	// 티켓 커스텀 제작 입장 요청
	void ReqPostEnterTicketCustomization(FString AccessToken);
	void OnReqPostEnterTicketCustomization(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful);
};
