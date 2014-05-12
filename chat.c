/* SIMPLE CHAT SERVER/CLIENT FOR LINUX
 * 
 * by Nestel
 *
 * APRIL/MAY 2014
 */

#include <stdio.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>

void chooseColor(char *msgCol, char *recvCol)
{
    int volba;
    
    printf("Choose the font color of your messages:\n\t1. Blue\n\t2. Red\n\t3. Black\n\t"
        "4. White\n\t5. Green\n\t6. Yellow\n\t7. Purple\n\t8. Cyan\n\t9. Default\n\t0. Keep color\nYour choice: ");
    scanf("%i", &volba);
    
    switch(volba)
    {
        case 1:
            strcpy(msgCol, "\e[0;34m");
            break;
        case 2:
            strcpy(msgCol, "\e[0;31m");
            break;
        case 3:
            strcpy(msgCol, "\e[0;30m");
            break;
        case 4:
            strcpy(msgCol, "\e[1;37m");
            break;
        case 5:
            strcpy(msgCol, "\e[0;32m");
            break;
        case 6:
            strcpy(msgCol, "\e[1;33m");
            break;
        case 7:
            strcpy(msgCol, "\e[0;35m");
            break;
        case 8:
            strcpy(msgCol, "\e[0;36m");
            break;
        case 9:
            strcpy(msgCol, "\e[0m");
            break;
        case 0:
            break;            
    }
    
    printf("Choose the font color of recieved messages:\n\t1. Blue\n\t2. Red\n\t3. Black\n\t"
        "4. White\n\t5. Green\n\t6. Yellow\n\t7. Purple\n\t8. Cyan\n\t9. Default\n\t0. Keep color\nYour choice: ");
    scanf("%i", &volba);
    
    switch(volba)
    {
        case 1:
            strcpy(recvCol, "\e[0;34m");
            break;
        case 2:
            strcpy(recvCol, "\e[0;31m");
            break;
        case 3:
            strcpy(recvCol, "\e[0;30m");
            break;
        case 4:
            strcpy(recvCol, "\e[1;37m");
            break;
        case 5:
            strcpy(recvCol, "\e[0;32m");
            break;
        case 6:
            strcpy(recvCol, "\e[1;33m");
            break;
        case 7:
            strcpy(recvCol, "\e[0;35m");
            break;
        case 8:
            strcpy(recvCol, "\e[0;36m");
            break;
        case 9:
            strcpy(recvCol, "\e[0m");
            break;
        case 0:
            break;            
    }   
}

int server()
{
    int socketDesc, newSocket, portNum, clientPort, c, volba = 1000, i = 0;
    struct sockaddr_in server, client;
    char message[2000] = "", *clientIp, *reply = "lol", myNick[20], clientNick[20], msgCol[10], recvCol[10], *resetCol = "\e[0m";
    
       //creates socket
    socketDesc = socket(AF_INET, SOCK_STREAM, 0);
    
    if ( socketDesc == -1 ) //if socket wasn't created
    {
        puts("Socket couldn't be created !");
        return -1;
    }   

    //get port number and nickname
    printf("Enter port number: ");
    scanf("%i", &portNum); 
    printf("Enter your nickname: ");
    scanf("%s", myNick);
    chooseColor(msgCol, recvCol);
    //set server params
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( portNum );
    
    //bind socket
    if ( bind( socketDesc, (struct sockaddr*) &server, sizeof(server)) < 0 ) //if socket wasn't bound
    {
        puts("Bind failed !");
        return -1;
    }
    
    //listen for incoming connections
    listen( socketDesc, 3 );
    
    puts("Waiting for incoming connections . . .");
    
    //idk what this does 
    c = sizeof(struct sockaddr_in);
    
    //return value probably potrebna only for  threading
    newSocket = accept(socketDesc, (struct sockaddr*)&client, (socklen_t*)&c );  
    
    //send nick to client
    send(newSocket, myNick, strlen(myNick), 0);
    puts("Waiting for client to enter his/her nickname . . .");
    
    //recv nick from client
    recv(newSocket, clientNick, 2000, 0);
    printf("%s connected !\n", clientNick);
    
    //enter first message (doesn't really work, but potrebne)
    printf("\r""%s %s: ", msgCol, myNick);
    fgets(message, sizeof(message), stdin);
    
    //the chat itself
    while ( strncmp(message, "<end>", 5) != 0 && strncmp(reply, "<end>", 5) != 0 ) //until recieved message or sent isnt <end>
    {                   
        send( newSocket, message, strlen(message), 0 );
       
        printf("%s" "\nWaiting for a reply . . .", resetCol);
        
        //delete all reply chars ( strcpy(reply, "") didn't do the trick i needed )
        reply = (char*) calloc (2000, sizeof(char)); 
        
        //probably nepotrebne i don't feel like testing if potrebne anymore
        fflush(stdout);
        
        recv( newSocket, reply, 2000, 0);  
        
        //Deletes "Waiting for a reply" message and prints recieved message
        printf("\r                                       \r" "%s %s: %s \n %s", recvCol, clientNick, reply, resetCol);  
        
        if ( i == 0)
        {
            system("clear");
            printf("\rChatting with %s !\n\n", clientNick);
            i++;
        }
        
        //prints user nick to prepare for message .. ?entry? :D                          
        printf("\r" "%s %s: ", msgCol, myNick);
        fgets(message, sizeof(message), stdin);
        
        while ( strncmp(message, "<color>", 7 ) == 0 )
        {
            chooseColor(msgCol, recvCol);
            printf("\r" "%s %s: ", msgCol, myNick);
            fgets(message, sizeof(message), stdin);
        }
    }
    //since sned function is in the beginning of while the "<end>" message has
    //to be sent after cycle break
    send( newSocket, message, strlen(message), 0 );
    
    // Client IP: %s | Client port: %i\n", clientIP, ntohs(client.sin_port));
       
    return 0;    
}

int client()
{
    int socketDesc, portNum, i = 0;
    struct sockaddr_in server;
    char message[2000], *reply = "lol", ip[20], myNick[20], clientNick[20], msgCol[10], recvCol[10], *resetCol = "\e[0m";
    
    socketDesc = socket(AF_INET, SOCK_STREAM, 0);
    
    if ( socketDesc == -1 )
    {
        printf("Socket couldn't be created !\n");
        return -1;
    }
    
    
    printf("Enter ip address: ");
    scanf("%s", ip);
    printf("Enter port number: ");
    scanf("%i", &portNum);
    printf("Enter your nickname: ");
    scanf("%s", myNick);
    chooseColor(msgCol, recvCol);
    //set server properties
    server.sin_family = AF_INET;  
    server.sin_addr.s_addr = inet_addr(ip);   
    server.sin_port = htons( portNum );
    
    if (connect(socketDesc , (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        puts("connect error");
        return -1;
    }
    puts("Waiting for server nickname . . .");
    recv(socketDesc, clientNick, 2000, 0);        
    printf("\nConnected to %s !\n", clientNick);  
    
    send(socketDesc, myNick, strlen(myNick), 0);
    printf("\nWaiting for a reply . . .");
    
    while ( strncmp(message, "<end>", 5) != 0 && strncmp(reply, "<end>", 5) != 0 )//strcmp(message, "end") != 0 || strcmp(reply, "end") != 0 )
    {   
        reply = (char*) calloc (2000, sizeof(char)); 
        fflush(stdout);
        
        recv( socketDesc, reply, 2000, 0);
        
        printf("\r                                       \r" "%s %s: %s \n", recvCol, clientNick, reply);      
        
        printf("\r" "%s %s: ", msgCol, myNick);
        fgets(message, sizeof(message), stdin);
        
        while ( strncmp(message, "<color>", 7 ) == 0 )
        {
            chooseColor(msgCol, recvCol);
            printf("\r" "%s %s: ", msgCol, myNick);
            fgets(message, sizeof(message), stdin);
        }
        
        send( socketDesc, message, strlen(message), 0 );
        
        if ( i == 0)
        {
            system("clear");
            printf("\rChatting with %s !\n\n", clientNick);
            i++;
        }
        printf("\nWaiting for a reply . . .");
    }
    
    close(socketDesc);    
}

int main()
{
    int socketDesc, newSocket, portNum, clientPort, c, volba = 1000, i = 0;
    char message[2000] = "", *clientIp, *reply = "lol", myNick[20], clientNick[20];
    
    //intro
    printf("Welcome to nstl's chat !\n\n");
    
    printf("Do you want to be the server or the client ?\n\t1. Server\n\t2. Client\nYour choice: ");
    scanf("%i", &volba);
    
    if ( volba == 1 )
    {
        return server();
    }
    else if ( volba == 2 )
    {
        return client();
    }
    
    return 0;
}


//SPRAVIT SERVER A CLIENT V JEDNOM, CEZ VOLBU !! 
