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
		Class c = defineClass(null, data, 0, data.length);
		//The passed name may be not the actual classname!
		//We allow both ways of access here.
		if(name != null && !name.equals(""))
			storedClasses.put(name,c);
		storedClasses.put(c.getName(),c);
	}

	public Object newInstance(String name) throws
	  ClassNotFoundException, InstantiationException, IllegalAccessException {
		return loadClass(name).newInstance();
	}

	public Class findClass(String name) throws ClassNotFoundException{
		if(storedClasses.containsKey(name)){
			return (Class)storedClasses.get(name);
		}
		throw new ClassNotFoundException();
	}
}