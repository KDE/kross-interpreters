import org.kde.kdebindings.java.krossjava.*;
import java.io.*;

/**
* This script sleeps for a random time between 0 and 5 seconds, then calls emitUpdate.
*/
public class DelayedScript {

    private TestWindow obj;
    private int id;

    public DelayedScript() throws Exception {
        System.out.println("DelayedScript: " + this.getClass().getClassLoader());
        id = (int)(10000 * Math.random());
        obj = (TestWindow)(KrossClassLoader.importModule("TestWindow"));
        obj.connect("scriptHook()",this,this.getClass().getMethod("scriptHook"));

        //TODO: this fails with an obscure classloader error. Also, inner classes don't work either.
        Thread t = new SleepyThread(id, obj);
        t.start();

        //obj.emitScriptHook();
    }

    public void scriptHook(){
        System.out.println("[" + id + "]" + " got scriptHook signal.");
    }
}
