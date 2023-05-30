#include "com_onikami_antelope_SigningRequestCallbackManager.h"
#include <eosclient/SigningRequestCallbackManager.hpp>

using namespace onikami::eosclient;

extern "C" JNIEXPORT void JNICALL Java_com_onikami_antelope_SigningRequestCallbackManager_callbackReceived
    (JNIEnv * env, jobject me, jstring url) {

    jboolean isCopy;
    const char *url_string = (env)->GetStringUTFChars(url, &isCopy);

    SigningRequestCallbackManager::getInstance()->onCallback(std::string(url_string));

    env->ReleaseStringUTFChars(url, url_string);
}
