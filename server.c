#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <stdio_ext.h>
#define dob_size 9
#define issue_date_size 7
#define card_no_size 17
#define ID_size 13
#define name_size 16
#define bank_size 20
#define email_size 30
#define address_size 41
extern int errno;
unsigned short port_no = 2002;
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
int main()
{
    int soc = socket(AF_INET, SOCK_STREAM, 0);
    if (soc == -1)
    {
        printf("\nError in socket call(%d)\n", errno);
        exit(1);
    }

    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(port_no);

    int ret = bind(soc, (struct sockaddr *)&server_address, sizeof(server_address));
    if (ret == -1)
    {
        printf("Error in bind call(%d)\n", errno);
        exit(1);
    }

    ret = listen(soc, 10);
    /*
    int listen(int sockfd, int backlog);
    The backlog argument defines the maximum length to which the queue of pending connections for sockfd may grow.
    */
    if (ret == -1)
    {
        printf("Error in ret call(%d)\n", errno);
        exit(1);
    }
    printf("\n Server is Live...\n");
    struct sockaddr_in from_address;
    char buffer[4096];
    int fromsize = sizeof(struct sockaddr_in);
    int connected_socket;
    int client_choice = 2; // initial_choice of client is normal text
    int first_time = 0;

    while (1)
    {
        connected_socket = accept(soc, (struct sockaddr *)&from_address, &fromsize);
        printf("\nGot a Connection..\n");
        if (connected_socket == -1)
        {
            printf("Error in accept call (%d)\n", errno);
            exit(1);
        }
        while (1)
        {
            if (client_choice == 2) // normal text choice
            {
                memset(&buffer, 0, sizeof(buffer));
                __fpurge(stdin);
                ret = read(connected_socket, buffer, sizeof(buffer));
                if (strlen(buffer) == 0)
                {
                    printf("\n..Connection Terminated..\n");
                    exit(0);
                }
                if (ret == -1)
                {
                    printf("Error in read call (%d)\n", errno);
                    exit(1);
                }
                if (first_time == 0) // the first data server will get is choice of the client 1-card data 2-data from stdin
                {
                    if (buffer[0] == '1' && strlen(buffer) == 1) // check if  choice is for card or not
                        client_choice = 1;                       // card choice
                    else
                        first_time = 1;
                }
                else
                {
                    printf("\n\nReceived Data :\n%s", buffer);
                }
            }
            else if (client_choice == 1) // card choice
            {
                memset(&obj, 0, sizeof(obj));
                ret = read(connected_socket, &obj, sizeof(obj));
                if (ret == -1)
                {
                    printf("Error in read call (%d)\n", errno);
                    exit(1);
                }
                if (strlen(obj.name) == 0)
                {
                    printf("\n..Connection Terminated..\n");
                    exit(0);
                }
                printf("\n\nReceived Data :\n");
                printf("\nName: %s\nCard_no: %s\nDOB: %s\nBank: %s\nAddress: %s\n", obj.name, obj.card_no, obj.dob, obj.bank, obj.address);
            }
        }
    }

    return 0;
}