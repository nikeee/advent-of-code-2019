#!/usr/bin/env nodejs

// Run with:
// ./index.js

const { createInterface } = require("readline");

const rl = createInterface({
	input: process.stdin,
	output: process.stdout,
	terminal: false,
});

let sum = 0;
rl.on("close", () => {
	console.log(sum);
});

rl.on("line", line => {
	const mass = parseInt(line);
	let requiredFuel = computeFuel(mass);
	do {
		sum += requiredFuel;
		requiredFuel = computeFuel(requiredFuel);
	} while(requiredFuel > 0);
});


function computeFuel(mass) {
	return ((mass / 3) | 0) - 2;
}
