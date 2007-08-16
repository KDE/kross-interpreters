//URGHL inner classes not working... :/
public class SleepyThread extends Thread {
    TestWindow obj;
    int id;

    public SleepyThread(int id, TestWindow obj){
        super();
        this.id = id;
        this.obj = obj;
    }

    public void run() {
        try {
            int sleepTime = (int)(5000 * Math.random());
            System.out.println("[" + id + "]" + " registered update and sleeping for " + sleepTime + " ms.");
            sleep(sleepTime);
        } catch(InterruptedException e) {
            e.printStackTrace();
        }
        obj.emitScriptHook();
    }
}
