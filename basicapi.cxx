#include <jni.h>
#include <boinc_api.h>

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
        env->Throw(newBoincException(env, status));
    }
}

JNIEXPORT jint JNICALL
Java_edu_berkeley_boinc_api_bridge_BasicAPI_boinc_1init_1options(
		JNIEnv *env,
		jclass cls,
		jobject jOptions)
{
    BOINC_OPTIONS options;
    jfieldID field;
    jclass jOptionsClass;

    jOptionsClass = env->GetObjectClass(jOptions);

    field = env->GetFieldID(jOptionsClass, "check_heartbeat", "B");
    options.check_heartbeat = env->GetBooleanField(jOptions, field);

    field = env->GetFieldID(jOptionsClass, "direct_process_action", "B");
    options.direct_process_action = env->GetBooleanField(jOptions, field);

    field = env->GetFieldID(jOptionsClass, "handle_process_control", "B");
    options.handle_process_control = env->GetBooleanField(jOptions, field);

    field = env->GetFieldID(jOptionsClass, "main_program", "B");
    options.main_program = env->GetBooleanField(jOptions, field);

    field = env->GetFieldID(jOptionsClass, "multi_process", "B");
    options.multi_process= env->GetBooleanField(jOptions, field);

    field = env->GetFieldID(jOptionsClass, "multi_thread", "B");
    options.multi_thread = env->GetBooleanField(jOptions, field);

    field = env->GetFieldID(jOptionsClass, "normal_thread_priority", "B");
    options.normal_thread_priority = env->GetBooleanField(jOptions, field);

    field = env->GetFieldID(jOptionsClass, "send_status_msgs", "B");
    options.send_status_msgs = env->GetBooleanField(jOptions, field);

	return boinc_init_options(&options);
}

jclass findBoincExceptionClass(JNIEnv *env)
{
    // Cached for performance since this can never change.
    static jclass boincExceClass = NULL;

    if (boincExceClass == NULL) {
    	boincExceClass = env->FindClass("edu/berkeley/boinc/jni/BoincException");
        // Check class not found ex
    }
    return boincExceClass;
}

jmethodID findBoincExceptionConstructor(JNIEnv *env, jclass boincExceClass)
{
    // Cached for performance since this can never change.
    static jmethodID constrID = NULL;

    if (constrID == NULL) {
    	constrID = env->GetMethodID(boincExceClass, "<init>", "()I");
    	if (constrID == NULL) {
    		// handle error
    	}
    }
    return constrID;
}

jthrowable newBoincException(JNIEnv *env, int status)
{
    jclass cls;
    jmethodID constr;

    cls = findBoincExceptionClass(env);
    constr = findBoincExceptionConstructor(env, cls);

	return (jthrowable) env->NewObject(cls, constr, status);
}

void throwNewBoincException(JNIEnv *env, int status)
{
	env->Throw(newBoincException(env, status));
}
