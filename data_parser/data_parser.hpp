#pragma once

#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h>

#include "msg_struct.h"

#define BUFSIZE     4096

void DumpHex(const void* data, size_t size) {
    char ascii[17];
    size_t i, j;
    ascii[16] = '\0';
    for (i = 0; i < size; ++i) {
        printf("%02x ", ((unsigned char*)data)[i]);
        if (((unsigned char*)data)[i] >= ' ' && ((unsigned char*)data)[i] <= '~') {
            ascii[i % 16] = ((unsigned char*)data)[i];
        } else {
            ascii[i % 16] = '.';
        }
        if ((i+1) % 8 == 0 || i+1 == size) {
            printf(" ");
            if ((i+1) % 16 == 0) {
                printf("|  %s \n", ascii);
            } else if (i+1 == size) {
                ascii[(i+1) % 16] = '\0';
                if ((i+1) % 16 <= 8) {
                    printf(" ");
                }
                for (j = (i+1) % 16; j < 16; ++j) {
                    printf("   ");
                }
                printf("|  %s \n", ascii);
            }
        }
    }
}

namespace mdpar {
    class UdpParser{
        public:
            UdpParser(int myport){
                makeSocket(myport);
            }

            void makeSocket(int myport){
                if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
                    perror("socket creation failed"); 
                    exit(EXIT_FAILURE); 
                } 

                bzero((char*)&servaddr, sizeof(servaddr));
                bzero((char*)&cliaddr, sizeof(cliaddr));
                    
                // Filling server information 
                servaddr.sin_family    = AF_INET; // IPv4 
                servaddr.sin_addr.s_addr = INADDR_ANY; 
                servaddr.sin_port = htons(myport); 
                    
                // Bind the socket with the server address 
                if ( bind(sockfd, (const struct sockaddr *)&servaddr,  
                        sizeof(servaddr)) < 0 ) 
                { 
                    perror("bind failed (????????? ??????)"); 
                    exit(EXIT_FAILURE); 
                } 

                slen = sizeof(cliaddr);
            }

            void recv(){
                recvLen = recvfrom(sockfd, (char *)buffer, BUFSIZE,  
                        MSG_WAITALL, ( struct sockaddr *) &cliaddr, 
                        &slen);
            }

            void printBuffer(){
                DumpHex(buffer, recvLen);
            }

            void parseData(){};
            void printData(){};
            

            int recvLen;
            char buffer[BUFSIZE];


        protected:
            int sockfd;
            struct sockaddr_in servaddr, cliaddr; 
            socklen_t slen;
    };

    class EgoStatusParser : public UdpParser {
    public:
        EgoStatusParser(int myport)
        : UdpParser(myport) {}

        void parseData(){
            this->data = (struct ego_vehicle_status*) buffer;
        }

        void printData(){
            printf("[Receiving from Morai..]\n");
            // printf("sharp                   : %c\n", evs->sharp);
            // printf("MaraiInfo               : %s\n", evs->MaraiInfo);
            // printf("doller                  : %c\n", evs->dollor);
            // printf("data_length             : %d\n", evs->data_length);

            printf("ctrl_mode (????????????)    : %s\n", (this->data->ctrl_mode == 2) ? "auto (??????)" : "keyboard (???????????????)");
            printf("gear (??????)             : ");
            switch (this->data->gear) {
            case(0):
                printf("M \n");
                break;
            case(1):
                printf("P (Parking, ??????)\n");
                break;
            case(2):
                printf("R (Reverse, ??????)\n");
                break;
            case(3):
                printf("N (Neutral, ??????)\n");
                break;
            case(4):
                printf("D (Drive, ??????)\n");
                break;
            case(5):
                printf("L (Low, ??????)\n");
                break;
            }
            
            printf("speed (??????)            : %.2fm/s\n", this->data->signed_velocity);
            printf("Accel (????????????)        : %.2f\n", this->data->accel);
            printf("Brake (??????????????????)    : %.2f\n", this->data->brake);
            printf("steer (????????????)        : %.2f\n\n", this->data->steer);
            
            printf("Position XYZ            : %5.2fm, \t%5.2fm, \t%5.2fm \n", this->data->posX, this->data->posY, this->data->posZ);
            printf("Roll Pitch Yaw          : %5.2fdeg, \t%5.2fdeg, \t%5.2fdeg \n", this->data->roll, this->data->pitch, this->data->yaw);
            printf("Velocity XYZ            : %5.2fkm/h, \t%5.2fkm/h, \t%5.2fkm/h \n", this->data->velocityX, this->data->velocityY, this->data->velocityZ);
            printf("Acceleration XYZ        : %5.2fm/s^2, \t%5.2fm/s^2, \t%5.2fm/s^2 \n", this->data->accelX, this->data->accelY, this->data->accelZ);
            printf("map_data_id             : %d\n", this->data->map_data_id);
            printf("link ID                 : %s\n", this->data->linkID);
            printf("========================================================================\n");
        }

        struct ego_vehicle_status* data;

    private:

    };

    class ObjectParser : public UdpParser {
    public:
        ObjectParser(int myport)
        : UdpParser(myport) {}

        void parseData(){
            this->data = (struct objectInfo*) buffer;
        }

        void printData(){
            objcnt = 0;

            printf("[Receiving from Morai..]\n");
            // // printf("sharp                   : %c\n", evs->sharp);
            // // printf("MaraiInfo               : %s\n", evs->MaraiInfo);
            // // printf("doller                  : %c\n", evs->dollor);
            // // printf("data_length             : %d\n", oi->data_length);

            printf(" object ID    Type          posXYZ               heading         sizeXYZ                 velXYZ\n");
            printf("---------------------------------------------------------------------------------------------------------\n");

            for(int i = 0; i < this->data->data_length / 106; ++i){
                if(this->data->data[i].objId != 0) ++objcnt;
                else break;
                printf("   %6d", this->data->data[i].objId);
                printf("  %6d", this->data->data[i].objType);
                printf("  (%7.2f, %7.2f, %7.2f)", this->data->data[i].posX, this->data->data[i].posY, this->data->data[i].posZ);
                printf("   %+6.2f", (this->data->data[i].heading));
                printf("  (%5.2f, %5.2f, %5.2f)", this->data->data[i].sizeX, this->data->data[i].sizeY, this->data->data[i].sizeZ);
                printf("  (%6.2f, %6.2f, %6.2f)", this->data->data[i].velocityX, this->data->data[i].velocityY, this->data->data[i].velocityZ);
                printf("\n");
            }
            printf("---------------------------------------------------------------------------------------------------------\n");
            printf("                                                                                     num of objects : %2d\n", objcnt);
            printf("=========================================================================================================\n");

        }

        struct objectInfo* data;
        int objcnt;

    private:
    };
}