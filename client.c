#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio_ext.h>
extern int errno;

#define dob_size 9
#define issue_date_size 7
#define card_no_size 17
#define ID_size 13
#define name_size 16
#define bank_size 20
#define email_size 30
#define address_size 41

char ip_address[] = "127.0.0.1";
unsigned short port_no = 2002;

int main()
{
    int soc = socket(PF_INET, SOCK_STREAM, 0); // IP4 TCP connection
    if (soc == -1)
    {
        printf("\nError in socket call(%d)\n", errno);
        exit(1);
    }
    printf("socket file descriptor = %d\n", soc);

    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));

    server_address.sin_family = AF_INET;
    int ip_textToBinary = inet_pton(AF_INET, ip_address, &server_address.sin_addr.s_addr);
    if (ip_textToBinary <= 0)
    {
        printf("\nError in inet_pton call(%d)\n", errno);
        exit(1);
    }
    server_address.sin_port = htons(port_no);

    int connect_ret = connect(soc, (struct sockaddr *)&server_address, sizeof(server_address));
    if (connect_ret == -1)
    {
        printf("Error in connect call (%d)\n", errno);
        exit(1);
    }
    printf("Connected to IP : %s Port : %hu\n", ip_address, port_no);
    char buffer[1024];
    printf("\nChoice option : \n1.Read from card database(record file)and send all cards\n2.Read from stdin\n3.Exit\n");
    int choice;
    scanf("%d", &choice);
    while (choice != 1 && choice != 2 && choice != 3)
    {
        printf("\nNo such option Enter proper option.\n");
        __fpurge(stdin);
        scanf("%d", &choice);
    }
    if (choice == 3)
        exit(0);
     __fpurge(stdin);
    memset(&buffer,0,sizeof(buffer));
    sprintf(buffer,"%d",choice);
    write(soc, buffer, strlen(buffer)); //sending client choice
    if (choice == 1)
    {

        struct card
        {
            short cvv;
            char dob[dob_size];
            char issue_date[issue_date_size];
            char expire_date[issue_date_size];
            char card_no[card_no_size];
            char ID[ID_size];
            char name[name_size];
            char bank[bank_size];
            char email[email_size];
            char address[address_size];
        } obj;

        int fd = open("record", O_RDONLY);
        if(fd<0)
        {
            perror("\nError opening file record\n");
            exit(0);
        }
        while (read(fd, &obj, sizeof(obj)) > 0)
        {
            if (write(soc, &obj, sizeof(obj))!=sizeof(struct card))
            {
                printf("Error in write(%d)\n", errno);
                exit(1);
            }
        }
        close(fd);
    }
    else if (choice == 2)
    {
        printf("\nEnter text (Ctrl + d to end): ");
        while (fgets(buffer, sizeof(buffer), stdin) != NULL)
        {
            if (write(soc, buffer, strlen(buffer)) == -1)
            {
                printf("Error in write(%d)\n", errno);
                exit(1);
            }
        }
    }
    return 0;
}