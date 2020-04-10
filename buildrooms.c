/*
author: Biran Shah
program 2: shahb.buildrooms file. This creates 7 randomized unique rooms and writes them into a directory that can be used for the adventure game.
-replaced all bool fx with int type so return 0 = true and return 1 = false
*/



#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//function prototypes
void setRooms();
int IsGraphFull();
void AddRandomConnection();
int GetRandomRoom();
int CanAddConnectionFrom(int x);
int ConnectionAlreadyExists(int x,int y);
void ConnectRoom(int x, int y);
int IsSameRoom(int x, int y);
char *makeFolder();
void writeRooms(char* directory);

struct room
{
  char* name;
  char* roomType;
  int numOutboundConnections;
  struct room* outboundConnections[6];
  int num;
};

//arr for all possible room names and types
char* allRooms[10] = {"mad", "bad", "sad", "dad", "rad", "add", "had", "pad", "lad", "fad"};
//char* roomType[3] = {"START_ROOM", "END_ROOM", "MID_ROOM"}; 
//arr of room structs to hold all the rooms that'll be used 
struct room gameRooms[7];


int main()
{
//init rand num gen
srand(time(NULL));

//initialize 7 rooms
setRooms();


// Create all connections in graph
while (IsGraphFull() == 1)
{
  AddRandomConnection();
}

//save each room to a file. make dir in this fx first, then save each room in this dir
char* roomFiles = makeFolder();
writeRooms(roomFiles);


/* for debugging


int i;
  for(i = 0; i < 7; i++)
  {
    printf("ROOM NAME: %s\n", gameRooms[i].name);

    int j;
    //Print the connections for each file. We use j+ 1 because we want to print connections starting at 1, not 0
    for(j = 0; j < gameRooms[i].numOutboundConnections; j++)
    {
      printf("CONNECTION %d: %s\n", j+1, gameRooms[i].outboundConnections[j]->name);
    }
    printf("ROOM TYPE: %s\n", gameRooms[i].roomType);
    printf("number connections: %d\n", gameRooms[i].numOutboundConnections);
 }
 */
return 0;
}
//create directory with my onid for room files to be saved
char *makeFolder()
{
    int pid = getpid();
    //set folder name
    char* pathway = "shahb.rooms.";
    //allocate size for folder name
    char *address = malloc(30);
    sprintf(address, "%s%d", pathway, getpid());
    //give permission to mkdir
    mkdir(address, 0700);
    return address;
}

void writeRooms(char* directory)
{
    //change curr working dir to new folder dir
    chdir(directory);
  
    //set up loop to write each room file
    int i;
    for(i = 0; i < 7; i++)
    {
        //set file ptr and open aka write a file named on each game room name
        FILE *filePointer = fopen(gameRooms[i].name, "w");
        //print game room name into its respective file
        fprintf(filePointer, "ROOM NAME: %s\n", gameRooms[i].name);
        //print outbound connections per room. j+1 bc number of connections #1 is not 0. 
        int j;
        for(j = 0; j < gameRooms[i].numOutboundConnections; j++)
        {
            fprintf(filePointer, "CONNECTION %d: %s\n", j+1, gameRooms[i].outboundConnections[j]->name);
        }
        fprintf(filePointer, "ROOM TYPE: %s\n", gameRooms[i].roomType);
        fclose(filePointer);
    //printf("number connections: %d\n", gameRooms[i].numOutboundConnections);
 }
}


/*
fx definition: 
    1. get 7 unique numbers from the 10 possible rooms and store them into an array called digits.
    2. then assign the array of room structures to corresponding index values from the allRooms array using the digits array values
    3. init the outbound connections to 0
    4. set the first room to start type, last to end room, and rest as mid room types
*/
void setRooms() 
{

    //declare array that'll hold 7 random rooms to be initialized
    int digits[7];
    //fill in digits arr with 7 random unique numbers 1-10
    int index;
    for(index = 0; index < 7; index++)
    {
        digits[index] = rand()%10;
    } 

    //check for unique index number so no room repeats 
    int checker;
    for(checker = 0; checker < 7; checker++)
    {
        int index;
        for(index = 0; index < 7; index++)
        { 
            if(checker == index) 
            break; 
            if(digits[checker] == digits[index])
            {
                digits[checker] = rand()%10;
                checker--; 
            } 
        } 
    }


    //set each room outbound connections to 0 and name each room
    int i;
    for(i = 0; i < 7; i++)
    {
        gameRooms[i].numOutboundConnections = 0;
        //logic: explained in fx def number 2.
         gameRooms[i].name = allRooms[digits[i]];
         gameRooms[i].num=i;
        
   //printf("room name:%s\t number connections:%d\tnum:%d\n", gameRooms[i].name, gameRooms[i].numOutboundConnections, gameRooms[i].num);

    }

    //set room types, 1st room = start, last = end, rest = mid
    //used from reading 2.1 for strcpy
    gameRooms[0].roomType = calloc(50, sizeof(char));
    strcpy(gameRooms[0].roomType, "START_ROOM");
 //   printf("%s\n", gameRooms[0].roomType);
        
    //last room = end room
    gameRooms[6].roomType = calloc(50, sizeof(char));
    strcpy(gameRooms[6].roomType, "END_ROOM");
//    printf("%s\n", gameRooms[6].roomType);

    //middle 5 rooms = mid rooms 
    int j;
    for (j=1; j<6; j++)
    {
        gameRooms[j].roomType = calloc(50, sizeof(char));
        strcpy(gameRooms[j].roomType, "MID_ROOM");
 //       printf("%s\n", gameRooms[j].roomType);
    }

}

/* Returns 0 if all rooms have 3 to 6 outbound connections, returns 1 if not.
*/
int IsGraphFull()  
{
  int i;
  int tally=0;
  for (i=0; i<7;i++)
  {
      if( gameRooms[i].numOutboundConnections >=3 && gameRooms[i].numOutboundConnections <=6)
      tally=tally+1;
  }
  if (tally == 7)
  {
      return 0;
  }
  else
  return 1;
}

// Adds a random, valid outbound connection from a Room to another Room
void AddRandomConnection()  
{
    //tried using variables as structs but failed so I used an int that represents the array of structs, allRooms, index values;
  int A; 
  int B;

  while(0)
  {
    A = GetRandomRoom();

    if (CanAddConnectionFrom(A) == 0)
      break;
  }

  do
  {
    B = GetRandomRoom();
  }
  // while( CanAddConnectionFrom(B) == 1 || IsSameRoom(A,B)==0 || ConnectionAlreadyExists(A, B) == 0);
  //don't need issameroom fx bc if A == B means same room index value aka same room
 while( CanAddConnectionFrom(B) == 1 || A == B || ConnectionAlreadyExists(A, B) == 0);
  ConnectRoom(A, B);  
  ConnectRoom(B, A);
  return;  
}

// Returns room number index from arr struct among 7 possible rooms
int GetRandomRoom()
{
    int x = rand()%7;
    return x;
}

// Returns 0 aka true if a connection can be added from Room < 6 outbound connections, returns 1 aka false otherwise
int CanAddConnectionFrom(int x) 
{
  if (gameRooms[x].numOutboundConnections < 6)
  {
      return 0;
  }
  else
  {
      return 1;
  }
  
}
//Returns true if a connection from Room x to Room y already exists, false otherwise
int ConnectionAlreadyExists(int x, int y)
{
    int i;
    for (i=0; i<gameRooms[y].numOutboundConnections; i++)
    {
        if (gameRooms[x].num == gameRooms[y].outboundConnections[i]->num)
        return 0;
    }
    return 1;
    
}

// Connects Rooms x and y together, does not check if this connection is valid
void ConnectRoom(int x, int y) 
{
    //used 2.1 reading to set a room connection
  gameRooms[x].outboundConnections[gameRooms[x].numOutboundConnections]= &gameRooms[y];
  gameRooms[x].numOutboundConnections++;
  return;
}


