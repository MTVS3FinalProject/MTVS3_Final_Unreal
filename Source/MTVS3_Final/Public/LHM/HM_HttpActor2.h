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
	
	// 매개 변수를 받는 생성자
	FReceptionSeats(int32 InId, const FString&  InName, const FString& InTime)
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
	TArray<FReceptionSeats> receptionSeats;
	
	FSeatsList() {}
};
#pragma endregion

#pragma region struct FMyReceptionSeatInfo

USTRUCT()
struct FMyReceptionTime
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
	
	FMyReceptionTime()
		: year(0)
		, month(0)
		, day(0)
		, time(TEXT(""))
	{}
	// 매개 변수를 받는 생성자
	FMyReceptionTime(int32 InYear, int32 InMonth, int32 InDay, const FString& InTime)
		: year(InYear)
		, month(InMonth)
		, day(InDay)
		, time(InTime)
	{}
};

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

	UPROPERTY(VisibleAnywhere, Category = "Default|Concert")
	FMyReceptionTime drawingTime;

	UPROPERTY(VisibleAnywhere, Category = "Default|MyReceptionSeatInfo")
	int32 competitionRate;

	FMyReceptionSeats()
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
	
	TArray<FReceptionSeats> GetReceptionSeats() const { return SeatsList.receptionSeats; }
	void SetReceptionSeats(const TArray<FReceptionSeats>& NewReceptionSeats) { SeatsList.receptionSeats = NewReceptionSeats; }
#pragma endregion

#pragma region FMyReceptionSeatInfo Getter & Setter Methods
	FMyReceptionSeats MyReceptionSeats; // 내가 접수한 좌석 정보를 저장할 변수
	//FMyReceptionSeats GetMyReceptionSeats() const { return  MyReceptionSeats; };
	void SetMyReceptionSeats(const FMyReceptionSeats& NewMyReceptionSeatInfo)
	{
		MyReceptionSeats = NewMyReceptionSeatInfo;
		SetMyReceptionDate(NewMyReceptionSeatInfo.drawingTime);
	};
	
	//void SetMyReceptionSeatId(int32 _SeatId) { MyReceptionSeats.seatId = _SeatId; SetMyReceptionSeats(MyReceptionSeats); }
	int32 GetMyReceptionSeatId() const { return MyReceptionSeats.seatId; }
	//void SetMyReceptionSeatName(const FString& _SeatName) { MyReceptionSeats.seatName = _SeatName; SetMyReceptionSeats(MyReceptionSeats); }
	//const FString& GetMyReceptionSeatName() const { return MyReceptionSeats.seatName; }
	//void SetMyReceptionSeatInfo(const FString& _SeatInfo) { MyReceptionSeats.seatInfo = _SeatInfo; SetMyReceptionSeats(MyReceptionSeats); }
	//const FString& GetMyReceptionSeatInfo() const { return MyReceptionSeats.seatInfo; }
	//void SetMyReceptionCompetitionRate(int32 _CompetitionRate) { MyReceptionSeats.competitionRate = _CompetitionRate; SetMyReceptionSeats(MyReceptionSeats); }
	//int32 GetMyReceptionCompetitionRate() const { return MyReceptionSeats.competitionRate; }

	FMyReceptionTime MyReceptionTime;
	//FMyReceptionTime GetMyReceptionDate() const { return MyReceptionTime; };
	void SetMyReceptionDate(const FMyReceptionTime& NewMyReceptionTime) { MyReceptionTime = NewMyReceptionTime; };
	
	//void SetMyReceptionYear(int32 _Year) { MyReceptionTime.year = _Year; SetMyReceptionDate(MyReceptionTime); }
	//int32 GetMyReceptionYear() const { return MyReceptionTime.year; }
	//void SetMyReceptionMonth(int32 _Month) { MyReceptionTime.month = _Month; SetMyReceptionDate(MyReceptionTime); }
	//int32 GetMyReceptionMonth() const { return MyReceptionTime.month; }
	//void SetMyReceptionDay(int32 _Day) { MyReceptionTime.day = _Day; SetMyReceptionDate(MyReceptionTime); }
	//int32 GetMyReceptionDay() const { return MyReceptionTime.day; }
	//void SetMyReceptionTime(const FString& _time) { MyReceptionTime.time = _time; SetMyReceptionDate(MyReceptionTime); }
	//const FString& GetMyReceptionTime() const { return MyReceptionTime.time; }
#pragma endregion
	
	UPROPERTY(VisibleAnywhere, Category = "Default|params")
	FString UserCode;
	const FString& GetUserCode() const { return UserCode; }
	void SetUserCode(const FString& _UserCode) { UserCode = _UserCode; }

	
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
	//const FString _url = "http://125.132.216.190:7878/api";
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
	void ReqPostPaymentSeat(int32 SeatId, FString AccessToken);
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
