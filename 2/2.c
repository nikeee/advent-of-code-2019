// Compile with:
// gcc -O3 -Wall -Wextra -o 2.out 2.c
// Run with:
// ./2.out

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

typedef uint32_t u32;

typedef struct
{
	bool has_value;
	u32 value;
} Optional;

const Optional no_result = {.has_value = false, .value = -1};
Optional ok(u32 result)
{
	return (Optional){.has_value = true, .value = result};
}

u32 initial_state[] = {1, 0, 0, 3, 1, 1, 2, 3, 1, 3, 4, 3, 1, 5, 0, 3, 2, 1, 10, 19, 1, 6, 19, 23, 1, 13, 23, 27, 1, 6, 27, 31, 1, 31, 10, 35, 1, 35, 6, 39, 1, 39, 13, 43, 2, 10, 43, 47, 1, 47, 6, 51, 2, 6, 51, 55, 1, 5, 55, 59, 2, 13, 59, 63, 2, 63, 9, 67, 1, 5, 67, 71, 2, 13, 71, 75, 1, 75, 5, 79, 1, 10, 79, 83, 2, 6, 83, 87, 2, 13, 87, 91, 1, 9, 91, 95, 1, 9, 95, 99, 2, 99, 9, 103, 1, 5, 103, 107, 2, 9, 107, 111, 1, 5, 111, 115, 1, 115, 2, 119, 1, 9, 119, 0, 99, 2, 0, 14, 0};

typedef enum
{
	OP_CODE_ADD = 1,
	OP_CODE_MULTIPLY = 2,
	OP_CODE_HALT = 99,
} OpCode;

Optional run_program(const size_t state_size, u32 state[state_size])
{
	for (
		size_t pc = 0;
		pc < state_size - 4 && state[pc] != OP_CODE_HALT;
		pc += 4
	) {
		const OpCode op_code = state[pc];
		switch (op_code)
		{
		case OP_CODE_ADD:
		{
			const u32 operand1Address = state[pc + 1];
			const u32 operand2Address = state[pc + 2];
			const u32 targetAddress = state[pc + 3];

			// Bounds checks
			if (operand1Address >= state_size || operand2Address >= state_size || targetAddress >= state_size)
				return no_result;

			state[targetAddress] = state[operand1Address] + state[operand2Address];
			break;
		}
		case OP_CODE_MULTIPLY:
		{
			const u32 operand1Address = state[pc + 1];
			const u32 operand2Address = state[pc + 2];
			const u32 targetAddress = state[pc + 3];

			// Bounds checks
			if (operand1Address >= state_size || operand2Address >= state_size || targetAddress >= state_size)
				return no_result;

			state[targetAddress] = state[operand1Address] * state[operand2Address];
			break;
		}
		default:
			return no_result; // Halt and catch fire
		}
	}
	return ok(state[0]);
}

int main()
{
	const u32 needle = 19690720;

	const size_t number_of_memory_cells = sizeof(initial_state) / sizeof(initial_state[0]);

	u32 *memory = calloc(sizeof(u32), number_of_memory_cells);

	for (u32 noun = 0; noun < 100; ++noun)
	{
		for (u32 verb = 0; verb < 100; ++verb)
		{
			memcpy(memory, &initial_state, sizeof(initial_state));

			memory[1] = noun;
			memory[2] = verb;

			Optional result = run_program(number_of_memory_cells, memory);

			if (!result.has_value)
			{
				printf("noun and verb resulted in invalid program state: %d, %d\n", noun, verb);
				continue;
			}

			if (result.value == needle)
			{
				u32 solution = 100 * noun + verb;
				printf("Found solution for 100 * noun + verb: %d\n", solution);
				free(memory);
				return EXIT_SUCCESS;
			}
		}
	}
	printf("Search space exhausted\n");
	free(memory);
	return EXIT_FAILURE;
}
