#include "data_parser.hpp"


#include <math.h>

int main(){
    mdpar::EgoStatusParser rd(8909);
    mdpar::ObjectParser od(7505);
    
    while(1){
        rd.recv();
        od.recv();

        // rd.parseData();
        // od.parseData();

        // rd.printBuffer();
        rd.printData();
        // od.printBuffer();
        od.printData();

        //ud.printBuffer();

        // {
        //     //[1] object에 대해서만 좌표 변환을 진행
        //     float yaw = rd.data->yaw;
        //     float transObjCoorX = od.data->data[0].posX - rd.data->posX;
        //     float transObjCoorY = od.data->data[0].posY - rd.data->posY;
        //     float transObjCoorZ = od.data->data[0].posZ - rd.data->posZ;

        //     float rotObjCoorX = cos(yaw * M_PI / 180) * transObjCoorX +  sin(yaw * M_PI / 180) * transObjCoorY;
        //     float rotObjCoorY =  -sin(yaw * M_PI / 180) * transObjCoorX +  cos(yaw * M_PI / 180) * transObjCoorY;

        //     printf("%f %f \n", cos(yaw * M_PI / 180), sin(yaw * M_PI / 180));

        //     printf("%f, %f, %f\n", rotObjCoorX, rotObjCoorY, transObjCoorZ);
        // }

    }
}