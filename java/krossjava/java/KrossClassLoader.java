package org.kde.kdebindings.java.krossjava;

import java.util.*;
import java.net.*;
import java.lang.reflect.*;
import java.io.File;

public class KrossClassLoader extends URLClassLoader {
	//This is ugly but I can't think of anything better for static access
	private static KrossClassLoader kcl = null;
	private Map storedClasses = new Hashtable();
	private Map extensions = new Hashtable();

	public KrossClassLoader(){
		super(new URL[0], KrossClassLoader.class.getClassLoader());
		kcl = this;
	}

	public void addClass(String name, byte[] data){
		//TODO: check difference between compiled
		//and non-compiled, compile if needed
		if(!isClassData(data)){
			//TODO: compile
			System.out.println("Didn't get a valid classfile!");
		}
		//TODO: don't overwrite if name already exists?
		Class c = defineClass(null, data, 0, data.length);
		//The passed name may be not the actual classname!
		//We allow both ways of access here.
		if(name != null && !name.equals(""))
			storedClasses.put(name,c);
		storedClasses.put(c.getName(),c);
	}

	//TODO: think about the right exception handling here
	public void addExtension(String name, long p) throws
	  ClassNotFoundException, InstantiationException, IllegalAccessException,
	  NoSuchMethodException, InvocationTargetException {
		KrossQExtension ext = (KrossQExtension)newInstance(name, new Long(p));
		extensions.put(name, ext);
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

	public Class findClass(String name) throws ClassNotFoundException{
		if(storedClasses.containsKey(name)){
			return (Class)storedClasses.get(name);
		}
		return super.findClass(name);
	}

	public void addURL(URL url){
		super.addURL(url);
	}

	public void addURL(String url) throws MalformedURLException{
		addURL(new URL(url));
	}

	public static boolean isClassData(byte[] data){
		//TODO: endianness?
		if(byteArrayToInt(data) == 0xCAFEBABE)
			return true;
		return false;
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
