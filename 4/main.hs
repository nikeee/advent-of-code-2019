-- Compile:
-- ghc -O3 -o 4.out main.hs
-- Run:
-- ./4.out

import Data.List

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

digits = ['0'..'9']
is_possible_digit :: String -> Char -> Bool
is_possible_digit candidate digit = (isInfixOf [digit, digit] candidate) && not (isInfixOf [digit, digit, digit] candidate)

-- "the two adjacent matching digits are not part of a larger group of matching digits."
-- We use a trick here:
--     We check if "123446" contains "44" and does not contain "444" (for every digit)
--     It is possible because a valid repetition consists out of two digits. If there is a third, the occurrence is not valid. There has to be at leat one, though.
--     The cases of "112111" or "111211" won't happen because the candidates are also filtered to be ascending.
--     Also, "111111" is invalid.
--     "111233" is valid.
has_equal_adjacent_chars_not_in_cluster :: String -> Bool
has_equal_adjacent_chars_not_in_cluster candidate = any (candidate `is_possible_digit`) digits

-- The order in filtering is not relevant.
-- So we put the filter that eliminates the most candidates first.
part_1 input = filter is_ascending (
			filter has_equal_adjacent_chars (
				filter length_6 (
					map show input
				)
			)
		)


part_2 input = filter is_ascending (
			filter has_equal_adjacent_chars_not_in_cluster (
				filter length_6 (
					map show input
				)
			)
		)

main :: IO ()
main = do
	putStrLn ("Part 1: " ++ (show (length (part_1 input_range))))
	putStrLn ("Part 2: " ++ (show (length (part_2 input_range))))
