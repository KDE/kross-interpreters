package org.kde.kdebindings.java.krossjava;

import java.util.*;
import java.util.zip.*;
import java.util.jar.*;
import java.io.*;
import java.net.*;
import java.lang.reflect.*;


public class KrossClassLoader extends URLClassLoader {
	//This is ugly but I can't think of anything better for static access
	private static KrossClassLoader kcl = null;
	private Map storedClasses = new Hashtable();
	private Map extensions = new Hashtable();

	private static final int UNKNOWN_DATA = 0;
	private static final int CLASS_DATA = 1;
	private static final int JAR_DATA = 2;

	public KrossClassLoader(){
		super(new URL[0], KrossClassLoader.class.getClassLoader());
		kcl = this;
	}

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

	public String addJARData(String name, byte[] data){
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
		//TODO - if mf, return correct name
		if(mf != null){
			Attributes attr = mf.getMainAttributes();
			return attr.getValue("Kross-Main");
		} else
			return "";
	}

	//TODO: think about the right exception handling here
	public KrossQExtension addExtension(String name, long p) throws
	  ClassNotFoundException, InstantiationException, IllegalAccessException,
	  NoSuchMethodException, InvocationTargetException {
		KrossQExtension ext = (KrossQExtension)newInstance(name, new Long(p));
		extensions.put(name, ext);
		return ext;
	}

	public Object newInstance(String name) throws
	  ClassNotFoundException, InstantiationException, IllegalAccessException {
		return loadClass(name).newInstance();
	}

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

	public Object newInstance(String name, Object arg) throws
	  ClassNotFoundException, InstantiationException, IllegalAccessException,
	  NoSuchMethodException, InvocationTargetException {
		Object[] args = new Object[1];
		args[0] = arg;
		return newInstance(name, args);
	}

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
/*
	public Class loadClass(String name, boolean resolve) throws ClassNotFoundException{
		//First, we see if it's already loaded.
		Class output = findLoadedClass(name);

		//Next, let's see if it's a custom class.
		if(output == null){
			try {
			output = findClass(name);
			} catch (ClassNotFoundException e){

			}
		}

		//All seems to fail, let's try the native classloader.
		//Exceptions can bubble up from here.
		if(output == null){
			ClassLoader par = getParent();
			if(par == null)
				output = getSystemClassLoader().loadClass(name);
			else
				output = par.loadClass(name);
		}

		//We had a correct result, see if it must be resolved...
		if(resolve && output != null){
			try {
				resolveClass(output);
			} catch(NullPointerException e) {
				//IMPOSSIBLE! (but assertions are java 1.4)
			}
			
		}

		return output;
	}
*/

	public Class findClass(String name) throws ClassNotFoundException{
		if(storedClasses.containsKey(name)){
			return (Class)storedClasses.get(name);
		}
		return super.findClass(name);
	}

//Why implement something like this? :/
//	public void addURL(URL url){
//		super.addURL(url);
//	}

	public static boolean isClassData(byte[] data){
		return getDataType(data) != UNKNOWN_DATA;
	}

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

	public static int byteArrayToInt(byte[] b) {
		int value = 0;
		for (int i = 0; i < 4; i++) {
			int shift = (4 - 1 - i) * 8;
			value += (b[i] & 0x000000FF) << shift;
		}
		return value;
	}

	public boolean isLoadedExtension(String name){
		return extensions.containsKey(name);
	}

	public KrossQExtension getLoadedExtension(String name){
		return (KrossQExtension)extensions.get(name);
	}
}
