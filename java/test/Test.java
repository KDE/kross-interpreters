import org.kde.kdebindings.java.krossjava.KrossQExtension;
import java.util.*;
import java.net.*;

public class Test {

    private TestObject to;

    public Test() {
        System.out.println("Hello World !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
        setUp();
        testVoid();
        testName();
        testMultiArgs();
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
        testQObject();

        testConnect();
    }

    public void assertEquals(Object actual, Object expected) {
        String n = actual.getClass().getName();
        if( (actual == null && expected == null) || actual.equals(expected) )
            System.out.println("passed type=" + n + " expected=" + expected);
        else
            System.out.println("FAILED type=" + n + " actual=" + actual + " expected=" + expected);
    }

    public void assertEquals(Object[] actual, Object[] expected) {
        if(actual.length != expected.length) {
            System.out.println("FAILED actual.length=" + actual.length + " expected.length=" + expected.length);
            return;
        }
        for(int i = 0; i < actual.length; i++) {
            if(actual[i] == null && expected[i] == null)
                continue;
            if( actual[i].equals( expected[i] ) )
                continue;
            System.out.println("FAILED type=" + actual[i].getClass().getName() + " actual=" + actual[i] + " expected=" + expected[i]);
            return;
        }
        System.out.println("passed expected=" + expected);
    }

    public void assertEquals(Map actual, Map expected) {
        if(actual.size() != expected.size()) {
            System.out.println("FAILED actual.size=" + actual.size() + " expected.size=" + expected.size());
            return;
        }
        Iterator it = expected.entrySet().iterator();
        while(it.hasNext()) {
            Map.Entry pairs = (Map.Entry)it.next();
            if( ! actual.containsKey(pairs.getKey()) ) {
                System.out.println("FAILED expected key=" + pairs.getKey());
                return;
            }
            Object expectedValue = pairs.getValue();
            Object actualValue = actual.get( pairs.getKey() );
            if( ! actualValue.equals(expectedValue) ) {
                System.out.println("FAILED actual.value=" + actualValue + " expected.value=" + expectedValue);
                return;
            }
        }
        System.out.println("passed expected=" + expected);
    }

    public void setUp() {
        to = (TestObject)(KrossQExtension.importModule("TestObject"));
    }

    public void testName() {
        String name = to.name();
        //System.out.println("TestObject name=" + name);
        assertEquals(name, "TestObject");
    }

    public void testVoid(){
        to.func_void();
        System.out.println("TestObject.func_void");
    }

    public void testMultiArgs() {
        String str = "1 + 2 = ";
        int num = 1+2;
        String result = to.func_qstring_qstring_int(str, num);
        //System.out.println("TestObject.func_qstring_qstring_int=" + result);
        assertEquals(result, str + ",3");
    }

    public void testInt() {
        //System.out.println("# TestObject.func_int_int(5)=" + to.func_int_int(5));
        assertEquals(to.func_int_int(0), 0);
        assertEquals(to.func_int_int(5), 5);
        assertEquals(to.func_int_int(-423), -423);
    }

    public void testUInt() {
        //System.out.println("TestObject.func_uint_uint(5)=" + to.func_uint_uint(5));
        assertEquals(to.func_int_int(0), 0);
        assertEquals(to.func_int_int(8692631), 8692631);
    }

    public void testString() {
        //System.out.println("TestObject.func_qstring_qstring(\"Kross Power!\")=" + to.func_qstring_qstring("Kross Power!"));
        assertEquals(to.func_qstring_qstring(""), "");
        assertEquals(to.func_qstring_qstring("Kross Power!"), "Kross Power!");
    }

    public void testBool() {
        //System.out.println("TestObject.func_bool_bool(true)=" + to.func_bool_bool(true));
        assertEquals(to.func_bool_bool(true), true);
        assertEquals(to.func_bool_bool(false), false);
    }

    public void testDouble() {
        //System.out.println("TestObject.func_double_double(3.14)=" + to.func_double_double(3.14));
        assertEquals(to.func_double_double(3.14), 3.14);
        assertEquals(to.func_double_double(-3.14), -3.14);
    }

    public void testLong() {
        //System.out.println("TestObject.func_qlonglong_qlonglong(1234567890123456L)=" + to.func_qlonglong_qlonglong(1234567890123456L));
        assertEquals(to.func_qlonglong_qlonglong(1234567890123456L), 1234567890123456L);
    }

    public void testULong() {
        //System.out.println("TestObject.func_qulonglong_qulonglong(1234567890123456L)=" + to.func_qulonglong_qulonglong(1234567890123456L));
        assertEquals(to.func_qulonglong_qulonglong(1234567890123456L), 1234567890123456L);
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
        //System.out.println("TestObject.func_qstringlist_qstringlist=(" + out[0] + "," + out[1] + "," + out[2] + ")");
        assertEquals(in,out);
    }

    public void testArrayList() {
        ArrayList in = new ArrayList();
        in.add(new Double(5.4));
        in.add(null);
        in.add(new Double(3.2));
        ArrayList out = to.func_qvariantlist_qvariantlist(in);
        //System.out.println("TestObject.func_qvariantlist_qvariantlist=(" + out.get(0) + "," + out.get(1) + "," + out.get(2) + ")");
        assertEquals(in,out);
    }

    public void testGenericArrayList() {
        ArrayList<Double> in = new ArrayList<Double>();
        in.add(new Double(5.4));
        in.add(null);
        in.add(new Double(3.2));
        ArrayList out = to.func_qvariantlist_qvariantlist(in);
        //System.out.println("TestObject.func_qvariantlist_qvariantlist=(" + out.get(0) + "," + out.get(1) + "," + out.get(2) + ")");
        assertEquals(in,out);
    }

    public void testMap() {
        Map in = new HashMap();
        in.put("a", new Integer(5));
        in.put("c", new Integer(4));
        Map out = to.func_qvariantmap_qvariantmap(in);
        //System.out.println("TestObject.func_qvariantmap_qvariantmap=(" + out.get("a") + "," + out.get("c") + ")");
        assertEquals(in,out);
    }

    public void testSize() {
        int[] in = {5,4};
        int[] out = to.func_qsize_qsize(in);
        //System.out.println("TestObject.func_qsize_qsize=(" + out[0] + "," + out[1] + ")");
        assertEquals(new Integer[]{in[0],in[1]}, new Integer[]{out[0],out[1]});
    }

    public void testIllegalSize() {
        try {
            int[] in = {5,4,3};
            int[] out = to.func_qsize_qsize(in);
            System.out.println("FAILED expected exception but got TestObject.func_qsize_qsize=(" + out[0] + "," + out[1] + ")");
        } catch(IllegalArgumentException e) {
            System.out.println("passed TestObject.func_qsize_qsize({5,4,3}) exception caught, okay.");
        }
    }

    public void testSizef() {
        double[] in = {16.1803,10.0};
        double[] out = to.func_qsizef_qsizef(in);
        //System.out.println("TestObject.func_qsizef_qsizef=(" + out[0] + "," + out[1] + ")");
        assertEquals(new Double[]{in[0],in[1]}, new Double[]{out[0],out[1]});
    }

    public void testPoint() {
        int[] in = {7,11};
        int[] out = to.func_qpoint_qpoint(in);
        //System.out.println("TestObject.func_qpoint_qpoint=(" + out[0] + "," + out[1] + ")");
        assertEquals(new Integer[]{in[0],in[1]}, new Integer[]{out[0],out[1]});
    }

    public void testPointf() {
        double[] in = {465458641.1534, 7894231.1654};
        double[] out = to.func_qpointf_qpointf(in);
        //System.out.println("TestObject.func_qpointf_qpointf=(" + out[0] + "," + out[1] + ")");
        assertEquals(new Double[]{in[0],in[1]}, new Double[]{out[0],out[1]});
    }

    public void testRect() {
        int[] in = {1,1,5,4};
        int[] out = to.func_qrect_qrect(in);
        //System.out.println("TestObject.func_qrect_qrect=(" + out[0] + "," + out[1] + "," + out[2] + "," + out[3] + ")");
        assertEquals(new Integer[]{in[0],in[1],in[2],in[3]}, new Integer[]{out[0],out[1],out[2],out[3]});
    }

    public void testRectf() {
        double[] in = {0.1, 2.3, 4.5, 6.7};
        double[] out = to.func_qrectf_qrectf(in);
        //System.out.println("TestObject.func_qrectf_qrectf=(" + out[0] + "," + out[1] + "," + out[2] + "," + out[3] + ")");
        assertEquals(new Double[]{in[0],in[1],in[2],in[3]}, new Double[]{out[0],out[1],out[2],out[3]});
    }

    public void testURL() {
        try {
            URL out = to.func_qurl_qurl(new URL("http://kross.dipe.org"));
            System.out.println("TestObject.func_qurl_qurl=" + out.toString());
        } catch(MalformedURLException e) {
            System.out.println("Meh.");
        }
    }

    public void testQObject() {
        System.out.println("TestObject.func_qobject_qobject(to [" + to + "])=" + to.func_qobject_qobject(to));
    }

    public void testConnect() {
        try {
            if(to.connect("signalVoid()",this,this.getClass().getMethod("signalVoid"))){
                 to.emitSignalVoid();
             } else {
                 System.out.println("FAILED to connect method to signalVoid");
             }
            if(to.connect("signalBool(bool)",this,this.getClass().getMethod("signalBool",Boolean.class))){
                 to.emitSignalBool(true);
             } else {
                 System.out.println("FAILED to connect method to signalBool");
             }
            if(to.connect("signalInt(int)",this,this.getClass().getMethod("signalInt",Integer.class))){
                 to.emitSignalInt(43);
             } else {
                 System.out.println("FAILED to connect method to signalInt");
             }
            if(to.connect("signalString(QString)",this,this.getClass().getMethod("signalString",String.class))){
                 to.emitSignalString("SignalString");
             } else {
                 System.out.println("FAILED to connect method to signalString");
             }
            if(to.connect("signalObject(QObject*)",this,this.getClass().getMethod("signalObject",KrossQExtension.class))){
                 to.emitSignalObject(to);
             } else {
                 System.out.println("FAILED to connect method to signalObject");
             }
        } catch(NoSuchMethodException e) {
            e.printStackTrace();
        }
    }

    public void signalVoid() {
       System.out.println("passed signalVoid");
    }
    public void signalBool(Boolean b) {
       assertEquals(b, true);
    }
    public void signalInt(Integer i) {
       assertEquals(i, 43);
    }
    public void signalString(String s) {
       assertEquals(s, "SignalString");
    }
    public void signalObject(KrossQExtension obj) {
       assertEquals(obj, to);
    }
}
