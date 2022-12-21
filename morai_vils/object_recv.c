#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h>

    
#define MYIPADDR    "165.246.240.47"
#define MYEGOPORT   8909
#define MAXLINE     2400 

#pragma pack(push, 1) //to read packet correctly

struct objInfo_data{
    uint16_t objId;
    short objType;
    float posX;
    float posY;
    float posZ;
    float heading;
    float sizeX;
    float sizeY;
    float sizeZ;
    float overhang;
    float wheelbase;
    float rear_overhang;
    float velocityX;
    float velocityY;
    float velocityZ;
    float accelX;
    float accelY;
    float accelZ;
    char linkID[38];
};

struct objectInfo{

    char sharp;
    char MoraiObjInfo[12];
    char dollor;
    unsigned int data_length;
    char aux_data[12];

    //data
    struct objInfo_data data[20];

    char zeroD;
    char zeroA;
};
#pragma pack(pop)

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
    
    struct objectInfo* oi;


    for(;;){
        n = recvfrom(sockfd, (char *)buffer, MAXLINE,  
                    MSG_WAITALL, ( struct sockaddr *) &cliaddr, 
                    &len); 

        // printf("length: %d\n", n);
        // for(int i = 0; i < n; i++){
        //     printf("%.2x",buffer[i]);
        // }
        // printf("\n");
        
        //data fitting
        oi = (struct ego_vehicle_status*) buffer;

        //setting trailer
        oi->zeroD = buffer[oi->data_length + 30];
        oi->zeroA = buffer[oi->data_length + 31];

        printf("[Receiving from Morai..]\n");
        // // printf("sharp                   : %c\n", evs->sharp);
        // // printf("MaraiInfo               : %s\n", evs->MaraiInfo);
        // // printf("doller                  : %c\n", evs->dollor);
        printf("data_length             : %d\n", oi->data_length);

        printf("  object ID        Type          posXYZ        heading       sizeXYZ        velXYZ\n");
        printf("----------------------------------------------------------------------------------\n");
        for(int i = 0; i < oi->data_length; i++){
            printf("  %6d", oi->data[i].objId);
            printf("  %6d", oi->data[i].objType);
            printf("  (%4.2f, %4.2f, %4.2f)", oi->data[i].posX, oi->data[i].posY, oi->data[i].posZ);
            printf("   %3.2f", oi->data[i].heading);
            printf("  (%3.2f, %3.2f, %3.2f)", oi->data[i].velocityX, oi->data[i].velocityY, oi->data[i].velocityZ);
            printf("  (%3.2f, %3.2f, %3.2f)", oi->data[i].sizeX, oi->data[i].sizeY, oi->data[i].sizeZ);
        }
        printf("==================================================================================\n");


        // printf("ctrl_mode (제어모드)    : %s\n", (evs->ctrl_mode == 2) ? "auto (오토)" : "keyboard (키보드제어)");
        // printf("gear (기어)             : ");
        // switch (evs->gear) {
        // case(0):
        //     printf("M \n");
        //     break;
        // case(1):
        //     printf("P (Parking, 주차)\n");
        //     break;
        // case(2):
        //     printf("R (Reverse, 후진)\n");
        //     break;
        // case(3):
        //     printf("N (Neutral, 중립)\n");
        //     break;
        // case(4):
        //     printf("D (Drive, 주행)\n");
        //     break;
        // case(5):
        //     printf("L (Low, 저속)\n");
        //     break;
        // }
        
        // printf("speed (속력)            : %.2fm/s\n", evs->signed_velocity);
        // printf("Accel (가속페달)        : %.2f\n", evs->accel);
        // printf("Brake (브레이크페달)    : %.2f\n", evs->brake);
        // printf("steer (조향입력)        : %.2f\n\n", evs->steer);
        
        // printf("Position XYZ            : %5.2fm, \t%5.2fm, \t%5.2fm \n", evs->posX, evs->posY, evs->posZ);
        // printf("Roll Pitch Yaw          : %5.2fdeg, \t%5.2fdeg, \t%5.2fdeg \n", evs->roll, evs->pitch, evs->yaw);
        // printf("Velocity XYZ            : %5.2fkm/h, \t%5.2fkm/h, \t%5.2fkm/h \n", evs->velocityX, evs->velocityY, evs->velocityZ);
        // printf("Acceleration XYZ        : %5.2fm/s^2, \t%5.2fm/s^2, \t%5.2fm/s^2 \n", evs->accelX, evs->accelY, evs->accelZ);
        // printf("map_data_id             : %d\n", evs->map_data_id);
        // printf("link ID                 : %s\n", evs->linkID);


    }
        
    return 0; 
}