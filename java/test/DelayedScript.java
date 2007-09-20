import org.kde.kdebindings.java.krossjava.*;
import java.io.*;

/**
* This script sleeps for a random time between 0 and 5 seconds, then calls emitUpdate.
*/
public class DelayedScript {

    private TestWindow obj;
    private int id;

    public DelayedScript() throws Exception {
        id = (int)(10000 * Math.random());
        obj = (TestWindow)(KrossQExtension.importModule("TestWindow"));
        obj.connect("scriptHook()",this,this.getClass().getMethod("scriptHook"));

        Thread t = new Thread(){
            public void run(){
                try {
                    int sleepTime = (int)(5000 * Math.random());
                    System.out.println("[" + id + "]" + " registered update and sleeping for " + sleepTime + " ms.");
                    sleep(sleepTime);
                } catch(InterruptedException e) {
                    e.printStackTrace();
                }
                obj.emitScriptHook();
            }
        };
        t.start();
    }

    public void scriptHook(){
        System.out.println("[" + id + "]" + " got scriptHook signal.");
    }
}
