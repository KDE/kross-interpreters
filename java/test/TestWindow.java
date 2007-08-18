import org.kde.kdebindings.java.krossjava.*;

public class TestWindow extends KrossQExtension {

    public TestWindow(Long p){
        super(p);
        System.out.println("TestWindow: " + this.getClass().getClassLoader());
    }

    public void emitScriptHook(){ invoke("emitScriptHook"); }
}
