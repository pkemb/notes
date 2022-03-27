#include <stdio.h>
#include "TestJNI.h"

JNIEXPORT jint JNICALL Java_TestJNI_testJniAdd
    (JNIEnv *env, jobject obj, jint a, jint b)
{
    printf("native code\n");
    return a + b;
}