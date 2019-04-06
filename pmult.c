#include <stdio.h>
#include <stdlib.h>

unsigned long long multiply_digits(unsigned long long n)
{
	/*
	 * return the product of all digits in n
	 */
	 
	 unsigned long long product = 1;
	 
	 while (n > 0)
	 {
		 product *= (n % 10);
		 n  /= 10;
	 }
	 
	 return product;
 }

unsigned int multiplicative_persistence(unsigned long long n)
{
	/*
	 * return the number of rounds n can pass 
	 */

	unsigned int levels = 0;

	while (n > 9) {
		levels++;
		n = multiply_digits(n);
	}

	return levels;
}

int main(int argc, char **argv)
{
	/*
	 * best is the current best number for persistent multiplication
	 * best_levels is how many rounds the best number passed current is
	 * the current number being tested current_levels is the number of
	 * rounds the current number passes max is the maximum number to test
	 * min is the minimum number to test 
	 */

	unsigned long long int best = 0;
	unsigned int best_levels = 0;
	unsigned long long int current;
	unsigned int current_levels;
	unsigned long long int max;
	unsigned long long int min;

	/*
	 * checking args 
	 */
	if ((argc < 2) || (argc > 3)) {
		fprintf(stderr, "usage: %s min max OR %s number\n", argv[0],
			argv[0]);
		
		return EXIT_FAILURE;
	}

	if (argc == 2) {
		current = strtoull(argv[1], NULL, 10);
		printf("Number: %llu\nLevels: %u\n", current,
		       multiplicative_persistence(current));
		
		return EXIT_SUCCESS;
	}

	if (argc == 3) {
		min = strtoull(argv[1], NULL, 10);
		max = strtoull(argv[2], NULL, 10);

		/*
		 * checking numbers from min to max 
		 */
		for (current = min; current <= max; current++) {
			current_levels = multiplicative_persistence(current);

			if (current_levels > best_levels) {
				best = current;
				best_levels = current_levels;
			}
		}
	}

	printf("Best Number: %llu\nLevels: %u\n", best, best_levels);

	return EXIT_SUCCESS;
}
