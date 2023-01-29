# pragma once
#include <math.h>

#include "data_parser.hpp"

struct objInfo {
    short           classes;
    unsigned int    idx;
    float           posX;
    float           posY;
    float           posZ;
    float           sizeX;
    float           sizeY;
    float           sizeZ;
    float           velX;
    float           velY;
    float           velZ;
    float           heading;
};

struct vehInfo{
    float           posX;
    float           posY;
    float           posZ;
    float           roll;
    float           pitch;
    float           yaw;
    
    char            mode;
    char            gear;
    float           speed;
    float           brake;
    float           steer;
    
};

struct localInfo{
    double          timestamp;
    double          longtitude;
    double          latitude;
    double          altitude;

};

class VehicleSensing{
public:
    VehicleSensing(){

    }

    void run(ego_vehicle_status *evs, objectInfo *oi){
        senseVehicleStatus(evs);
        senseObject(oi);
        senseLocal(evs);
    }

    void senseVehicleStatus(ego_vehicle_status *evs){

        this->vehicle.posX      = evs->posX;
        this->vehicle.posY      = evs->posY;
        this->vehicle.posZ      = evs->posZ;
        this->vehicle.roll      = evs->roll;
        this->vehicle.pitch     = evs->pitch;
        this->vehicle.yaw       = evs->yaw;
        this->vehicle.yaw       = evs->yaw;
        this->vehicle.mode      = evs->ctrl_mode;
        this->vehicle.gear      = evs->gear;
        this->vehicle.speed     = evs->signed_velocity;
        this->vehicle.brake     = evs->brake;
        this->vehicle.steer     = evs->steer;

    }
        
    void senseObject(objectInfo *oi){
        
        objNum = 0;
        for(int i = 0; ; ++i){
            if(oi->data[i].objId != 0) ++objNum;
            else break;
            this->objs[i].classes   = oi->data[i].objType;
            this->objs[i].idx       = oi->data[i].objId;
            this->objs[i].posX      = oi->data[i].posX;
            this->objs[i].posY      = oi->data[i].posY;
            this->objs[i].posZ      = oi->data[i].posZ;
            this->objs[i].sizeX     = oi->data[i].sizeX;
            this->objs[i].sizeY     = oi->data[i].sizeY;
            this->objs[i].sizeZ     = oi->data[i].sizeZ;
            this->objs[i].velX      = oi->data[i].velocityX;
            this->objs[i].velY      = oi->data[i].velocityY;
            this->objs[i].velZ      = oi->data[i].velocityZ;
            this->objs[i].heading   = oi->data[i].heading;

        }

        convertObj2VehCoor(true);
    }

    void senseLocal(ego_vehicle_status *evs){
        this->maplocal.longtitude   = evs->posX;
        this->maplocal.latitude     = evs->posY;
        this->maplocal.altitude     = evs->posZ;
        this->maplocal.timestamp    = 0;
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
    struct localInfo maplocal;

};