#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h>
#include <time.h>

#define SIMIPADDR   "165.246.170.87"    //시뮬레이터 구동 pc ip 주소
#define SIMCMDPORT  9091                //시뮬레이터 cmd host port
#define MYIPADDR    "165.246.240.47"    //나의 ip
#define MYEGOPORT   16456               //제어 메세지를 받는 나의 port 번호
#define BUFSIZE     256

#pragma pack(push, 1) //to read packet correctly
struct ctrl_msg{
    double auto_manual ;
    double brake;
    double estop;
    double gear;
    double speed;
    double steer;
    double tmp1;
    double tmp2;
    double tmp3;
    double tmp4;
};

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

class recvCtrl{
public:
    recvCtrl(){
        // Creating socket file descriptor 
        if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
            perror("socket creation failed"); 
            exit(EXIT_FAILURE); 
        } 

        bzero((char*)&servaddr, sizeof(servaddr));
        bzero((char*)&cliaddr, sizeof(cliaddr));
            
        // Filling server information 
        servaddr.sin_family    = AF_INET; // IPv4 
        servaddr.sin_addr.s_addr = inet_addr(MYIPADDR); 
        servaddr.sin_port = htons(MYEGOPORT); 
            
        // Bind the socket with the server address 
        if ( bind(sockfd, (const struct sockaddr *)&servaddr,  
                sizeof(servaddr)) < 0 ) 
        { 
            perror("bind failed (바인드 에러)"); 
            exit(EXIT_FAILURE); 
        } 

        len = sizeof(cliaddr);  //len is value/result 
    }

    ctrl_msg* recvCtrlMsg(){
        int recvLen;
        recvLen = recvfrom(sockfd, (char *)buffer, BUFSIZE,  
                    MSG_WAITALL, ( struct sockaddr *) &cliaddr, 
                    &len);
        cmg = (struct ctrl_msg*) buffer;
        printCtrlRecv();
        return cmg;
    }

private:
    void printCtrlRecv (){
        printf("[Receiving from ctrl..]\n");
        printf("auto manual             : %.2f\n", cmg->auto_manual);
        printf("Brake (브레이크)        : %.2f\n", cmg->brake);
        printf("EStop                   : %.2f\n", cmg->estop);
        printf("gear (기어)             : ");
        switch ((int)cmg->gear) {
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
        printf("speed (속력)            : %.2fm/s\n", cmg->speed);
        printf("steer (조향입력)        : %.2f\n", cmg->steer);
        printf("time                    : %.2f\n", cmg->tmp4);
        printf("========================================================================\n");
    }

    int sockfd; 
    char buffer[BUFSIZE];
    struct sockaddr_in servaddr, cliaddr; 
    socklen_t len;
    struct ctrl_msg* cmg;
};

class sendSim{
public:
    sendSim(){
        // Creating socket file descriptor 
        if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
            perror("socket creation failed"); 
            exit(EXIT_FAILURE); 
        } 
        
        bzero((char*)&servaddr, sizeof(servaddr));
            
        // Filling server information 
        servaddr.sin_family = AF_INET; 
        servaddr.sin_addr.s_addr = inet_addr(SIMIPADDR); 
        servaddr.sin_port = htons(SIMCMDPORT); 

        resetPreValue(); 
    }

    void sendCtrlMsg(ctrl_msg* cmg){
        evs.gear = cmg->gear; // M : 0, P : 1 (주차), R : 2 (후진), N : 3 (중립), D : 4 (주행), L : 5 (L단)

        //evs.velocity = 10; //km/h
        //evs.acceleration = 0.1; //m/s^2
        evs.accelerator = cmg->speed; //0~1
        evs.brake = cmg->brake; //0~1
        evs.steering = cmg->steer; //-1~1

        sendto(sockfd, &evs, sizeof(struct ego_vehicle_cmd), 
        NULL, (const struct sockaddr *) &servaddr,  
        sizeof(servaddr));
        printSendingMsg();
    }

private:
    void resetPreValue(){
        strncpy(evs.prefix, "#MoraiCtrlCmd$", 14);
        evs.data_length = 23;
        strncpy(evs.aux_data, "\0\0\0\0\0\0\0\0\0\0\0\0", 12);
        evs.zeroD = 0x0D;
        evs.zeroA = 0x0A;

        evs.ctrl_mode = 0; 
        //1 : KeyBoard, 2 : AutoMode, else : dual

        evs.cmdtype = 1; 
        // 1: Throttle제어(accel/brake/steering), 
        // 2: Velocity제어(velocity/steering), 
        // 3: Acceleration제어
    }

    void printSendingMsg(){
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
    }

    int sockfd; 
    struct sockaddr_in servaddr; 
    struct ego_vehicle_cmd evs;
};


// // -lptread
// #include <pthread.h>



int main(){
    // pthread_t pthread[2];

    // pthread_create(&pthread[0], NULL, func1, void*){

    // }

    recvCtrl controlMessage;
    sendSim send2simMessage;


    //send2simMessage.sendCtrlMsg(controlMessage.recvCtrlMsg());

    ctrl_msg* ctrlTmp;
    clock_t recvTime, sendTime = 0;
    for(;;){
        ctrlTmp = controlMessage.recvCtrlMsg();
        recvTime = clock();
        
        if((sendTime - recvTime) < 50) continue;

        send2simMessage.sendCtrlMsg(ctrlTmp);
        sendTime = clock();

    }
}