// Compile:
// rustc -O -o 3.out main.rs
// Run:
// ./3.out < input.txt

use std::{collections::HashMap, error::Error, io};

#[derive(Debug)]
struct Path {
    amount: i32,
    direction: Direction,
}

#[derive(Debug)]
enum Direction {
    Up,
    Down,
    Left,
    Right,
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

fn main() -> Result<(), Box<dyn Error>> {
    let line_0 = read_parsed_paths()?;
    let line_1 = read_parsed_paths()?;

    let origin = Point { x: 0, y: 0 };

    let line_0_step_map = get_visited_points(line_0, origin);
    let line_1_step_map = get_visited_points(line_1, origin);

    // Part 1
    let min_distance_to_origin = line_0_step_map
        .iter()
        .filter(|(point, _)| line_1_step_map.contains_key(point))
        .map(|(common_point, _)| origin.manhatten_distance_to(&common_point))
        .min();

    match min_distance_to_origin {
        Some(distance) => println!("Part 1: Found minimum distance: {}", distance),
        None => println!("No intersection found"),
    }

    // Part 2
    let min_distance_in_steps = line_0_step_map
        .iter()
        .filter_map(|(point, line_0_steps)| {
            line_1_step_map
                .get(point)
                .map(|line_1_steps| (line_0_steps, line_1_steps))
        })
        .map(|(c0, c1)| c0 + c1)
        .min();

    match min_distance_in_steps {
        Some(distance) => println!(
            "Part 2: The fewest combined steps the wires must take to reach an intersection: {}",
            distance
        ),
        _ => println!("No intersection found"),
    }

    Ok(())
}

fn read_parsed_paths() -> Result<Vec<Path>, Box<dyn Error>> {
    let mut line = String::new();
    io::stdin().read_line(&mut line)?;

    parse_cable(line.trim().to_string())
}

fn get_visited_points(paths: Vec<Path>, mut location: Point) -> HashMap<Point, u32> {
    use Direction::*;

    let mut steps_map = HashMap::with_capacity(paths.len());

    let mut current_step_count: u32 = 0;

    for path in paths {
        let move_point: fn(&mut Point) = match path.direction {
            Up => |p| p.y += 1,
            Down => |p| p.y -= 1,
            Left => |p| p.x -= 1,
            Right => |p| p.x += 1,
        };

        for _ in 0..path.amount {
            current_step_count += 1;
            move_point(&mut location);
            steps_map.entry(location).or_insert(current_step_count);
        }
    }

    steps_map
}

fn parse_cable(line: String) -> Result<Vec<Path>, Box<dyn Error>> {
    use Direction::*;

    line.split(",")
        .map(String::from)
        .filter(|s| !s.is_empty())
        .map(|command| {
            let dir = command.chars().next().unwrap().to_ascii_uppercase();

            let amount_str = without_first(&command);
            let amount = amount_str.parse::<i32>()?;

            let res = Path {
                amount,
                direction: match dir {
                    'R' => Right,
                    'U' => Up,
                    'D' => Down,
                    'L' => Left,
                    _ => Err(InvalidDirection(dir))?,
                },
            };

            Ok(res)
        })
        .collect()
}

#[derive(Debug)]
struct InvalidDirection(char);
impl std::fmt::Display for InvalidDirection {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        write!(f, "Unable to parse direction '{}'", self.0)
    }
}
impl Error for InvalidDirection {}

fn without_first(string: &str) -> &str {
    string
        .char_indices()
        .nth(1)
        .and_then(|(i, _)| string.get(i..))
        .unwrap_or("")
}
