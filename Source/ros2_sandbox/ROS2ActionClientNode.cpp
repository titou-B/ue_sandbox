// Copyright 2020-2021 Rapyuta Robotics Co., Ltd.

#include "ROS2ActionClientNode.h"

#include "Kismet/GameplayStatics.h"
#include "rclcUtilities.h"

AROS2ActionClientNode::AROS2ActionClientNode()
{
    Node = CreateDefaultSubobject<UROS2NodeComponent>(TEXT("ROS2NodeComponent"));

    // these parameters can be change from BP
    Node->Name = TEXT("action_client_node");
    Node->Namespace = TEXT("cpp");
}

void AROS2ActionClientNode::BeginPlay()
{
    Super::BeginPlay();
    Node->Init();

    ROS2_CREATE_ACTION_CLIENT(Node,
                              this,
                              ActionName,
                              UROS2FibonacciAction::StaticClass(),
                              &AROS2ActionClientNode::GoalResponseCallback,
                              &AROS2ActionClientNode::ResultCallback,
                              &AROS2ActionClientNode::FeedbackCallback,
                              &AROS2ActionClientNode::CancelCallback,
                              FibonacciActionClient);

    // ROS2_CREATE_ACTION_CLIENT_WITH_QOS(Node,
    //                           this,
    //                           ActionName,
    //                           UROS2FibonacciAction::StaticClass(),
    //                           &AROS2ActionClientNode::GoalResponseCallback,
    //                           &AROS2ActionClientNode::ResultCallback,
    //                           &AROS2ActionClientNode::FeedbackCallback,
    //                           &AROS2ActionClientNode::CancelCallback,
    //                           UROS2QoS::Services,
    //                           UROS2QoS::Services,
    //                           UROS2QoS::Default,
    //                           UROS2QoS::Services,
    //                           FibonacciActionClient);

    SendGoal();
}

void AROS2ActionClientNode::SendGoal()
{
    // Create goal
    UROS2FibonacciAction* FibonacciAction = Cast<UROS2FibonacciAction>(FibonacciActionClient->Action);
    FROSFibonacciSGReq goalRequest;
    goalRequest.Order = Order;
    FibonacciAction->SetGoalRequest(goalRequest);

    // send goal
    if (!FibonacciActionClient->SendGoal())
    {
        // if it failes, retry after 1s
        UE_LOG_WITH_INFO_NAMED(
            LogRos2SandBox, Warning, TEXT("[%s][C++][send goal] failed to sendo goal. retry in 1s..."), *ActionName);
        GetWorld()->GetTimerManager().SetTimer(ActionTimerHandle, this, &AROS2ActionClientNode::SendGoal, 1.f, false);
    }
    else
    {
        UE_LOG_WITH_INFO_NAMED(LogRos2SandBox, Log, TEXT("[%s][C++][send goal] order: %i"), *ActionName, Order);
    }
}

void AROS2ActionClientNode::FeedbackCallback(UROS2GenericAction* InAction)
{
    UROS2FibonacciAction* FibonacciAction = Cast<UROS2FibonacciAction>(InAction);
    FROSFibonacciFB feedback;
    FibonacciAction->GetFeedback(feedback);


    FString feedbackString;
    for (auto s : feedback.Sequence)
    {
        feedbackString += FString::FromInt(s) + ", ";
    }

    UE_LOG_WITH_INFO_NAMED(LogRos2SandBox,
                           Log,
                           TEXT("[%s][C++][received feedback callback] feedback sequence: %s"),
                           *ActionName,
                           *feedbackString);
}

void AROS2ActionClientNode::ResultCallback(UROS2GenericAction* InAction)
{
    UROS2FibonacciAction* FibonacciAction = Cast<UROS2FibonacciAction>(InAction);

    example_interfaces__action__Fibonacci_GetResult_Response result_msg;
    example_interfaces__action__Fibonacci_GetResult_Response__init(&result_msg);

    auto data = FibonacciAction->GetResultResponse();
    data = &result_msg;

    // Structure pour recevoir l’en-tête de la réponse
    rmw_request_id_t result_response_header;

    UE_LOG_WITH_INFO(LogRos2SandBox, Error, TEXT("------- rcl_action_take_result_response"));
    RCSOFTCHECK(rcl_action_take_result_response(&FibonacciActionClient->client, &result_response_header, data));

    UE_LOG_WITH_INFO_NAMED(LogRos2SandBox, Log, TEXT("Result received with status %d : "), result_msg.status);
    FString DebugResultString;
    for (size_t i = 0; i < result_msg.result.sequence.size; ++i) {
        DebugResultString += FString::FromInt(result_msg.result.sequence.data[i]) + ", ";
    }
    UE_LOG(LogRos2SandBox, Warning, TEXT("%s"), *DebugResultString);

    FROSFibonacciGRRes resultResponse;
    FibonacciAction->GetResultResponse(resultResponse);


    // Log request and response
    FString resultString;
    for (int s : resultResponse.Sequence)
    {
        UE_LOG_WITH_INFO_NAMED(LogRos2SandBox, Log, TEXT("%d, "), s);
        resultString += FString::FromInt(s) + ", ";
    }
    UE_LOG_WITH_INFO_NAMED(
        LogRos2SandBox, Log, TEXT("[%s][C++][received result callback] result is: %s"), *ActionName, *resultString);
}

void AROS2ActionClientNode::GoalResponseCallback(UROS2GenericAction* InAction)
{
    UROS2FibonacciAction* FibonacciAction = Cast<UROS2FibonacciAction>(InAction);
    FROSFibonacciSGRes goalResponse;
    FibonacciAction->GetGoalResponse(goalResponse);

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

        // // → Récupérer le goal ID utilisé
        // FROSFibonacciSGReq sentGoal;
        // FibonacciAction->GetGoalRequest(sentGoal);

        // // → Le copier dans la requête de résultat
        // FROSFibonacciGRReq resultRequest;
        // resultRequest.GoalId = sentGoal.GoalId;

        // // → Appliquer et envoyer
        // FibonacciAction->SetResultRequest(resultRequest);

        FibonacciActionClient->SendResultRequest();
    }
}

void AROS2ActionClientNode::CancelCallback()
{
    int cancelResult = FibonacciActionClient->Action->GetCancelResponseReturnCode();
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