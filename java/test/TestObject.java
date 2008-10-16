import org.kde.kdebindings.java.krossjava.*;
import java.util.*;
import java.net.URL;

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



    // Following lines are samples for arguments... The question is, if it wouldn't be easier
    // to provide multiple invoke's for every number of arguments one (afair signals and slots
    // are not able to handle more than 9 anyway + we would work around the rather expensive
    // list packing/unpacking.

    public void func_void(){ invoke("func_void"); }
    public Boolean func_bool_bool(Boolean b){ return (Boolean)invoke("func_bool_bool", b); }
    public Integer func_int_int(Integer i){ return (Integer)invoke("func_int_int", i); }
    public Integer func_uint_uint(Integer i){ return (Integer)invoke("func_uint_uint", i); }
    public Double func_double_double(Double d){ return (Double)invoke("func_double_double", d); }
    public Long func_qlonglong_qlonglong(Long l){ return (Long)invoke("func_qlonglong_qlonglong", l); }
    public Long func_qulonglong_qulonglong(Long l){ return (Long)invoke("func_qulonglong_qulonglong", l); }
    public String func_qstring_qstring_int(String str, Integer i){ return (String)invoke("func_qstring_qstring_int", str, i); }
    public byte[] func_qbytearray_qbytearray(byte[] b){ return (byte[])invoke("func_qbytearray_qbytearray", b); }
    public String func_qstring_qstring(String s) { return (String)invoke("func_qstring_qstring", s); }
    public String[] func_qstringlist_qstringlist(String[] s){ return (String[])invoke("func_qstringlist_qstringlist", s); }
    public ArrayList func_qvariantlist_qvariantlist(ArrayList l){ return (ArrayList)invoke("func_qvariantlist_qvariantlist", l); }
    public Map func_qvariantmap_qvariantmap(Map m){ return (Map)invoke("func_qvariantmap_qvariantmap", m); }
    public int[] func_qsize_qsize(int[] i){ return (int[])invoke("func_qsize_qsize", i); }
    public double[] func_qsizef_qsizef(double[] i){ return (double[])invoke("func_qsizef_qsizef", i); }
    public int[] func_qpoint_qpoint(int[] i){ return (int[])invoke("func_qpoint_qpoint", i); }
    public double[] func_qpointf_qpointf(double[] i){ return (double[])invoke("func_qpointf_qpointf", i); }
    public int[] func_qrect_qrect(int[] i){ return (int[])invoke("func_qrect_qrect", i); }
    public double[] func_qrectf_qrectf(double[] i){ return (double[])invoke("func_qrectf_qrectf", i); }
    public URL func_qurl_qurl(URL u){ return (URL)invoke("func_qurl_qurl", u); }
    public KrossQExtension func_qobject_qobject(KrossQExtension obj){ return (KrossQExtension)invoke("func_qobject_qobject", obj); }

    public void emitSignalVoid(){ invoke("emitSignalVoid"); }
    public void emitSignalBool(Boolean b){ invoke("emitSignalBool", b); }
    public void emitSignalInt(Integer i){ invoke("emitSignalInt", i); }
    public void emitSignalString(String s){ invoke("emitSignalString", s); }
    public void emitSignalObject(KrossQExtension obj){ invoke("emitSignalObject", obj); }
}
