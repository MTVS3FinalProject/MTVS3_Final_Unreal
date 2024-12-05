// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/HM_Tree.h"
#include "HttpModule.h"
#include "ImageUtils.h"
#include "Interfaces/IHttpResponse.h"

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
}

// Called when the game starts or when spawned
void AHM_Tree::BeginPlay()
{
	Super::BeginPlay();

	// for (int32 i = 0; i < 15; i++)
	// {
	// 	Ticats[i]->SetVisibility(false);
	// }
}

// Called every frame
void AHM_Tree::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AHM_Tree::InitializeTicketTabs(int32 TicketTreeId, const FString& TicketImg)
{
	// 유효성 검사 및 텍스처 다운로드 처리
	if (TicketTreeId < 0 || TicketTreeId >= Ticats.Num())
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid TicketTreeId: %d"), TicketTreeId);
		return;
	}

	UStaticMeshComponent* Ticat = Ticats[TicketTreeId-1];
	if (Ticat)
	{
		Ticat->SetVisibility(true);

		// URL에서 텍스처를 다운로드
		FHttpModule* Http = &FHttpModule::Get();
		if (!Http) return;

		TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = Http->CreateRequest();
		HttpRequest->OnProcessRequestComplete().BindLambda([Ticat](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
		{
			if (bWasSuccessful && Response.IsValid() && Response->GetResponseCode() == 200)
			{
				TArray<uint8> ImageData = Response->GetContent();
				UTexture2D* DownloadedTexture = nullptr;

				// 이미지 데이터에서 텍스처 생성
				DownloadedTexture = FImageUtils::ImportBufferAsTexture2D(ImageData);

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

void AHM_Tree::ApplyTicketImageFromUrl(const FString& TicketImgUrl)
{
	FHttpModule* Http = &FHttpModule::Get();
	if (!Http) return;

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = Http->CreateRequest();
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &AHM_Tree::OnTicketImageDownloaded);
	HttpRequest->SetURL(TicketImgUrl);
	HttpRequest->SetVerb("GET");
	HttpRequest->ProcessRequest();
}

void AHM_Tree::OnTicketImageDownloaded(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (bWasSuccessful && Response.IsValid() && Response->GetResponseCode() == 200)
	{
		TArray<uint8> ImageData = Response->GetContent();
		UTexture2D* DownloadedTexture = nullptr;

		// 이미지 데이터에서 텍스처 생성
		DownloadedTexture = FImageUtils::ImportBufferAsTexture2D(ImageData);
		if (DownloadedTexture)
		{
			for (UStaticMeshComponent* Ticat : Ticats)
			{
				if (Ticat && !Ticat->IsVisible())
				{
					Ticat->SetVisibility(true);
                    
					UMaterialInstanceDynamic* DynamicMaterial = Ticat->CreateAndSetMaterialInstanceDynamic(0);
					if (DynamicMaterial)
					{
						DynamicMaterial->SetTextureParameterValue(FName(TEXT("BaseTexture")), DownloadedTexture);
					}
					break; // 한 번만 적용
				}
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to download image or invalid response."));
	}
}

