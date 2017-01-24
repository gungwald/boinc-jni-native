#include <string>
#include <jni.h>
#include <boinc_api.h>
#include <app_ipc.h>	/* boinc_resolve_filename */

#include "edu_berkeley_boinc_jni_Boinc.h"

static jclass findBoincExceptionClass(JNIEnv *env);
static jthrowable newBoincException(JNIEnv *env, int status);
static void throwNewBoincException(JNIEnv *env, int status);
static void handleTimerEvent();

const int BOINC_OK = 0;

static jobject globalTimerHandler = NULL;
static jmethodID globalHandleTimerEventMethodID = NULL;
static JavaVM *globalJvm = NULL;

/**
 * Calls the native no-argument boinc_init function.
 *
 * The status return value from boinc_init is converted to a
 * BoincException, if it is non-zero.
 *
 * @throws	BoincException If initialization fails
 */
JNIEXPORT void JNICALL Java_edu_berkeley_boinc_jni_Boinc_init__
  (JNIEnv *env, jobject boinc)
{
    int status;

    if ((status = boinc_init()) == BOINC_OK) {
        // Success
    }
    else {
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

	if ((status = boinc_init_options(&options)) == BOINC_OK) {
        // Success
	}
	else {
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

	if ((status = boinc_finish((int) jStatusIn)) == BOINC_OK) {
        // Success
	}
	else {
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
    	if ((status = boinc_finish_message((int) jStatusIn, message, jIsNotice)) == BOINC_OK) {
            // Success
    	}
    	else {
    		throwNewBoincException(env, status);
    	}
        // ReleaseStringUTFChars has no return value and does not throw an
    	// exception.
        env->ReleaseStringUTFChars(jMessage, message);
	}
    // If GetStringUTFChars returns NULL, an OutOfMemoryError was thrown.
    // In that case, this function ends without doing anything else so that
    // the caller can receive the error.
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
		if ((status = boinc_resolve_filename_s(logicalFileName, physicalFileName)) == BOINC_OK) {
            jPhysicalFileName = env->NewStringUTF(physicalFileName.c_str());
            // If NewStringUTF returns NULL, an OutOfMemoryError was thrown.
		}
		else {
			throwNewBoincException(env, status);
		}
        // ReleaseStringUTFChars has no return value and does not throw an
    	// exception.
        env->ReleaseStringUTFChars(jLogicalFileName, logicalFileName);
	}
    // If GetStringUTFChars returns NULL, an OutOfMemoryError was thrown.
    // In that case, this function ends without doing anything else so that
    // the caller can receive the error.

    return jPhysicalFileName;
}

/*
 * Class:     edu_berkeley_boinc_jni_Boinc
 * Method:    isTimeToCheckpoint
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_edu_berkeley_boinc_jni_Boinc_isTimeToCheckpoint
  (JNIEnv *env, jobject boinc)
{
    // This function has no way to indicate that an error occurred.
	return (jboolean) boinc_time_to_checkpoint();
}

/*
 * Class:     edu_berkeley_boinc_jni_Boinc
 * Method:    setMinCheckpointPeriod
 * Signature: (I)I
 */
JNIEXPORT void JNICALL Java_edu_berkeley_boinc_jni_Boinc_setMinmumCheckpointPeriod
  (JNIEnv *env, jobject boinc, jint nsecs)
{
    int status;

	if ((status = boinc_set_min_checkpoint_period((int) nsecs)) != BOINC_OK) {
		throwNewBoincException(env, status);
	}
}

/*
 * Class:     edu_berkeley_boinc_jni_Boinc
 * Method:    checkpointCompleted
 * Signature: ()I
 */
JNIEXPORT void JNICALL Java_edu_berkeley_boinc_jni_Boinc_reportThatCheckpointIsCompleted
  (JNIEnv *env, jobject boinc)
{
	int status;

	if ((status = boinc_checkpoint_completed()) != BOINC_OK) {
		throwNewBoincException(env, status);
	}
}

/*
 * Class:     edu_berkeley_boinc_jni_Boinc
 * Method:    reportFractionDone
 * Signature: (D)I
 */
JNIEXPORT void JNICALL Java_edu_berkeley_boinc_jni_Boinc_reportFractionDone
  (JNIEnv *env, jobject boinc, jdouble jFractionDone)
{
    int status;
	if ((status = boinc_fraction_done(jFractionDone)) != BOINC_OK) {
		throwNewBoincException(env, status);
	}
}

/*
 * Class:     edu_berkeley_boinc_jni_Boinc
 * Method:    beginCriticalSection
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_edu_berkeley_boinc_jni_Boinc_beginCriticalSection
  (JNIEnv *env, jobject boinc)
{
	boinc_begin_critical_section();
}

/*
 * Class:     edu_berkeley_boinc_jni_Boinc
 * Method:    endCriticalSection
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_edu_berkeley_boinc_jni_Boinc_endCriticalSection
  (JNIEnv *env, jobject boinc)
{
	boinc_end_critical_section();
}

/*
 * Class:     edu_berkeley_boinc_jni_Boinc
 * Method:    registerTimerCallback
 * Signature: (Ledu/berkeley/boinc/jni/TimerHandler;)V
 */
JNIEXPORT void JNICALL Java_edu_berkeley_boinc_jni_Boinc_registerTimerCallback
  (JNIEnv *env, jobject boinc, jobject jTimerHandlerInstance)
{
    jclass jTimerHandlerClass;
    jmethodID callbackMethod;

    if ((jTimerHandlerClass = env->GetObjectClass(jTimerHandlerInstance)) != NULL) {
        if ((callbackMethod = env->GetMethodID(jTimerHandlerClass, "handleTimerEvent", "()V")) != NULL) {
            if (env->GetJavaVM(&globalJvm) == JNI_OK) {
                globalTimerHandler = jTimerHandlerInstance;
            	globalHandleTimerEventMethodID = callbackMethod;
            	boinc_register_timer_callback(handleTimerEvent);
            }
        }
    }
}

/*
 * Class:     edu_berkeley_boinc_jni_Boinc
 * Method:    exitAndRestart
 * Signature: (ILjava/lang/String;Z)V
 */
JNIEXPORT void JNICALL Java_edu_berkeley_boinc_jni_Boinc_exitAndRestart
  (JNIEnv *env, jobject boinc, jint jDelaySeconds, jstring jReason, jboolean jIsNotice)
{
    int status;
    bool isNotice;
    const char *reason;
    jboolean isCopy;

    if ((reason = env->GetStringUTFChars(jReason, &isCopy)) != NULL) {
        jIsNotice ? isNotice=true : isNotice=false;
    	if ((status = boinc_temporary_exit((int) jDelaySeconds, reason, isNotice)) != BOINC_OK) {
    		throwNewBoincException(env, status);
    	}
        env->ReleaseStringUTFChars(jReason, reason);
    }
}

/* Private functions */

/**
 * Finds and caches the BoincException class.
 * @param	env	The JNI environment
 * @throws  ClassFormatError if the class data does not specify a valid class.
 * @throws  ClassCircularityError if a class or interface would be its own superclass or superinterface.
 * @throws  NoClassDefFoundError if no definition for a requested class or interface can be found.
 * @throws  OutOfMemoryError if the system runs out of memory.
 */
jclass findBoincExceptionClass(JNIEnv *env)
{
    // Cached for performance since this can never change.
    static jclass cls = NULL;

    if (cls == NULL) {
    	cls = env->FindClass("edu/berkeley/boinc/jni/BoincException");
        // cls will be NULL if it failed. An exception probably also occurred.
    }
    return cls;
}

/**
 * Finds the constructor or returns the cached constructor if it was
 * previously found.
 * @return The constructor's method or NULL if it was not found.
 * @throws NoSuchMethodError: if the specified method cannot be found.
 * @throws ExceptionInInitializerError: if the class initializer fails due to an exception.
 * @throws OutOfMemoryError: if the system runs out of memory.
 */
jmethodID findBoincExceptionConstructor(JNIEnv *env, jclass cls)
{
    // Cached for performance since this can never change.
    static jmethodID method = NULL;

    if (method == NULL) {
    	method = env->GetMethodID(cls, "<init>", "()I");
        // method will be NULL if it failed. An exception probably also occurred.
    }
    return method;
}

/**
 * Creates and returns the BoincException Java object. If any failure occurs,
 * NULL will be returned and a Java exception will be set to be thrown.
 * @param	env	The JNI environment
 * @param	status	The code to be passed to the BoincException constructor
 * @return	A BoincException instance or NULL if a failure occurs
 * @throws  InstantiationException: if the class is an interface or an abstract class.
 * @throws  OutOfMemoryError: if the system runs out of memory.
 */
jthrowable newBoincException(JNIEnv *env, int status)
{
    jclass cls;
    jmethodID constr;
    jthrowable e = NULL;

    if ((cls = findBoincExceptionClass(env)) != NULL) {
    	if ((constr = findBoincExceptionConstructor(env, cls)) != NULL) {
            // If this fails, e will be assigned NULL, which is what should
    		// be returned in this case and a Java exception will be set.
    		e = (jthrowable) env->NewObject(cls, constr, (jint) status);
    	}
    }
    // If any failure occurred, then e was not given valid value, but still
    // has its initial value of NULL. That is exactly what needs to be
    // returned to indicate that an error occurred. Whatever function failed
    // must have also set a Java exception to be thrown.
	return e;
}

/**
 * Creates and throws a BoincException.
 * @param	env	The JNI environment
 * @param	status	The code to be passed to the BoincException constructor
 * @return	A BoincException instance or NULL if a failure occurs
 * @throw	ClassNotFoundException, MethodNotFound
 * @throw	BoincException	If nothing else fails
 */
void throwNewBoincException(JNIEnv *env, int status)
{
    jthrowable e;

    if ((e = newBoincException(env, status)) != NULL) {
    	env->Throw(e);
    }
    // If newBoincException failed with a NULL return, a Java
    // exception will be set to be thrown on returning to Java
    // code. If it succeeds, then a new BoincException will be
    // thrown. In either case, nothing is returned because
    // exceptions are always "thrown".
}

/**
 * Native callback for timer event. Calls the registered Java timer handler.
 */
void handleTimerEvent()
{
    JNIEnv *env;

    if (globalJvm != NULL) {
    	if (globalTimerHandler != NULL && globalHandleTimerEventMethodID != NULL) {
    		if ((globalJvm->AttachCurrentThread((void **) &env, NULL)) == JNI_OK) {
    			env->CallVoidMethod(globalTimerHandler, globalHandleTimerEventMethodID);
    		}
    	}
	}
}
