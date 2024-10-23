// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HttpFwd.h"
#include "HM_HttpActor2.generated.h"

// SeatIdDTO 구조체
USTRUCT()
struct FSeatIdDTO
{
    GENERATED_BODY()

public:
    FSeatIdDTO() : SeatId(TEXT("")) {}

    const FString& GetSeatId() const { return SeatId; }
    void SetSeatId(const FString& InSeatId) { SeatId = InSeatId; }

private:
    UPROPERTY(VisibleAnywhere, Category = "Default|Seat")
    FString SeatId;
};

// MyReceptionSeatInfoDTO 구조체
USTRUCT()
struct FMyReceptionSeatInfoDTO
{
    GENERATED_BODY()

public:
    FMyReceptionSeatInfoDTO()
        : m_SeatId(TEXT(""))
        , m_ConcertDate(TEXT(""))
        , m_SeatInfo(TEXT(""))
        , m_DrawingTime(TEXT(""))
        , m_CompetitionRate(TEXT(""))
    {}

    const FString& GetSeatId() const { return m_SeatId; }
    void SetSeatId(const FString& SeatId) { m_SeatId = SeatId; }

    const FString& GetConcertDate() const { return m_ConcertDate; }
    void SetConcertDate(const FString& ConcertDate) { m_ConcertDate = ConcertDate; }

    const FString& GetSeatInfo() const { return m_SeatInfo; }
    void SetSeatInfo(const FString& SeatInfo) { m_SeatInfo = SeatInfo; }

    const FString& GetDrawingTime() const { return m_DrawingTime; }
    void SetDrawingTime(const FString& DrawingTime) { m_DrawingTime = DrawingTime; }

    const FString& GetCompetitionRate() const { return m_CompetitionRate; }
    void SetCompetitionRate(const FString& CompetitionRate) { m_CompetitionRate = CompetitionRate; }

private:
    UPROPERTY(VisibleAnywhere, Category = "Default|Seat")
    FString m_SeatId;

    UPROPERTY(VisibleAnywhere, Category = "Default|Seat")
    FString m_ConcertDate;

    UPROPERTY(VisibleAnywhere, Category = "Default|Seat")
    FString m_SeatInfo;

    UPROPERTY(VisibleAnywhere, Category = "Default|Seat")
    FString m_DrawingTime;

    UPROPERTY(VisibleAnywhere, Category = "Default|Seat")
    FString m_CompetitionRate;
};

// Concert 구조체
USTRUCT()
struct FConcertDTO
{
    GENERATED_BODY()

public:
    FConcertDTO()
        : m_ConcertName(TEXT("Concert01")) // 테스트용 나중에 받아올거임
        , m_ConcertYear(0)
        , m_ConcertMonth(0)
        , m_ConcertDay(0)
        , m_ConcertTime(TEXT(""))
    {}

    const FString& GetConcertName() const { return m_ConcertName; }
    void SetConcertName(const FString& ConcertName) { m_ConcertName = ConcertName; }

    int32 GetConcertYear() const { return m_ConcertYear; }
    void SetConcertYear(int32 ConcertYear) { m_ConcertYear = ConcertYear; }

    int32 GetConcertMonth() const { return m_ConcertMonth; }
    void SetConcertMonth(int32 ConcertMonth) { m_ConcertMonth = ConcertMonth; }

    int32 GetConcertDay() const { return m_ConcertDay; }
    void SetConcertDay(int32 ConcertDay) { m_ConcertDay = ConcertDay; }

    const FString& GetConcertTime() const { return m_ConcertTime; }
    void SetConcertTime(const FString& ConcertTime) { m_ConcertTime = ConcertTime; }

private:
    UPROPERTY(VisibleAnywhere, Category = "Default|Concert")
    FString m_ConcertName;

    UPROPERTY(VisibleAnywhere, Category = "Default|Concert")
    int32 m_ConcertYear;

    UPROPERTY(VisibleAnywhere, Category = "Default|Concert")
    int32 m_ConcertMonth;

    UPROPERTY(VisibleAnywhere, Category = "Default|Concert")
    int32 m_ConcertDay;

    UPROPERTY(VisibleAnywhere, Category = "Default|Concert")
    FString m_ConcertTime;
};

// ConcertReservation 구조체
USTRUCT()
struct FConcertReservation
{
    GENERATED_BODY()

public:
    FConcertReservation()
        : m_SeatId(TEXT("")), m_SeatInfo(TEXT("")) , m_SeatPrice(0), m_CompetitionRate(0), m_UserCode(TEXT("")),
          m_NeedCoin(0), m_UserName(TEXT("")), m_UserPhoneNumber(0), m_UserAddress(TEXT("")) {}

    const FString& GetSeatId() const { return m_SeatId; }
    void SetSeatId(const FString& SeatId) { m_SeatId = SeatId; }

    const FString& GetSeatInfo() const { return m_SeatInfo; }
    void SetSeatInfo(const FString& SeatInfo) { m_SeatInfo = SeatInfo; }

    int32 GetSeatPrice() const { return m_SeatPrice; }
    void SetSeatPrice(int32 SeatPrice) { m_SeatPrice = SeatPrice; }

    const TArray<FSeatIdDTO>& GetAvailableSeats() const { return m_AvailableSeats; }
    void SetAvailableSeats(const TArray<FSeatIdDTO>& AvailableSeats) { m_AvailableSeats = AvailableSeats; }

    const TArray<FSeatIdDTO>& GetReceptionSeats() const { return m_ReceptionSeats; }
    void SetReceptionSeats(const TArray<FSeatIdDTO>& ReceptionSeats) { m_ReceptionSeats = ReceptionSeats; }

    const TArray<FMyReceptionSeatInfoDTO>& GetMyReceptionSeats() const { return m_MyReceptionSeats; }
    void SetMyReceptionSeats(const TArray<FMyReceptionSeatInfoDTO>& MyReceptionSeats) { m_MyReceptionSeats = MyReceptionSeats; }

    int32 GetCompetitionRate() const { return m_CompetitionRate; }
    void SetCompetitionRate(int32 CompetitionRate) { m_CompetitionRate = CompetitionRate; }

    const FString& GetUserCode() const { return m_UserCode; }
    void SetUserCode(const FString& UserCode) { m_UserCode = UserCode; }

    int32 GetNeedCoin() const { return m_NeedCoin; }
    void SetNeedCoin(int32 NeedCoin) { m_NeedCoin = NeedCoin; }

    const FString& GetUserName() const { return m_UserName; }
    void SetUserName(const FString& UserName) { m_UserName = UserName; }

    int32 GetUserPhoneNumber() const { return m_UserPhoneNumber; }
    void SetUserPhoneNumber(int32 UserPhoneNumber) { m_UserPhoneNumber = UserPhoneNumber; }

    const FString& GetUserAddress() const { return m_UserAddress; }
    void SetUserAddress(const FString& UserAddress) { m_UserAddress = UserAddress; }

    void SetReservationInfo(
        const FString& SeatId,
        FString& SeatInfo,
        int32 SeatPrice,
        const TArray<FSeatIdDTO>& AvailableSeats,
        const TArray<FSeatIdDTO>& ReceptionSeats,
        const TArray<FMyReceptionSeatInfoDTO>& MyReceptionSeats,
        int32 CompetitionRate,
        const FString& UserCode,
        int32 NeedCoin,
        const FString& UserName,
        int32 UserPhoneNumber,
        const FString& UserAddress)
    {
        m_SeatId = SeatId;
        m_SeatInfo = SeatInfo;
        m_SeatPrice = SeatPrice;
        m_AvailableSeats = AvailableSeats;
        m_ReceptionSeats = ReceptionSeats;
        m_MyReceptionSeats = MyReceptionSeats;
        m_CompetitionRate = CompetitionRate;
        m_UserCode = UserCode;
        m_NeedCoin = NeedCoin;
        m_UserName = UserName;
        m_UserPhoneNumber = UserPhoneNumber;
        m_UserAddress = UserAddress;
    }

private:
    UPROPERTY(VisibleAnywhere, Category = "Default|Reservation|Seat")
    FString m_SeatId;
    
    UPROPERTY(VisibleAnywhere, Category = "Default|Reservation|Seat")
    FString m_SeatInfo;

    UPROPERTY(VisibleAnywhere, Category = "Default|Reservation|Seat")
    int32 m_SeatPrice;

    UPROPERTY(VisibleAnywhere, Category = "Default|Reservation|Seat")
    TArray<FSeatIdDTO> m_AvailableSeats;

    UPROPERTY(VisibleAnywhere, Category = "Default|Reservation|Seat")
    TArray<FSeatIdDTO> m_ReceptionSeats;
    
    UPROPERTY(VisibleAnywhere, Category = "Default|Reservation|Seat")
    TArray<FMyReceptionSeatInfoDTO> m_MyReceptionSeats;

    UPROPERTY(VisibleAnywhere, Category = "Default|Reservation|System")
    int32 m_CompetitionRate;

    UPROPERTY(VisibleAnywhere, Category = "Default|Reservation|System")
    FString m_UserCode;

    UPROPERTY(VisibleAnywhere, Category = "Default|Reservation|System")
    int32 m_NeedCoin;

    UPROPERTY(VisibleAnywhere, Category = "Default|Reservation|User")
    FString m_UserName;

    UPROPERTY(VisibleAnywhere, Category = "Default|Reservation|User")
    int32 m_UserPhoneNumber;

    UPROPERTY(VisibleAnywhere, Category = "Default|Reservation|User")
    FString m_UserAddress;
};

UCLASS()
class MTVS3_FINAL_API AHM_HttpActor2 : public AActor
{
	GENERATED_BODY()

private:
    FConcertDTO m_Concert; // 콘서트 정보를 저장할 변수
    FConcertReservation m_ConcertReservation; // 콘서트 예약 정보를 저장할 변수

public:
    // FConcert 관련 메서드
    const FString& GetConcertName() const { return m_Concert.GetConcertName(); }
    int32 GetConcertYear() const { return m_Concert.GetConcertYear(); }
    int32 GetConcertMonth() const { return m_Concert.GetConcertMonth(); }
    int32 GetConcertDay() const { return m_Concert.GetConcertDay(); }
    const FString& GetConcertTime() const { return m_Concert.GetConcertTime(); }

    void SetConcertInfo(const FString& Name , int32 Year , int32 Month , int32 Day , const FString& Time)
    {
        m_Concert.SetConcertName(Name);
        m_Concert.SetConcertYear(Year);
        m_Concert.SetConcertMonth(Month);
        m_Concert.SetConcertDay(Day);
        m_Concert.SetConcertTime(Time);
    }

    // FConcertReservation 관련 메서드
    const FString& GetSeatId() const { return m_ConcertReservation.GetSeatId(); }
    void SetSeatId(const FString& SeatId) { m_ConcertReservation.SetSeatId(SeatId); }

    const FString& GetSeatInfo() const { return m_ConcertReservation.GetSeatInfo(); }
    void SetSeatInfo(const FString& SeatInfo) { m_ConcertReservation.SetSeatId(SeatInfo); }

    int32 GetSeatPrice() const { return m_ConcertReservation.GetSeatPrice(); }
    void SetSeatPrice(int32 SeatPrice) { m_ConcertReservation.SetSeatPrice(SeatPrice); }

    const TArray<FSeatIdDTO>& GetAvailableSeats() const { return m_ConcertReservation.GetAvailableSeats(); }
    void SetAvailableSeats(const TArray<FSeatIdDTO>& AvailableSeats) { m_ConcertReservation.SetAvailableSeats(AvailableSeats); }

    const TArray<FSeatIdDTO>& GetReceptionSeats() const { return m_ConcertReservation.GetReceptionSeats(); }
    void SetReceptionSeats(const TArray<FSeatIdDTO>& ReceptionSeats) { m_ConcertReservation.SetReceptionSeats(ReceptionSeats); }

    const TArray<FMyReceptionSeatInfoDTO>& GetMyReceptionSeats() const { return m_ConcertReservation.GetMyReceptionSeats(); }
    void SetMyReceptionSeats(const TArray<FMyReceptionSeatInfoDTO>& MyReceptionSeats) { m_ConcertReservation.SetMyReceptionSeats(MyReceptionSeats); }

    int32 GetCompetitionRate() const { return m_ConcertReservation.GetCompetitionRate(); }
    void SetCompetitionRate(int32 CompetitionRate) { m_ConcertReservation.SetCompetitionRate(CompetitionRate); }

    const FString& GetUserCode() const { return m_ConcertReservation.GetUserCode(); }
    void SetUserCode(const FString& UserCode) { m_ConcertReservation.SetUserCode(UserCode); }

    int32 GetNeedCoin() const { return m_ConcertReservation.GetNeedCoin(); }
    void SetNeedCoin(int32 NeedCoin) { m_ConcertReservation.SetNeedCoin(NeedCoin); }

    const FString& GetUserName() const { return m_ConcertReservation.GetUserName(); }
    void SetUserName(const FString& UserName) { m_ConcertReservation.SetUserName(UserName); }

    int32 GetUserPhoneNumber() const { return m_ConcertReservation.GetUserPhoneNumber(); }
    void SetUserPhoneNumber(int32 UserPhoneNumber) { m_ConcertReservation.SetUserPhoneNumber(UserPhoneNumber); }

    const FString& GetUserAddress() const { return m_ConcertReservation.GetUserAddress(); }
    void SetUserAddress(const FString& UserAddress) { m_ConcertReservation.SetUserAddress(UserAddress); }

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

	UPROPERTY()
	class UMH_BuyTicketWidget* BuyTicketUI;
    void SetBuyTicketUI(UMH_BuyTicketWidget* InBuyTicketUI);

#pragma endregion

	// 백엔드에 요청 보낼 때만 api 포함, 프론트는 api X
	const FString _url = "https://ticketaka.shop/api";

//===========================================================================================================

	// 공연장 선택 UI 요청
    void ReqGetConcertInfo(FString AccessToken);

    // 공연장 선택 UI 요청에 대한 응답
    void ResGetConcertInfo(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful);
    
    // 공연장 입장 요청
	void ReqPostConcertEntry(FString ConcertName , FString AccessToken);

	// 공연장 입장 요청에 대한 응답
	void OnResPostConcertEntry(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful);

//===========================================================================================================

	// 좌석 조회 요청
	void ReqPostSeatRegistrationInquiry(FString ConcertName , FString SeatId , FString AccessToken);

	// 좌석 조회 요청에 대한 응답
	void OnResPostSeatRegistrationInquiry(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful);

	// 좌석 접수 요청
	void ReqPostRegisterSeat(FString ConcertName , FString SeatId , FString AccessToken);

	// 좌석 접수 요청에 대한 응답
	void OnResPostRegisterSeat(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful);

    // 접수된 좌석 조회 요청
    void ReqPostCompletedRegisteredSeat(FString ConcertName , FString AccessToken);

    // 접수된 좌석 조회 요청에 대한 응답
    void OnResPostCompletedRegisteredSeat(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful);

	// 좌석 취소 요청
	void ReqDeleteCancelRegisteredSeat(FString ConcertName , FString SeatId , FString AccessToken);

	// 좌석 취소 요청에 대한 응답
	void OnResDeleteCancelRegisteredSeat(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful);

//===========================================================================================================

	// 좌석 게임 결과 요청
	void ReqPostGameResult(FString ConcertName , FString SeatId , FString AccessToken);

    // 좌석 게임 결과 요청에 대한 응답
    void OnResPostGameResult(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful);

	// 결제시 회원 인증용 QR 요청
	void ReqGetMemberAuthQR(FString AccessToken);

	// 결제시 회원 인증용 QR 요청에 대한 응답
	void OnResGetMemberAuthQR(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful);

	// 결제시 회원 인증 사진 업로드 확인
	void ReqGetPostConfirmMemberPhoto(FString UserCode , FString AccessToken);

	// 결제시 회원 인증 사진 업로드 확인에 대한 응답
	void OnResGetPostConfirmMemberPhoto(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful);

	// 예매자 정보 입력 요청
	void ReqPostReservationinfo(FString UserName, int32 UserPhoneNum , FString UserAddress , FString AccessToken);

	// 예매자 정보 입력 요청에 대한 응답
	void OnResPostReservationinfo(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful);

	// 좌석 결제 요청
	void ReqPostPaymentSeat(FString ConcertName, FString SeatId, FString AccessToken);

	// 좌석 결제 요청에 대한 응답
	void OnResPostPaymentSeat(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful);

};
