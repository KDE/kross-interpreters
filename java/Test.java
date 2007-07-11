import org.kde.kdebindings.java.krossjava.KrossClassLoader;

public class Test {

    private TestObject to;

    public Test() {
        System.out.println("Hello World !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
        setUp();
        testName();
        testInt();
        testUInt();
        testString();
        testBool();
        testDouble();  //TODO: this doesn't pass yet
        testByteArray();  //TODO: this neither
    }

    public void setUp() {
        to = (TestObject)(KrossClassLoader.importModule("TestObject"));
    }

    public void testName() {
        String name = to.name();
        System.out.println("TestObject name=" + name);
    }

    public void testInt() {
        System.out.println("TestObject.func_int_int(5)=" + to.func_int_int(5));
    }

    public void testUInt() {
        System.out.println("TestObject.func_uint_uint(5)=" + to.func_uint_uint(5));
    }

    public void testString() {
        System.out.println("TestObject.func_qstring_qstring(\"Kross Power!\")=" + to.func_qstring_qstring("Kross Power!"));
    }

    public void testBool() {
        System.out.println("TestObject.func_bool_bool(true)=" + to.func_bool_bool(true));
    }

    public void testDouble() {
        //TODO: this returns 3.0 instead of 3.14
        System.out.println("TestObject.func_double_double(3.14)=" + to.func_double_double(3.14));
    }

    public void testByteArray() {
        //The problem here is that encapsulating every single byte in a Byte is very wasteful.
        //And we also need this convertion on the C side, when reading in class files.
        //However, byte[] is also a jobject, so perhaps it'll work like this.

        //Byte[] in = {new Byte((byte)65), new Byte((byte)0), new Byte((byte)66)};
        byte[] in = {65, 0, 66};
        byte[] out = to.func_qbytearray_qbytearray(in);
        System.out.println("TestObject.func_qbytearray_qbytearray=(" + out[0] + "," + out[1] + "," + out[2] + ")");
    }
}
