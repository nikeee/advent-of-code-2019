// Compile:
// g++ -std=c++17 -O3 -Wall -Wextra -o 7.out main.cpp
// Run:
// ./7.out < input.txt

#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <vector>
#include <queue>
#include <variant>
#include <optional>
#include <cmath>

using namespace std;

enum class OpCode
{
	NOP = 0,
	ADD = 1,
	MULTIPLY = 2,
	INPUT = 3,
	OUTPUT = 4,
	JUMP_IF_TRUE = 5,
	JUMP_IF_FALSE = 6,
	LESS_THAN = 7,
	EQUALS = 8,
	HALT = 99,
};

enum class ParameterMode
{
	POSITION = 0,
	IMMEDIATE = 1,
};

struct InvalidMemoryReference : public exception
{
	const char *what() const throw()
	{
		return "Invalid reference to address in memory.";
	}
};
struct InvalidInstruction : public exception
{
	const char *what() const throw()
	{
		return "Reached invalid instruction.";
	}
};

vector<int> get_input_program(char delimiter = ',')
{
	vector<int> res;
	string token;
	while (getline(cin, token, delimiter))
		res.push_back(stoi(token));
	return res;
}

ParameterMode get_parameter_mode(const int instruction, const int parameter_number)
{
	// See solution of day 5 for explanation.

	int p = pow(10, 1 + parameter_number);
	int mode = (instruction / p) % 10;

	return (int)ParameterMode::POSITION <= mode && mode <= (int)ParameterMode::IMMEDIATE
			   ? (ParameterMode)mode
			   : throw InvalidInstruction();
}

int get_parameter_value(const size_t pc, const vector<int> &state, const int instruction, const int parameter_number)
{
	if ((pc + parameter_number) >= state.size())
		throw InvalidMemoryReference();

	int operand_address_or_value = state[pc + parameter_number];

	auto mode = get_parameter_mode(instruction, parameter_number);

	switch (mode)
	{
	case ParameterMode::POSITION:
		return 0 <= operand_address_or_value && (uint)operand_address_or_value < state.size()
				   ? state[operand_address_or_value]
				   : throw InvalidMemoryReference();
	case ParameterMode::IMMEDIATE:
		return operand_address_or_value;
	default:
		throw InvalidInstruction();
	}
}

size_t get_target_address(const size_t pc, const vector<int> &state, const int parameter_number)
{
	if ((pc + parameter_number) >= state.size())
		throw InvalidMemoryReference();

	auto target_address = state[pc + parameter_number];

	// Bounds checks, target_address won't be writable otherwise
	return 0 <= target_address && (size_t)target_address < state.size()
			   ? (size_t)target_address
			   : throw InvalidMemoryReference();
}

struct ExecutionState
{
	size_t pc;
};

variant<optional<int>, ExecutionState> run_program(vector<int> &state, size_t &pc, queue<int> &input_queue, queue<int> &output_queue)
{
	optional<int> last_output;

	while (
		pc < state.size() && state[pc] != (int)OpCode::HALT)
	{
		int instruction = state[pc];
		OpCode op_code = (OpCode)(instruction % 100);

		// cout << pc << ": " << instruction << " (" << (int)op_code << ")" << endl;

		switch (op_code)
		{
		case OpCode::NOP:
		{
			pc += 1; // <instruction>
			continue;
		}
		case OpCode::ADD:
		{
			auto operand1 = get_parameter_value(pc, state, instruction, 1);
			auto operand2 = get_parameter_value(pc, state, instruction, 2);
			auto target_address = get_target_address(pc, state, 3);

			state[target_address] = operand1 + operand2;

			pc += 4; // <instruction>, <op1>, <op2>, <target-spec>
			continue;
		}
		case OpCode::MULTIPLY:
		{
			auto operand1 = get_parameter_value(pc, state, instruction, 1);
			auto operand2 = get_parameter_value(pc, state, instruction, 2);
			auto target_address = get_target_address(pc, state, 3);

			state[target_address] = operand1 * operand2;

			pc += 4; // <instruction>, <op1>, <op2>, <target-spec>
			continue;
		}
		case OpCode::INPUT:
		{
			if (input_queue.size() == 0)
			{
				// We have no data provided by another amplifier. We exit here and continue later.
				return ExecutionState{pc};
			}

			int value = input_queue.front();
			input_queue.pop();

			auto target_address = get_target_address(pc, state, 1);

			state[target_address] = value;

			pc += 2; // <instruction>, <target-spec>
			continue;
		}
		case OpCode::OUTPUT:
		{
			auto value = get_parameter_value(pc, state, instruction, 1);
			last_output = value;

			output_queue.push(value);

			pc += 2; // <instruction>, <target-spec>
			continue;
		}
		case OpCode::JUMP_IF_TRUE:
		{
			auto predicate = get_parameter_value(pc, state, instruction, 1);

			if (predicate != 0)
			{
				auto jump_target = get_parameter_value(pc, state, instruction, 2);
				pc = jump_target;
			}
			else
			{
				// "do nothing" means just step over the entire instruction + parameters
				pc += 3; // <instruction>, <predicate>, <jump-target>
			}
			continue;
		}
		case OpCode::JUMP_IF_FALSE:
		{
			auto predicate = get_parameter_value(pc, state, instruction, 1);

			if (predicate == 0)
			{
				auto jump_target = get_parameter_value(pc, state, instruction, 2);
				pc = jump_target;
			}
			else
			{
				// "do nothing" means just step over the entire instruction + parameters
				pc += 3; // <instruction>, <predicate>, <jump-target>
			}
			continue;
		}
		case OpCode::LESS_THAN:
		{
			auto left = get_parameter_value(pc, state, instruction, 1);
			auto right = get_parameter_value(pc, state, instruction, 2);
			auto target_address = get_target_address(pc, state, 3);

			state[target_address] = left < right ? 1 : 0;

			pc += 4; // <instruction>, <p1>, <p2>, <target>
			continue;
		}
		case OpCode::EQUALS:
		{
			auto left = get_parameter_value(pc, state, instruction, 1);
			auto right = get_parameter_value(pc, state, instruction, 2);
			auto target_address = get_target_address(pc, state, 3);

			state[target_address] = left == right ? 1 : 0;

			pc += 4; // <instruction>, <p1>, <p2>, <target>
			continue;
		}
		default:
			// Halt and catch fire
			cout << "Unable to handle: " << (int)op_code << " :(" << endl;
			throw InvalidInstruction();
		}
	}

	return last_output;
}

int main()
{
	const vector<int> initial_state = get_input_program();

	vector<int> working_memory;
	int max_output = -1;

	vector<int> phase_settings = {5, 6, 7, 8, 9};
	sort(phase_settings.begin(), phase_settings.end());

	do
	{
		vector<vector<int>> amplifier_states = {initial_state, initial_state, initial_state, initial_state, initial_state};
		vector<queue<int>> outputs = {
			queue<int>({phase_settings[1]}),
			queue<int>({phase_settings[2]}),
			queue<int>({phase_settings[3]}),
			queue<int>({phase_settings[4]}),
			queue<int>({phase_settings[0], 0}),
		};
		vector<size_t> pcs = {0, 0, 0, 0, 0};

		optional<int> halting_value;
		do
		{
			variant<optional<int>, ExecutionState> res;
			res = run_program(amplifier_states[0], pcs[0], outputs[4], outputs[0]);
			res = run_program(amplifier_states[1], pcs[1], outputs[0], outputs[1]);
			res = run_program(amplifier_states[2], pcs[2], outputs[1], outputs[2]);
			res = run_program(amplifier_states[3], pcs[3], outputs[2], outputs[3]);
			res = run_program(amplifier_states[4], pcs[4], outputs[3], outputs[4]);

			if (holds_alternative<optional<int>>(res))
			{
				halting_value = get<optional<int>>(res);

				if (!halting_value.has_value())
					continue;

				if (*halting_value > max_output)
					max_output = *halting_value;
			}

		} while (!halting_value.has_value());
	} while (next_permutation(phase_settings.begin(), phase_settings.end()));

	cout << "max_output: " << max_output << endl;
	return 0;
}
