# AIM-VILS
ERP-42를 통한 VILS 구축을 위한 통신 시스템

## Function Provide
- Ego Vehicle 상태 정보 수신
- Ego Vehicle Control message 입력
- Ego Vehicle 주변 Object 정보 수신


### basic communicate function
|기능|packet 전송 방향|작성 코드|
|------|---|---|
|Ego Vehicle 상태 정보 수신|Sim -> code|[ego_vehicle_status_recv.c](/morai_vils/ego_vehicle_status_recv.c)|
| Ego Vehicle Control message 입력|code -> Sim|[ego_vehicle_cmd.c](/morai_vils/ego_vehicle_cmd.c)|
|Ego Vehicle 주변 Object 정보 수신|Sim -> code|[object_recv.c](/morai_vils/object_recv.c)|

---

## How to Use 