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

void parse_string(const string &str, vector<int> &cont, char delim = ',')
{
	stringstream ss(str);
	string token;
	while (getline(ss, token, delim))
	{
		int int_token = stoi(token);
		cont.push_back(int_token);
	}
}

optional<ParameterMode> get_parameter_mode(const int instruction, const int parameter_number)
{
	// See solution of day 5 for explanation.

	int p = pow(10, 1 + parameter_number);
	int mode = (instruction / p) % 10;

	switch (mode)
	{
	case 0:
		return ParameterMode::POSITION;
	case 1:
		return ParameterMode::IMMEDIATE;
	default:
		return {};
	}
}

optional<int> get_parameter_value(const size_t pc, const vector<int> &state, const int instruction, const int parameter_number)
{
	if ((pc + parameter_number) >= state.size())
		return {};

	int operand_address_or_value = state[pc + parameter_number];

	auto mode = get_parameter_mode(instruction, parameter_number);
	if (!mode.has_value())
		return {};

	switch (*mode)
	{
	case ParameterMode::POSITION:

		return 0 <= operand_address_or_value && (uint)operand_address_or_value < state.size()
				   ? state[operand_address_or_value]
				   : (optional<int>)nullopt;

	case ParameterMode::IMMEDIATE:
		return operand_address_or_value;
	default:
		return {};
	}
}

optional<size_t> get_target_address(const size_t pc, const vector<int> &state, const int parameter_number)
{
	if ((pc + parameter_number) >= state.size())
		return {};

	auto target_address = state[pc + parameter_number];

	// Bounds checks, target_address won't be writable otherwise
	return 0 <= target_address && (size_t)target_address < state.size()
			   ? (size_t)target_address
			   : (optional<size_t>){};
}

optional<int> run_program(vector<int> &state)
{
	optional<int> last_output;

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
			if (!operand1.has_value())
				return {};

			auto operand2 = get_parameter_value(pc, state, instruction, 2);
			if (!operand2.has_value())
				return {};

			auto target_address = get_target_address(pc, state, 3);
			if (!target_address.has_value())
				return {};

			state[*target_address] = *operand1 + *operand2;

			pc += 4; // <instruction>, <op1>, <op2>, <target-spec>
			continue;
		}
		case OpCode::MULTIPLY:
		{
			auto operand1 = get_parameter_value(pc, state, instruction, 1);
			if (!operand1.has_value())
				return {};

			auto operand2 = get_parameter_value(pc, state, instruction, 2);
			if (!operand2.has_value())
				return {};

			auto target_address = get_target_address(pc, state, 3);
			if (!target_address.has_value())
				return {};

			state[*target_address] = *operand1 * *operand2;

			pc += 4; // <instruction>, <op1>, <op2>, <target-spec>
			continue;
		}
		case OpCode::INPUT:
		{
			cout << "Please enter something:" << endl;

			int value;
			cin >> value;

			auto target_address = get_target_address(pc, state, 1);
			if (!target_address.has_value())
				return {};

			state[*target_address] = value;

			pc += 2; // <instruction>, <target-spec>
			continue;
		}
		case OpCode::OUTPUT:
		{
			auto value = get_parameter_value(pc, state, instruction, 1);
			if (!value.has_value())
				return {};

			last_output = *value;

			cout << "Output: " << *value << endl;

			pc += 2; // <instruction>, <target-spec>
			continue;
		}
		case OpCode::JUMP_IF_TRUE:
		{
			auto predicate = get_parameter_value(pc, state, instruction, 1);
			if (!predicate.has_value())
				return {};

			if (*predicate != 0)
			{
				auto jump_target = get_parameter_value(pc, state, instruction, 2);
				if (!jump_target.has_value())
					return {};

				pc = *jump_target;
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
			if (!predicate.has_value())
				return {};

			if (*predicate == 0)
			{
				auto jump_target = get_parameter_value(pc, state, instruction, 2);
				if (!jump_target.has_value())
					return {};

				pc = *jump_target;
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
			if (!left.has_value())
				return {};

			auto right = get_parameter_value(pc, state, instruction, 2);
			if (!right.has_value())
				return {};

			auto target_address = get_target_address(pc, state, 3);
			if (!target_address.has_value())
				return {};

			state[*target_address] = *left < *right ? 1 : 0;

			pc += 4; // <instruction>, <p1>, <p2>, <target>
			continue;
		}
		case OpCode::EQUALS:
		{
			auto left = get_parameter_value(pc, state, instruction, 1);
			if (!left.has_value())
				return {};

			auto right = get_parameter_value(pc, state, instruction, 2);
			if (!right.has_value())
				return {};

			auto target_address = get_target_address(pc, state, 3);
			if (!target_address.has_value())
				return {};

			state[*target_address] = *left == *right ? 1 : 0;

			pc += 4; // <instruction>, <p1>, <p2>, <target>
			continue;
		}
		default:
			cout << (int)op_code << " :(" << endl;
			return {}; // Halt and catch fire
		}
	}

	return last_output;
}

int main()
{

	string initial_sate_input;
	getline(cin, initial_sate_input);

	vector<int> initial_state;
	parse_string(initial_sate_input, initial_state);

	auto working_memory = initial_state; // copy vector (as in the swift solution)
	auto result = run_program(working_memory);

	if (result.has_value())
		cout << "Result: " << *result << endl;
	else
		cout << "Program did not return anything." << endl;

	return 0;
}
