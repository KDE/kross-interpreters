import org.kde.kdebindings.java.krossjava.*;

public class TestWindow extends KrossQExtension {

    public TestWindow(Long p){
        super(p);
    }

    public void emitScriptHook(){ invoke("emitScriptHook"); }
}
