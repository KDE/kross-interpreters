package org.kde.kdebindings.java.krossjava;

import java.util.*;
import java.util.zip.*;
import java.util.jar.*;
import java.io.*;
import java.net.*;
import java.lang.reflect.*;

/**
* KrossClassLoader is a ClassLoader that allows the required flexibility
* to make the bridge between Java and C.
*/
public class KrossClassLoader extends URLClassLoader {
	//This is ugly but I can't think of anything better for static access
	private static KrossClassLoader kcl = null;
	private Map storedClasses = new Hashtable();
	private Map extensions = new Hashtable();

	public static final int UNKNOWN_DATA = 0;
	public static final int CLASS_DATA = 1;
	public static final int JAR_DATA = 2;

	/**
	* Constructor.
	*/
	public KrossClassLoader(){
		super(new URL[0], KrossClassLoader.class.getClassLoader());
		kcl = this;
	}

	/**
	* Defines code in this classloader. This can be used to define an extension, a script,
	* or even a collection of classes via a JAR file. This method inspects the given data
	* and then executes addSingleClass or addJARData.
	* @param name A name to register the code by. This is used to prevent double definitions.
	* @param data The raw class data, either Java bytecode or a JAR file.
	* @return The correct classname of the added script.
	*/
	public String addClass(String name, byte[] data){
		//TODO: check difference between compiled
		//and non-compiled, compile if needed
		switch(getDataType(data)){
			case UNKNOWN_DATA:
				//TODO: compile
				System.out.println("Didn't get a valid script!");
				return "";
			case CLASS_DATA:
				return addSingleClass(name, data);
			case JAR_DATA:
				return addJARData(name, data);
			default:
				System.out.println("Unknown class data!");
				return "";
		}
	}

	/**
	* Defines a new Java class.
	* @param name A name to register the code by. This is used to prevent double definitions.
	* @param data A complete class definition in bytecode.
	* @return The real classname as defined by the bytecode.
	*/
	public String addSingleClass(String name, byte[] data){
		//TODO: check difference between compiled
		//and non-compiled, compile if needed
		if(getDataType(data) != CLASS_DATA){
			//TODO: compile
			System.out.println("Didn't get a valid classfile!");
		}
		
		if(storedClasses.containsKey(name)){
			//System.out.println("Class " + name + " already loaded.");
			return ((Class)storedClasses.get(name)).getName();
		}
		try {
			Class c = defineClass(null, data, 0, data.length);
			//The passed name may be not the actual classname!
			//We allow both ways of access here.
			if(name != null && !name.equals(""))
				storedClasses.put(name,c);
			storedClasses.put(c.getName(),c);
			return c.getName();
		} catch (LinkageError e) {
			e.printStackTrace();
		}
		return "";
	}

	/**
	* Defines a set of new classes. This makes it easy to add complex scripts.
	* Every .class file in the JAR is defined as a new class. If the manifest contains a
	* "Kross-Main" attribute in the main section, the value is considered to be the entry
	* class of a script and will be returned as the script name.
	* @param name A name to register the code by. This is used to prevent double definitions.
	* @param data The contents of a JAR file.
	* @return The value of the Kross-Main attribute, or "" if none could be found.
	*/
	public String addJARData(String name, byte[] data){
		//TODO: perhaps make this useful as collection of KrossQExtensions, too?
		ZipInputStream zis = new ZipInputStream(new ByteArrayInputStream(data));
		Manifest mf = null;
		try{
			byte[] buff = new byte[1024];
			ZipEntry entry = zis.getNextEntry();
			while( entry != null){
				String entryname = entry.getName();
				if(entryname.endsWith(".class")){
					ByteArrayOutputStream bos = new ByteArrayOutputStream();
					int actread = zis.read(buff);
					while(actread > 0){
						bos.write(buff, 0, actread);
						actread = zis.read(buff);
					}
					addSingleClass(entryname, bos.toByteArray());
				} else if(entryname.equals("META-INF/MANIFEST.MF")) {
					mf = new Manifest(zis);

				}
				entry = zis.getNextEntry();
			}
		} catch(IOException e) {
			//I don't think this can happen, unless perhaps with wrong data... Hmm.
			e.printStackTrace();
		}
		if(mf != null){
			Attributes attr = mf.getMainAttributes();
			String val = attr.getValue("Kross-Main");
			if(val != null)
				return val;
			else
				return "";
		} else
			return "";
	}

	/**
	* Adds an extension to the list of known extensions that can be imported.
	* @param name The name of the previously-defined KrossQExtension class that bridges this extension.
	* @param p A pointer to the JVMExtension in the C world.
	* @return An instance of the KrossQExtension, set up with the given pointer.
	*/
	public KrossQExtension addExtension(String name, long p) throws
	  ClassNotFoundException, InstantiationException, IllegalAccessException,
	  NoSuchMethodException, InvocationTargetException {
		//TODO: think about the right exception handling here
		KrossQExtension ext = (KrossQExtension)newInstance(name, new Long(p));
		extensions.put(name, ext);
		return ext;
	}

	/**
	* Creates a new instance of a given class with the default constructor.
	*/
	public Object newInstance(String name) throws
	  ClassNotFoundException, InstantiationException, IllegalAccessException {
		return loadClass(name).newInstance();
	}

	/**
	* Creates a new instance of a given class, using the given objects as parameters
	* to the constructor.
	*/
	public Object newInstance(String name, Object[] args) throws
	  ClassNotFoundException, InstantiationException, IllegalAccessException,
	  NoSuchMethodException, InvocationTargetException {
		Class c = loadClass(name);
		Class[] sig = new Class[args.length];
		for(int i=0;i<args.length;i++) {
			sig[i] = args[i].getClass();
		}
		Constructor con = c.getConstructor(sig);
		return con.newInstance(args);
	}

	/**
	* Creates a new instance of a given class, using a single object as parameter to the constructor.
	*/
	public Object newInstance(String name, Object arg) throws
	  ClassNotFoundException, InstantiationException, IllegalAccessException,
	  NoSuchMethodException, InvocationTargetException {
		Object[] args = new Object[1];
		args[0] = arg;
		return newInstance(name, args);
	}

	/**
	* Returns a loaded KrossQExtension to use in scripts.
	* @param name The name by which a module was registered.
	* @return The KrossQExtension bridging the given module.
	*/
	public static KrossQExtension importModule(String name) {
		if(kcl == null){
			//TODO: either exception or C++ error handling
			System.out.println("Oops, KCL not initialized yet!");
			return null;
		}
		if(kcl.isLoadedExtension(name)){
			return kcl.getLoadedExtension(name);
		} else {
			//TODO: throw exception
			System.out.println("Module not found: " + name);
			return null;
		}
	}

	/**
	* Searches a class by name.
	* @see URLClassLoader#findClass(String)
	*/
	public Class findClass(String name) throws ClassNotFoundException{
		if(storedClasses.containsKey(name)){
			return (Class)storedClasses.get(name);
		}
		return super.findClass(name);
	}

	/**
	* Checks whether data is loadable by this classloader.
	* @param data The data to be inspected.
	* @return True if the data is in a known format (class bytecode, JAR), false otherwise.
	*/
	public static boolean isClassData(byte[] data){
		return getDataType(data) != UNKNOWN_DATA;
	}

	/**
	* Returns the type of data, as determined by magic number.
	* @param data The data to be inspected.
	* @return One of UNKNOWN_DATA, CLASS_DATA or JAR_DATA.
	*/
	public static int getDataType(byte[] data){
		if(data == null || data.length < 4)
			return UNKNOWN_DATA;
		//TODO: endianness?
		int magic = byteArrayToInt(data);
		if(magic == 0xCAFEBABE)
			return CLASS_DATA;
		if(magic == 0x504b0304) //PK\003\004
			return JAR_DATA;
		return UNKNOWN_DATA;
	}

	/**
	* Returns the magic number for some data.
	* @param data The data to be inspected.
	* @return An int representing the numeric value of the first four bytes.
	*/
	public static int byteArrayToInt(byte[] b) {
		int value = 0;
		for (int i = 0; i < 4; i++) {
			int shift = (4 - 1 - i) * 8;
			value += (b[i] & 0x000000FF) << shift;
		}
		return value;
	}

	/**
	* Checks whether a given name is associated to a module.
	*/
	public boolean isLoadedExtension(String name){
		return extensions.containsKey(name);
	}

	/**
	* Returns the module with the given name.
	*/
	public KrossQExtension getLoadedExtension(String name){
		return (KrossQExtension)extensions.get(name);
	}
}
