package net.pleb.audio;

import static org.lwjgl.openal.AL10.*;

import java.io.IOException;
import java.io.InputStream;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.IntBuffer;
import java.nio.ShortBuffer;

import javax.management.RuntimeErrorException;
import javax.sound.sampled.AudioFormat;
import javax.sound.sampled.AudioInputStream;
import javax.sound.sampled.AudioSystem;
import javax.sound.sampled.UnsupportedAudioFileException;

import org.lwjgl.BufferUtils;
import org.lwjgl.openal.AL10;

public class PLSound {
	int ID = 0;
	int bufferID = 0;
	float volume = 0.5f;
	
	public static boolean on = true;
	
	public boolean playing = false;
	private boolean started = false;
	
	public PLSound(String path) {
		this(path, 0.5f);
	}
	
	private static ByteBuffer convert(byte[] data, boolean sixteenbit, ByteOrder order) {
		ByteBuffer dest = ByteBuffer.allocateDirect(data.length);
		dest.order(ByteOrder.nativeOrder());
		ByteBuffer src = ByteBuffer.wrap(data);
		src.order(order);
		if (sixteenbit) {
			ShortBuffer dest_short = dest.asShortBuffer();
			ShortBuffer src_short = src.asShortBuffer();
			while (src_short.hasRemaining())
				dest_short.put(src_short.get());
		} else {
			while (src.hasRemaining())
				dest.put(src.get());
		}
		dest.rewind();
		return dest;
	}
	
	private void loadSound(String path) {
		InputStream is = PLSound.class.getResourceAsStream(path);
		AudioInputStream ais = null;
		try {
			ais = AudioSystem.getAudioInputStream(is);
		} catch (UnsupportedAudioFileException e) {
			e.printStackTrace();
		} catch (IOException e) {
			e.printStackTrace();
		}
		
		AudioFormat audioformat = ais.getFormat();
		
		int channels = 0;
		if (audioformat.getChannels() == 1) {
			if (audioformat.getSampleSizeInBits() == 8) {
				channels = AL10.AL_FORMAT_MONO8;
			} else if (audioformat.getSampleSizeInBits() == 16) {
				channels = AL10.AL_FORMAT_MONO16;
			} else {
				throw new RuntimeErrorException(null, "Illegal sample size");
			}
		} else if (audioformat.getChannels() == 2) {
			if (audioformat.getSampleSizeInBits() == 8) {
				channels = AL10.AL_FORMAT_STEREO8;
			} else if (audioformat.getSampleSizeInBits() == 16) {
				channels = AL10.AL_FORMAT_STEREO16;
			} else {
				throw new RuntimeErrorException(null, "Illegal sample size");
			}
		} else {
			throw new RuntimeErrorException(null, "Only mono or stereo is supported");
		}
		
		ByteBuffer buffer = null;
		try {
			int available = ais.available();
			if(available <= 0) {
				available = ais.getFormat().getChannels() * (int) ais.getFrameLength() * ais.getFormat().getSampleSizeInBits() / 8;
			}
			byte[] buf = new byte[ais.available()];
			int read = 0, total = 0;
			while ((read = ais.read(buf, total, buf.length - total)) != -1
				&& total < buf.length) {
				total += read;
			}
			boolean doubleByte = audioformat.getSampleSizeInBits() == 16;
			ByteOrder order = ByteOrder.LITTLE_ENDIAN;
			if(audioformat.isBigEndian()) {
				order = ByteOrder.BIG_ENDIAN;
			}
			buffer = convert(buf, doubleByte, order);
		} catch (IOException ioe) {
			return;
		}
		
		try {
			ais.close();
		} catch (IOException ioe) {
		}
		
		IntBuffer ibuffer = BufferUtils.createIntBuffer(1);
		alGenBuffers(ibuffer);
		alBufferData(ibuffer.get(0), channels, buffer, (int) audioformat.getSampleRate());
		buffer.clear();
		
		bufferID = ibuffer.get(0);
	}
	
	public PLSound(String path, float volume) {
		this.volume = volume;
		
		loadSound(path);
		
		ID = alGenSources();
		
		alSourcei(ID, AL_BUFFER, bufferID);
		//alSource3f(ID, AL_POSITION, pos.x,pos.y,pos.z);
		alSource3f(ID, AL_VELOCITY, 0,0,0);
		alSourcef(ID, AL_GAIN, volume);
		
		PLAudioManager.addSound(this);
	}
	
	public float getVolume() {
		return volume;
	}
	
	public void setVolume(float vol) {
		volume = vol;
		alSourcef(ID, AL_GAIN, volume);
	}
	
	public void volUp(float amount) {
		volume += amount;
		if(volume > 1) {
			volume = 1;
		}
		
		alSourcef(ID, AL_GAIN, volume);
	}
	
	public void volDown(float amount) {
		volume -= amount;
		if(volume < 0) {
			volume = 0;
		}
		alSourcef(ID, AL_GAIN, volume);
	}
	
	public void destroy() {
		stop();
		alDeleteSources(ID);
		alDeleteBuffers(bufferID);
	}
	
	public void play() {
		if(!on) {
			return;
		}
		alSourcePlay(ID);
		playing = true;
		started = true;
	}
	
	public void pause() {
		if(!on) {
			return;
		}
		alSourcePause(ID);
		playing = false;
	}
	
	public void toggle() {
		if(!on) {
			return;
		}
		if(!playing && started) {
			alSourcePlay(ID);
			playing = true;
		} else if (playing) {
			alSourcePause(ID);
			playing = false;
		}
	}
	
	public void resume() {
		if(!on) {
			return;
		}
		if(!playing && started) {
			alSourcePlay(ID);
		}
		playing = true;
	}
	
	public void stop() {
		alSourceStop(ID);
		playing = false;
		started = false;
	}
	
	public void setLooping(boolean loop) {
		if(loop) {
			alSourcei (ID, AL_LOOPING,  AL_TRUE);
		} else {
			alSourcei (ID, AL_LOOPING,  AL_FALSE);
		}
		
	}
}
