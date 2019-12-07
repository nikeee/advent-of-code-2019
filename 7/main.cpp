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
#include <set>
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

optional<int> run_program(vector<int> &state, vector<int> supplied_input = {})
{
	optional<int> last_output;

	size_t supplied_input_index = 0;

	size_t pc = 0;
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
			int value;

			if (supplied_input.size() > 0 && supplied_input_index < supplied_input.size())
			{
				// cout << "Using program-supplied input value: " << supplied_input[supplied_input_index] << endl;
				value = supplied_input[supplied_input_index];
				++supplied_input_index;
			}
			else
			{
				cout << "Please enter something:" << endl;
				cin >> value;
			}

			auto target_address = get_target_address(pc, state, 1);

			state[target_address] = value;

			pc += 2; // <instruction>, <target-spec>
			continue;
		}
		case OpCode::OUTPUT:
		{
			auto value = get_parameter_value(pc, state, instruction, 1);
			last_output = value;

			// cout << "Output: " << *value << endl;

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
	vector<int> initial_state = get_input_program();

	vector<int> working_memory;
	int max_output = -1;

	vector<int> phase_settings = {0, 1, 2, 3, 4};
	sort(phase_settings.begin(), phase_settings.end());

	do
	{
		working_memory = initial_state;
		auto result_amp_a = run_program(working_memory, {phase_settings[0], 0});
		if (!result_amp_a.has_value())
			continue;

		working_memory = initial_state;
		auto result_amp_b = run_program(working_memory, {phase_settings[1], *result_amp_a});
		if (!result_amp_b.has_value())
			continue;

		working_memory = initial_state;
		auto result_amp_c = run_program(working_memory, {phase_settings[2], *result_amp_b});
		if (!result_amp_c.has_value())
			continue;

		working_memory = initial_state;
		auto result_amp_d = run_program(working_memory, {phase_settings[3], *result_amp_c});
		if (!result_amp_d.has_value())
			continue;

		working_memory = initial_state;
		auto result_amp_e = run_program(working_memory, {phase_settings[4], *result_amp_d});
		if (!result_amp_e.has_value())
			continue;

		cout << phase_settings[0] << " " << phase_settings[1] << " " << phase_settings[2] << " " << phase_settings[3] << " " << phase_settings[4] << " -> " << *result_amp_e << endl;

		if (*result_amp_e > max_output)
			max_output = *result_amp_e;

	} while (next_permutation(phase_settings.begin(), phase_settings.end()));

	cout << "max_output: " << max_output << endl;
	return 0;
}
