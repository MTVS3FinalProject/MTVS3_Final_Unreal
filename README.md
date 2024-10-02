# MTVS3_Final_Unreal

## (1/3) Issue Convention
### ✅기능 이슈
* 이슈 작성 후 작업 진행 바랍니다.
* 체크리스트에 체크, 완료되면 Close 해주세요.<br>
> [Feat] HJ_UI<br>
> * Description<br>
> WBP 제작 및 코드 작성<br>
> * To Do<br>
> 시작 UI<br>
> 엔딩 UI
### 🙏요청 사항
* 제목에 ```[요청 대상] 이슈 이름```을 작성
* ```요청 대상```은 이슈 확인 시 Comment를 남긴 후 체크리스트에 체크, 완료되면 Close 해주세요.
> [희진] 에셋 위치 조정 필요<br>
> * Description<br>
> PSH 폴더 B2_Map을 참고하시고 에셋 크기와 위치를 맞춰주시면 감사하겠습니다.<br>
> * To Do<br>
> 키패드 크기 줄이기<br>
> 컬러 다이얼 크기 키우기

## (2/3) Commit Message Convention
> 브랜치명: 이니셜_이름 (예: ```KHJ_Proto```)<br>
> 작은 변경사항이라도 꼭 중간마다 커밋, 푸쉬 부탁드립니다.
* ```Feat``` : 새로운 기능을 추가할 경우
* ```Fix``` : 버그를 고친 경우
* ```!HOTFIX``` : 치명적인 버그를 고친 경우
* ```Refactor``` : 리팩토링
* ```Comment``` : 주석 추가 및 변경
* ```Docs``` : 문서를 수정한 경우
* ```Chore``` : 자잘한 수정에 대한 커밋
* ```Rename``` : 파일 혹은 폴더명을 수정하거나 옮기는 작업만인 경우
* ```Remove``` : 파일을 삭제하는 경우
* ```Style``` : 코드 스타일 혹은 포맷 등에 관한 커밋
> [Feat] 화면 비율 960*540<br>
> [Fix] 효과음 조정, 엔딩 컷신 종료 시 화면 멈추기<br>
> [Chore] 캐릭터 크기 1.15배<br>

## (3/3) Naming Convention
#### 1. 폴더명
* 언리얼 Content 폴더 내에 각자 ```이니셜``` 폴더를 만들어 작업 진행
  * 폴더 이름은 ```PascalCase```, 공백과 유니코드 문자 및 기타 기호 사용 금지
  * 맵 파일은 Map 폴더 만들어 저장
* C++ 클래스 이름 설정 ```이니셜_클래스 이름```
* C++ 클래스 이니셜 폴더에 저장
  ```
  Class Type: Public
  Name: HJ_Actor
  Path: C:/Projects/Unreal/MTVS3_Final_Unreal/MTVS3_Final/Source/MTVS3_Final/Public/HJ
  ```
  
#### 2. 블루프린트 이름

|Prefix/Suffix|내용|예시|
|------|---|---|
|BP_|Blueprint|BP_Player|
|WBP_|Widget Blueprint|WBP_Start|
|ABP_|Animation Blueprint|ABP_Player|
|AM_|Animation Montage|AM_Die|
|A_|Animation Sequence|A_Run|
|BS_|Blend Space|BS_Move|
|LS_|Level Sequence|LS_Ending|
|M_|Material|M_Transparent|
|SPR_|Sprite|SPR_Arrow|
|A_|Sound Wave|A_Enter|
|A_Cue|Sound Cue|A_Enter_Cue|
|ATT_|Sound Attenuation|ATT_HJ|
|_SC|Sound Concurrency|HJ_SC|
|Font_|Font|Font_Noto_Sans|

#### 3. CPP 변수명
  * 기본적으로 파스칼 케이스 사용 (예: ```Score```)
  * 부울 변수는 ```b```로 시작 (예: ```bPendingDestruction, bHasFadedIn```)
  * 열거형은 ```E```로 시작 (예: ```enum class EColorBits { ECB_Red, ECB_Green, ECB_Blue };```)

#### 4. CPP 함수명
* Bool을 반환하는 정보 함수는 질문을 해야 합니다.
  * 즉, 동사형태로 Is, Was, Can 등
* RPC호출에는 Target에 대한 접두사가 붙어야 한다.
  ```
  UFUNCTION(Server, Unreliable)
  void ServerLook(FVector2D Axis);

  UFUNCTION(NetMulticast, Unreliable)
  void MulticastLook(FVector2D Axis);
  ```
* 이벤트 핸들러와 디스패처는 다음으로 시작해야 한다. ```On```
  * 예: OnDeath, OnRespawn, OnJump, OnFire, OnColorChange
  ```
  UPROPERTY(ReplicatedUsing = OnRep_GuestNickname , BlueprintReadOnly , Category = Authentication)
  FString GuestNickname;

  UFUNCTION()
  void OnRep_GuestNickname();
  ```
