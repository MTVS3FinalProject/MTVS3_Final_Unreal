// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/HM_Tree.h"

#include "EngineUtils.h"
#include "HttpModule.h"
#include "ImageUtils.h"
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
	if (TicatAsset.Succeeded())
	{
		Ticats.SetNum(15);
		for (int32 i = 0; i < 15; i++)
		{
			FName TicatName = *FString::Printf(TEXT("Ticat_%d"), i + 1);
			UStaticMeshComponent* TicatComp = CreateDefaultSubobject<UStaticMeshComponent>(TicatName);
			Ticats[i] = TicatComp;
			Ticats[i]->SetVisibility(false);
			
			if (TicatComp)
			{
				TicatComp->SetupAttachment(Tree);
				TicatComp->SetStaticMesh(TicatAsset.Object);
				TicatComp->SetRelativeRotation(FRotator(90, 180, 90));
			}
		}
	}
	
	// Ticats 배열 초기화
	TicatVisibilities.SetNum(15, false);
}

// Called when the game starts or when spawned
void AHM_Tree::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AHM_Tree::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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
}

void AHM_Tree::InitializeTicketTabs(int32 TicketTreeId, const FString& TicketImg)
{
	if (TicketTreeId < 0 || TicketTreeId >= Ticats.Num())
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid TicketTreeId: %d"), TicketTreeId);
		return;
	}

	UStaticMeshComponent* Ticat = Ticats[TicketTreeId-1];
	if (Ticat)
	{
		Ticat->SetVisibility(true);

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

void AHM_Tree::Server_ApplyTicketImage_Implementation(const FString& TicketImgUrl)
{
	for (int32 i = 0; i < Ticats.Num(); i++)
	{
		if (Ticats[i] && !TicatVisibilities[i]) // 비활성화된 Ticat만 처리
		{
			TicatVisibilities[i] = true; // Replicated 변수 업데이트
			OnRep_TicatVisibility(); // 서버에서도 즉시 처리
			
			for (APlayerController* PC : TActorRange<APlayerController>(GetWorld()))
			{
				if (ATTPlayer* TTPlayer = Cast<ATTPlayer>(PC->GetPawn()))
				{
					TTPlayer->Multicast_ApplyTicketImage(TicketImgUrl);
				}
			}
			break;
		}
	}
}

void AHM_Tree::Server_RequestInitializeTicketTabs_Implementation(int32 TicketTreeId, const FString& TicketImg)
{
	if (TicketTreeId < 0 || TicketTreeId >= Ticats.Num())
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid TicketTreeId: %d"), TicketTreeId);
		return;
	}
	
	for (APlayerController* PC : TActorRange<APlayerController>(GetWorld()))
	{
		if (ATTPlayer* TTPlayer = Cast<ATTPlayer>(PC->GetPawn()))
		{
			TTPlayer->Multicast_InitializeTicketTabs(TicketTreeId, TicketImg);
			UE_LOG(LogTemp , Log , TEXT("Player->Multicast_InitializeTicketTabs"));
		}
	}
}

// void AHM_Tree::ApplyTicketImageFromUrl(const FString& TicketImgUrl)
// {
// 	FHttpModule* Http = &FHttpModule::Get();
// 	if (!Http) return;
//
// 	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = Http->CreateRequest();
// 	HttpRequest->OnProcessRequestComplete().BindUObject(this, &AHM_Tree::OnTicketImageDownloaded);
// 	HttpRequest->SetURL(TicketImgUrl);
// 	HttpRequest->SetVerb("GET");
// 	HttpRequest->ProcessRequest();
// }
//
// void AHM_Tree::OnTicketImageDownloaded(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
// {
// 	if (bWasSuccessful && Response.IsValid() && Response->GetResponseCode() == 200)
// 	{
// 		TArray<uint8> ImageData = Response->GetContent();
// 		UTexture2D* DownloadedTexture = nullptr;
//
// 		// 이미지 데이터에서 텍스처 생성
// 		DownloadedTexture = FImageUtils::ImportBufferAsTexture2D(ImageData);
// 		if (DownloadedTexture)
// 		{
// 			for (UStaticMeshComponent* Ticat : Ticats)
// 			{
// 				if (Ticat && !Ticat->IsVisible())
// 				{
// 					Ticat->SetVisibility(true);
//                     
// 					UMaterialInstanceDynamic* DynamicMaterial = Ticat->CreateAndSetMaterialInstanceDynamic(0);
// 					if (DynamicMaterial)
// 					{
// 						DynamicMaterial->SetTextureParameterValue(FName(TEXT("BaseTexture")), DownloadedTexture);
// 					}
// 					break; // 한 번만 적용
// 				}
// 			}
// 		}
// 	}
// 	else
// 	{
// 		UE_LOG(LogTemp, Warning, TEXT("Failed to download image or invalid response."));
// 	}
// }

void AHM_Tree::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// 네트워크에 동기화
	DOREPLIFETIME(AHM_Tree, Ticats);
	DOREPLIFETIME(AHM_Tree, TicatVisibilities);
}

