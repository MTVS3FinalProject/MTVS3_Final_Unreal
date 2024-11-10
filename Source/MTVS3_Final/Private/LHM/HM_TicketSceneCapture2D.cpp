// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/HM_TicketSceneCapture2D.h"

#include "Components/SceneCaptureComponent2D.h"
#include "Components/WidgetComponent.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "LHM/HM_FinalTicket.h"
#include "LHM/HM_TicketCustom.h"

// Sets default values
AHM_TicketSceneCapture2D::AHM_TicketSceneCapture2D()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// RootComponent 설정
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	// SceneCaptureComponent2D 생성 및 설정
	SceneCaptureComponent2D = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("SceneCaptureComponent2D"));
	SceneCaptureComponent2D->SetupAttachment(RootComponent);

	// WidgetComponent 생성 및 설정
	TicketCustomComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("TicketCustomComp"));
	TicketCustomComp->SetupAttachment(RootComponent);
	TicketCustomComp->SetDrawSize(FVector2D(1920, 1080));
	TicketCustomComp->SetVisibility(true);
	
}

// Called when the game starts or when spawned
void AHM_TicketSceneCapture2D::BeginPlay()
{
	Super::BeginPlay();

	if (TicketCutomWidget)
	{
		TicketCutomUI = CreateWidget<UHM_TicketCustom>(GetWorld(), TicketCutomWidget);
		TicketCustomComp->SetWidget(TicketCutomUI);
	}

	if (FinalTicketWidget)
	{
		FinalTicketUI = CreateWidget<UHM_FinalTicket>(GetWorld(), FinalTicketWidget);
	}
}

// Called every frame
void AHM_TicketSceneCapture2D::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
void AHM_TicketSceneCapture2D::CaptureUsingSceneCapture()
{
	if (!SceneCaptureComponent2D || !FinalTicketUI->Img_FinalTicket) return;

	// 렌더 타겟 생성 및 설정
	UTextureRenderTarget2D* RenderTarget = NewObject<UTextureRenderTarget2D>();
	RenderTarget->InitCustomFormat(1920, 1080, PF_B8G8R8A8, true);  // 해상도 설정
	//RenderTarget->ClearColor = FLinearColor::Transparent;  // 투명 배경 설정
	RenderTarget->UpdateResource();

	// SceneCaptureComponent2D 설정
	SceneCaptureComponent2D->TextureTarget = RenderTarget;
	
	// 씬 캡처 (위젯 포함된 UI 캡처)
	SceneCaptureComponent2D->CaptureScene();  // 씬을 캡처하여 RenderTarget에 저장

	// Render Target을 UTexture2D로 변환하여 Img_FinalTicket에 적용
	UTexture2D* CapturedTexture = ConvertRenderTargetToTexture(this, RenderTarget);
	if (CapturedTexture)
	{
		CapturedTexture->UpdateResource();  // 리소스 업데이트

		UE_LOG(LogTemp, Log, TEXT("Captured texture info - Width: %d, Height: %d"),
		CapturedTexture->GetSizeX(), CapturedTexture->GetSizeY());
		
		FSlateBrush Brush;
		Brush.SetResourceObject(CapturedTexture);
		Brush.ImageSize = FVector2D(1920, 1080);
		FinalTicketUI->Img_FinalTicket->SetBrush(Brush);

		// // 브러시 적용 후 딜레이를 사용하여 이미지 갱신
		// FTimerHandle TimerHandle;
		// GetWorld()->GetTimerManager().SetTimer(
		// 	TimerHandle,
		// 	[this, Brush]()
		// 	{
		// 		if (FinalTicketUI->Img_FinalTicket)
		// 		{
		// 			FinalTicketUI->Img_FinalTicket->SetBrush(Brush);
		// 			FinalTicketUI->Img_FinalTicket->SetVisibility(ESlateVisibility::Visible);
		// 			UE_LOG(LogTemp, Log, TEXT("Img_FinalTicket updated with captured texture."));
		// 		}
		// 	},
		// 	1.f,  // 1초 지연
		// 	false
		// );
		
		UE_LOG(LogTemp, Log, TEXT("SceneCapture2D capture completed successfully."));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to convert RenderTarget to Texture2D"));
	}

	if (FinalTicketUI->Img_FinalTicket->GetBrush().GetResourceObject() == CapturedTexture)
	{
		UE_LOG(LogTemp, Log, TEXT("Captured texture successfully applied to Img_FinalTicket brush."));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to apply captured texture to Img_FinalTicket brush."));
	}
}

UTexture2D* AHM_TicketSceneCapture2D::ConvertRenderTargetToTexture(UObject* WorldContextObject,
														  UTextureRenderTarget2D* RenderTarget)
{
	if (!RenderTarget) return nullptr;

	UTexture2D* NewTexture = NewObject<UTexture2D>(WorldContextObject);
	UKismetRenderingLibrary::ConvertRenderTargetToTexture2DEditorOnly(WorldContextObject, RenderTarget, NewTexture);

	if (!NewTexture)
	{
		UE_LOG(LogTemp, Warning, TEXT("ConvertRenderTargetToTexture2D failed to create texture"));
	}
	
	return NewTexture;
}