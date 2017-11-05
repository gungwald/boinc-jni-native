#
# Builds the native part of Boinc-jni.
# Make sure the bit length of Java and gcc match 64 with 64 or 32 with 32.
#
# Warning: Windows will not grab the getenv.dll in the local directory
# by default if there is another one in the PATH. Argh. I though I was
# generating a 32-bit getenv.dll for a long time because it was still
# grabing the old 32-bit version at runtime. Use "depends" to check the
# bitness of the getenv.dll file.
#

# Locations subject to change
BOINC_INC=-I../boinc/api -I../boinc/lib
CLASSES_DIR=../boinc-jni/classes

BOINC_HEADER=edu_berkeley_boinc_jni_Boinc.h
BOINC_CLASS_FILE=edu/berkeley/boinc/jni/Boinc.class
BOINC_CLASS=edu.berkeley.boinc.jni.Boinc

# Dynamically locate Java include files and shared libraries.
ifeq ($(OS),Windows_NT)
    JNI_INC=-I"$(JAVA_HOME)/include"
    TARGET=boinc-jni.dll
else
    UNAME := $(shell uname -s)
    ifeq ($(UNAME),Darwin)
        JNI_INC=-I/System/Library/Frameworks/JavaVM.framework/Versions/CurrentJDK/Headers -I/Developer/SDKs/MacOSX10.4u.sdk/System/Library/Frameworks/JavaVM.framework/Versions/A/Headers
        TARGET=libboinc-jni.jnilib
        MAKE_SHARED_LIB=-dynamiclib
    else 
    	ifeq ($(UNAME),Linux)
        	JNI_INC=-I/usr/lib/jvm/java/include -I/usr/lib/jvm/java/include/linux
        	TARGET=libboinc-jni.so    
        	MAKE_SHARED_LIB=-shared
            SHARED_LIB_CODE=-fpic
    	else
        	JNI_INC=-I/usr/java/include
        	TARGET=libboinc-jni.so
        	MAKE_SHARED_LIB=-shared
            SHARED_LIB_CODE=-fpic
        endif
    endif
endif

CPPFLAGS=-D_JNI_IMPLEMENTATION_ $(JNI_INC) $(BOINC_INC)
CXXFLAGS=-Wall

all: $(TARGET)

$(TARGET): basicapi.o
	$(CXX) $(MAKE_SHARED_LIB) -o $(TARGET) $<

basicapi.o: basicapi.cxx $(BOINC_HEADER)
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) $(SHARED_LIB_CODE) $<
	
$(BOINC_HEADER): $(CLASSES_DIR)/$(BOINC_CLASS_FILE)
	javah -classpath $(CLASSES_DIR) -jni $(BOINC_CLASS)

clean:
	rm *.o $(TARGET) $(BOINC_HEADER)
