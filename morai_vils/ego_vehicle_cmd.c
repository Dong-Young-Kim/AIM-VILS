#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h>
    
//#define SIMIPADDR   "127.0.0.1"
#define SIMIPADDR   "165.246.170.87"
#define SIMCMDPORT     9091

#pragma pack(push, 1) //to read packet correctly
struct ego_vehicle_cmd{

    char prefix[14];
    unsigned int data_length;
    char aux_data[12];

    //data
    char ctrl_mode;
    char gear;
    char cmdtype;
    
    float velocity;
    float acceleration;
    float accelerator;
    float brake;
    float steering;

    char zeroD;
    char zeroA;
};
#pragma pack(pop)

// Driver code 
int main() { 
    int sockfd; 
    struct sockaddr_in     servaddr; 
    
    // Creating socket file descriptor 
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
        perror("socket creation failed"); 
        exit(EXIT_FAILURE); 
    } 
    
    memset(&servaddr, 0, sizeof(servaddr)); 
        
    // Filling server information 
    servaddr.sin_family = AF_INET; 
    servaddr.sin_addr.s_addr = inet_addr(SIMIPADDR); 
    servaddr.sin_port = htons(SIMCMDPORT);  

    //len = sizeof(cliaddr);

    struct ego_vehicle_cmd evs;

    strncpy(evs.prefix, "#MoraiCtrlCmd$", 14);
    evs.data_length = 23;
    strncpy(evs.aux_data, "\0\0\0\0\0\0\0\0\0\0\0\0", 12);
    evs.zeroD = 0x0D;
    evs.zeroA = 0x0A;


    evs.ctrl_mode = 0; //1 : KeyBoard, 2 : AutoMode, else : dual
    evs.cmdtype = 1; // 1: Throttle제어(accel/brake/steering), 2: Velocity제어(velocity/steering), 3: Acceleration제어

    evs.gear = 4; // M : 0, P : 1 (주차), R : 2 (후진), N : 3 (중립), D : 4 (주행), L : 5 (L단)

    evs.velocity = 18; //km/h
    evs.acceleration = 2; //m/s^2
    evs.accelerator = 0.5; //0~1
    evs.brake = 0.00; //0~1
    evs.steering = -0.3; //-1~1

    for(;;){
        sendto(sockfd, &evs, sizeof(struct ego_vehicle_cmd), 
                MSG_CONFIRM, (const struct sockaddr *) &servaddr,  
                sizeof(servaddr));

        printf("[Sending to Morai..]\n");
        printf("gear (기어)             : ");
        switch (evs.gear) {
        case(0):
            printf("M \n");
            break;
        case(1):
            printf("P (Parking, 주차)\n");
            break;
        case(2):
            printf("R (Reverse, 후진)\n");
            break;
        case(3):
            printf("N (Neutral, 중립)\n");
            break;
        case(4):
            printf("D (Drive, 주행)\n");
            break;
        case(5):
            printf("L (Low, 저속)\n");
            break;
        }
        printf("speed (속력)            : %.2fm/s\n", evs.velocity);
        printf("Accellerator (가속페달) : %.2f\n", evs.accelerator);
        printf("Acceleration (가속도)   : %.2f\n", evs.acceleration);
        printf("Brake (브레이크페달)    : %.2f\n", evs.brake);
        printf("steer (조향입력)        : %.2f\n", evs.steering);
        printf("========================================================================\n");

        usleep(20000); //50 - a Hz
    };
    
    close(sockfd); 
    return 0; 
}