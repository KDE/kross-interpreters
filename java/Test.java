import org.kde.kdebindings.java.krossjava.KrossClassLoader;
import java.util.*;
import java.net.*;

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
        testDouble();
        testLong();
        testULong();
        testByteArray();
        testStringList();
        testArrayList();
        testGenericArrayList();
        testMap();
        testSize();
        testIllegalSize();
        testSizef();
        testPoint();
        testPointf();
        testRect();
        testRectf();
        testURL();
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
        System.out.println("TestObject.func_double_double(3.14)=" + to.func_double_double(3.14));
    }

    public void testLong() {
        System.out.println("TestObject.func_qlonglong_qlonglong(1234567890123456L)=" + to.func_qlonglong_qlonglong(1234567890123456L));
    }

    public void testULong() {
        System.out.println("TestObject.func_qulonglong_qulonglong(1234567890123456L)=" + to.func_qulonglong_qulonglong(1234567890123456L));
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

    public void testStringList() {
        String[] in = {"Lorem", null, "ipsum"};
        String[] out = to.func_qstringlist_qstringlist(in);
        System.out.println("TestObject.func_qstringlist_qstringlist=(" + out[0] + "," + out[1] + "," + out[2] + ")");
    }

    public void testArrayList() {
        ArrayList in = new ArrayList();
        in.add(new Double(5.4));
        in.add(null);
        in.add(new Double(3.2));
        ArrayList out = to.func_qvariantlist_qvariantlist(in);
        System.out.println("TestObject.func_qvariantlist_qvariantlist=(" + out.get(0) + "," + out.get(1) + "," + out.get(2) + ")");
    }

    public void testGenericArrayList() {
        ArrayList<Double> in = new ArrayList<Double>();
        in.add(new Double(5.4));
        in.add(null);
        in.add(new Double(3.2));
        ArrayList out = to.func_qvariantlist_qvariantlist(in);
        System.out.println("TestObject.func_qvariantlist_qvariantlist=(" + out.get(0) + "," + out.get(1) + "," + out.get(2) + ")");
    }

    public void testMap() {
        Map in = new HashMap();
        in.put("a", new Integer(5));
        in.put("c", new Integer(4));
        Map out = to.func_qvariantmap_qvariantmap(in);
        System.out.println("TestObject.func_qvariantmap_qvariantmap=(" + out.get("a") + "," + out.get("c") + ")");
    }

    public void testSize() {
        int[] in = {5,4};
        int[] out = to.func_qsize_qsize(in);
        System.out.println("TestObject.func_qsize_qsize=(" + out[0] + "," + out[1] + ")");
    }

    public void testIllegalSize() {
        int[] in = {5,4,3};
        try {
            int[] out = to.func_qsize_qsize(in);
            System.out.println("TestObject.func_qsize_qsize=(" + out[0] + "," + out[1] + ")");
        } catch(IllegalArgumentException e) {
            System.out.println("TestObject.func_qsize_qsize({5,4,3}) exception caught, okay.");
        }
    }

    public void testSizef() {
        double[] in = {16.1803,10.0};
        double[] out = to.func_qsizef_qsizef(in);
        System.out.println("TestObject.func_qsizef_qsizef=(" + out[0] + "," + out[1] + ")");
    }

    public void testPoint() {
        int[] in = {7,11};
        int[] out = to.func_qpoint_qpoint(in);
        System.out.println("TestObject.func_qpoint_qpoint=(" + out[0] + "," + out[1] + ")");
    }

    public void testPointf() {
        double[] in = {465458641.1534, 7894231.1654};
        double[] out = to.func_qpointf_qpointf(in);
        System.out.println("TestObject.func_qpointf_qpointf=(" + out[0] + "," + out[1] + ")");
    }

    public void testRect() {
        int[] in = {1,1,5,4};
        int[] out = to.func_qrect_qrect(in);
        System.out.println("TestObject.func_qrect_qrect=(" + out[0] + "," + out[1] + "," + out[2] + "," + out[3] + ")");
    }

    public void testRectf() {
        double[] in = {0.1, 2.3, 4.5, 6.7};
        double[] out = to.func_qrectf_qrectf(in);
        System.out.println("TestObject.func_qrectf_qrectf=(" + out[0] + "," + out[1] + "," + out[2] + "," + out[3] + ")");
    }

    public void testURL() {
        try {
            URL out = to.func_qurl_qurl(new URL("http://kross.dipe.org"));
            System.out.println("TestObject.func_qurl_qurl=" + out.toString());
        } catch(MalformedURLException e) {
            System.out.println("Meh.");
        }
    }
}
