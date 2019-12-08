// Compile:
// javac Main.java
// Run:
// java Main < input.txt

import java.util.*;
import java.util.function.Function;

public class Main {
	private static final int IMAGE_WIDTH = 25;
	private static final int IMAGE_HEIGHT = 6;
	private static final int PIXELS_PER_LAYER = IMAGE_WIDTH * IMAGE_HEIGHT;

	public static void main(String[] args) {
		try (Scanner in = new Scanner(System.in)) {
			var line = in.nextLine();
			var layers = readLayers(line);

			Optional<Integer> part1Solution = layers.stream()
				.min(Comparator.comparing(countPixelsEqualTo(0)))
				.map(l -> {
					return countPixelsEqualTo(1).apply(l) * countPixelsEqualTo(2).apply(l);
				});

			if (part1Solution.isEmpty()) {
				System.out.println("No solution found.");
			} else {
				System.out.println("Part 1 solution: " + part1Solution.get());
			}
		}
	}

	static List<int[][]> readLayers(String input) {
		var numbers = input.chars()
			.map(c -> c - '0')
			.toArray();

		assert numbers.length % PIXELS_PER_LAYER == 0;

		var layers = new ArrayList<int[][]>();

		for (int i = 0; i < numbers.length; ++i) {
			var layer = i / PIXELS_PER_LAYER;
			if (layer >= layers.size()) {
				layers.add(new int[IMAGE_WIDTH][IMAGE_HEIGHT]);
			}

			var l = layers.get(layer);

			var x = i % IMAGE_WIDTH;
			var y = (i / IMAGE_WIDTH) % (IMAGE_HEIGHT);

			l[x][y] = numbers[i];
		}

		return layers;
	}

	static Function<int[][], Integer> countPixelsEqualTo(int number) {
		return layer -> {
			var counter = 0;

			for (int[] column : layer)
				for (int i : column)
					if (i == number)
						++counter;

			return counter;
		};
	}
}
