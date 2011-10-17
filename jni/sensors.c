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


#include <android/sensor.h>
#include <android/log.h>

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "sensors-library", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "sensors-library", __VA_ARGS__))

static ASensorRef* sensor_handlers = NULL;
static int num_connections = 0;

static const char* ACCEL_URI = "sensor:Accelerometer";

static int getSensorType(const char* uri) {
	int dev = -1;

	if(strcmp(uri,ACCEL_URI) == 0) {
		dev = ASENSOR_TYPE_ACCELEROMETER;
	}

	return dev;
}

static ASensorRef getSensor(int handle) {
	ASensorRef sensor = sensor_handlers[handle - 1];

	LOGI("Sensor Got: %d",sensor);

	return sensor;
}

static int addSensor(ASensorRef sensor) {
	num_connections++;

	if(sensor_handlers == NULL){
		sensor_handlers = malloc(sizeof(ASensorRef));
	}

	sensor_handlers[num_connections - 1] = sensor;

	LOGI("Sensor Added: %d",sensor);

	return num_connections ;
}

jint Java_com_telefonica_sensors_SensorNative_connect( JNIEnv* env, jobject thiz, jstring uri) {
	 ASensorManager* sensorManager;
	 ASensorRef sensor;

	 jboolean isCopy;
	 const char * suri = (*env)->GetStringUTFChars(env, uri, &isCopy);

	 LOGI("Connect Request for URI: %s",suri);

	 sensorManager = ASensorManager_getInstance();

	 sensor = ASensorManager_getDefaultSensor(sensorManager,getSensorType(suri));

	 jint handle = addSensor(sensor);

	 (*env)->ReleaseStringUTFChars(env, uri, suri);

	 LOGI("Connected!. Handle: %d",handle);

	 return handle;
}

jstring Java_com_telefonica_sensors_SensorNative_getVendor( JNIEnv* env, jobject thiz,jint handle ) {
	 ASensorRef sensor;

	 LOGI("getVendor(). Handle: %d",handle);

	sensor = getSensor(handle);

	LOGI("Vendor: %s",ASensor_getVendor(sensor));

	 return (*env)->NewStringUTF(env, ASensor_getVendor(sensor));
}

jstring Java_com_telefonica_sensors_SensorNative_getType( JNIEnv* env, jobject thiz,jint handle ) {

	 ASensorRef sensor = getSensor(handle);
	 char* typeString;

	 LOGI("getType(). Handle: %d",handle);


	 typeString = malloc(25);
	 sprintf(typeString,"%d",ASensor_getType(sensor));

	 jstring result = (*env)->NewStringUTF(env,typeString);

	 free(typeString);

	 return result;
}

jstring Java_com_telefonica_sensors_SensorNative_getName( JNIEnv* env, jobject thiz,jint handle ) {
	 ASensor const* sensor;

	 LOGI("getName(). Handle: %d",handle);

	 sensor = getSensor(handle);

	 return (*env)->NewStringUTF(env, ASensor_getName(sensor));
}

jint Java_com_telefonica_sensors_SensorNative_getMinDelay( JNIEnv* env, jobject thiz,jint handle ) {
	 ASensor const* sensor;

	 LOGI("geMinDelay(). Handle: %d",handle);

	 sensor = getSensor(handle);

	 return ASensor_getMinDelay(sensor);
}

jfloat Java_com_telefonica_sensors_SensorNative_getResolution( JNIEnv* env, jobject thiz,jint handle ) {
	 ASensor const* sensor;

	 LOGI("getResolution(). Handle: %d",handle);

	 sensor = getSensor(handle);

	 return ASensor_getResolution(sensor);
}
