import org.kde.kdebindings.java.krossjava.*;

public class TestObjectImpl extends KrossQExtensionImpl implements TestObject, KrossQExtension {

    public TestObjectImpl(Long p){
        super(p);
    }

    /**
    * Returns the objectName of the wrapped QObject.
    *
    * In our case this could be "MyTestObject" or
    * however we named our TestObject-instance in
    * the jvmscript.cpp file.
    */
    public String name(){ return null; }

}
