#include "com_onikami_antelope_SingningRequestCallbackManager.h"
#include <eosclient/SigningRequestCallbackManager.hpp>

JNIEXPORT void JNICALL Java_com_onikami_antelope_SingningRequestCallbackManager_callbackReceived
    (JNIEnv * env, jclass me, jstring url) {
    
    SigningReqyuestCallbackManager::getInstance()->onCallback(url);
}
