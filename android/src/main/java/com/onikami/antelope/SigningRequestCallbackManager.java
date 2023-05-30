package com.onikami.antelope;

public class SigningRequestCallbackManager {
    private native void callbackReceived(String url);

    public void onCallback(String url) {
        this.callbackReceived(url);
    }
}