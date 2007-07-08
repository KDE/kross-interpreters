import org.kde.kdebindings.java.krossjava.*;

public class TestObject extends KrossQExtension {

    public TestObject(Long p){
        super(p);
    }

    /**
    * Returns the objectName of the wrapped QObject.
    *
    * In our case this could be "MyTestObject" or
    * however we named our TestObject-instance in
    * the jvmscript.cpp file.
    */
    public String name(){ return (String)invoke("name"); }

}
