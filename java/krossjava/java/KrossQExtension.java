package org.kde.kdebindings.java.krossjava;

import java.lang.reflect.Method;

/**
* KrossQExtension represents a QObject in the Java world. Methods can be called on this
* object just as if they would be called on the actual QObject. Usually, the only method
* needed from this class is connect(). The various invoke methods are provided just-in-case,
* but are used in the actual subclasses.
*/
public abstract class KrossQExtension {
	private long p;

	protected KrossQExtension(Long p) {
		this.p = p.longValue();
	}

	private long getPointer(){
		return p;
	}

	/**
	* Returns a loaded KrossQExtension to use in scripts.
	* @param name The name by which a module was registered.
	* @return The KrossQExtension bridging the given module.
	*/
	public static KrossQExtension importModule(String name) {
		return KrossClassLoader.importModule(name);
	}

	public Object invoke(String name){
		return invokeNative(getPointer(), name, 0, null, null, null, null,
			null, null, null, null, null, null);
	}

	public Object invoke(String name, Object arg1){
		return invokeNative(getPointer(), name, 1, arg1, null, null, null,
			null, null, null, null, null, null);
	}

	public Object invoke(String name, Object arg1, Object arg2){
		return invokeNative(getPointer(), name, 2, arg1, arg2, null, null,
			null, null, null, null, null, null);
	}

	public Object invoke(String name, Object arg1, Object arg2,
			Object arg3){
		return invokeNative(getPointer(), name, 3, arg1, arg2, arg3, null,
			null, null, null, null, null, null);
	}

	public Object invoke(String name, Object arg1, Object arg2,
			Object arg3, Object arg4){
		return invokeNative(getPointer(), name, 4, arg1, arg2, arg3, arg4,
			null, null, null, null, null, null);
	}

	public Object invoke(String name, Object arg1, Object arg2,
			Object arg3, Object arg4, Object arg5){
		return invokeNative(getPointer(), name, 5, arg1, arg2, arg3, arg4,
			arg5, null, null, null, null, null);
	}

	public Object invoke(String name, Object arg1, Object arg2,
			Object arg3, Object arg4, Object arg5,
			Object arg6){
		return invokeNative(getPointer(), name, 6, arg1, arg2, arg3, arg4,
			arg5, arg6, null, null, null, null);
	}

	public Object invoke(String name, Object arg1, Object arg2,
			Object arg3, Object arg4, Object arg5,
			Object arg6, Object arg7){
		return invokeNative(getPointer(), name, 7, arg1, arg2, arg3, arg4,
			arg5, arg6, arg7, null, null, null);
	}

	public Object invoke(String name, Object arg1, Object arg2,
			Object arg3, Object arg4, Object arg5,
			Object arg6, Object arg7, Object arg8){
		return invokeNative(getPointer(), name, 8, arg1, arg2, arg3, arg4,
			arg5, arg6, arg7, arg8, null, null);
	}

	public Object invoke(String name, Object arg1, Object arg2,
			Object arg3, Object arg4, Object arg5,
			Object arg6, Object arg7, Object arg8,
			Object arg9){
		return invokeNative(getPointer(), name, 9, arg1, arg2, arg3, arg4,
			arg5, arg6, arg7, arg8, arg9, null);
	}

	public Object invoke(String name, Object arg1, Object arg2,
			Object arg3, Object arg4, Object arg5,
			Object arg6, Object arg7, Object arg8,
			Object arg9, Object arg10){
		return invokeNative(getPointer(), name, 10, arg1, arg2, arg3, arg4,
			arg5, arg6, arg7, arg8, arg9, arg10);
	}

	private native Object invokeNative(long qobjpointer, String name, int argc,
		Object arg0, Object arg1, Object arg2, Object arg3,
		Object arg4, Object arg5, Object arg6, Object arg7,
		Object arg8, Object arg9);

	/**
	* Connects a Qt signal to a Java method. When the signal is emitted, the function will be called.
	*
	* @param signal The signature of the signal to connect with, for example valueChanged(int).
	* @param receiver The object on which to call the method.
	* @param member The method to call when a signal is emitted. Note that the signature of this method should match
	* the one of the signal.
	* @return True if the connection succeeded, false if not.
	*/
	public boolean connect(String signal, Object receiver, Method member){
		return connect(getPointer(), signal, receiver, member);
	}

	private native boolean connect(long qobjpointer, String signal, Object receiver, Method member);
}