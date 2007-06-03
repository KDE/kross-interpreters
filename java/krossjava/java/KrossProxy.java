package org.kde.kdebindings.java.krossjava;

import java.lang.*;
import java.lang.reflect.*;

public class KrossProxy implements java.lang.reflect.InvocationHandler {
	private Object obj;

	public static Object newInstance(Object obj) {
		return java.lang.reflect.Proxy.newProxyInstance(
			obj.getClass().getClassLoader(),
			obj.getClass().getInterfaces(),
			new KrossProxy(obj));
	}

	private KrossProxy(Object obj) {
		this.obj = obj;
	}

	public Object invoke(Object proxy, Method method, Object[] args) throws Throwable {
		Object result;
		try {
			System.out.println("[PROXY] before method " + method.getName());
			result = method.invoke(obj, args);
		} catch (InvocationTargetException e) {
			throw e.getTargetException();
		} catch (Exception e) {
			throw new RuntimeException("unexpected invocation exception: " +
				e.getMessage());
		} finally {
			System.out.println("[PROXY] after method " + method.getName());
		}
		return result;
	}
}
