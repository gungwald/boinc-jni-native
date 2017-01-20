#
# Builds which.jar with standard Java or OpenJDK and MinGW gcc.
# Make sure the bit length of Java and gcc match 64 with 64 or 32 with 32.
#
# Warning: Windows will not grab the getenv.dll in the local directory
# by default if there is another one in the PATH. Argh. I though I was
# generating a 32-bit getenv.dll for a long time because it was still
# grabing the old 32-bit version at runtime. Use "depends" to check the
# bitness of the getenv.dll file.
#

CC=g++
BOINC_INC=-I../boinc/api -I../boinc/lib
JNI_INC=-I/System/Library/Frameworks/JavaVM.framework/Versions/CurrentJDK/Headers -I/Developer/SDKs/MacOSX10.4u.sdk/System/Library/Frameworks/JavaVM.framework/Versions/A/Headers
JAVA_CLASSES=../boinc-jni/classes
TARGET=libboinc-jni.jnilib

all: $(TARGET)

$(TARGET): basicapi.o
	$(CC) -dynamiclib -o $(TARGET) basicapi.o

basicapi.o: basicapi.cxx edu_berkeley_boinc_jni_Boinc.h
	$(CC) -Wall -D_JNI_IMPLEMENTATION_ $(JNI_INC) $(BOINC_INC) -c basicapi.cxx
	
edu_berkeley_boinc_jni_Boinc.h: $(JAVA_CLASSES)/edu/berkeley/boinc/jni/Boinc.class
	javah -classpath $(JAVA_CLASSES) -jni edu.berkeley.boinc.jni.Boinc

clean:
	rm *.o $(TARGET) edu_berkeley*.h

