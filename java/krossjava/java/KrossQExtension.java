package org.kde.kdebindings.java.krossjava;

public abstract class KrossQExtension {
	private long p;

	protected KrossQExtension(Long p) {
		this.p = p.longValue();
	}

	private long getPointer(){
		return p;
	}

	//TODO: perhaps these should be protected?
	public Object invoke(String name, Object[] args){
		return invokeNative(getPointer(), name, args);
	}

	public Object invoke(String name){
		return invokeNative(getPointer(), name, null);
	}

	private native Object invokeNative(long qobjpointer, String name, Object[] args);
}