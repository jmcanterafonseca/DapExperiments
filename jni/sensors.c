/**
 *  Glue code for sensors
 *
 *
 */

#include <jni.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

#include <android/sensor.h>
#include <android/log.h>
#include <android/looper.h>

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "sensors-library", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "sensors-library", __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "sensors-library", __VA_ARGS__))

static const char* ACCEL_URI = "sensor:Accelerometer";
static pthread_mutex_t list_mutex;

static JavaVM* cached_jvm;
static jclass class_SensorImpl = NULL;
static jclass class_SensorEvent = NULL;
static jmethodID meth_sensorData = NULL;
static jmethodID meth_consEvent = NULL;
static jmethodID meth_sensorData2 = NULL;
static jfieldID field_handle = NULL;
static jfieldID field_timestamp = NULL;
static jfieldID field_data = NULL;

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* jvm, void *reserved) {
	JNIEnv *env;
	jclass clsSensorImpl;
	jclass clsSensorEvent;

	cached_jvm = jvm; /* cache the JavaVM pointer */

	if ((*jvm)->GetEnv(jvm, (void **) &env, JNI_VERSION_1_2)) {
		return JNI_ERR; /* JNI version not supported */
	}

	clsSensorImpl = (*env)->FindClass(env, "com/telefonica/sensors/SensorImpl");
	if (clsSensorImpl == NULL) {
		return JNI_ERR;
	}

	/* Use weak global ref to allow C class to be unloaded */
	class_SensorImpl = (*env)->NewWeakGlobalRef(env, clsSensorImpl);
	if (class_SensorImpl == NULL) {
		return JNI_ERR;
	}

	meth_sensorData = (*env)->GetStaticMethodID(env, clsSensorImpl, "sensorDataCB",
			"(ILjava/lang/String;)V");
	if (meth_sensorData == NULL) {
		return JNI_ERR;
	}

	pthread_mutex_init(&list_mutex, NULL);

	return JNI_VERSION_1_2;
}

JNIEXPORT void JNICALL JNI_OnUnload(JavaVM *jvm, void *reserved) {
	pthread_mutex_destroy(&list_mutex);
}

typedef struct SensorCnxData SensorCnxData;

struct SensorCnxData {
	ASensorRef sensor;
	ALooper* looper;
	const char* status;
	int interval;
	int hasToFinish;
	int handle;
};

typedef struct SensorThreadData SensorThreadData;

struct SensorThreadData {
	SensorCnxData* sdata;
	jobject target;
};

typedef struct SensorCnxListNode SensorCnxListNode;

struct SensorCnxListNode {
	SensorCnxListNode* prev;
	SensorCnxListNode* next;
	SensorCnxData* data;
};

// Pointers to the double linked list with the handlers
static SensorCnxListNode* firstCnxNode = NULL;

// Adds a node to the list
static void addNode(SensorCnxListNode* node) {
	pthread_mutex_lock(&list_mutex);

	if (firstCnxNode == NULL) {
		firstCnxNode = node;
		firstCnxNode->prev = NULL;
		firstCnxNode->next = NULL;
	} else {
		SensorCnxListNode* aux = node;
		aux->next = node;
		node->prev = aux;
		node->next = NULL;
	}

	pthread_mutex_unlock(&list_mutex);
}

// Removes a node from the list
static void removeNode(SensorCnxListNode* node) {

	pthread_mutex_lock(&list_mutex);

	SensorCnxListNode* prev = node->prev;
	SensorCnxListNode* next = node->next;

	if (node == firstCnxNode) {
		firstCnxNode = node->next;
		if (firstCnxNode != NULL) {
			firstCnxNode->prev = NULL;
		}
	} else if (next != NULL && prev != NULL) {
		next->prev = node->prev;
		prev->next = node->next;
	} else if (next == NULL) {
		// Last node to be removed
		prev->next = NULL;
	}

	free(node);

	pthread_mutex_unlock(&list_mutex);
}

static int getSensorType(const char* uri) {
	int dev = -1;

	if (strcmp(uri, ACCEL_URI) == 0) {
		dev = ASENSOR_TYPE_ACCELEROMETER;
	}

	return dev;
}

static SensorCnxListNode* handle2Node(int handle) {
	SensorCnxListNode* ret = (SensorCnxListNode*) handle;

	return ret;
}

static int node2Handle(SensorCnxListNode* node) {
	return (int)node;
}

static SensorCnxData* getCnxData(int handle) {
	SensorCnxData* ret = handle2Node(handle)->data;

	return ret;
}

static ASensorRef getSensor(int handle) {
	ASensorRef sensor = getCnxData(handle)->sensor;

	LOGI("Sensor Got: %d", sensor);

	return sensor;
}

static int addSensor(ASensorRef sensor) {

	SensorCnxListNode* node = (SensorCnxListNode*) malloc(
			sizeof(SensorCnxListNode));

	node->data = (SensorCnxData*) malloc(sizeof(SensorCnxData));

	node->data->sensor = sensor;

	addNode(node);

	LOGI("Sensor Added: %d", sensor);

	node->data->handle = (int) node;
	node->data->status = "open";

	return node->data->handle;
}

JNIEXPORT jint JNICALL Java_com_telefonica_sensors_SensorNative_connect(
		JNIEnv* env, jobject thiz, jstring uri) {
	ASensorManager* sensorManager;
	ASensorRef sensor;

	jboolean isCopy;
	const char* suri = (*env)->GetStringUTFChars(env, uri, &isCopy);

	LOGI("Connect Request for URI: %s", suri);

	sensorManager = ASensorManager_getInstance();

	sensor = ASensorManager_getDefaultSensor(sensorManager,
			getSensorType(suri));

	int handle = addSensor(sensor);

	LOGI("Connected!. Handle: %d", handle);

	return handle;

}

JNIEXPORT jstring JNICALL Java_com_telefonica_sensors_SensorNative_getVendor(
		JNIEnv* env, jobject thiz, jint handle) {
	ASensorRef sensor;

	LOGI("getVendor(). Handle: %d", handle);

	sensor = getSensor(handle);

	LOGI("Vendor: %s", ASensor_getVendor(sensor));

	return (*env)->NewStringUTF(env, ASensor_getVendor(sensor));
}

JNIEXPORT jstring JNICALL Java_com_telefonica_sensors_SensorNative_getType(
		JNIEnv* env, jobject thiz, jint handle) {

	LOGI("getType(). Handle: %d", handle);

	ASensorRef sensor = getSensor(handle);
	char* typeString;

	typeString = malloc(25);
	sprintf(typeString, "%d", ASensor_getType(sensor));

	jstring result = (*env)->NewStringUTF(env, typeString);

	free(typeString);

	return result;
}

JNIEXPORT jstring JNICALL Java_com_telefonica_sensors_SensorNative_getName(
		JNIEnv* env, jobject thiz, jint handle) {
	ASensor const* sensor;

	LOGI("getName(). Handle: %d", handle);

	sensor = getSensor(handle);

	return (*env)->NewStringUTF(env, ASensor_getName(sensor));
}

JNIEXPORT jint JNICALL Java_com_telefonica_sensors_SensorNative_getMinDelay(
		JNIEnv* env, jobject thiz, jint handle) {
	ASensor const* sensor;

	LOGI("geMinDelay(). Handle: %d", handle);

	sensor = getSensor(handle);

	return ASensor_getMinDelay(sensor);
}

JNIEXPORT jfloat JNICALL Java_com_telefonica_sensors_SensorNative_getResolution(
		JNIEnv* env, jobject thiz, jint handle) {
	ASensor const* sensor;

	LOGI("getResolution(). Handle: %d", handle);

	sensor = getSensor(handle);

	return ASensor_getResolution(sensor);
}

// Callback function called new sensor data is available
static int sensorCB(int fd, int events, void* data) {
	/*
	 LOGI("The callback has been invoked: events: %d fd: %d\n");

	 ASensorEvent* event = (ASensorEvent*)data;

	 LOGI("yyyyy Events received: %d %d %d\n",event->acceleration.x, event->acceleration.y,event->acceleration.z);
	 */
}

// Serializes to JSON format the event
static char* serializeEvent(ASensorEvent asv) {
	char* ret = malloc(200);
	bzero(ret,sizeof(ret));

	sprintf(ret,"{data: {x:%f,y:%f,z:%f}}",asv.acceleration.x,asv.acceleration.y,asv.acceleration.z);

	return ret;
}

// Invoked to send the data back to the Java layer
static void on_sensor_data(ASensorEvent event, int handle, jobject target) {
	JNIEnv* env;

	(*cached_jvm)->AttachCurrentThread(cached_jvm, &env, NULL);

	jclass c = (*env)->GetObjectClass(env, target);

	// jmethodID mc = (*env)->GetMethodID(env, clsSensorEvent, "<init>", "()V");

	// jobject obj = (*env)->NewObject(env, clsSensorEvent, mc);

	jstring str = (*env)->NewStringUTF(env,serializeEvent(event));

	(*env)->CallStaticVoidMethod(env, c, meth_sensorData, handle,str);

	(*cached_jvm)->DetachCurrentThread(cached_jvm);
}


// Thread in charge of watching
static void* watcher(void* param) {

	LOGI("Watch thread started");

	SensorThreadData* sthr = (SensorThreadData*) param;
	SensorCnxData* wd = sthr->sdata;

	wd->looper = ALooper_prepare(0);

	ASensorEventQueue* the_queue = ASensorManager_createEventQueue(
			ASensorManager_getInstance(), wd->looper, ALOOPER_POLL_CALLBACK,
			sensorCB, NULL);

	ASensorEventQueue_setEventRate(the_queue, wd->sensor, wd->interval * 1000);

	ASensorEventQueue_enableSensor(the_queue, wd->sensor);

	ASensorEvent sensorEvents[20];

	int finish = 0;

	while (!finish) {
		int events;
		int fd;
		int pollRes = ALooper_pollOnce(-1, &events, &fd, (void**) NULL);
		if (pollRes == ALOOPER_POLL_CALLBACK) {
			LOGI("Num Events: %d FD: %d\n", events, fd);
			bzero(sensorEvents, sizeof(sensorEvents));
			ssize_t numEvents = ASensorEventQueue_getEvents(the_queue,
					sensorEvents, 20);
			LOGI("Num events got: %d", numEvents);
			if (numEvents > 0) {
				LOGI(
						"aaaa Events received: %f %f %f\n", sensorEvents[0].acceleration.x, sensorEvents[0].acceleration.y, sensorEvents[0].acceleration.z);
				on_sensor_data(sensorEvents[0], wd->handle, sthr->target);
			}
		} else if (pollRes == ALOOPER_POLL_WAKE) {
			finish = 1;
		}

		if (wd->hasToFinish) {
			finish = 1;
		}
	}

	// Now it is time to release resources bound to the thread
	ASensorEventQueue_disableSensor(the_queue, wd->sensor);
	ASensorManager_destroyEventQueue(ASensorManager_getInstance(), the_queue);

	LOGI("Watch Thread finished");
}

JNIEXPORT void JNICALL Java_com_telefonica_sensors_SensorNative_watch(JNIEnv* env, jobject thiz,
		jint handle, jint interval) {

	LOGI("Watch invoked at the native layer");

pthread_t theThread;

SensorCnxData* wd = getCnxData(handle);

SensorThreadData* sthr = malloc(sizeof(SensorThreadData));
sthr->sdata = wd;
sthr->target= thiz;

wd->hasToFinish = 0;

wd->status = "watching";

int rc = pthread_create(&theThread, NULL, watcher, (void *) sthr);
}

static void doEnd(int handle) {
	SensorCnxData* cd = getCnxData(handle);

	if(strcmp(cd->status,"watching") == 0) {
		cd->hasToFinish = 1;
		cd->status = "open";
		ALooper_wake(cd->looper);
	}
}

JNIEXPORT void JNICALL Java_com_telefonica_sensors_SensorNative_end(JNIEnv* env,
	jobject thiz, jint handle) {

LOGI("End Watch invoked. Handle: %d", handle);

	doEnd(handle);

}

static void doKill(int handle) {
	doEnd(handle);

	// Remove the node
	SensorCnxListNode* node = handle2Node(handle);

	removeNode(node);
}

JNIEXPORT void JNICALL Java_com_telefonica_sensors_SensorNative_kill(JNIEnv* env,
	jobject thiz, jint handle) {

	LOGI("Kill invoked. Handle: %d",handle);

	doKill(handle);
}

JNIEXPORT void JNICALL Java_com_telefonica_sensors_SensorNative_killAll(JNIEnv* env,
	jobject thiz) {

	LOGI("Kill All invoked");

	SensorCnxListNode* node = firstCnxNode;
	while(firstCnxNode != NULL) {
		doKill(node2Handle(firstCnxNode));
	}

}

