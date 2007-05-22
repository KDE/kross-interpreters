package org.kde.kdebindings.java.krossjava;

import java.util.*;

public class KrossClassLoader extends ClassLoader {
	private Map storedClasses = new Hashtable();

	public KrossClassLoader(){
		super(KrossClassLoader.class.getClassLoader());
	}

	public void addClass(String name, byte[] data){
		//TODO: check difference between compiled
		//and non-compiled, compile if needed
		//TODO: don't overwrite if name already exists?
		//TODO: if name = "", make up a new one
		storedClasses.put(name,data);
	}

	public Object newInstance(String name) throws
	  ClassNotFoundException, InstantiationException, IllegalAccessException {
		return loadClass(name).newInstance();
	}

	public Class findClass(String name) throws ClassNotFoundException{
		if(storedClasses.containsKey(name)){
			byte[] b = (byte[])storedClasses.get(name);
			return defineClass(name, b, 0, b.length);
		}
		throw new ClassNotFoundException();
	}
}