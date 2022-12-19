#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h>

#define SIMIPADDR   "165.246.170.87"
#define SIMCMDPORT  9091 
#define MYIPADDR    "165.246.170.80"
#define MYEGOPORT   16456
#define MAXLINE     256


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

void makeSocket();
void printCtrlRecv (struct ctrl_msg* cmg);

// Driver code 
int main() { 
    int sockfd; 
    char buffer[MAXLINE];
    struct sockaddr_in servaddr, cliaddr; 
        
    // Creating socket file descriptor 
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
        perror("socket creation failed"); 
        exit(EXIT_FAILURE); 
    } 
        
    memset(&servaddr, 0, sizeof(servaddr)); 
    memset(&cliaddr, 0, sizeof(cliaddr)); 
        
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
        
    int len, n; 
    
    len = sizeof(cliaddr);  //len is value/result 
    
    struct ctrl_msg* cmg;

    for(;;){
        n = recvfrom(sockfd, (char *)buffer, MAXLINE,  
                    MSG_WAITALL, ( struct sockaddr *) &cliaddr, 
                    &len);         
        cmg = (struct ctrl_msg*) buffer;
        printCtrlRecv(cmg);


    }
        
    return 0; 
}

void printCtrlRecv (struct ctrl_msg* cmg){
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