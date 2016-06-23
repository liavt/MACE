package net.pleb.graphics;

import net.pleb.system.util.PLVector2f;

public class PLShape {
	public PLVector2f pos;
	public float rot;
	public PLVector2f size;
	public PLVector2f[] verticies;
	
	public PLShape(PLVector2f pos, float rot, PLVector2f size, PLVector2f[] verticies) {
		this.pos = pos;
		this.rot = rot;
		this.size = size;
		this.verticies = verticies;
	}
}
