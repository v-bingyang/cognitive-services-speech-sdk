package com.microsoft.cognitiveservices.speech.audio;
//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import com.microsoft.cognitiveservices.speech.util.Contracts;
import com.microsoft.cognitiveservices.speech.SpeechConfig;

/**
 * Represents audio input stream used for custom audio input configurations.
 * Note: close() must be called in order to relinquish underlying resources held by the object.
 */
public class AudioInputStream
{
    // load the native library.
    static {
        // trigger loading of native library
        try {
            Class.forName(SpeechConfig.class.getName());
        }
        catch (ClassNotFoundException ex) {
            throw new IllegalStateException(ex);
        }
    }

    /**
     * Creates a memory backed PushAudioInputStream using the default format (16 kHz, 16 bit, mono PCM).
     * @return The audio input stream being created.
     */
    public static PushAudioInputStream createPushStream() {
        return PushAudioInputStream.create();
    }

    /**
     * Creates a memory backed PushAudioInputStream with the specified audio format.
     * @param format The audio data format in which audio will be written to the push audio stream's write() method.
     * @return The audio input stream being created.
     */
    public static PushAudioInputStream createPushStream(AudioStreamFormat format) {
        return PushAudioInputStream.create(format);
    }

    /**
     * Creates a PullAudioInputStream that delegates to the specified callback interface for read() and close() methods, using the default format (16 kHz, 16 bit, mono PCM).
     * @param callback The custom audio input object, derived from PullAudioInputStreamCallback
     * @return The audio input stream being created.
     */
    public static PullAudioInputStream createPullStream(PullAudioInputStreamCallback callback) {
        return PullAudioInputStream.create(callback);
    }

    /**
     * Creates a PullAudioInputStream that delegates to the specified callback interface for read() and close() methods.
     * @param callback The custom audio input object, derived from PullAudioInputStreamCallback
     * @param format The audio data format in which audio will be returned from the callback's read() method.
     * @return The audio input stream being created.
     */
    public static PullAudioInputStream createPullStream(PullAudioInputStreamCallback callback, AudioStreamFormat format) {
        return PullAudioInputStream.create(callback, format);
    }

    /**
     * Explicitly frees any external resource attached to the object
     * Note: close() must be called in order to relinquish underlying resources held by the object.
     */
    public void close() {
        if (this._streamImpl != null) {
            this._streamImpl.delete();
        }
        this._streamImpl = null;
    }

    /*! \cond PROTECTED */

    protected AudioInputStream(com.microsoft.cognitiveservices.speech.internal.AudioInputStream stream) {
        Contracts.throwIfNull(stream, "stream");
        this._streamImpl = stream;
    }

    protected com.microsoft.cognitiveservices.speech.internal.AudioInputStream _streamImpl;

    /*! \endcond */

    /*! \cond INTERNAL */

    /**
     * Returns the audio input configuration.
     * @return The implementation of the stream.
     */
    public com.microsoft.cognitiveservices.speech.internal.AudioInputStream getStreamImpl() {
        return this._streamImpl;
    }

    /*! \endcond */
}
