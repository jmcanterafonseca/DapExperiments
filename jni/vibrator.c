/*
 * Copyright (C) 2010 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

//BEGIN_INCLUDE(all) xx
#include <jni.h>
#include <errno.h>
#include <string.h>
#include <android/log.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <strings.h>

#define ON_COMMAND "on"
#define OFF_COMMAND "off"


#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "sensors-library", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "sensors-library", __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "server-vibrator", __VA_ARGS__))


static int makeAddr(const char* name, struct sockaddr_un* pAddr, socklen_t* pSockLen) {
	int nameLen = strlen(name);
	if (nameLen >= (int) sizeof(pAddr->sun_path) - 1) /* too long? */
		return -1;
	pAddr->sun_path[0] = '\0'; /* abstract namespace */
	strcpy(pAddr->sun_path + 1, name);
	pAddr->sun_family = AF_LOCAL;
	*pSockLen = 1 + nameLen + offsetof(struct sockaddr_un, sun_path);
	return 0;
}

static int sendOnCommand(char* command) {

	struct sockaddr_un sockAddr;
	socklen_t sockLen;


	if (makeAddr("com.telefonica.owd.hardware", &sockAddr, &sockLen) < 0)
		return -1;

	int fd = socket(AF_LOCAL, SOCK_STREAM, PF_UNIX);
	if (fd < 0) {
		LOGE("client socket()");
		return -1;
	}

	LOGI("CLIENT %s\n", sockAddr.sun_path+1);

	if (connect(fd, (const struct sockaddr*) &sockAddr, sockLen) < 0) {
		LOGE("client connect()");
		close(fd);
		return -1;
	}

	if (write(fd, command, strlen(command)) < 0) {
		LOGE("client write()");
		close(fd);

		return -1;
	}

	// We read the request Id in the response
	char sreqId[10];
	bzero(sreqId,sizeof(sreqId));

	FILE *socket_stream_in = fdopen(fd, "r");

	// Command can be "on/off"
	fgets(sreqId, sizeof(sreqId) - 1, socket_stream_in);

	LOGI("Request Id returned by the server: %s",sreqId);

	sreqId[strlen(sreqId) - 1] = '\0';

	int reqId = atoi(sreqId);

	LOGI("Request Id returned by the server: %d",reqId);

	fclose(socket_stream_in);
	close(fd);

	return reqId;
}

static int requestVibration(int duration) {
	char message[10];

	bzero(message,sizeof(message));

	sprintf(message,"%s\n%d\n",ON_COMMAND,duration);

	return sendOnCommand(message);
}

static int requestPatternVibration(char* pattern) {
	char message[256];

	bzero(message,sizeof(message));

	sprintf(message,"%s\n%s",ON_COMMAND,pattern);

	return sendOnCommand(message);
}


jint Java_com_telefonica_vibrator_VibratorNative_simple( JNIEnv* env, jobject thiz, jint duration) {
	 LOGI("Vibration Request: duration: %d",duration);

	 int result = requestVibration(duration);

	 if(result == -1) {
		 // An error happened
	 }

	 LOGI("Vibration request sent!!: Result: %d",result);

	 return result;
}


// Multiple vibration
jint Java_com_telefonica_vibrator_VibratorNative_multiple( JNIEnv* env, jobject thiz, jintArray pattern) {

	 jsize len = (*env)->GetArrayLength(env,pattern);

	 LOGI("Multiple Vibration Request: length: %d",len);

	 jint* elements = (*env)->GetIntArrayElements(env,pattern,0);

	 if(len == 1) {
		 requestVibration(elements[0]);
	 }
	 else {
		 	 char serialized[256];
		 	 bzero(serialized,sizeof(serialized));

		 	char number[7];
		 	int j = 0;
		 	 for( j = 0; j < len; j++) {
		 		bzero(number,sizeof(number));

		 		sprintf(number,"%d\n",elements[j]);
		 		strcat(serialized,number);
		 	 }

		 	int result = requestPatternVibration(serialized);

		 	 LOGI("Vibration request pattern sent!!: %s",serialized);
	 }

	 (*env)->ReleaseIntArrayElements(env,pattern,elements,0);
}

void Java_com_telefonica_vibrator_VibratorNative_cancel( JNIEnv* env, jobject thiz,jint reqId) {

}
