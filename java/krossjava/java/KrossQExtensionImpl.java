package org.kde.kdebindings.java.krossjava;

public abstract class KrossQExtensionImpl implements KrossQExtension {
	private long p;

	public KrossQExtensionImpl(Long p) {
		this.p = p.longValue();
	}

	public long getPointer(){
		return p;
	}
}
