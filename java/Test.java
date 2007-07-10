import org.kde.kdebindings.java.krossjava.KrossClassLoader;

public class Test {

    public Test() {
        System.out.println("Hello World !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
        TestObject to = (TestObject)(KrossClassLoader.importModule("TestObject"));
        //String name = to.name();
        //System.out.println("TestObject name=" + name);
        //System.out.println("TestObject.func_int_int(5)=" + to.func_int_int(5));
        System.out.println("TestObject.func_qstring_qstring(\"Kross Power!\")=" + to.func_qstring_qstring("Kross Power!"));
    }

}
