#include <jni.h>
#include <boinc_api.h>

#include "edu_berkeley_boinc_api_bridge_BasicAPI.h"

JNIEXPORT jint JNICALL
Java_edu_berkeley_boinc_api_bridge_BasicAPI_boinc_1init(
		JNIEnv *env,
		jclass cls)
{
    return (jint) boinc_init();
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
