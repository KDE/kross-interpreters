public class TestObjectImpl implements TestObject {

    /**
    * Returns the objectName of the wrapped QObject.
    *
    * In our case this could be "MyTestObject" or
    * however we named our TestObject-instance in
    * the jvmscript.cpp file.
    */
    public native String name();

}
