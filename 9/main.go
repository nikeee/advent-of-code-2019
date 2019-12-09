// Compile:
//     go build -o 9.out main.go
// Run:
//     ./9.out < input.txt
// Compiler version:
//     go version
//     go version go1.13.1 linux/amd64

package main

import (
	"fmt"
	"strings"
	"strconv"
	"errors"
	"math"
)

type OpCode int64
const (
	nop OpCode = 0
	add OpCode = 1
	multiply OpCode = 2
	input OpCode = 3
	output OpCode = 4
	jumpIfTrue OpCode = 5
	jumpIfFalse OpCode = 6
	lessThan OpCode = 7
	equals OpCode = 8
	halt OpCode = 99
);

func runProgram(state []int64) error {
	pc := 0

	for pc < len(state) && OpCode(state[pc]) != halt {
		instruction := state[pc]
		opCode := instruction % 100

		// fmt.Printf("%d: %d (%d)\n", pc, instruction, opCode)

		switch OpCode(opCode) {
		case nop:
			pc += 1 // <instruction>
		case add:
			operand1, err := getParameterValue(pc, state, instruction, 1)
			if err != nil {
				return err
			}
			operand2, err := getParameterValue(pc, state, instruction, 2)
			if err != nil {
				return err
			}
			targetAddress, err := getTargetAddress(pc, state, 3)
			if err != nil {
				return err
			}

			state[targetAddress] = operand1 + operand2

			pc += 4 // <instruction>, <op1>, <op2>, <target-spec>
		case multiply:
			operand1, err := getParameterValue(pc, state, instruction, 1)
			if err != nil {
				return err
			}
			operand2, err := getParameterValue(pc, state, instruction, 2)
			if err != nil {
				return err
			}
			targetAddress, err := getTargetAddress(pc, state, 3)
			if err != nil {
				return err
			}

			state[targetAddress] = operand1 * operand2

			pc += 4 // <instruction>, <op1>, <op2>, <target-spec>
		case input:
			fmt.Println("Please enter something:")

			var value int64
			_, err := fmt.Scan(&value)
			if err != nil {
				return err
			}

			targetAddress, err := getTargetAddress(pc, state, 1)
			if err != nil {
				return err
			}
			state[targetAddress] = value

			pc += 2 // <instruction>, <target-spec>
		case output:
			value, err := getParameterValue(pc, state, instruction, 1)
			if err != nil {
				return err
			}

			fmt.Println(value)

			pc += 2 // <instruction>, <source-spec>
		case jumpIfTrue:
			predicate, err := getParameterValue(pc, state, instruction, 1)
			if err != nil {
				return err
			}

			if predicate != 0 {
				jumpTarget, err := getParameterValue(pc, state, instruction, 2)
				if err != nil {
					return err
				}
				pc = int(jumpTarget)
			} else {
				// "do nothing" means just step over the entire instruction + parameters
				pc += 3 // <instruction>, <predicate>, <jump-target>
			}
		case jumpIfFalse:
			predicate, err := getParameterValue(pc, state, instruction, 1)
			if err != nil {
				return err
			}

			if predicate == 0 {
				jumpTarget, err := getParameterValue(pc, state, instruction, 2)
				if err != nil {
					return err
				}
				pc = int(jumpTarget)
			} else {
				// "do nothing" means just step over the entire instruction + parameters
				pc += 3 // <instruction>, <predicate>, <jump-target>
			}
		case lessThan:
			left, err := getParameterValue(pc, state, instruction, 1)
			if err != nil {
				return err
			}
			right, err := getParameterValue(pc, state, instruction, 2)
			if err != nil {
				return err
			}
			targetAddress, err := getTargetAddress(pc, state, 3)
			if err != nil {
				return err
			}

			if left < right {
				state[targetAddress] = 1
			} else {
				state[targetAddress] = 0
			}

			pc += 4 // <instruction>, <p1>, <p2>, <target>
		case equals:
			left, err := getParameterValue(pc, state, instruction, 1)
			if err != nil {
				return err
			}
			right, err := getParameterValue(pc, state, instruction, 2)
			if err != nil {
				return err
			}
			targetAddress, err := getTargetAddress(pc, state, 3)
			if err != nil {
				return err
			}

			if left == right {
				state[targetAddress] = 1
			} else {
				state[targetAddress] = 0
			}

			pc += 4 // <instruction>, <p1>, <p2>, <target>
		case halt:
			return nil
		default:
			fmt.Println(":(")
			return errors.New("Unable to process instruction") // Halt and catch fire
		}
	}
	return nil
}

func getTargetAddress(pc int, state []int64, parameterNumber int) (int64, error) {
	if (pc + parameterNumber) >= len(state) {
		return 0, errors.New("pc + parameterNumber out of bounds")
	}

	targetAddress := state[pc + parameterNumber]

	// Bounds checks, targetAddress won't be writable otherwise
	if 0 <= targetAddress || targetAddress < int64(len(state)) {
		return targetAddress, nil
	}
	return 0, errors.New("targetAddress out of bounds")
}

type ParameterMode int
const (
	position ParameterMode = 0
	immediate ParameterMode = 1
	// relative ParameterMode = 2
)

func getParameterValue(pc int, state []int64, instruction int64, parameterNumber int) (int64, error) {
	if (pc + parameterNumber) >= len(state) {
		return 0, errors.New("pc + parameterNumber out of bounds")
	}

	operandAddressOrValue := state[pc + parameterNumber]

	mode, err := getParameterMode(instruction, parameterNumber)
	if err != nil {
		return 0, err
	}

	// Bounds checks
	if mode == position && (operandAddressOrValue < 0 || operandAddressOrValue >= int64(len(state))) {
		return 0, errors.New("operandAddressOrValue out of bounds")
	}

	if mode == position {
		return state[operandAddressOrValue], nil
	}
	return operandAddressOrValue, nil
}

func getParameterMode(instruction int64, parameterNumber int) (ParameterMode, error) {
	// See solution of day 5 for explanation

	p := int64(math.Pow10(1 + parameterNumber))
	mode := (instruction / p) % 10

	switch mode {
		case 0:
			return position, nil
		case 1:
			return immediate, nil
		default:
			return position, errors.New("unsupported parameter mode")
	}
}

func main() {
	var input string
	fmt.Scanln(&input)
	initialState := parseMemory(input)
	runProgram(initialState)
}

func parseMemory(input string) []int64 {
	memoryString := strings.Split(input, ",");

	res := make([]int64, len(memoryString));
	for i := 0; i < len(memoryString); i++ {
		cell, _ := strconv.ParseInt(memoryString[i], 10, 64)
		res[i] = cell
	}

	return res
}
