//ROS2PublisherNode.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "ROS2NodeComponent.h"
#include "ROS2Publisher.h"
#include "RRROS2StringPublisher.h"

#include "ROS2PublisherNode.generated.h"

UCLASS()
class ROS2_SANDBOX_API AROS2PublisherNode : public AActor
{
    GENERATED_BODY()

public:
    AROS2PublisherNode();

    UFUNCTION()
    void UpdateMessage(UROS2GenericMsg* InMessage);

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UROS2NodeComponent* Node = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString TopicName = TEXT("str_topic");

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString TopicNameLoop = TEXT("str_topic_loop");

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString TopicNameCustom = TEXT("str_topic_custom");

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    URRROS2StringPublisher* StringPublisher = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UROS2Publisher* Publisher = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UROS2Publisher* LoopPublisher = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float PublicationFrequencyHz = 1.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Message = TEXT("Hello from C++");

    UPROPERTY()
    int32 Count = 0;

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;
};