// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/HM_Tree.h"

#include "EngineUtils.h"
#include "HttpModule.h"
#include "ImageUtils.h"
#include "HJ/TTGameInstance.h"
#include "HJ/TTHallGameState.h"
#include "HJ/TTPlayer.h"
#include "Interfaces/IHttpResponse.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AHM_Tree::AHM_Tree()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	Tree = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Tree"));
	Tree->SetupAttachment(RootComponent);
	
	static ConstructorHelpers::FObjectFinder<UStaticMesh> TicatAsset(
		TEXT("/Game/Greek_island/Assets/Fish/Ticat1"));
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> TicatMaterial(
		TEXT("/Game/LHM/Material/M_Ticat"));
	if (TicatAsset.Succeeded() && TicatMaterial.Succeeded())
	{
		Ticats.SetNum(20);
		
		for (int32 i = 0; i < 20; i++)
		{
			FName TicatName = *FString::Printf(TEXT("Ticat_%d"), i + 1);
			UStaticMeshComponent* TicatComp = CreateDefaultSubobject<UStaticMeshComponent>(TicatName);
			Ticats[i] = TicatComp;
			Ticats[i]->SetVisibility(false);
			
			if (TicatComp)
			{
				TicatComp->SetupAttachment(Tree);
				TicatComp->SetStaticMesh(TicatAsset.Object);
				TicatComp->SetMaterial(0, TicatMaterial.Object);
				TicatComp->SetRelativeRotation(FRotator(90, 180, 90));

				//// 2줄 배치 계산
				//int32 Row = i / 10; // 0 또는 1
				//int32 Column = i % 10; // 0부터 9까지

				// 위치 계산 (20 간격으로 배치)
				//FVector RelativeLocation = FVector(0.0f, Column * 100.0f, Row * 100.0f);
				//TicatComp->SetRelativeLocation(RelativeLocation);
			}
		}
	}
	
	// TicatVisibilities 배열 초기화
	TicatVisibilities.SetNum(20, false);
	
	bReplicates = true;
}

// Called when the game starts or when spawned
void AHM_Tree::BeginPlay()
{
	Super::BeginPlay();

	if (ATTHallGameState* HallState = GetWorld()->GetGameState<ATTHallGameState>())
	{
		HallState->OnTicketImageUpdated.AddDynamic(this, &AHM_Tree::ApplyTicketImage);
	}
}

// Called every frame
void AHM_Tree::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AHM_Tree::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// 네트워크에 동기화
	DOREPLIFETIME(AHM_Tree, Ticats);
	DOREPLIFETIME(AHM_Tree, TicatVisibilities);
}

void AHM_Tree::OnRep_TicatVisibility()
{
	for (int32 i = 0; i < Ticats.Num(); i++)
	{
		if (Ticats[i])
		{
			Ticats[i]->SetVisibility(TicatVisibilities[i]); // Replicated 상태로 동기화
		}
	}
	UE_LOG(LogTemp, Log, TEXT("OnRep_TicatVisibility - Visibility updated"));
}

void AHM_Tree::InitializeTicketTabs(int32 TicketTreeId, const FString& TicketImg)
{
	if (TicketTreeId < 1 || TicketTreeId > Ticats.Num()) return;

	UStaticMeshComponent* Ticat = Ticats[TicketTreeId-1];
	if (Ticat)
	{
		if (HasAuthority())
		{
			TicatVisibilities[TicketTreeId-1] = true; // 서버에서만 변경
			OnRep_TicatVisibility(); // 클라이언트 동기화
		}
		
		FHttpModule* Http = &FHttpModule::Get();
		if (!Http) return;

		TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = Http->CreateRequest();
		HttpRequest->OnProcessRequestComplete().BindLambda([Ticat](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
		{
			if (bWasSuccessful && Response.IsValid() && Response->GetResponseCode() == 200)
			{
				TArray<uint8> ImageData = Response->GetContent();
				UTexture2D* DownloadedTexture = FImageUtils::ImportBufferAsTexture2D(ImageData);

				if (DownloadedTexture)
				{
					UMaterialInstanceDynamic* DynamicMaterial = Ticat->CreateAndSetMaterialInstanceDynamic(0);
					if (DynamicMaterial)
					{
						DynamicMaterial->SetTextureParameterValue(FName(TEXT("BaseTexture")), DownloadedTexture);
					}
				}
			}
		});

		HttpRequest->SetURL(TicketImg);
		HttpRequest->SetVerb("GET");
		HttpRequest->ProcessRequest();
	}
}

void AHM_Tree::ApplyTicketImage(int32 TicketTreeId, FString TicketImgUrl)
{
	if (TicketTreeId < 1 || TicketTreeId > Ticats.Num()) return;
	int32 idx = TicketTreeId - 1;

	if (HasAuthority())
	{
		TicatVisibilities[idx] = true; // 서버에서만 변경
		OnRep_TicatVisibility(); // 클라이언트 동기화
	}

	// 이미지 다운로드 및 적용
	FHttpModule* Http = &FHttpModule::Get();
	if (!Http) return;

	TSharedRef<IHttpRequest , ESPMode::ThreadSafe> HttpRequest = Http->CreateRequest();
	HttpRequest->OnProcessRequestComplete().BindLambda(
		[this, idx](FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful)
		{
			if (bWasSuccessful && Response.IsValid() && Response->GetResponseCode() == 200)
			{
				TArray<uint8> ImageData = Response->GetContent();
				UTexture2D* DownloadedTexture = FImageUtils::ImportBufferAsTexture2D(ImageData);

				if (DownloadedTexture)
				{
					UMaterialInstanceDynamic* DynamicMaterial = Ticats[idx]->CreateAndSetMaterialInstanceDynamic(0);
					if (DynamicMaterial)
					{
						DynamicMaterial->SetTextureParameterValue(FName(TEXT("BaseTexture")) , DownloadedTexture);
					}
				}
			}
		});

	HttpRequest->SetURL(TicketImgUrl);
	HttpRequest->SetVerb("GET");
	HttpRequest->ProcessRequest();
}

