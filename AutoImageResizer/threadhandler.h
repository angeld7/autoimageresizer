#include "AutoImageResizer.h"
#include <pthread.h>
#include <dirent.h>
#include <CoreServices/CoreServices.h>

int checkdirectories(location** loc);
void* resizeAndSave(void *vargp);
void syncronizeThreads();
void startCheck(location* loc,char* dir,int recursive);
void fileAddedToDir(ConstFSEventStreamRef streamRef,
                void *clientCallBackInfo,
                size_t numEvents,
                void *eventPaths,
                const FSEventStreamEventFlags eventFlags[],
                const FSEventStreamEventId eventIds[]);
int waitForEvents(location** loc);
void* checkDir(void* dirinfo);
void* runLoop(void* nothing);