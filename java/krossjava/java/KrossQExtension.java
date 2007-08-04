package org.kde.kdebindings.java.krossjava;

import java.lang.reflect.Method;

public abstract class KrossQExtension {
	private long p;

	protected KrossQExtension(Long p) {
		this.p = p.longValue();
	}

	private long getPointer(){
		return p;
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

	public boolean connect(String signal, Object receiver, Method member){
		return connect(getPointer(), signal, receiver, member);
	}

	private native boolean connect(long qobjpointer, String signal, Object receiver, Method member);
}