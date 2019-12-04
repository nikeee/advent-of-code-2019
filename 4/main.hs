-- Compile:
-- ghc -o 4.out main.hs
-- Run:
-- ./4.out

-- "Your puzzle input is 307237-769058."
input_range = [307237..769058]

-- "It is a six-digit number."
length_6 candidate = (length candidate) == 6

-- "The value is within the range given in your puzzle input."
-- (Satisfied by choosing the range as input)

-- "Two adjacent digits are the same (like 22 in 122345)."
has_equal_adjacent_chars [] = False
has_equal_adjacent_chars [_] = False
has_equal_adjacent_chars (a:b:xs) = (a == b) || has_equal_adjacent_chars (b:xs)

-- "Going from left to right, the digits never decrease;"
-- "they only ever increase or stay the same (like 111123 or 135679)."
is_ascending [] = True
is_ascending [_] = True
is_ascending (a:b:xs) = a <= b && is_ascending (b:xs)

-- The order in filtering is not relevant.
-- So we put the filter that eliminates the most candidates first.
part_1 = filter is_ascending (
			filter has_equal_adjacent_chars (
				filter length_6 (
					map show input_range
				)
			)
		)

main =
	putStrLn ("Part 1: " ++ (show (length part_1)))
