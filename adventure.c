/*
author:Biran Shah
description: Adventure program interface that uses the created rooms from buildrooms.c to create an Adventure gameplay simulation
this program also provides and writes the current time when inputted into a file called "currentTime.txt"
*/
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

//create room struct
struct room {
    char name[15];
    int count;
    struct room* connections[6];
    char type[20];
};

char roomSteps[150];
//var for the while loop
int a;
//tally num steps taken
int steps;
//stores current room name
char currentRoom[20];
//track room index in struct for gameplay
int roomTrack;
int winner;
int secondT;
//struct room index for game winner
char roomPast[200];
//for mutex
pthread_mutex_t first = PTHREAD_MUTEX_INITIALIZER;
pthread_t second;

//struct of rooms to hold all gameplay rooms
struct room adventure[7];
//fx prototypes
void getRecentDir(char* mostRecentDir);
void createRooms(char* mostRecentDir);
int userGame();
void* printTimeFile();


int main()
{
    //set char arr aka str to store most recent room folder name
    char mostRecentDir[256];
    memset(mostRecentDir, '\0', sizeof(mostRecentDir));
    //send this arr/str to this fx that stores recent room folder name in this arr 
    getRecentDir(mostRecentDir);
//  printf("Newest entry found is: %s\n", mostRecentDir);
    //use the most recent dir name to create rooms for gameplay
    createRooms(mostRecentDir);

    memset(currentRoom, '\0', sizeof(currentRoom));
    memset(roomSteps, '\0', sizeof(roomSteps));
    memset(roomPast, '\0', sizeof(roomPast));

    //get starting room
    //init array to hold path taken
    //init int to tally steps taken



    //a is for while loop status, steps is for steps taken
    a =10;
    steps =0;
    roomTrack=0;
    int h;
    //loop is to get the starting room for game play
    for (h=0;h<7;h++)
    {
        if(strcmp(adventure[h].type, "START_ROOM")==0)
        {
            //applied from: https://stackoverflow.com/questions/10063222/freeing-strings-in-c
            //explanation: 1st comment on question says use strncpy than strcpy to avoid buffer overflows
            //..bc I know room name not more than 7 or 8 chars, I put a size of 10 to be safe and both arrs have enough space.
            strncpy(currentRoom, adventure[h].name, 10);
            roomTrack = h;
//           printf("room track in game start is: %d\n", roomTrack);
//            printf("starting room is: %s\n",adventure[h].name);
//            printf("current room is: %s \n", currentRoom );
        }
        //save end room struct index location
        if(strcmp(adventure[h].type, "END_ROOM")==0)
        {
            winner = h;
//           printf("end room is: %s\n", adventure[winner].name);
        }
    }
     //lock thread 
	pthread_mutex_lock(&first);
    //create second time thread (used lecture 2.3)
    secondT = pthread_create(&second, NULL, (void*)&printTimeFile, NULL);

    while (a == 10)
    {
        //print game text
            printf("CURRENT LOCATION: %s\n", currentRoom);
            printf("POSSIBLE CONNECTIONS: ");
        //this loop prints the current room connections 
        int p;
        for (p=0; p<adventure[roomTrack].count;p++)
        {   //print all but last connection bc comma vs period and \n
            if (p < adventure[roomTrack].count-1)
            {
                printf("%s, ", adventure[roomTrack].connections[p]->name);
            }
            else
            {   //means last connection aka print period + \n
               printf("%s.\n", adventure[roomTrack].connections[p]->name );
                
            }
            
        }
        printf("WHERE TO? >");
        //go to user input fx for future game play steps
        userGame();

    }

    return 0;
}   


/*  this fx saves the most recent rooms folder into a
string I declared and set in main() bc easy for me to send this string around to diff fx's
*/
void getRecentDir(char* mostRecentDir) 
{
  int newestDirTime = -1; // Modified timestamp of newest subdir examined
  char targetDirPrefix[32] = "shahb.rooms."; // Prefix we're looking for
 // char newestDirName[256]; // Holds the name of the newest dir that contains prefix
 // memset(newestDirName, '\0', sizeof(newestDirName));

  DIR* dirToCheck; // Holds the directory we're starting in
  struct dirent *fileInDir; // Holds the current subdir of the starting dir
  struct stat dirAttributes; // Holds information we've gained about subdir

  dirToCheck = opendir("."); // Open up the directory this program was run in

  if (dirToCheck > 0) // Make sure the current directory could be opened
  {
    while ((fileInDir = readdir(dirToCheck)) != NULL) // Check each entry in dir
    {
      if (strstr(fileInDir->d_name, targetDirPrefix) != NULL) // If entry has prefix
      {
        // printf("Found the prefex: %s\n", fileInDir->d_name);
        stat(fileInDir->d_name, &dirAttributes); // Get attributes of the entry

        if ((int)dirAttributes.st_mtime > newestDirTime) // If this time is bigger
        {
          newestDirTime = (int)dirAttributes.st_mtime;
          memset(mostRecentDir, '\0', sizeof(mostRecentDir));
          strcpy(mostRecentDir, fileInDir->d_name);
         // printf("Newer subdir: %s, new time: %d\n",
          //       fileInDir->d_name, newestDirTime);
        }
      }
    }
  }
  closedir(dirToCheck); // Close the directory we opened
  return;
 // printf("Newest entry found is: %s\n", mostRecentDir);
}

/*
1. pass in most recent directory string, then open room folder of most recent dir as done in reading 2.4
2. use a loop for to iterate through each room file (7x bc 7 rooms) and save each file contents into same adventure struct as buildrooms
3. using fgets and sscanf, read each line per room file, extract connections and room name into each room struct, and close file and exit directory.
*/
void createRooms(char* mostRecentDir)
{
   //set new str for saving file path from most recent dir 
    char roomDir[75] = "./";
    //combine recent dir name to the file path string bc need ./ in front to access folder
    strcat(roomDir, mostRecentDir);
//    printf("I am in creatRooms fx\n");
    //
    DIR* dirStruct;
    struct dirent *files;
    //set struct to open most recent rooms folder to open and read file data
    dirStruct = opendir(roomDir);
    int roomIndex=0;
    //first two lines used from reading 2.4. this lets me go through all files in my rooms folder
     if (dirStruct > 0) 
    {
        while ((files = readdir(dirStruct)) != NULL) // Check each entry in dir
        {
//            printf("I am in room directory\n");
			if (!strcmp(files->d_name, ".."))
			{
				continue;
			}
			if (!strcmp(files->d_name, "."))
			{
				continue;
			}
            strcpy(adventure[roomIndex].name, files->d_name);
//            printf("%s\n", adventure[roomIndex].name);
            roomIndex++;
            }
        }   
            //init file pointer for opening and reading rooms
          FILE* filePointer;

            

        //after first while loop saves all room names in adventure, now make for loop w/ 7 iterations to open up each room and access its contents
        //to read it to save into adventure[x] room struct
          int x;
          for (x=0; x<7; x++)
          {
//              printf("%s\n", adventure[x].name);
              int read;
              ///make new room file path str to open each room file
              char roomFile[50]; 
              memset(roomFile, '\0', sizeof(roomFile));
              //complete room file address
              strcpy(roomFile, "./");
              strcat(roomFile, mostRecentDir);
              strcat(roomFile, "/");
              //then add each room file name.
              strcat(roomFile, adventure[x].name);
            //open each room file for read only
            filePointer = fopen(roomFile, "r");

        //
        //set line buffer string
        char buffer[75];
        memset(buffer, '\0',20);
        //get a line and store it into buffer
        while (fgets(buffer, sizeof(buffer), filePointer) !=NULL)
        {

            
            //explanation: bc all file lines contain 3 fields, sscanf lets me assign each field to a str 
            //then each line in the buffer string can be checked for field 2 that indicates line is for room type
            char spam[20];
            char ID[20];
            char room_type[20];

            //save each line component into 3 strings (all lines have 3 strings)
            sscanf(buffer, "%s %s %s", spam, ID, room_type);
 //           printf(" %s %s %s \n", spam,ID,room_type);
            //if a string has TYPE it means it's the last line of each file...
            //... so save 3rd part of that line in room type
            if(strcmp(ID, "TYPE:")==0)
            {
                strcpy(adventure[x].type, room_type);
            }

            char copy[20];
//            printf("line for room %s is %s %s %s\n", adventure[x].name, spam, ID, room_type);
            //if first string word == connection, then proceed to assign room connections
            if(strcmp(spam, "CONNECTION") == 0)
            {
//                printf(" connection: %s for room %s\n", room_type, adventure[x].name);
                //used a second string for name convenience

                strcpy(copy, room_type);
                int b;
                //needed this loop bc I need to find right index location in adventure struct to assign correct address 
                for (b=0; b<7; b++)
                {
                    //get connection room index location
                    if (strcmp(copy, adventure[b].name) == 0)
                    {
                        //assign connection sameway done in the buildrooms.c file
                        adventure[x].connections[adventure[x].count] = &adventure[b];
                        adventure[x].count++;
                    }
                }
            }

        }   //end reading stream for current room

            fclose(filePointer);    
    }
        //close directory of shahb.rooms.pid once done 
        closedir(dirStruct);
}


//fx gets user gameplay input, if matches room then progresses game to next room and if matches end room then win, 
//if matches time, starts writing time on second thread, if no input match: error
int userGame()
{
    //set string for input
    char bufferLine[20];
    memset(bufferLine, '\0',sizeof(bufferLine));
    //read input
    fgets(bufferLine, 20, stdin);
    //fgets add \n end of string so this removes it (https://stackoverflow.com/a/2396637)
    bufferLine[strlen(bufferLine)-1]='\0';

//    printf("user input userinpT%sT", bufferLine);

    //to get current time, used the example from the suggested strftime fx https://linux.die.net/man/3/strftime
    if(strcmp(bufferLine, "time" )==0 )
    {
            
        //unlocks main thread
		pthread_mutex_unlock(&first); 
		//lets thread end
		pthread_join(second, NULL);

        //this is to read the current time and print on screen
        FILE *readTime;
        char readBuf[80];
        memset(readBuf, '\0', sizeof(readBuf));
        //set file stream and open it for reading
        readTime = fopen("currentTime.txt", "r");
        //get file content aka one line of time and print
        fgets(readBuf, sizeof(readBuf), readTime);
        printf("\n%s\n", readBuf);
		//lock main thread
		pthread_mutex_lock(&first); 

        return a;            
    }

//    printf("%s\n", adventure[roomTrack].name);
    int p;
    for (p=0; p<adventure[roomTrack].count; p++)
    {  
//        printf("%s", adventure[roomTrack].connections[p]->name);
        if(strcmp(bufferLine, adventure[roomTrack].connections[p]->name) ==0 )
//          if(strcmp("mad", adventure[roomTrack].connections[p]->name) ==0 )
        {
            printf("\n\n");

             int h;
            //loop is to get connecting room index location in adventure struct
            for (h=0;h<7;h++)
            {
                if(strcmp(bufferLine, adventure[h].name)==0)
                {
                    //tried something new here from: https://stackoverflow.com/questions/10063222/freeing-strings-in-c
                    //explanation: 1st comment on question says use strncpy than strcpy to avoid buffer overflows
                    //..bc I know room name not more than 7 or 8 chars, I put a size of 10 to be safe and both arrs have enough space.
                    strncpy(currentRoom, adventure[h].name, 10);
                    roomTrack = h;

                    steps++;
                    strncpy(currentRoom, bufferLine, 10);
                    strcat(roomPast, currentRoom);
                    strcat(roomPast, "\n");
                }
            }    
//            printf("string matched with room outbound\n");          
//            printf("room track is now: %d\n", roomTrack);
//            printf("room track in str match is: %d\n", roomTrack);
            if(strcmp(adventure[roomTrack].type,"END_ROOM" ) ==0 )
            {
                printf("\nYOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\n");
                printf("YOU TOOK %d STEPS. YOUR PATH TO VICTORY WAS: \n%s", steps, roomPast);
                a = 11;
                pthread_mutex_unlock(&first);
                return a;
            }            
        return a;  
        }       
    }
        printf("\nHUH? I DON’T UNDERSTAND THAT ROOM. TRY AGAIN.\n\n");
        return a;
}
//if user enters time, this fx gets current time and writes it to currentFile.txt.
//
void* printTimeFile()
{
    //lock thread
    pthread_mutex_lock(&first);
    //used linked strftime as reference
    //set string to store time
    char buf[80];
    //init time 
    time_t now;
    struct tm *timeStruct;
    //get local time
    now = time(NULL);
    timeStruct = localtime(&now);
    //save local time into time buffer
    strftime(buf, sizeof(buf), "%I:%M%P, %A, %B %d, %Y", timeStruct);
//       printf("\n%s\n", buf);
    FILE *writeTime;    //set up file ptr
    //open file stream and use w+ to overwrite old time file if already exists
    writeTime = fopen("currentTime.txt", "w+");
    //print time from string into file and close it
    fprintf(writeTime, "%s\n", buf);
    fclose(writeTime);

    //unlock thread
	pthread_mutex_unlock(&first);

}