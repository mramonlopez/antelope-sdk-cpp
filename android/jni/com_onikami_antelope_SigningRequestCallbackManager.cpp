#include "com_onikami_antelope_SigningRequestCallbackManager.h"
#include <eosclient/SigningRequestCallbackManager.hpp>

using namespace onikami::eosclient;

extern "C" JNIEXPORT void JNICALL Java_com_onikami_antelope_SigningRequestCallbackManager_callbackReceived
    (JNIEnv * env, jobject me, jstring url) {

    SigningRequestCallbackManager::getInstance()->onCallback(reinterpret_cast<const char *>(url));
}
