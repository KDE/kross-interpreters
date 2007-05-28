package org.kde.kdebindings.java.krossjava;

import java.util.*;
import java.net.*;
import java.io.File;

public class KrossClassLoader extends URLClassLoader {
	private Map storedClasses = new Hashtable();

	public KrossClassLoader(){
		super(new URL[0], KrossClassLoader.class.getClassLoader());
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
		//TODO: perhaps this is not always a file?
		File f = new File(url);
		addURL(f.toURL());
	}
}