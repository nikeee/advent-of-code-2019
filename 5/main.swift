// Compile:
// swiftc -o 5.out main.swift
// Run:
// ./5.out < input.txt

enum OpCode: Int {
	case nop = 0 // Not defined in task, usd to make parsing easier
	case add = 1
	case multiply = 2
	case input = 3
	case output = 4
	case halt = 99
}


func runProgram(state: inout [Int]) -> Int? {
	var pc = 0;
	while(
		pc < state.count
		&& state[pc] != OpCode.halt.rawValue
	) {

		let instruction = state[pc]
		let opCode = instruction % 100

		// print("\(pc): \(instruction)")

		switch opCode {
			case OpCode.nop.rawValue:
				pc += 1
				continue
			case OpCode.add.rawValue:

				guard let operand1 = getParameterValue(pc, state, instruction, parameterNumber: 1) else {
					return nil
				}
				guard let operand2 = getParameterValue(pc, state, instruction, parameterNumber: 2) else {
					return nil
				}

				let targetAddress = state[pc + 3] // Always an address

				state[targetAddress] = operand1 + operand2

				pc += 4
			case OpCode.multiply.rawValue:

				guard let operand1 = getParameterValue(pc, state, instruction, parameterNumber: 1) else {
					return nil
				}
				guard let operand2 = getParameterValue(pc, state, instruction, parameterNumber: 2) else {
					return nil
				}

				let targetAddress = state[pc + 3] // Always an address

				state[targetAddress] = operand1 * operand2

				pc += 4
			case OpCode.input.rawValue:

				let targetAddress = state[pc + 1]

				// Bounds checks
				if targetAddress >= state.count {
					return nil
				}

				print("Please enter something:")

				let value = Int(readLine(strippingNewline: true)!)!

				state[targetAddress] = value

				pc += 2
			case OpCode.output.rawValue:

				guard let value = getParameterValue(pc: pc, instruction: instruction, parameterNumber: 1, state: state) else {
					return nil
				}

				print(value)

				pc += 2
			default:
				print(":(")
				return nil // Halt and catch fire
		}
	}
	return 0
}

enum ParameterMode: Int {
	case position = 0
	case immediate = 1
}

func getParameterValue(_ pc: Int, _ state: [Int], _ instruction: Int, parameterNumber: Int) -> Int? {
	if pc + parameterNumber >= state.count {
		return nil
	}

	let operandAddressOrValue = state[pc + parameterNumber]

	guard let mode = getParameterMode(instruction: instruction, parameterNumber: parameterNumber) else {
		return nil
	}

	// Bounds checks
	if mode == ParameterMode.position && operandAddressOrValue >= state.count {
		return nil
	}

	return mode == ParameterMode.position
					? state[operandAddressOrValue]
					: operandAddressOrValue
}

func getParameterMode(instruction: Int, parameterNumber: Int) -> ParameterMode? {
	// Example instruction:
	// ABCDE
	//  1002
	// DE - two-digit opcode,      02 == opcode 2
	//  C - mode of 1st parameter,  0 == position mode
	//  B - mode of 2nd parameter,  1 == immediate mode
	//  A - mode of 3rd parameter,  0 == position mode, omitted due to being a leading zero

	// For the 1st param (C):
	// ABCDE / 100 = ABC
	// ABC % 10 = C

	// For the 2nd param (B):
	// ABCDE / 1000 = AB
	// AB % 10 = B

	// -> We need to divide by 10^(1 + param-position) and do % 10 on the result

	let p = pow(base: 10, exponent: 1 + parameterNumber)
	let mode = (instruction / p) % 10

	switch mode {
		case ParameterMode.position.rawValue:
			return ParameterMode.position
		case ParameterMode.immediate.rawValue:
			return ParameterMode.immediate
		default:
			return nil
	}
}

func main() {
	if let initialStateInput = readLine(strippingNewline: true) {
		let initialState = initialStateInput
				.split(separator: ",")
				.map { Int($0)! }

		var workingMemory = initialState
		let result = runProgram(state: &workingMemory)
		if result != 0 {
			print("The program crashed :(")
		}
	}
}

func pow(base: Int, exponent: Int) -> Int {
	var res = 1
	for _ in 0..<exponent {
		res *= base
	}
	return res
}

main()
