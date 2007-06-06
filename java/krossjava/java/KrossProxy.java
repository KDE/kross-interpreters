package org.kde.kdebindings.java.krossjava;

import java.lang.*;
import java.lang.reflect.*;

public class KrossProxy implements java.lang.reflect.InvocationHandler {
	private KrossQExtension obj;

	public static Object newInstance(KrossQExtension obj) {
		return java.lang.reflect.Proxy.newProxyInstance(
			obj.getClass().getClassLoader(),
			obj.getClass().getInterfaces(),
			new KrossProxy(obj));
	}

	private KrossProxy(KrossQExtension obj) {
		this.obj = obj;
	}

	public Object invoke(Object proxy, Method method, Object[] args) throws Throwable {
		Object result;
		try {
			System.out.println("[PROXY] Going to execute QObj method " + method.getName());
			result = invokeNative(obj.getPointer(), method.getName(), args);
		} catch (Exception e) {
			throw new RuntimeException("unexpected invocation exception: " +
				e.getMessage());
		} finally {
			System.out.println("[PROXY] Finished executing QObj method " + method.getName());
		}
		return result;
	}

	public native Object invokeNative(long qobjpointer, String name, Object[] args);
}
