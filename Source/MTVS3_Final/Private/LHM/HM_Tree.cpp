// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/HM_Tree.h"

#include "HttpModule.h"
#include "ImageUtils.h"
#include "NiagaraSystem.h"
#include "HJ/TTHallGameState.h"
#include "Interfaces/IHttpResponse.h"
#include "Net/UnrealNetwork.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"

// Sets default values
AHM_Tree::AHM_Tree()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	Tree = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Tree"));
	Tree->SetupAttachment(RootComponent);
	Tree->SetRelativeScale3D(FVector3d(0.8f));
	
	static ConstructorHelpers::FObjectFinder<UStaticMesh> TicatAsset(
		TEXT("/Game/KJM/Assets/Object/CM_Ticat"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> TicatClipAsset(
		TEXT("/Game/KJM/Assets/Object/CM_Ticat_Clip"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> PhysicsParentAsset(
		TEXT("/Game/KHJ/Assets/SM_BoxBrush"));
	FSoftObjectPath NiagaraPath(TEXT("/Game/LHM/Effects/MegaMagicVFXBundle/N_LightBlastCharged"));
	UNiagaraSystem* NiagaraSystem = Cast<UNiagaraSystem>(NiagaraPath.TryLoad());
	
	if (TicatAsset.Succeeded() && TicatClipAsset.Succeeded() && PhysicsParentAsset.Succeeded() && NiagaraSystem)
	{
		Ticats.SetNum(40);
		TicatClips.SetNum(40);
		PhysicsConstraints.SetNum(40);
		PhysicsParents.SetNum(40);
		NiagaraEffects.SetNum(40);
		
		for (int32 i = 0; i < 40; i++)
		{
			FName TicatName = *FString::Printf(TEXT("Ticat_%d"), i + 1);
			FName TicatClipName = *FString::Printf(TEXT("TicatClip_%d"), i + 1);
			FName PhysicsConstName = *FString::Printf(TEXT("Physics_%d"), i + 1);
			FName PhysicsParentName = *FString::Printf(TEXT("PhysicsParent_%d"), i + 1);
			UStaticMeshComponent* TicatComp = CreateDefaultSubobject<UStaticMeshComponent>(TicatName);
			UStaticMeshComponent* TicatClipComp = CreateDefaultSubobject<UStaticMeshComponent>(TicatClipName);
			UPhysicsConstraintComponent* PhysicsComp = CreateDefaultSubobject<UPhysicsConstraintComponent>(PhysicsConstName);
			UStaticMeshComponent* PhysicsParentComp = CreateDefaultSubobject<UStaticMeshComponent>(PhysicsParentName);

			Ticats[i] = TicatComp;
			Ticats[i]->SetVisibility(false);
			
			TicatClips[i] = TicatClipComp;
			TicatClips[i]->SetVisibility(false);
			
			PhysicsConstraints[i] = PhysicsComp;
			PhysicsConstraints[i]->SetVisibility(true);
			
			PhysicsParents[i] = PhysicsParentComp;
			PhysicsParents[i]->SetVisibility(false);
			
			if (Ticats[i] && TicatClips[i] && PhysicsConstraints[i] && PhysicsParents[i])
			{
				PhysicsConstraints[i]->SetupAttachment(Tree);
				PhysicsConstraints[i]->SetConstrainedComponents(PhysicsParentComp, NAME_None, TicatClipComp, NAME_None);
				PhysicsConstraints[i]->SetRelativeScale3D(FVector3d(3.f));
			
				PhysicsParents[i]->SetupAttachment(PhysicsComp);
				PhysicsParents[i]->SetStaticMesh(PhysicsParentAsset.Object);
				PhysicsParents[i]->SetRelativeScale3D(FVector3d(0.01f));
				PhysicsParents[i]->SetRelativeLocation(FVector3d(0,0,1));
				
				
				TicatClips[i]->SetupAttachment(PhysicsComp);
				TicatClips[i]->SetStaticMesh(TicatClipAsset.Object);
				
				Ticats[i]->SetupAttachment(TicatClipComp);
				Ticats[i]->SetStaticMesh(TicatAsset.Object);
			}

			// Niagara
			FName NiagaraName = *FString::Printf(TEXT("Niagara_%d"), i + 1);
			UNiagaraComponent* NiagaraComp = CreateDefaultSubobject<UNiagaraComponent>(NiagaraName);
			NiagaraEffects[i] = NiagaraComp;
			if(NiagaraEffects[i])
			{
				NiagaraEffects[i]->SetupAttachment(TicatComp);
				NiagaraEffects[i]->SetAsset(NiagaraSystem);
				NiagaraEffects[i]->SetRelativeLocation(FVector(0, 0, -50));
				NiagaraEffects[i]->SetRelativeScale3D(FVector3d(0.1f));
				NiagaraEffects[i]->SetAutoActivate(false);
			}
		}
	}
	
	// 배열 초기화
	TicatClipVisibilities.SetNum(40, false);
	TicatVisibilities.SetNum(40, false);
	
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

	for (auto* TicatClip : TicatClips)
	{
		if (TicatClip)
		{
			TicatClip->SetSimulatePhysics(true);
			TicatClip->SetEnableGravity(true);
			TicatClip->SetMassScale(NAME_None, 100);
		}
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
	DOREPLIFETIME(AHM_Tree, TicatClips);
	DOREPLIFETIME(AHM_Tree, TicatVisibilities);
	DOREPLIFETIME(AHM_Tree, TicatClipVisibilities);
}

void AHM_Tree::OnRep_TicatVisibility()
{
	for (int32 i = 0; i < Ticats.Num(); i++)
	{
		if (Ticats[i] && TicatClips[i])
		{
			TicatClips[i]->SetVisibility(TicatClipVisibilities[i]); // Replicated 상태로 동기화
			Ticats[i]->SetVisibility(TicatVisibilities[i]); // Replicated 상태로 동기화
		}
	}
}

void AHM_Tree::InitializeTicketTabs(int32 TicketTreeId, const FString& TicketImg)
{
	if (TicketTreeId < 1 || TicketTreeId > Ticats.Num()) return;

	UStaticMeshComponent* TicatClip = TicatClips[TicketTreeId-1];
	UStaticMeshComponent* Ticat = Ticats[TicketTreeId-1];
	if (Ticat && TicatClip)
	{
		if (HasAuthority())
		{
			TicatClipVisibilities[TicketTreeId-1] = true; // 서버에서만 변경
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
						DynamicMaterial->SetTextureParameterValue(FName(TEXT("BaseTexture")) , DownloadedTexture);
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
		TicatClipVisibilities[idx] = true; // 서버에서만 변경
		TicatVisibilities[idx] = true; // 서버에서만 변경
		OnRep_TicatVisibility(); // 클라이언트 동기화
	}

	NiagaraEffects[idx]->Activate(true);

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

						// UV 스케일 및 오프셋 조정
						DynamicMaterial->SetScalarParameterValue(FName(TEXT("UVScaleX")) , 1.0f);
						DynamicMaterial->SetScalarParameterValue(FName(TEXT("UVScaleY")) , 505.0f / 888.0f);
						// 텍스처 비율 유지
						DynamicMaterial->SetScalarParameterValue(FName(TEXT("UVOffsetX")) , 0.0f);
						DynamicMaterial->SetScalarParameterValue(FName(TEXT("UVOffsetY")) , 0.0f);
					}
				}
			}
		});

	HttpRequest->SetURL(TicketImgUrl);
	HttpRequest->SetVerb("GET");
	HttpRequest->ProcessRequest();
}

