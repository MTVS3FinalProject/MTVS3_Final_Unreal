// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/HM_Tree.h"

#include "EngineUtils.h"
#include "HttpModule.h"
#include "ImageUtils.h"
#include "HJ/TTHallGameState.h"
#include "HJ/TTPlayer.h"
#include "Interfaces/IHttpResponse.h"
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
	
	// TicatVisibilities 배열 초기화
	TicatVisibilities.SetNum(15, false);
	
	bReplicates = true;
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
	if (TicketTreeId < 1 || TicketTreeId > Ticats.Num()) return;

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

void AHM_Tree::Client_ApplyTicketImage_Implementation(int32 TicketTreeId, const FString& TicketImgUrl)
{
	 APlayerController* PC = GetWorld()->GetFirstPlayerController();
	 if (PC && PC->IsLocalController())
	 {
	 	TActorIterator<ATTPlayer> It(GetWorld());
	 	ATTPlayer* TTPlayer = *It;
	 	if (TTPlayer && TTPlayer->IsLocallyControlled())
	 	{
	 		TTPlayer->SetOwner(PC);
	 		this->SetOwner(PC);
			Server_ApplyTicketImage(TicketTreeId, TicketImgUrl);
		}
	}
}

void AHM_Tree::Server_ApplyTicketImage_Implementation(int32 TicketTreeId, const FString& TicketImgUrl)
{
	if (!HasAuthority()) return;
	if (TicketTreeId < 1 || TicketTreeId > Ticats.Num()) return;

	int32 idx = TicketTreeId - 1;

	if (Ticats[idx] && !TicatVisibilities[idx]) // 비활성화된 Ticat만 처리
	{
		TicatVisibilities[idx] = true; // Replicated 변수 업데이트
		OnRep_TicatVisibility(); // 서버에서도 즉시 처리

		APlayerController* PC = GetWorld()->GetFirstPlayerController();
		if (PC && PC->IsLocalController())
		{
			TActorIterator<ATTPlayer> It(GetWorld());
			ATTPlayer* TTPlayer = *It;
			if (TTPlayer)
			{
				UE_LOG(LogTemp , Log , TEXT("TTPlayer->Multicast_ApplyTicketImage(i, TicketImgUrl);"));
				TTPlayer->Multicast_ApplyTicketImage(idx , TicketImgUrl);
			}
		}
	}
}

void AHM_Tree::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// 네트워크에 동기화
	DOREPLIFETIME(AHM_Tree, Ticats);
	DOREPLIFETIME(AHM_Tree, TicatVisibilities);
}

