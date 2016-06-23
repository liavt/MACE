package net.pleb.system.util;

import java.security.SecureRandom;
import java.util.Random;

public final class PLRandom {
	// static class for calling random numbers.
	// saves memory so you dont need to call a new random() every single class.
	// there is only one global random()
	private static SecureRandom rand = new SecureRandom();

	private PLRandom() {
	}

	public static String randomLetter() {
		return String.valueOf((char) (rand.nextInt(26) + 'a'));
	}

	/**
	 * Has a 50% chance of returning negative or positive
	 * 
	 * @return an int that is either negative or positive
	 */
	public static int rollSigned() {
		return PLRandom.rollBool() ? -1 : 1;
	}

	public static float rollFloat() {
		return rand.nextFloat();
	}

	/**
	 * Identical to {@link Random#nextInt}, except for the fact that it adds one
	 * to the result
	 * <p>
	 * Returns a pseudorandom number in between 1 and the parameter num.
	 * 
	 * @param num
	 *            The maximum number that can be returned
	 * 
	 * @return {@link Random#nextInt}+1
	 */
	public static int roll(int num) {
		if (num < 0) {
			throw new IllegalArgumentException("Can't roll a number less than 0!");
		} else if (num == 0) {
			return 0;
		} else {
			return rand.nextInt(num) + 1;
		}
	}

	public static int roll(int num, int mod) {
		return roll(num) + mod;
	}

	public static boolean rollBool() {
		return rand.nextBoolean();
	}

	// returns one of the random enum
	public static <T extends Enum<?>> T randomEnum(Class<T> clazz) {
		int x = rand.nextInt(clazz.getEnumConstants().length);
		return clazz.getEnumConstants()[x];
	}

	// has a num2 percent chance to return true
	public static boolean rollPercent(double num2, int mod) {
		double num = num2;
		String text = Double.toString(Math.abs(num));
		int integerPlaces = text.indexOf('.');
		int decimalPlaces = text.length() - integerPlaces - 1;
		// this loop is to convert num2 into a whole number, because the random
		// class cant roll a random double with a specific amount of decimal
		// points
		int multiplier = (10 * (decimalPlaces - 1));
		int totalnum = 100 + multiplier;
		num *= multiplier + 1;
		int a = PLRandom.roll(totalnum);
		return a <= (num + mod);
	}

	public static boolean rollPercent(double num2) {
		return rollPercent(num2, 0);
	}

	public static boolean rollPercent(float num) {
		return rollPercent((double) num);
	}

	public static boolean rollPercent(int i) {
		return PLRandom.rollPercent(i, 0);
	}

	public static PLColor randomColor() {
		return new PLColor(PLRandom.roll(255), PLRandom.roll(255), PLRandom.roll(255));
	}
}
