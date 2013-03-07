#include "threadhandler.h"
#include "fileIO.h"
#include "resizer.h"

struct image {
    location* loc;
    char* imageName;
    char* dir;
};

pthread_t* threads;
pthread_t* directoryCheckers;
pthread_mutex_t checkLock;
int nextThread = 0;
location **locations;

//setup the event threads, do the starting check, then wait for system events
int checkdirectories(location** loc){
    int x;
    locations =   loc;
    threads = (pthread_t *)malloc(sizeof(pthread_t)*number_of_resize_threads);
    pthread_mutex_init (&checkLock, NULL);
    for (x=0; x< number_of_locations;x++) {
        if (locations[x]->check_on_start) {
            startCheck(locations[x], locations[x]->source_path, locations[x]->include_sub_directories);
        }
    }
    syncronizeThreads();
    return waitForEvents(locations);
}

//Use Apple File System Events API to monitor image directories
int waitForEvents(location** loc){
    CFStringRef directory;
    directoryCheckers = (pthread_t *)malloc(sizeof(pthread_t)*number_of_locations);
    int x;
    for (x=0; x<number_of_locations; x++) {
        directory = CFStringCreateWithCString(NULL, loc[x]->source_path, kCFStringEncodingUTF8);
        CFArrayRef pathsToWatch = CFArrayCreate(NULL, (const void **)&directory, 1, NULL);
        FSEventStreamRef stream;
        CFAbsoluteTime latency = 3.0;
        stream = FSEventStreamCreate(NULL,
                                     &fileAddedToDir,
                                     NULL,
                                     pathsToWatch,
                                     kFSEventStreamEventIdSinceNow,
                                     latency,
                                     kFSEventStreamCreateFlagNone
                                     );
        pthread_create(&directoryCheckers[x], NULL, runLoop, stream);
    }
    pthread_join(directoryCheckers[0], NULL);
    return 0;
}

//Run loop to wait for system events
void* runLoop(void* stream){
    FSEventStreamScheduleWithRunLoop(stream, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);
    FSEventStreamStart(stream);
    while(1){
        CFRunLoopRunInMode(kCFRunLoopDefaultMode, 10, 1);
    }
}

//This function is passed to the resizing threads that will resize an image then save it
void* resizeAndSave(void *vargp){
	IplImage* new_img;
    struct image* img = (struct image*) vargp;
    char* imgPath = (char*)calloc(sizeof(char),(strlen(img->dir)+strlen(img->imageName))+1);
    char* dest = (char*)calloc(sizeof(char),(strlen(img->dir)+strlen(img->loc->dest_name)+strlen(img->imageName)+2));
    char* destDir = (char*)calloc(sizeof(char),(strlen(img->dir)+strlen(img->loc->dest_name)+2));
    sprintf(imgPath,"%s%s",img->dir,img->imageName);
    sprintf(dest, "%s%s/%s",img->dir,img->loc->dest_name,img->imageName);
    sprintf(destDir, "%s%s/",img->dir,img->loc->dest_name);
    FILE* f;
    if(!(f = fopen(dest, "r"))){
        if((new_img = resize(imgPath,img->loc))){
            if(saveImage(new_img, destDir, dest, img->loc)){
                perror("Error: Cannot save resized image.");
            }
        }
    }else{
        fclose(f);
    }
    free(dest);
    free(destDir);
    free(imgPath);
    free(img->dir);
    free(img->imageName);
    free(img);
	return 0;
}
//This function is called when a system event is fired.
void fileAddedToDir(ConstFSEventStreamRef streamRef,
                    void *clientCallBackInfo,
                    size_t numEvents,
                    void *eventPaths,
                    const FSEventStreamEventFlags eventFlags[],
                    const FSEventStreamEventId eventIds[])
{
    char **paths = eventPaths;
    
    int x;
    for (x=0; x<numEvents; x++) {
        //printf("%s[%d]\n",paths[x],x);
        char* folderName;
        char* lastFolderName;
        char* tok;
        char* path;
        int i;
        location *loc;
        for (i = 0; i < number_of_locations; i++) {
            if(pthread_equal(pthread_self(), directoryCheckers[i])){
                loc = locations[i];
            }
        }
        tok = malloc(sizeof(char)*(strlen(paths[x])+1));
        path = malloc(sizeof(char)*(strlen(paths[x])+1));
        strcpy(tok,paths[x]);
        strcpy(path,paths[x]);
        folderName = strtok(tok,"/");
        while (folderName != NULL) {
            lastFolderName = folderName;
            folderName = strtok(NULL, "/");
        }
        if (strcmp(lastFolderName, loc->dest_name)&&(loc->include_sub_directories||!strcmp(loc->source_path,path))) {
            startCheck(loc, path, loc->include_sub_directories);
        }
        free(tok);
        free(path);
    }
    syncronizeThreads();
}
//This function is used to loop through the folders to look for images. 
void startCheck(location* loc, char* dir, int recursive){
    struct image* img;
    DIR *dp;
    struct dirent* file;
    dp = opendir(dir);
    if(dp){
        while ((file=readdir(dp))) {
            if(nextThread == number_of_resize_threads){
                syncronizeThreads();
            }
            if(file->d_type == DT_REG && file->d_name[0] != '.'){
                img = malloc(sizeof(struct image));
                img->loc = loc;
                img->imageName = (char*)malloc(sizeof(char)*strlen(file->d_name)+1);
                img->dir = (char*)malloc(sizeof(char)*strlen(dir)+1);
                strcpy(img->imageName,file->d_name);
                strcpy(img->dir,dir);
                pthread_create(&threads[nextThread], NULL, resizeAndSave, img);
                nextThread++;
            }
            if(recursive){
                if(file->d_type == DT_DIR && file->d_name[0]!='.' && strcmp(file->d_name, loc->dest_name)){
                    char* newDir;
                    newDir = (char*)malloc(sizeof(char)*(file->d_namlen+strlen(dir)+2));
                    sprintf(newDir,"%s%s/",dir,file->d_name);
                    startCheck(loc,newDir,recursive);
                    free(newDir);
                }
            }
        }
        closedir(dp);
    }
}
//Called when maximum resize thread threshold is reached.
void syncronizeThreads(){
    int x;
    for (x=0; x<number_of_resize_threads; x++) {
        pthread_join(threads[x], NULL);
    }
    nextThread = 0;
}
