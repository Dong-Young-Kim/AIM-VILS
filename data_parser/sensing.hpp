# pragma once
#include <math.h>

#include "msg_struct.h"
#include "data_parser.hpp"

struct objInfo {
    short classes;
    unsigned int idx;
    float posX;
    float posY;
    float posZ;
    float sizeX;
    float sizeY;
    float sizeZ;
    float velX;
    float velY;
    float velZ;
    float heading;
};

struct vehInfo{
    float posX;
    float posY;
    float posZ;
    float roll;
    float pitch;
    float yaw;
    
    char mode;
    char gear;
    float speed;
    float brake;
    float steer;
    
};

struct localInfo{
    double timestamp;
    double longtitude;
    double latitude;
    double altitude;

};

class VehicleSensing{
public:
    VehicleSensing(){

    }

    void run(ego_vehicle_status *evs, objectInfo *oi){
        senseVehicleStatus(evs);
        senseObject(oi);
        senseLocal();
    }

    void senseVehicleStatus(ego_vehicle_status *evs){
        evs->posX       = this->vehicle.posX;
        evs->posY       = this->vehicle.posY;
        evs->posZ       = this->vehicle.posZ;
        evs->roll       = this->vehicle.roll;
        evs->pitch      = this->vehicle.pitch;
        evs->yaw        = this->vehicle.yaw;
        evs->yaw        = this->vehicle.yaw;
        evs->ctrl_mode  = this->vehicle.mode;
        evs->gear       = this->vehicle.gear;
        evs->signed_velocity = this->vehicle.speed;
        evs->brake      = this->vehicle.brake;
        evs->steer      = this->vehicle.steer;
    }
        
    void senseObject(objectInfo *oi){

        for(int i = 0; ; i++){
            this->objs[i].classes = oi->data[i].objType;

            if(oi->data->objId == 0){
                
            }
        }

        convertObj2VehCoor(true);
    }

    void senseLocal(){
        
    }

    inline float convertHeadingE2NCW(float headingE){
        return ((90 - headingE) > 180) ? ( -270 - headingE) : (90 - headingE);
    }

    void convertObj2VehCoor(bool swch){ //true -> vehicle coordinate, false -> ENU coordinate
        if (swch == false ) return;

        float tmpX, tmpY, tmpZ;
        for(int i = 0; i < objNum; i++){
            //translation
            tmpX = objs[i].posX - vehicle.posX;
            tmpY = objs[i].posY - vehicle.posY;
            tmpZ = objs[i].posZ - vehicle.posZ;

            //rotation
            objs[i].posX =  cos(vehicle.yaw * M_PI / 180) * tmpX + sin(vehicle.yaw * M_PI / 180) * tmpY;
            objs[i].posY = -sin(vehicle.yaw * M_PI / 180) * tmpX + cos(vehicle.yaw * M_PI / 180) * tmpY;
            objs[i].posZ = tmpZ;
        }
    }
    
private:

    struct vehInfo vehicle;
    short objNum;
    struct objInfo objs[20];

};