// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HttpFwd.h"
#include "HM_HttpActor2.generated.h"

#pragma region struct FConcertInfo
USTRUCT()
struct FConcertInfo
{
    GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, Category = "Default|Concert")
	int32 concertId;
	
    UPROPERTY(VisibleAnywhere, Category = "Default|Concert")
    FString concertName;
    
    UPROPERTY(VisibleAnywhere, Category = "Default|Concert")
    int32 year;
    
    UPROPERTY(VisibleAnywhere, Category = "Default|Concert")
    int32 month;
    
    UPROPERTY(VisibleAnywhere, Category = "Default|Concert")
    int32 day;
    
    UPROPERTY(VisibleAnywhere, Category = "Default|Concert")
    FString time;
	
    FConcertInfo()
	    : concertId(0)
        , concertName(TEXT(""))
        , year(0)
        , month(0)
        , day(0)
        , time(TEXT(""))
    {}
};
#pragma endregion

#pragma region struct FAvailableSeats
USTRUCT()
struct FAvailableSeats
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, Category = "Default|Concert")
	int32 seatId;
	
	UPROPERTY(VisibleAnywhere, Category = "Default|Concert")
	FString seatName;
    
	UPROPERTY(VisibleAnywhere, Category = "Default|Concert")
	FString drawingTime;
	
	FAvailableSeats()
		: seatId(0)
		, seatName(TEXT(""))
		, drawingTime(TEXT(""))
	{}
};
#pragma endregion

#pragma region struct FReceptionSeats
USTRUCT()
struct FReceptionSeats
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, Category = "Default|Concert")
	int32 seatId;
	
	UPROPERTY(VisibleAnywhere, Category = "Default|Concert")
	FString seatName;
    
	UPROPERTY(VisibleAnywhere, Category = "Default|Concert")
	FString drawingTime;
	
	FReceptionSeats()
		: seatId(0)
		, seatName(TEXT(""))
		, drawingTime(TEXT(""))
	{}
};
#pragma endregion

#pragma region struct FMyReceptionSeatInfo
USTRUCT()
struct FMyReceptionSeats
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, Category = "Default|MyReceptionSeatInfo")
	int32 seatId;

	UPROPERTY(VisibleAnywhere, Category = "Default|MyReceptionSeatInfo")
	FString seatName;
	
	UPROPERTY(VisibleAnywhere, Category = "Default|MyReceptionSeatInfo")
	FString seatInfo;

	UPROPERTY(VisibleAnywhere, Category = "Default|MyReceptionSeatInfo")
	int32 year;

	UPROPERTY(VisibleAnywhere, Category = "Default|MyReceptionSeatInfo")
	int32 month;
	
	UPROPERTY(VisibleAnywhere, Category = "Default|MyReceptionSeatInfo")
	int32 day;
	
	UPROPERTY(VisibleAnywhere, Category = "Default|MyReceptionSeatInfo")
	FString time;

	UPROPERTY(VisibleAnywhere, Category = "Default|MyReceptionSeatInfo")
	int32 competitionRate;

	FMyReceptionSeats()
		: seatId(0)
		, seatName(TEXT(""))
		, seatInfo(TEXT(""))
		, year(0)
		, month(0)
		, day(0)
		, time(TEXT(""))
		, competitionRate(0)
	{}
};
#pragma endregion

#pragma region struct FSeatId
USTRUCT()
struct FSeatId
{
    GENERATED_BODY()

public:
    FSeatId()
	: SeatId(TEXT(""))
	, DrawingTime(TEXT(""))
	{}

    const FString& GetSeatId() const { return SeatId; }
    void SetSeatId(const FString& _SeatId) { SeatId = _SeatId; }

    const FString& GetDrawingTime() const { return DrawingTime; }
    void SetDrawingTime(const FString& _DrawingTime) { DrawingTime = _DrawingTime; }

private:
    UPROPERTY(VisibleAnywhere, Category = "Default|Seat")
    FString SeatId;

    UPROPERTY(VisibleAnywhere, Category = "Default|Seat")
	FString DrawingTime;
};
#pragma endregion

UCLASS()
class MTVS3_FINAL_API AHM_HttpActor2 : public AActor
{
	GENERATED_BODY()

public:
#pragma region FConcertInfo Getter & Setter Methods
    FConcertInfo ConcertInfo; // 콘서트 정보를 저장할 변수
	
	FConcertInfo GetConcertInfo() const { return ConcertInfo; };
	void SetConcertInfo(const FConcertInfo& NewConcertInfo) { ConcertInfo = NewConcertInfo; };
	
	void SetConcertId(const int32 _ConcertId) { ConcertInfo.concertId = _ConcertId; SetConcertInfo(ConcertInfo); };
    int32 GetConcertId() const { return ConcertInfo.concertId; }
	void SetConcertName(const FString& ConcertName) { ConcertInfo.concertName = ConcertName; SetConcertInfo(ConcertInfo); };
    const FString& GetConcertName() const { return ConcertInfo.concertName; }
	void SetConcertYear(int32 ConcertYear) { ConcertInfo.year = ConcertYear; SetConcertInfo(ConcertInfo);}
    int32 GetConcertYear() const { return ConcertInfo.year; }
	void SetConcertMonth(int32 ConcertMonth) { ConcertInfo.month = ConcertMonth; SetConcertInfo(ConcertInfo);}
    int32 GetConcertMonth() const { return ConcertInfo.month; }
	void SetConcertDay(int32 ConcertDay) { ConcertInfo.day = ConcertDay; SetConcertInfo(ConcertInfo);}
    int32 GetConcertDay() const { return ConcertInfo.day; }
	void SetConcertTime(const FString& ConcertTime) { ConcertInfo.time = ConcertTime; SetConcertInfo(ConcertInfo);}
    const FString& GetConcertTime() const { return ConcertInfo.time; }
#pragma endregion

#pragma region FAvailableSeats Getter & Setter Methods
	FAvailableSeats AvailableSeats; // 접수 가능한 좌석 정보를 저장할 변수

	FAvailableSeats GetAvailableSeats() const { return  AvailableSeats; };
	void SetAvailableSeats(const FAvailableSeats& NewAvailableSeats) { AvailableSeats = NewAvailableSeats; };

	void SetAvailableSeatId(const int32 _SeatId) { AvailableSeats.seatId = _SeatId; SetAvailableSeats(AvailableSeats); };
	int32 GetAvailableSeatId() const { return AvailableSeats.seatId; }
	void SetAvailableSeatName(const FString& _SeatName) { AvailableSeats.seatName = _SeatName; SetAvailableSeats(AvailableSeats); };
	const FString& GetAvailableSeatName() const { return AvailableSeats.seatName; }
	void SetAvailableSeatDrawingTime(const FString& _DrawingTime) { AvailableSeats.drawingTime = _DrawingTime; SetAvailableSeats(AvailableSeats); };
	const FString& GetAvailableDrawingTime() const { return AvailableSeats.drawingTime; }
#pragma endregion

#pragma region FReceptionSeats Getter & Setter Methods
	FReceptionSeats ReceptionSeats; // 접수 가능한 좌석 정보를 저장할 변수

	FReceptionSeats GetReceptionSeats() const { return  ReceptionSeats; };
	void SetReceptionSeats(const FReceptionSeats& NewReceptionSeats) { ReceptionSeats = NewReceptionSeats; };

	void SetReceptionSeatId(const int32 _SeatId) { ReceptionSeats.seatId = _SeatId; SetReceptionSeats(ReceptionSeats); };
	int32 GetReceptionSeatId() const { return ReceptionSeats.seatId; }
	void SetReceptionSeatName(const FString& _SeatName) { ReceptionSeats.seatName = _SeatName; SetReceptionSeats(ReceptionSeats); };
	const FString& GetReceptionSeatName() const { return ReceptionSeats.seatName; }
	void SetReceptionSeatDrawingTime(const FString& _DrawingTime) { ReceptionSeats.drawingTime = _DrawingTime; SetReceptionSeats(ReceptionSeats); };
	const FString& GetReceptionDrawingTime() const { return ReceptionSeats.drawingTime; }
#pragma endregion

#pragma region FMyReceptionSeatInfo Getter & Setter Methods
	FMyReceptionSeats MyReceptionSeats; // 내가 접수한 좌석 정보를 저장할 변수

	FMyReceptionSeats GetMyReceptionSeats() const { return  MyReceptionSeats; };
	void SetMyReceptionSeats(const FMyReceptionSeats& NewMyReceptionSeatInfo) { MyReceptionSeats = NewMyReceptionSeatInfo; };
	
	void SetMyReceptionSeatId(int32 _SeatId) { MyReceptionSeats.seatId = _SeatId; SetMyReceptionSeats(MyReceptionSeats); }
	int32 GetMyReceptionSeatId() const { return MyReceptionSeats.seatId; }
	void SetMyReceptionSeatName(const FString& _SeatName) { MyReceptionSeats.seatName = _SeatName; SetMyReceptionSeats(MyReceptionSeats); }
	const FString& GetMyReceptionSeatName() const { return MyReceptionSeats.seatName; }
	void SetMyReceptionSeatInfo(const FString& _SeatInfo) { MyReceptionSeats.seatInfo = _SeatInfo; SetMyReceptionSeats(MyReceptionSeats); }
	const FString& GetMyReceptionSeatInfo() const { return MyReceptionSeats.seatInfo; }
	void SetMyReceptionYear(int32 _Year) { MyReceptionSeats.year = _Year; SetMyReceptionSeats(MyReceptionSeats); }
	int32 GetMyReceptionYear() const { return MyReceptionSeats.year; }
	void SetMyReceptionMonth(int32 _Month) { MyReceptionSeats.month = _Month; SetMyReceptionSeats(MyReceptionSeats); }
	int32 GetMyReceptionMonth() const { return MyReceptionSeats.month; }
	void SetMyReceptionDay(int32 _Day) { MyReceptionSeats.day = _Day; SetMyReceptionSeats(MyReceptionSeats); }
	int32 GetMyReceptionDay() const { return MyReceptionSeats.day; }
	void SetMyReceptionTime(const FString& _time) { MyReceptionSeats.time = _time; SetMyReceptionSeats(MyReceptionSeats); }
	const FString& GetMyReceptionTime() const { return MyReceptionSeats.time; }
	void SetMyReceptionCompetitionRate(int32 _CompetitionRate) { MyReceptionSeats.competitionRate = _CompetitionRate; SetMyReceptionSeats(MyReceptionSeats); }
	int32 GetMyReceptionCompetitionRate() const { return MyReceptionSeats.competitionRate; }
#pragma endregion
	
	UPROPERTY(VisibleAnywhere, Category = "Default|params")
	FString UserCode;
	const FString& GetUserCode() const { return UserCode; }
	void SetUserCode(const FString& _UserCode) { UserCode = _UserCode; }

	UPROPERTY(VisibleAnywhere, Category = "Default|params")
	int32 SeatFloor;
	int32 GetSeatFloor() const { return SeatFloor; }
	void SetSeatFloor(int32 _SeatFloor) { SeatFloor = _SeatFloor; }
	
public:	
	// Sets default values for this actor's properties
	AHM_HttpActor2();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
#pragma region UI

	UPROPERTY()
	class UMainWidget* MainUI;
    void SetMainUI(UMainWidget* InMainUI);

	UPROPERTY()
	class UMH_TicketingWidget* TicketingUI;
    void SetTicketingUI(UMH_TicketingWidget* InTicketingUI);

#pragma endregion

	// 백엔드에 요청 보낼 때만 api 포함, 프론트는 api X
	const FString _url = "https://ticketaka.shop/api";

#pragma region HTTP : Entry Concert
	// 공연장 선택 UI 요청
    void ReqGetConcertInfo(FString AccessToken);

    // 공연장 선택 UI 요청에 대한 응답
    void OnResGetConcertInfo(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful);
    
    // 공연장 입장 요청
	void ReqGetConcertEntry(FString AccessToken);

	// 공연장 입장 요청에 대한 응답
	void OnResGetConcertEntry(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful);
#pragma endregion

#pragma region HTTP : Register Seat
	// 좌석 조회 요청
	void ReqGetSeatRegistrationInquiry(FString SeatName , FString AccessToken);

	// 좌석 조회 요청에 대한 응답
	void OnResGetSeatRegistrationInquiry(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful);

	// 좌석 접수 요청
	void ReqGetRegisterSeat(FString SeatName , FString AccessToken);

	// 좌석 접수 요청에 대한 응답
	void OnResGetRegisterSeat(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful);

    // 내가 접수한 좌석 조회 요청 => 알림창에서(?) 접수한 좌석 조회할 때
    void ReqGetMyRegisteredSeat(FString AccessToken);

    // 내가 접수한 좌석 조회 요청에 대한 응답
    void OnResGetMyRegisteredSeat(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful);

	// 좌석 취소 요청
	void ReqDeleteCancelRegisteredSeat(FString SeatId , FString AccessToken);

	// 좌석 취소 요청에 대한 응답
	void OnResDeleteCancelRegisteredSeat(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful);

#pragma endregion

#pragma region HTTP : Lucky Drawing
	// 추첨 시작 알림 요청
	void ReqPostNoticeGameStart(FString SeatName , FString AccessToken);

	// 추첨 시작 알림 요청에 대한 응답
	void OnResPostNoticeGameStart(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful);
	
	// 좌석 게임 결과 요청
	void ReqPostGameResult(FString SeatName , FString AccessToken);

    // 좌석 게임 결과 요청에 대한 응답
    void OnResPostGameResult(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful);

#pragma endregion

#pragma region HTTP : Seat payment
	// 결제시 회원 인증용 QR 요청
	void ReqGetMemberAuthQR(FString AccessToken);

	// 결제시 회원 인증용 QR 요청에 대한 응답
	void OnResGetMemberAuthQR(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful);

	// 결제시 회원 인증 사진 업로드 확인
	void ReqGetPostConfirmMemberPhoto(FString AccessToken);

	// 결제시 회원 인증 사진 업로드 확인에 대한 응답
	void OnResGetPostConfirmMemberPhoto(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful);
	
	// 예매자 정보 입력 요청
	void ReqPostReservationinfo(FText UserName, FText UserPhoneNum , FText UserAddress1 , FText UserAddress2 , FString AccessToken);

	// 예매자 정보 입력 요청에 대한 응답
	void OnResPostReservationinfo(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful);

	// 좌석 결제 요청
	void ReqPostPaymentSeat(FString SeatId, FString AccessToken);

	// 좌석 결제 요청에 대한 응답
	void OnResPostPaymentSeat(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful);
	
	// 예매 정보 불러오기 요청
	void ReqGetReservationInfo(FString AccessToken);

	// 예매 정보 불러오기 요청에 대한 응답
	void OnResGetReservationInfo(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful);

#pragma endregion

#pragma region HTTP : 개발자 키
    // 개발자 키(2) Cheat Seat 좌석 게임 결과 요청
    void ReqPostCheatGameResult( FString AccessToken);
	// 개발자 키(2-1) 좌석 결제 요청/응답
	void ReqPostCheatPaymentSeat(FString AccessToken);
	void OnResPostCheatPaymentSeat(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful);
#pragma endregion
};
