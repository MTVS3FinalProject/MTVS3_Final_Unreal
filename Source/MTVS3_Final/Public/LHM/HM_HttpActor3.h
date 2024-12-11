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
		: ticketId(1)
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

USTRUCT()
struct FMails
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, Category = "Default|Titles")
	int32 mailId;
    
	UPROPERTY(VisibleAnywhere, Category = "Default|Titles")
	FString subject;
	
	UPROPERTY(VisibleAnywhere, Category = "Default|Titles")
	FString mailCategory;
	
	UPROPERTY(VisibleAnywhere, Category = "Default|Titles")
	bool isRead;
	
	FMails()
		: mailId(0)
		, subject(TEXT(""))
		, mailCategory(TEXT(""))
		, isRead(false)
	{}
	// 매개 변수를 받는 생성자
	FMails(int32 InId, const FString& InSubject, const FString& InCategory, bool InIsRead)
		: mailId(InId)
		, subject(InSubject)
		, mailCategory(InCategory)
		, isRead(InIsRead)
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

	UPROPERTY()
	class UHM_PuzzleWidget* PuzzleUI;
	void SetPuzzleUI(UHM_PuzzleWidget* InPuzzleUI);
	
	UPROPERTY()
	class UHM_TreeCustomTicketWidget* TreeTicketUI;
	void SetTreeTicketUI(UHM_TreeCustomTicketWidget* InTreeTicketUI);

	const FString _url = "https://ticketaka.shop/api";
	
#pragma endregion

#pragma region FTitles/FStickers/FTickets/FMails/BackgroundId Getter & Setter Methods
	TArray<FTitles> TitleItems;
	TArray<FStickers> StickerItems;
	TArray<FTickets> TicketItems;
	TArray<FMails> Mails;
	
	// 인벤토리, 우편함 데이터를 가져오는 함수
	TArray<FTitles>& GetTitleItems() { return TitleItems; }
	void SetTitleItems(TArray<FTitles>& NewTitleItems ) { TitleItems = NewTitleItems; };
	TArray<FStickers>& GetStickerItems() { return StickerItems; }
	void SetStickerItems(TArray<FStickers>& NewStickerItems ) { StickerItems = NewStickerItems; };
	TArray<FTickets>& GetTicketItems() { return TicketItems; }
	void SetTicketItems(TArray<FTickets>& NewTicketItems ) { TicketItems = NewTicketItems; };
	TArray<FMails>& GetMails() { return Mails; }
	void SetMails(TArray<FMails>& NewMails ) { Mails = NewMails; };

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
	int32 TicketID;
	int32 GetTicketId() const { return TicketID; }
	void SetTicketId(int32 _TicketId) { TicketID = _TicketId; }
	
#pragma endregion
	
#pragma region HTTP Methods
	// 인벤토리 정보 요청
	void ReqGetInventoryData(FString AccessToken);
	void OnResGetInventoryData(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful);

	// Puzzle 결과, Sticker 획득 요청
	void ReqPostPuzzleResultAndGetSticker(int32 Rank, FString Nickname, FString AccessToken);
	void OnResPostPuzzleResultAndGetSticker(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful);
	
	// 커스텀 티켓 저장 요청 - multipart
	void ReqPostSaveCustomTicketMultipart(const TArray<uint8>& ImageData, TArray<int32> StickerList, int32 BackGroundId, FString AccessToken);
	// 커스텀 티켓 저장 요청에 대한 응답
	void OnResPostSaveCustomTicket(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful);
	
	// 배경 생성 요청
	void ReqPostBackground(FString AccessToken);
	void OnResPostBackground(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful);
	
	// 티켓 커스텀 제작 입장 요청
	void ReqGetEnterTicketCustomization(FString AccessToken);
	void OnResGetEnterTicketCustomization(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful);

	// 타이틀 장착 요청
	void ReqGetEquipTheTitle(int32 TitleID, FString AccessToken);
	void OnResGetEquipTheTitle(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful);

	// 타이틀 해제 요청
	void ReqGetNotEquipTheTitle(FString AccessToken);
	void OnResGetNotEquipTheTitle(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful);

	// 좌석 결제 미루기
	void ReqPostponePaymentSeat(FString AccessToken);
	void OnResPostponePaymentSeat(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful);

	// 우편함 조회
	void ReqGetMailbox(FString AccessToken);
	void OnResGetPostponePaymentSeat(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful);
	
	// 특정 우편 조회
	void ReqGetSpecificMail(int32 MailId, FString AccessToken);
	void OnResGetSpecificMail(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful);

	// 좌석 결제 미루기 우편 조회
	void ReqGetPostponePaymentSeatMail(int32 MailId, FString AccessToken);
	void OnResGetPostponePaymentSeatMail(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful);
	
	// 퍼즐 결과 우편 조회
	void ReqGetPuzzleMail(int32 MailId, FString AccessToken);
	void OnResGetPuzzleMail(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful);

	// 커뮤니티홀 나무 조회
	void ReqGetCommunityTree(FString AccessToken);
	void OnResGetCommunityTree(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful);

	// 커뮤니티홀 나무에 달 커스텀 티켓 조회
	void ReqGetCustomTicketHangOnTree(FString AccessToken);
	void OnResGetCustomTicketHangOnTree(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful);
	
	// 커뮤니티홀 나무에 티켓 달기
	void ReqPostHangingTicketFromTree(int32 TicketId, FString AccessToken);
	void OnResPostHangingTicketFromTree(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful);
	
	// 커뮤니티홀 나무 티켓 삭제
	void ReqDeleteCommunityTree(FString AccessToken);
	void OnResDeleteCommunityTree(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful);
	
#pragma endregion
};
