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



    // Following lines are samples for arguments... The question is, if it wouldn't be easier
    // to provide multiple invoke's for every number of arguments one (afair signals and slots
    // are not able to handle more then 9 anyway + we would work around the rather expensive
    // list packing/unpacking.

    //public boolean func_bool_bool(boolean b){ return (boolean)invoke("func_bool_bool", b); }
    public Integer func_int_int(Integer i){
        Object[] oa = {i};
        return (Integer)invoke("func_int_int", oa);
    }
    //public int func_uint_uint(int i){ return (int)invoke("func_uint_uint", i); }
    //public double func_double_double(double d){ return (double)invoke("func_double_double", d); }
    //public Byte[] func_qbytearray_qbytearray(Byte[] b){ return (Byte[])invoke("func_qbytearray_qbytearray", b); }
    public String func_qstring_qstring(String s) {
        Object[] oa = {s};
        return (String)invoke("func_qstring_qstring", oa);
    }
    //public String[] func_qstringlist_qstringlist(String[] s){ return (String[])invoke("func_qstringlist_qstringlist", s); }
    //public ArrayList func_qvariantlist_qvariantlist(ArrayList l){ return (ArrayList)invoke("func_qvariantlist_qvariantlist", l); }
    //public Map func_qvariantmap_qvariantmap(Map m){ return (Map)invoke("func_qvariantmap_qvariantmap", m); }

}
