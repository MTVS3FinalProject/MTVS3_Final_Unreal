// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HttpFwd.h"
#include "HJ/TTPlayer.h"
#include "HM_HttpActor2.generated.h"

class ATTPlayer;
#pragma region struct FConcertInfo

USTRUCT()
struct FConcertTime
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, Category = "Default|Concert")
	int32 year;
    
	UPROPERTY(VisibleAnywhere, Category = "Default|Concert")
	int32 month;
    
	UPROPERTY(VisibleAnywhere, Category = "Default|Concert")
	int32 day;
    
	UPROPERTY(VisibleAnywhere, Category = "Default|Concert")
	FString time;
	
	FConcertTime()
		: year(0)
		, month(0)
		, day(0)
		, time(TEXT(""))
	{}
	// 매개 변수를 받는 생성자
	FConcertTime(int32 InYear, int32 InMonth, int32 InDay, const FString& InTime)
		: year(InYear)
		, month(InMonth)
		, day(InDay)
		, time(InTime)
	{}
};

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
	FConcertTime concertTime;
	
    FConcertInfo()
	    : concertId(1)
        , concertName(TEXT(""))
        , concertTime(0,0,0,TEXT(""))
    {}
};
#pragma endregion

#pragma region struct FSeatsList
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

	// 매개 변수를 받는 생성자
	FAvailableSeats(int32 InId, const FString&  InName, const FString& InTime)
		: seatId(InId)
		, seatName(InName)
		, drawingTime(InTime)
	{}
};

USTRUCT()
struct FReservedSeats
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, Category = "Default|Concert")
	int32 seatId;
	
	UPROPERTY(VisibleAnywhere, Category = "Default|Concert")
	FString seatName;
    
	UPROPERTY(VisibleAnywhere, Category = "Default|Concert")
	FString drawingTime;
	
	FReservedSeats()
		: seatId(0)
		, seatName(TEXT(""))
		, drawingTime(TEXT(""))
	{}
	
	// 매개 변수를 받는 생성자
	FReservedSeats(int32 InId, const FString&  InName, const FString& InTime)
		: seatId(InId)
		, seatName(InName)
		, drawingTime(InTime)
	{}
};

USTRUCT()
struct FMyReceptionSeats
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, Category = "Default|Concert")
	int32 seatId;
	
	UPROPERTY(VisibleAnywhere, Category = "Default|Concert")
	FString seatName;
    
	UPROPERTY(VisibleAnywhere, Category = "Default|Concert")
	FString drawingTime;
	
	FMyReceptionSeats()
		: seatId(0)
		, seatName(TEXT(""))
		, drawingTime(TEXT(""))
	{}
	
	// 매개 변수를 받는 생성자
	FMyReceptionSeats(int32 InId, const FString&  InName, const FString& InTime)
		: seatId(InId)
		, seatName(InName)
		, drawingTime(InTime)
	{}
};

USTRUCT()
struct FSeatsList
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, Category = "Default|Concert")
	TArray<FAvailableSeats> availableSeats;
	UPROPERTY(VisibleAnywhere, Category = "Default|Concert")
	TArray<FReservedSeats> reservedSeats;
	UPROPERTY(VisibleAnywhere, Category = "Default|Concert")
	TArray<FMyReceptionSeats> myReceptionSeats;
	
	FSeatsList() {}
};
#pragma endregion

#pragma region struct FMyReceptionSeatInfo

USTRUCT()
struct FReceptionTime
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, Category = "Default|Concert")
	int32 year;
    
	UPROPERTY(VisibleAnywhere, Category = "Default|Concert")
	int32 month;
    
	UPROPERTY(VisibleAnywhere, Category = "Default|Concert")
	int32 day;
    
	UPROPERTY(VisibleAnywhere, Category = "Default|Concert")
	FString time;
	
	FReceptionTime()
		: year(0)
		, month(0)
		, day(0)
		, time(TEXT(""))
	{}
	// 매개 변수를 받는 생성자
	FReceptionTime(int32 InYear, int32 InMonth, int32 InDay, const FString& InTime)
		: year(InYear)
		, month(InMonth)
		, day(InDay)
		, time(InTime)
	{}
};

USTRUCT()
struct FReceptionSeats
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, Category = "Default|MyReceptionSeatInfo")
	int32 seatId;

	UPROPERTY(VisibleAnywhere, Category = "Default|MyReceptionSeatInfo")
	FString seatName;
	
	UPROPERTY(VisibleAnywhere, Category = "Default|MyReceptionSeatInfo")
	FString seatInfo;

	UPROPERTY(VisibleAnywhere, Category = "Default|Concert")
	FReceptionTime drawingTime;

	UPROPERTY(VisibleAnywhere, Category = "Default|MyReceptionSeatInfo")
	int32 competitionRate;

	FReceptionSeats()
		: seatId(0)
		, seatName(TEXT(""))
		, seatInfo(TEXT(""))
		, drawingTime(0,0,0,TEXT(""))
		, competitionRate(0)
	{}
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
	void SetConcertInfo(const FConcertInfo& NewConcertInfo)
	{
		ConcertInfo = NewConcertInfo;
		SetConcertDate(NewConcertInfo.concertTime);
	};
	
	//void SetConcertId(const int32 _ConcertId) { ConcertInfo.concertId = _ConcertId; SetConcertInfo(ConcertInfo); };
    int32 GetConcertId() const { return ConcertInfo.concertId; }
	//void SetConcertName(const FString& ConcertName) { ConcertInfo.concertName = ConcertName; SetConcertInfo(ConcertInfo);};
    //const FString& GetConcertName() const { return ConcertInfo.concertName; }

	FConcertTime ConcertTime;
	//FConcertTime GetConcertDate() const { return ConcertTime; };
	void SetConcertDate(const FConcertTime& NewConcertTime) { ConcertTime = NewConcertTime; };
	
	//void SetConcertYear(int32 Year) { ConcertTime.year = Year; SetConcertDate(ConcertTime);}
    //int32 GetConcertYear() const { return ConcertTime.year; }
	//void SetConcertMonth(int32 Month) { ConcertTime.month = Month; SetConcertDate(ConcertTime);}
    //int32 GetConcertMonth() const { return ConcertTime.month; }
	//void SetConcertDay(int32 Day) { ConcertTime.day = Day; SetConcertDate(ConcertTime);}
    //int32 GetConcertDay() const { return ConcertTime.day; }
	//void SetConcertTime(const FString& Time) { ConcertTime.time = Time; SetConcertDate(ConcertTime);}
    //const FString& GetConcertTime() const { return ConcertTime.time; }
#pragma endregion

#pragma region FSeatsList Getter & Setter Methods
	FSeatsList SeatsList; // 전체 시트 정보

	FSeatsList GetSeatsList() const { return  SeatsList; };
	void SetSeatsList(const FSeatsList& NewSeatsList) { SeatsList = NewSeatsList; };

	TArray<FAvailableSeats> GetAvailableSeats() const { return SeatsList.availableSeats; };
	void SetAvailableSeats(TArray<FAvailableSeats>& NewAvailableSeats ) { SeatsList.availableSeats = NewAvailableSeats; };
	
	TArray<FReservedSeats> GetReservedSeats() const { return SeatsList.reservedSeats; }
	void SetReservedSeats(const TArray<FReservedSeats>& NewReceptionSeats) { SeatsList.reservedSeats = NewReceptionSeats; }
	
	TArray<FMyReceptionSeats> GetMyReceptionSeats() const { return SeatsList.myReceptionSeats; }
	void SetMyReceptionSeats(const TArray<FMyReceptionSeats>& NewMyReceptionSeats) { SeatsList.myReceptionSeats = NewMyReceptionSeats; }

	FMyReceptionSeats MySeats; // 내가 접수한 좌석 정보를 저장할 변수
	int32 GetMyReceptionSeatId() const { return MySeats.seatId; }
	
#pragma endregion

#pragma region FReceptionSeatInfo Getter & Setter Methods
	FReceptionSeats ReceptionSeats; // 내가 접수한 좌석 정보를 저장할 변수
	//FReceptionSeats GetReceptionSeats() const { return ReceptionSeats; };
	void SetReceptionSeats(const FReceptionSeats& NewReceptionSeatInfo)
	{
		ReceptionSeats = NewReceptionSeatInfo;
		SetReceptionDate(NewReceptionSeatInfo.drawingTime);
	};
	
	void SetReceptionSeatId(int32 _SeatId) { ReceptionSeats.seatId = _SeatId; }
	int32 GetReceptionSeatId() const { return ReceptionSeats.seatId; }
	//void SetReceptionSeatName(const FString& _SeatName) { ReceptionSeats.seatName = _SeatName; SetReceptionSeats(ReceptionSeats); }
	//const FString& GetReceptionSeatName() const { return ReceptionSeats.seatName; }
	//void SetReceptionSeatInfo(const FString& _SeatInfo) { ReceptionSeats.seatInfo = _SeatInfo; SetReceptionSeats(ReceptionSeats); }
	//const FString& GetReceptionSeatInfo() const { return ReceptionSeats.seatInfo; }
	//void SetReceptionCompetitionRate(int32 _CompetitionRate) { ReceptionSeats.competitionRate = _CompetitionRate; SetReceptionSeats(ReceptionSeats); }
	//int32 GetReceptionCompetitionRate() const { return ReceptionSeats.competitionRate; }

	FReceptionTime ReceptionTime;
	//FReceptionTime GetReceptionDate() const { return ReceptionTime; };
	void SetReceptionDate(const FReceptionTime& NewReceptionTime) { ReceptionTime = NewReceptionTime; };
	
	//void SetReceptionYear(int32 _Year) { ReceptionTime.year = _Year; SetReceptionDate(ReceptionTime); }
	//int32 GetReceptionYear() const { return ReceptionTime.year; }
	//void SetReceptionMonth(int32 _Month) { ReceptionTime.month = _Month; SetReceptionDate(ReceptionTime); }
	//int32 GetReceptionMonth() const { return ReceptionTime.month; }
	//void SetReceptionDay(int32 _Day) { ReceptionTime.day = _Day; SeReceptionDate(ReceptionTime); }
	//int32 GetReceptionDay() const { return ReceptionTime.day; }
	//void SetReceptionTime(const FString& _time) { ReceptionTime.time = _time; SetReceptionDate(ReceptionTime); }
	//const FString& GetReceptionTime() const { return ReceptionTime.time; }
#pragma endregion
	
	UPROPERTY(VisibleAnywhere, Category = "Default|params")
	FString UserCode;
	const FString& GetUserCode() const { return UserCode; }
	void SetUserCode(const FString& _UserCode) { UserCode = _UserCode; }

	UPROPERTY(VisibleAnywhere, Category = "Default|params")
	int32 PostponeSeatId;
	int32 GetPostponeSeatId() const { return PostponeSeatId; }
	void SetPostponeSeatId(int32 _PostponeSeatId) { PostponeSeatId = _PostponeSeatId; }
public:	
	// Sets default values for this actor's properties
	AHM_HttpActor2();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	 
#pragma region UI & URL
	UPROPERTY()
	class UMainWidget* MainUI;
    void SetMainUI(UMainWidget* InMainUI);

	UPROPERTY()
	class UMH_TicketingWidget* TicketingUI;
    void SetTicketingUI(UMH_TicketingWidget* InTicketingUI);

	const FString _url = "https://ticketaka.shop/api";
	
#pragma endregion

#pragma region HTTP : Entry Concert
	// 콘서트 정보 요청
	class ATTPlayer* TargetPlayer;
	void ReqGetConcertInfo(FString AccessToken, ATTPlayer* TTPlayer);
    void OnResGetConcertInfo(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful);
    
    // 공연장 입장 요청
	void ReqGetConcertEntry(FString AccessToken);
	void OnResGetConcertEntry(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful);
#pragma endregion

#pragma region HTTP : Register Seat
	// 좌석 조회 요청
	void ReqGetSeatRegistrationInquiry(FString SeatId , FString AccessToken); // int32 SeatId 로 변경해야함
	void OnResGetSeatRegistrationInquiry(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful);

	// 좌석 접수 요청
	void ReqGetRegisterSeat(FString SeatId , FString AccessToken);  // int32 SeatId 로 변경해야함
	void OnResGetRegisterSeat(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful);

    // 내가 접수한 좌석 조회 요청 => 알림창에서(?) 접수한 좌석 조회할 때
    void ReqGetMyRegisteredSeat(FString AccessToken);
    void OnResGetMyRegisteredSeat(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful);

	// 좌석 취소 요청1
	void ReqDeleteCancelRegisteredSeat(FString SeatId , FString AccessToken); // int32 SeatId 로 변경해야함
	void OnResDeleteCancelRegisteredSeat(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful);

	// 좌석 취소 요청2
	void ReqDeleteCancelRegisteredSeat2(FString SeatId , FString AccessToken); // int32 SeatId 로 변경해야함
	void OnResDeleteCancelRegisteredSeat2(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful);
	
#pragma endregion

#pragma region HTTP : Lucky Drawing
	// 추첨 시작 알림 요청
	void ReqPostNoticeGameStart(FString SeatId , FString AccessToken); // int32 SeatId 로 변경해야함
	void OnResPostNoticeGameStart(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful);
	
	// 좌석 게임 결과 요청
	void ReqPostGameResult(FString SeatId , FString AccessToken); // int32 SeatId 로 변경해야함
    void OnResPostGameResult(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful);

#pragma endregion

#pragma region HTTP : Seat payment
	// 결제시 회원 인증용 QR 요청
	void ReqGetMemberAuthQR(FString AccessToken);
	void OnResGetMemberAuthQR(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful);

	// 결제시 회원 인증 사진 업로드 확인
	void ReqGetPostConfirmMemberPhoto(FString AccessToken);
	void OnResGetPostConfirmMemberPhoto(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful);
	
	// 예매자 정보 입력 요청
	void ReqPostReservationinfo(FText UserName, FText UserPhoneNum , FText UserAddress1 , FText UserAddress2 , FString AccessToken);
	void OnResPostReservationinfo(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful);

	// 좌석 결제 요청
	void ReqPostPaymentSeat(FString SeatId, FString AccessToken);
	void OnResPostPaymentSeat(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful);
	
	// 예매 정보 불러오기 요청
	void ReqGetReservationInfo(FString AccessToken);
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
