#pragma once
// UE
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

// rclUE
#include <Actions/ROS2TeachingSession.h>
#include <ROS2ActionClient.h>
#include <ROS2Node.h>

// Ros2SandBox_UE
#include "ros2_sandbox.h"

#include "TeachingSessionActionClientNode.generated.h"

UCLASS()
class ROS2_SANDBOX_API ATeachingSessionActionClientNode : public AActor
{
    GENERATED_BODY()

public:
    ATeachingSessionActionClientNode();

    virtual void BeginPlay() override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UROS2NodeComponent* Node = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UROS2ActionClient* TeachingSessionActionClient = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString ActionName = TEXT("teaching_session");

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString SessionType = TEXT("POINT");

    UPROPERTY()
    FTimerHandle ActionTimerHandle;

    UFUNCTION()
    void FeedbackCallback(UROS2GenericAction* InAction);
    UFUNCTION()
    void ResultCallback(UROS2GenericAction* InAction);
    UFUNCTION()
    void GoalResponseCallback(UROS2GenericAction* InAction);
    UFUNCTION()
    void CancelCallback();

    UFUNCTION()
    void SendGoal();

private:
    FROSTeachingSessionFB FeedbackMsg;
    FROSTeachingSessionSGReq GoalRequest;
};