import org.kde.kdebindings.java.krossjava.KrossClassLoader;

public class Test {

    public Test() {
        System.out.println("Hello World !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
        TestObject to = (TestObject)(KrossClassLoader.importModule("TestObject"));
        String name = to.name();
        System.out.println("TestObject name=" + name);
    }

}
