package net.pleb.graphics.renderer;

import static org.lwjgl.glfw.GLFW.*;
import static org.lwjgl.opengl.GL11.*;

import org.lwjgl.opengl.GL;

import net.pleb.graphics.PLShape;
import net.pleb.system.util.PLColor;
import net.pleb.system.util.PLVector2f;
import net.pleb.window.PLWindow;

public class PLRendererGL extends PLRenderer {
	
	private PLWindow window;
	
	public PLRendererGL(PLWindow window, boolean vSync) {
		this.window = window;
		
		glfwMakeContextCurrent(window.getGLFWWindow());
		if(vSync) {
			glfwSwapInterval(1);
		} else {
			glfwSwapInterval(0);
		}
		
		GL.createCapabilities();
		
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0, window.getWidth(), 0, window.getHeight(), -1, 1);
		glMatrixMode(GL_MODELVIEW);
	}

	@Override
	public void render(PLShape shape) {
		glPushMatrix();
		
		glColor3f(1, 1, 1);
		
		glTranslatef(shape.pos.x, shape.pos.y, 0);
		glRotatef(shape.rot, 0, 0, 1);
		glScalef(shape.size.x, shape.size.y, 0);
		
		glBegin(GL_POLYGON);
		for(PLVector2f vert:shape.verticies) {
			glVertex2f(vert.x, vert.y);
		}
		glEnd();
		
		glPopMatrix();
	}

	@Override
	public void clear(PLColor color) {
		glClearColor(color.getRedAsFloat(), color.getGreenAsFloat(), color.getBlueAsFloat(), color.getAlphaAsFloat());
		glClear(GL_COLOR_BUFFER_BIT);
	}

	@Override
	public void swapBuffers() {
		glfwSwapBuffers(window.getGLFWWindow());
	}

}
