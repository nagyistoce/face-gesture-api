#include <iostream>

#define DEBUG_MODE
#define RESOURCES_PATH "../resources/"
#define HAAR_CASCADE_PATH (string(RESOURCES_PATH) + "haarCascades/")
#define FACE_CASCADE (string(HAAR_CASCADE_PATH) + "haarcascade_frontalface_alt.xml")

#define BUFFER_SIZE 4
#define MHI_DURATION 1
#define MAX_TIME_DELTA 0.5
#define MIN_TIME_DELTA 0.05
