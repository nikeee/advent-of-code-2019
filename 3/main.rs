// Compile:
// rustc -o 3.out main.rs
// Run:
// ./3.out < input.txt

use std::io;
use std::vec;
use std::collections::HashSet;

#[derive(Debug)]
enum Direction {
	Up { amount: i32 },
	Down { amount: i32 },
	Left { amount: i32 },
	Right { amount: i32 },
}

#[derive(Hash, Eq, PartialEq, Clone, Copy, Debug)]
struct Point {
	x: i32,
	y: i32,
}
impl Point {
	fn manhatten_distance_to(&self, point: &Point) -> i32 {
		(self.x - point.x).checked_abs().unwrap() + (self.y - point.y).checked_abs().unwrap()
	}
}


fn main() -> io::Result<()> {

	let line_0 = read_parsed_directions();
	let line_1 = read_parsed_directions();

	let origin = Point {x: 0, y: 0};

	let line_0_points = get_visited_points(line_0, origin);
	let line_1_points = get_visited_points(line_1, origin);

	let min_distance = line_0_points.intersection(&line_1_points)
			.map(|common_point| origin.manhatten_distance_to(&common_point))
			.min();

	match min_distance {
		Some(distance) => println!("Found minimum distance: {}", distance),
		None => println!("No intersection found"),
	}

	Ok(())
}

fn read_parsed_directions() -> vec::Vec<Direction> {
	let mut line = String::new();
	io::stdin().read_line(&mut line).unwrap();
	return parse_cable(line.trim().to_string());
}

fn get_visited_points(directions: vec::Vec<Direction>, origin: Point) -> HashSet<Point> {
	let mut res = HashSet::new();

	let mut location = origin;

	for op in directions {
		match op {
			Direction::Up{amount} => {
				for _ in 0..amount {
					location.y += 1;
					res.insert(Point {x: location.x, y: location.y});
				}
			},
			Direction::Down{amount} => {
				for _ in 0..amount {
					location.y -= 1;
					res.insert(Point {x: location.x, y: location.y});
				}
			},
			Direction::Left{amount} => {
				for _ in 0..amount {
					location.x -= 1;
					res.insert(Point {x: location.x, y: location.y});
				}
			},
			Direction::Right{amount} => {
				for _ in 0..amount {
					location.x += 1;
					res.insert(Point {x: location.x, y: location.y});
				}
			},
		};
	}

	return res;
}

fn parse_cable(line: String) -> vec::Vec<Direction> {
	return line.split(",")
				.map(String::from)
				.map(|command| {

					let dir = command.chars().next().unwrap();

					let amount_str = without_first(&command);
					let amount = amount_str.parse::<i32>().unwrap();

					return match dir  {
						'R' | 'r' => Direction::Right{amount},
						'U' | 'u' => Direction::Up{amount},
						'D' | 'd' => Direction::Down{amount},
						'L' | 'l' => Direction::Left{amount},
						_ => panic!(),
					}
				})
				.collect();
}

fn without_first(string: &str) -> &str {
    string
        .char_indices()
        .nth(1)
        .and_then(|(i, _)| string.get(i..))
        .unwrap_or("")
}
