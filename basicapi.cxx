#include <string>
#include <jni.h>
#include <boinc_api.h>
#include <app_ipc.h>	/* boinc_resolve_filename */

#include "edu_berkeley_boinc_jni_Boinc.h"

const int BOINC_API_SUCCESS = 0;

static jclass findBoincExceptionClass(JNIEnv *env);
static jthrowable newBoincException(JNIEnv *env, int status);
static void throwNewBoincException(JNIEnv *env, int status);

/*
 * Class:     edu_berkeley_boinc_jni_Boinc
 * Method:    init
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_edu_berkeley_boinc_jni_Boinc_init__
  (JNIEnv *env, jobject boinc)
{
    int status;

    if ((status = boinc_init()) != BOINC_API_SUCCESS) {
        throwNewBoincException(env, status);
    }
}

/*
 * Class:     edu_berkeley_boinc_jni_Boinc
 * Method:    init
 * Signature: (ZZZZZZZZ)V
 */
JNIEXPORT void JNICALL Java_edu_berkeley_boinc_jni_Boinc_init__ZZZZZZZZ
  (JNIEnv *env, jobject boinc, jboolean normalThreadPriority, jboolean mainProgram, jboolean checkHeartbeat, jboolean handleProcessControl, jboolean sendStatusMessages, jboolean directProcessAction, jboolean multiThread, jboolean multiProcess)
{
	int status;
    BOINC_OPTIONS options;

    options.normal_thread_priority = normalThreadPriority;
    options.main_program = mainProgram;
    options.check_heartbeat = checkHeartbeat;
    options.handle_process_control = handleProcessControl;
    options.send_status_msgs = sendStatusMessages;
    options.direct_process_action = directProcessAction;
    options.multi_thread = multiThread;
    options.multi_process = multiProcess;

	if ((status = boinc_init_options(&options)) != BOINC_API_SUCCESS) {
		throwNewBoincException(env, status);
	}
}

/*
 * Class:     edu_berkeley_boinc_jni_Boinc
 * Method:    finish
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_edu_berkeley_boinc_jni_Boinc_finish__I
  (JNIEnv *env, jobject jBoind, jint jStatusIn)
{
    int status;

	if ((status = boinc_finish((int) jStatusIn)) != BOINC_API_SUCCESS) {
		throwNewBoincException(env, status);
	}
}

/*
 * Class:     edu_berkeley_boinc_jni_Boinc
 * Method:    finish
 * Signature: (ILjava/lang/String;Z)V
 */
JNIEXPORT void JNICALL Java_edu_berkeley_boinc_jni_Boinc_finish__ILjava_lang_String_2Z
  (JNIEnv *env, jobject jBoinc, jint jStatusIn, jstring jMessage, jboolean jIsNotice)
{
    int status;
    const char *message;
    jboolean isCopy;

    if ((message = env->GetStringUTFChars(jMessage, &isCopy)) != NULL) {
    	if ((status = boinc_finish_message((int) jStatusIn, message, jIsNotice)) != BOINC_API_SUCCESS) {
    		throwNewBoincException(env, status);
    	}
        env->ReleaseStringUTFChars(jMessage, message);
	}
}

/*
 * Class:     edu_berkeley_boinc_jni_Boinc
 * Method:    resolveFileName
 * Signature: (Ljava/lang/String;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_edu_berkeley_boinc_jni_Boinc_resolveFileName
  (JNIEnv *env, jobject jBoinc, jstring jLogicalFileName)
{
    int status;
    const char *logicalFileName;
    jboolean isCopy;
    std::string physicalFileName;
    jstring jPhysicalFileName = NULL;

	if ((logicalFileName = env->GetStringUTFChars(jLogicalFileName, &isCopy)) != NULL) {
		if ((status = boinc_resolve_filename_s(logicalFileName, physicalFileName)) == BOINC_API_SUCCESS) {
            jPhysicalFileName = env->NewStringUTF(physicalFileName.c_str());
		}
		else {
			throwNewBoincException(env, status);
		}
        env->ReleaseStringUTFChars(jLogicalFileName, logicalFileName);
	}
    return jPhysicalFileName;
}

/* Private functions */

jclass findBoincExceptionClass(JNIEnv *env)
{
    // Cached for performance since this can never change.
    static jclass boincExceClass = NULL;

    if (boincExceClass == NULL) {
    	boincExceClass = env->FindClass("edu/berkeley/boinc/jni/BoincException");
        // boincExceClass will be NULL if it failed. An exception probably also occurred.
    }
    return boincExceClass;
}

/**
 * Finds the constructor or returns the cached constructor if it was
 * previously found.
 * @return The constructor's method or NULL if it was not found.
 */
jmethodID findBoincExceptionConstructor(JNIEnv *env, jclass boincExceClass)
{
    // Cached for performance since this can never change.
    static jmethodID constrID = NULL;

    if (constrID == NULL) {
    	constrID = env->GetMethodID(boincExceClass, "<init>", "()I");
        // constrID will be NULL if it failed. An exception probably also occurred.
    }
    return constrID;
}

jthrowable newBoincException(JNIEnv *env, int status)
{
    jclass cls;
    jmethodID constr;
    jthrowable e = NULL;

    if ((cls = findBoincExceptionClass(env)) != NULL) {
    	if ((constr = findBoincExceptionConstructor(env, cls)) != NULL) {
    		e = (jthrowable) env->NewObject(cls, constr, (jint) status);
            // If this fails e will be NULL, which is what should be returned
    		// in this case.
    	}
    }
	return e;
}

void throwNewBoincException(JNIEnv *env, int status)
{
    jthrowable e;

    if ((e = newBoincException(env, status)) != NULL) {
    	env->Throw(e);
    }
}
