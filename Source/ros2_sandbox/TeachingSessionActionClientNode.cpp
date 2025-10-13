// Copyright 2020-2021 Rapyuta Robotics Co., Ltd.

#include "TeachingSessionActionClientNode.h"

#include "Kismet/GameplayStatics.h"
#include "rclcUtilities.h"

ATeachingSessionActionClientNode::ATeachingSessionActionClientNode()
{
    Node = CreateDefaultSubobject<UROS2NodeComponent>(TEXT("ROS2NodeComponent"));

    // these parameters can be change from BP
    Node->Name = TEXT("teaching_session_action_client_node");
    Node->Namespace = TEXT("cpp");
}

void ATeachingSessionActionClientNode::BeginPlay()
{
    Super::BeginPlay();
    Node->Init();

    ROS2_CREATE_ACTION_CLIENT(Node,
                              this,
                              ActionName,
                              UROS2TeachingSessionAction::StaticClass(),
                              &ATeachingSessionActionClientNode::GoalResponseCallback,
                              &ATeachingSessionActionClientNode::ResultCallback,
                              &ATeachingSessionActionClientNode::FeedbackCallback,
                              &ATeachingSessionActionClientNode::CancelCallback,
                              TeachingSessionActionClient);

    SendGoal();
}

void ATeachingSessionActionClientNode::SendGoal()
{
    // Create goal
    UROS2TeachingSessionAction* TeachingSessionAction = Cast<UROS2TeachingSessionAction>(TeachingSessionActionClient->Action);
    FROSTeachingSessionSGReq goalRequest;
    goalRequest.SessionType = SessionType;
    TeachingSessionAction->SetGoalRequest(goalRequest);

    // send goal
    if (!TeachingSessionActionClient->SendGoal())
    {
        // if it failes, retry after 1s
        UE_LOG_WITH_INFO_NAMED(
            LogRos2SandBox, Warning, TEXT("[%s][C++][send goal] failed to sendo goal. retry in 1s..."), *ActionName);
        GetWorld()->GetTimerManager().SetTimer(ActionTimerHandle, this, &ATeachingSessionActionClientNode::SendGoal, 1.f, false);
    }
    else
    {
        UE_LOG_WITH_INFO_NAMED(LogRos2SandBox, Log, TEXT("[%s][C++][send goal] SessionType: %i"), *ActionName, *SessionType);
    }
}

void ATeachingSessionActionClientNode::FeedbackCallback(UROS2GenericAction* InAction)
{
    UROS2TeachingSessionAction* TeachingSessionAction = Cast<UROS2TeachingSessionAction>(InAction);
    FROSTeachingSessionFB feedback;
    TeachingSessionAction->GetFeedback(feedback);

    UE_LOG_WITH_INFO_NAMED(LogRos2SandBox,
                           Log,
                           TEXT("[%s][C++][received feedback callback] NewPoseLabel: %s, NewPose: X=%.3f Y=%.3f Z=%.3f | Orientation: X=%.3f Y=%.3f Z=%.3f W=%.3f"),
                           *ActionName,
                           *(feedback.NewPoseLabel),
                           feedback.NewPose.Position.X, feedback.NewPose.Position.Y, feedback.NewPose.Position.Z,
                           feedback.NewPose.Orientation.X, feedback.NewPose.Orientation.Y, feedback.NewPose.Orientation.Z, feedback.NewPose.Orientation.W);
}

void ATeachingSessionActionClientNode::ResultCallback(UROS2GenericAction* InAction)
{
    UROS2TeachingSessionAction* TeachingSessionAction = Cast<UROS2TeachingSessionAction>(InAction);
    FROSTeachingSessionGRRes resultResponse;
    TeachingSessionAction->GetResultResponse(resultResponse);

    // Log request and response
    UE_LOG_WITH_INFO_NAMED(
        LogRos2SandBox, Log, TEXT("[%s][C++][received result callback] result is: success: %"), *ActionName, resultResponse.bSuccess ? TEXT("true") : TEXT("false"), *(resultResponse.Message));

    // // update SessionType and send next goal
    // SessionType++;
    // SendGoal();
}

void ATeachingSessionActionClientNode::GoalResponseCallback(UROS2GenericAction* InAction)
{
    UROS2TeachingSessionAction* TeachingSessionAction = Cast<UROS2TeachingSessionAction>(InAction);
    FROSTeachingSessionSGRes goalResponse;
    TeachingSessionAction->GetGoalResponse(goalResponse);

    if (!goalResponse.bAccepted)
    {
        UE_LOG_WITH_INFO_NAMED(LogRos2SandBox,
                               Warning,
                               TEXT("[%s][C++][receive goal response callback] goal request is rejected. retry in 1s..."),
                               *ActionName);
    }
    else
    {
        UE_LOG_WITH_INFO_NAMED(
            LogRos2SandBox, Log, TEXT("[%s][C++][receive goal response callback] goal request is accepted."), *ActionName);
        TeachingSessionActionClient->SendResultRequest();
    }
}

void ATeachingSessionActionClientNode::CancelCallback()
{
    int cancelResult = TeachingSessionActionClient->Action->GetCancelResponseReturnCode();
    if (cancelResult != FROSCancelGoalRes::ERROR_NONE)
    {
        UE_LOG_WITH_INFO_NAMED(
            LogRos2SandBox, Log, TEXT("[%s][C++][received cancel response callback] failed to cancel action"), *ActionName);
    }
    else
    {
        UE_LOG_WITH_INFO_NAMED(
            LogRos2SandBox, Log, TEXT("[%s][C++][received cancel response callback] succeeded to cancel action"), *ActionName);
    }
}