package net.pleb.graphics.shapes;

import net.pleb.graphics.PLShape;
import net.pleb.system.util.PLVector2f;

public class PLShapeRectangle extends PLShape{

	private static final PLVector2f[] VERTICIES = {
				new PLVector2f(-0.5f, -0.5f),
				new PLVector2f( 0.5f, -0.5f),
				new PLVector2f( 0.5f,  0.5f),
				new PLVector2f(-0.5f,  0.5f)
			};
	
	public PLShapeRectangle(PLVector2f pos, PLVector2f rot, PLVector2f size) {
		super(pos, rot, size, VERTICIES);
	}
}
