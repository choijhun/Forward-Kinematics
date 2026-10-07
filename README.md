# Forward Kinematics
- Motion data를 parsing 하고 부모 joint의 transformation을 자식 joint로 전달하는 forward kinematics로 각 frame별 3d skelton pose 시각화
# BVH Pasring
- Root, joint End site 를 파싱하여 skeleton hierachy 구성
# Quaternian Rotation
- BVH 의 rotation을 Quaternian으로 변환
- joint의 rotation channel을 결합하여 local rotation 계산
- Quaternian 의 multiplication, inverse ,exponential 연산 구현
# Forward Kinematics
- Parent joint의 global position과 rotation을 이용해서 child joint의 위치, 회전을 계산
- Root에서 끝 joint까지 계층적으로  transformation을 전달하여 각 frame의 skeleton pose 생성

https://github.com/user-attachments/assets/87a98ef4-80dd-468b-b236-3f7f85860f4b



https://github.com/user-attachments/assets/d2f937e8-6128-41ae-811d-59e32a6ba902

