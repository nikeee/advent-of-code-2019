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
				let operand1AddressOrValue = state[pc + 1]
				guard let operand1Mode = getParameterMode(instruction: instruction, parameterNumber: 1) else {
					return nil
				}

				let operand2AddressOrValue = state[pc + 2]
				guard let operand2Mode = getParameterMode(instruction: instruction, parameterNumber: 2) else {
					return nil
				}

				let targetAddress = state[pc + 3] // Always an address

				// Bounds checks
				if (operand1Mode == ParameterMode.position && operand1AddressOrValue >= state.count)
					|| (operand2Mode == ParameterMode.position && operand2AddressOrValue >= state.count)
					|| (targetAddress >= state.count) {
					return nil
				}

				let operand1Value = operand1Mode == ParameterMode.position
														? state[operand1AddressOrValue]
														: operand1AddressOrValue

				let operand2Value = operand2Mode == ParameterMode.position
														? state[operand2AddressOrValue]
														: operand2AddressOrValue

				state[targetAddress] = operand1Value + operand2Value

				pc += 4
			case OpCode.multiply.rawValue:

				let operand1AddressOrValue = state[pc + 1]
				guard let operand1Mode = getParameterMode(instruction: instruction, parameterNumber: 1) else {
					return nil
				}

				let operand2AddressOrValue = state[pc + 2]
				guard let operand2Mode = getParameterMode(instruction: instruction, parameterNumber: 2) else {
					return nil
				}

				let targetAddress = state[pc + 3] // Always an address

				// Bounds checks
				if (operand1Mode == ParameterMode.position && operand1AddressOrValue >= state.count)
					|| (operand2Mode == ParameterMode.position && operand2AddressOrValue >= state.count)
					|| (targetAddress >= state.count) {
					return nil
				}

				let operand1Value = operand1Mode == ParameterMode.position
														? state[operand1AddressOrValue]
														: operand1AddressOrValue

				let operand2Value = operand2Mode == ParameterMode.position
														? state[operand2AddressOrValue]
														: operand2AddressOrValue

				state[targetAddress] = operand1Value * operand2Value

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

				let sourceAddressOrValue = state[pc + 1]
				guard let operandMode = getParameterMode(instruction: instruction, parameterNumber: 1) else {
					return nil
				}

				// Bounds checks
				if operandMode == ParameterMode.position && sourceAddressOrValue >= state.count {
					return nil
				}

				let value = operandMode == ParameterMode.position
												? state[sourceAddressOrValue]
												: sourceAddressOrValue

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
