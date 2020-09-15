package tests.unit;

import com.microsoft.cognitiveservices.speech.CancellationReason;
import com.microsoft.cognitiveservices.speech.transcription.ConversationTranscriber;

import java.util.ArrayList;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.TimeoutException;
import java.util.concurrent.atomic.AtomicBoolean;

public class ConversationTranscriberHelper {
    public static ArrayList<String> getTranscriberResult(ConversationTranscriber ct) throws InterruptedException, ExecutionException, TimeoutException {
        String result;
        final ArrayList<String> phrases = new ArrayList<>();

        AtomicBoolean allDone = new AtomicBoolean(false);

        ct.transcribing.addEventListener((o, e) -> {
            System.out.println("Conversation transcriber transcibing:" + e.toString());
        });

        ct.transcribed.addEventListener((o, e) -> {
            phrases.add(e.getResult().getText());
            System.out.println("Conversation transcriber transcribed:" + e.toString());
        });

        // SDK does not send out sessionstopped when there is an error.
        ct.canceled.addEventListener((o, e) -> {
            if (e.getReason() != CancellationReason.EndOfStream)
            {
                allDone.set(true);
                System.out.println("Conversation transcriber canceled:" + e.toString());
            }
        });

        ct.sessionStopped.addEventListener((o, e) -> {
            allDone.set(true);
            System.out.println("Conversation transcriber stopped:" + e.toString());
        });

        ct.startTranscribingAsync().get();

        // wait for 300 seconds for sessionStopped or cancelled, othewise we time out
        int timeoutInMillis = 300000;
        long now = System.currentTimeMillis();
        while(((System.currentTimeMillis() - now) < timeoutInMillis) && (allDone.get() == false ) ) {
            Thread.sleep(200);
        }

        if( allDone.get() == false) {
            System.out.println("timeout!" + timeoutInMillis);
        }

        ct.stopTranscribingAsync().get();
        return phrases;
    }
}
