package org.kde.kdebindings.java.krossjava;

public abstract class KrossQExtension {
	private long p;

	public KrossQExtension(Long p) {
		this.p = p.longValue();
	}

	public long getPointer(){
		return p;
	}

	public Object invoke(String name, Object[] args){
		return invokeNative(getPointer(), name, args);
	}

	public Object invoke(String name){
		return invokeNative(getPointer(), name, null);
	}

	public native Object invokeNative(long qobjpointer, String name, Object[] args);
}