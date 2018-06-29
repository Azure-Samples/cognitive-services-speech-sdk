//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import java.lang.reflect.Field;
import java.util.ArrayList;

import com.microsoft.cognitiveservices.speech.samples.SampleSettings;
import com.microsoft.cognitiveservices.speech.samples.Stoppable;



/**
 * @author fmegen
 *
 */
public class Main {

    private static void usage() {
        Class<?> c = SampleSettings.class;

        System.out.print("usage: main_java ");
        for(Field f : c.getDeclaredFields()) {
            if(String.class == f.getType()) {
                try {
                    System.out.print(" --" + f.getName() + " " + f.get(null));
                } catch (IllegalArgumentException | IllegalAccessException e) {
                    // TODO Auto-generated catch block
                    e.printStackTrace();
                }
            }
        }

        // all tests without wakeword
        System.out.print(" SampleSimpleRecognize SampleRecognizeWithIntermediateResults SampleRecognizeIntent");

        System.out.println("");
    }

    /**
      * @param args
      * @throws UnsupportedOperationException 
      */
    public static void main(String[] args) {
        if((args != null) && (args.length == 1) &&
          (args[0] == "--help" || args[0] == "/?" || args[0] == "-h")) {
            usage();
            return;
        }

        try {
            ArrayList<String> sample = new ArrayList<String>();

            for(int n=0; n<args.length; n++) {
                if(args[n].startsWith("-")) {
                    String option = args[n].substring(1);
                    if(option.startsWith("-"))
                        option = option.substring(1);

                    Field field =  SampleSettings.class.getDeclaredField(option);
                    field.set(null,  args[++n]);
                }
                else {
                   sample.add(args[n]);
                }
            }

            if(sample.isEmpty() || (sample.size() == 1 && sample.get(0).toLowerCase().equals("all"))) {
                sample.clear();
                sample.add("SampleSimpleRecognize");   
                sample.add("SampleRecognizeWithIntermediateResults");   

                sample.add("SampleRecognizeIntent");   

                // You need a KWS extension for this to work.
                //sample.add("SampleRecognizeWithWakeWord");
                //sample.add("SampleRecognizeIntentWithWakeWord");   
            }

            for(String a : sample) {
                Class<?> c = Class.forName("com.microsoft.cognitiveservices.speech.samples." + a);
                Object obj = c.newInstance();

                if(obj instanceof Runnable) {
                    Runnable run = (Runnable)obj;

                    System.out.println("Running Sample " + a);
                    run.run();
                }

                if(obj instanceof Stoppable) {
                    Stoppable stop = (Stoppable)obj;

                    System.out.println("Press <return> key to stop " + a);
                    System.in.read();

                    stop.stop();
                }

            }

            Thread.sleep(4000);
            System.exit(0);
        }
        catch(Exception ex) {
            SampleSettings.displayException(ex);
            usage();
        }
    }
}
