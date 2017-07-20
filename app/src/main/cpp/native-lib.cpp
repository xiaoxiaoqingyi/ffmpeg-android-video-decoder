#include <jni.h>
#include <string>

extern "C"
JNIEXPORT jstring JNICALL
Java_com_opensource_ffmpeg_1android_1video_1decoder_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}
