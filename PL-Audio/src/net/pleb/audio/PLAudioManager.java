package net.pleb.audio;

import java.nio.IntBuffer;
import java.util.ArrayList;
import java.util.List;

import org.lwjgl.openal.AL;
import org.lwjgl.openal.ALC;
import org.lwjgl.openal.ALCCapabilities;

import static org.lwjgl.openal.ALC10.*;

public class PLAudioManager {
	
	private static List<PLSound> sounds = new ArrayList<PLSound>();
	
	private static long device;
	private static long context;
	
	public static void init() {
		device = alcOpenDevice("");
		IntBuffer buffer = null;
		context = alcCreateContext(device, buffer);
		alcMakeContextCurrent(context);
		ALCCapabilities ccapabilities = ALC.createCapabilities(device);
		AL.createCapabilities(ccapabilities);
	}
	
	public static void setUniversalSoundVolume(float volume) {
		for(PLSound s:sounds) {
			s.setVolume(volume);
		}
	}
	
	public static void addSound(PLSound sound) {
		sounds.add(sound);
	}
	
	public static void stopAll() {
		for(PLSound s:sounds) {
			s.destroy();
		}
		
		alcCloseDevice(device);
		alcDestroyContext(context);
		ALC.destroy();
	}
}
