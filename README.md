# Prerequisites

- Unreal Engine 5.5.4 (UE5) installed (Linux recommended)
- ROS2 (Humble)
- rclUE and RapyutaSimulationPlugins dependencies (branch UE5.5_devel_humble)
- A compatible environment (Ubuntu 22.04 Jammy recommended)


# Run some Examples !

## Actions

### UE Server - external Client

1. Instanciate `ROS2ActionServerNode` into your scene
2. Run your scene
3. From a ROS2 terminal, run :

    ```bash
    ros2 action send_goal /cpp/fibonacci_action example_interfaces/action/Fibonacci "{order: '4'}"
    ```

Expected result :

1. terminal :

    ```bash
    ros2 action send_goal /cpp/fibonacci_action example_interfaces/action/Fibonacci "{order: '4'}"
    Waiting for an action server to become available...
    Sending goal:
        order: 4

    Goal accepted with ID: 616c3039db41484596b1c737fb431e13

    Result:
        sequence:
    - 0
    - 1
    - 1
    - 2
    - 3
    - 5
    - 8

    Goal finished with status: SUCCEEDED
    ```

2. UE :

    ```bash
    LogROS2Node: [ROS2NodeComponent.cpp@109, void UROS2NodeComponent::Init()] [ROS2NodeComponent] start initializing..
    LogROS2Node: [ROS2NodeComponent.cpp@114, void UROS2NodeComponent::Init()] [ROS2NodeComponent] rclc_node_init_default
    LogROS2Node: [ROS2NodeComponent.cpp@120, void UROS2NodeComponent::Init()] [ROS2NodeComponent] ROS2Node inited
    PIE: Server logged in
    PIE: Play in editor total start time 0.658 seconds.
    LogROS2Action: [ROS2ActionServer.cpp@74, virtual void UROS2ActionServer::ProcessReady(rcl_wait_set_t *)] 2. Action Server - Received goal request
    LogROS2Action: [ROS2ActionServer.cpp@126, void UROS2ActionServer::SendGoalResponse()] 3. Action Server - Send goal response
    LogRos2SandBox: [ROS2ActionServerNode.cpp@95, void AROS2ActionServerNode::GoalCallback(UROS2GenericAction *)] [ROS2ActionServerNode_UAID_A0CEC81436F6AA9802_1481516927] [fibonacci_action][C++][goal callback]
    LogROS2Action: [ROS2ActionServer.cpp@90, virtual void UROS2ActionServer::ProcessReady(rcl_wait_set_t *)] 6. Action Server - Received result request
    LogRos2SandBox: [ROS2ActionServerNode.cpp@123, void AROS2ActionServerNode::ResultCallback()] [ROS2ActionServerNode_UAID_A0CEC81436F6AA9802_1481516927] [fibonacci_action][C++][result callback] Start fibonacci calculation
    LogRos2SandBox: [ROS2ActionServerNode.cpp@52, void AROS2ActionServerNode::Execute()] [ROS2ActionServerNode_UAID_A0CEC81436F6AA9802_1481516927] [fibonacci_action][C++][update feedback] added 1
    LogROS2Action: [ROS2ActionServer.cpp@139, void UROS2ActionServer::SendFeedback()] 7. Action Server - Publish feedback
    LogRos2SandBox: [ROS2ActionServerNode.cpp@52, void AROS2ActionServerNode::Execute()] [ROS2ActionServerNode_UAID_A0CEC81436F6AA9802_1481516927] [fibonacci_action][C++][update feedback] added 2
    LogROS2Action: [ROS2ActionServer.cpp@139, void UROS2ActionServer::SendFeedback()] 7. Action Server - Publish feedback
    LogRos2SandBox: [ROS2ActionServerNode.cpp@52, void AROS2ActionServerNode::Execute()] [ROS2ActionServerNode_UAID_A0CEC81436F6AA9802_1481516927] [fibonacci_action][C++][update feedback] added 3
    LogROS2Action: [ROS2ActionServer.cpp@139, void UROS2ActionServer::SendFeedback()] 7. Action Server - Publish feedback
    LogRos2SandBox: [ROS2ActionServerNode.cpp@52, void AROS2ActionServerNode::Execute()] [ROS2ActionServerNode_UAID_A0CEC81436F6AA9802_1481516927] [fibonacci_action][C++][update feedback] added 5
    LogROS2Action: [ROS2ActionServer.cpp@139, void UROS2ActionServer::SendFeedback()] 7. Action Server - Publish feedback
    LogRos2SandBox: [ROS2ActionServerNode.cpp@52, void AROS2ActionServerNode::Execute()] [ROS2ActionServerNode_UAID_A0CEC81436F6AA9802_1481516927] [fibonacci_action][C++][update feedback] added 8
    LogROS2Action: [ROS2ActionServer.cpp@139, void UROS2ActionServer::SendFeedback()] 7. Action Server - Publish feedback
    LogROS2Action: [ROS2ActionServer.cpp@152, void UROS2ActionServer::SendResultResponse()] 9. Action Server - Send result response
    LogRos2SandBox: [ROS2ActionServerNode.cpp@76, void AROS2ActionServerNode::Execute()] [ROS2ActionServerNode_UAID_A0CEC81436F6AA9802_1481516927] [fibonacci_action][C++][send result] result is: 0, 1, 1, 2, 3, 5, 8,
    ```

### UE Action Client - External Action Server

This example differs from [the one in the documentation](https://rclue.readthedocs.io/en/latest/examples/action_client_example.html#code)

this code has been added in `void AROS2ActionClientNode::GoalResponseCallback(UROS2GenericAction* InAction)`

```c++
// → Le copier dans la requête de résultat
FROSFibonacciGRReq resultRequest;
FibonacciAction->SetGoalIdToResultRequest(resultRequest);

// → Appliquer et envoyer
FibonacciAction->SetResultRequest(resultRequest);
```

1. Instanciate `ROS2ActionClientNode` into your scene
2. From a terminal, run :

    ```bash
    python3 scripts/fibo_action_server.py
    ```

3. Run your scene

Expected result :

1. terminal :

    ```bash
    python3 scripts/fibo_action_server.py
    let's spin fibonacci_action_server !
    [INFO] [1760530597.150115593] [fibonacci_action_server]: Executing goal...
    [INFO] [1760530597.150562315] [fibonacci_action_server]: Feedback: array('i', [0, 1, 1])
    [INFO] [1760530598.151596273] [fibonacci_action_server]: Feedback: array('i', [0, 1, 1, 2])
    [INFO] [1760530599.152416379] [fibonacci_action_server]: Result: array('i', [0, 1, 1, 2])
    ```

2. UE :

    ```bash
    LogROS2Node: [ROS2NodeComponent.cpp@109, void UROS2NodeComponent::Init()] [ROS2NodeComponent] start initializing..
    LogROS2Node: [ROS2NodeComponent.cpp@114, void UROS2NodeComponent::Init()] [ROS2NodeComponent] rclc_node_init_default
    LogROS2Node: [ROS2NodeComponent.cpp@120, void UROS2NodeComponent::Init()] [ROS2NodeComponent] ROS2Node inited
    LogROS2Action: [ROS2ActionClient.cpp@162, bool UROS2ActionClient::SendGoal()] 1. Action Client - Send goal
    LogRos2SandBox: [ROS2ActionClientNode.cpp@67, void AROS2ActionClientNode::SendGoal()] [ROS2ActionClientNode_UAID_A0CEC81436F6A79802_2017847399] [fibonacci_action][C++][send goal] order: 3
    PIE: Server logged in
    PIE: Play in editor total start time 1.575 seconds.
    LogROS2Action: [ROS2ActionClient.cpp@85, virtual void UROS2ActionClient::ProcessReady(rcl_wait_set_t *)] 2. Action Client - Attempting to take goal status message
    LogROS2Action: [ROS2ActionClient.cpp@103, virtual void UROS2ActionClient::ProcessReady(rcl_wait_set_t *)]  → Received 1 goal status entries
    LogROS2Action: [ROS2ActionClient.cpp@116, virtual void UROS2ActionClient::ProcessReady(rcl_wait_set_t *)]  → Goal 111383BFAED2C9403BE2504439BD3C20 has status 1
    LogROS2Action: [ROS2ActionClient.cpp@126, virtual void UROS2ActionClient::ProcessReady(rcl_wait_set_t *)] 4. Action Client - Received goal response
    LogRos2SandBox: [ROS2ActionClientNode.cpp@144, void AROS2ActionClientNode::GoalResponseCallback(UROS2GenericAction *)] [ROS2ActionClientNode_UAID_A0CEC81436F6A79802_2017847399] [fibonacci_action][C++][receive goal response callback] goal request is accepted.
    LogROS2Action: [ROS2ActionClient.cpp@178, void UROS2ActionClient::SendResultRequest()] 5. Action Client - Send result request
    LogSlate: Took 0.014208 seconds to synchronously load lazily loaded font '../../../Engine/Content/Slate/Fonts/DroidSansFallback.ttf' (3848K)
    LogROS2Action: [ROS2ActionClient.cpp@77, virtual void UROS2ActionClient::ProcessReady(rcl_wait_set_t *)] 8. Action Client - Received feedback
    LogRos2SandBox: [ROS2ActionClientNode.cpp@88, void AROS2ActionClientNode::FeedbackCallback(UROS2GenericAction *)] [ROS2ActionClientNode_UAID_A0CEC81436F6A79802_2017847399] [fibonacci_action][C++][received feedback callback] feedback sequence: 0, 1, 1,
    LogROS2Action: [ROS2ActionClient.cpp@85, virtual void UROS2ActionClient::ProcessReady(rcl_wait_set_t *)] 2. Action Client - Attempting to take goal status message
    LogROS2Action: [ROS2ActionClient.cpp@103, virtual void UROS2ActionClient::ProcessReady(rcl_wait_set_t *)]  → Received 1 goal status entries
    LogROS2Action: [ROS2ActionClient.cpp@116, virtual void UROS2ActionClient::ProcessReady(rcl_wait_set_t *)]  → Goal 111383BFAED2C9403BE2504439BD3C20 has status 2
    LogROS2Action: [ROS2ActionClient.cpp@77, virtual void UROS2ActionClient::ProcessReady(rcl_wait_set_t *)] 8. Action Client - Received feedback
    LogRos2SandBox: [ROS2ActionClientNode.cpp@88, void AROS2ActionClientNode::FeedbackCallback(UROS2GenericAction *)] [ROS2ActionClientNode_UAID_A0CEC81436F6A79802_2017847399] [fibonacci_action][C++][received feedback callback] feedback sequence: 0, 1, 1, 2,
    LogROS2Action: [ROS2ActionClient.cpp@85, virtual void UROS2ActionClient::ProcessReady(rcl_wait_set_t *)] 2. Action Client - Attempting to take goal status message
    LogROS2Action: [ROS2ActionClient.cpp@103, virtual void UROS2ActionClient::ProcessReady(rcl_wait_set_t *)]  → Received 1 goal status entries
    LogROS2Action: [ROS2ActionClient.cpp@116, virtual void UROS2ActionClient::ProcessReady(rcl_wait_set_t *)]  → Goal 111383BFAED2C9403BE2504439BD3C20 has status 4
    LogROS2Action: [ROS2ActionClient.cpp@142, virtual void UROS2ActionClient::ProcessReady(rcl_wait_set_t *)] 10. Action Client - Received result response
    LogRos2SandBox: [ROS2ActionClientNode.cpp@125, void AROS2ActionClientNode::ResultCallback(UROS2GenericAction *)] [ROS2ActionClientNode_UAID_A0CEC81436F6A79802_2017847399] [fibonacci_action][C++][received result callback] result is: 0, 1, 1, 2,
    ```
