#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h>

    
#define MYIPADDR    "127.0.0.1"
#define MYEGOPORT   7505
#define MAXLINE     2400 

#pragma pack(push, 1) //to read packet correctly

struct objectInfo_data{
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
    struct objectInfo_data data[20];

    char zeroD;
    char zeroA;
};
#pragma pack(pop)

inline float headingBaseE2N(float headingE){
    return (headingE > 90) ? headingE - 270 : headingE + 90;
}

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
        oi = (struct objectInfo*) buffer;

        //setting trailer
        oi->zeroD = buffer[oi->data_length + 30];
        oi->zeroA = buffer[oi->data_length + 31];

        int objcnt = 0;

        printf("[Receiving from Morai..]\n");
        // // printf("sharp                   : %c\n", evs->sharp);
        // // printf("MaraiInfo               : %s\n", evs->MaraiInfo);
        // // printf("doller                  : %c\n", evs->dollor);
        // // printf("data_length             : %d\n", oi->data_length);

        printf(" object ID    Type          posXYZ               heading         sizeXYZ                 velXYZ\n");
        printf("---------------------------------------------------------------------------------------------------------\n");

        for(int i = 0; i < oi->data_length / 106; ++i){
            if(oi->data[i].objId != 0) ++objcnt;
            else break;
            printf("   %6u", oi->data[i].objId);
            printf("  %6d", oi->data[i].objType);
            printf("  (%7.2f, %7.2f, %7.2f)", oi->data[i].posX, oi->data[i].posY, oi->data[i].posZ);
            printf("   %+6.2f", (oi->data[i].heading));
            printf("  (%5.2f, %5.2f, %5.2f)", oi->data[i].sizeX, oi->data[i].sizeY, oi->data[i].sizeZ);
            printf("  (%6.2f, %6.2f, %6.2f)", oi->data[i].velocityX, oi->data[i].velocityY, oi->data[i].velocityZ);
            printf("\n");
        }
        printf("---------------------------------------------------------------------------------------------------------\n");
        printf("                                                                                     num of objects : %2d\n", objcnt);
        printf("=========================================================================================================\n");

    }
        
    return 0; 
}