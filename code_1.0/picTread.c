#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

void* takePic(void* args);

int main(int argc, const char **argv)
{
	pthread_t pics;
	pthread_create(&pics,NULL,takePic,NULL);
	printf("Taking Picture!\n");
	pthread_join(pics,NULL);
	printf("Picture Done!");
	return 0;
}

void* takePic(void* args){
	system("./pic.sh");
	return NULL;
}