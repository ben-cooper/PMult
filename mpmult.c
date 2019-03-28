#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

/* arrays to be populated by each thread */
unsigned long long *best_numbers;
unsigned int *best_levels;

/* argument structure for explore_range function */
typedef struct {
	unsigned long long min;
	unsigned long long max;
	unsigned int thread;
} explore_range_args;

explore_range_args *init_explore_range_args(unsigned long long int min,
					    unsigned long long int max,
					    unsigned long int thread)
{
	/* returns pointer to explore_range_args struct */
	explore_range_args *ret_val = malloc(sizeof(explore_range_args));

	ret_val->min = min;
	ret_val->max = max;
	ret_val->thread = thread;

	return ret_val;
}

unsigned long long multiply_digits(unsigned long long n)
{
	/*
	 * return the product of all digits in n
	 */

	unsigned long long product = 1;

	while (n > 0) {
		product *= (n % 10);
		n /= 10;
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

void *explore_range(void *thread_args)
{
	/* runs in pthread and populates best_numbers and best_levels arrays */
	unsigned long long int current;
	unsigned int current_levels;

	/* casting arguments */
	explore_range_args *args = (explore_range_args *) thread_args;

	printf("Min: %llu, Max: %llu, Thread: %u\n", args->min, args->max,
	       args->thread);

	/* finding number with highest multiplicative persistence within given range */
	for (current = args->min; current < args->max; current++) {
		current_levels = multiplicative_persistence(current);

		if (current_levels > best_levels[args->thread]) {
			best_numbers[args->thread] = current;
			best_levels[args->thread] = current_levels;
		}
	}

	free(args);

	return NULL;
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

	unsigned long long int max;
	unsigned long long int min;
	unsigned long long int current;
	unsigned long int num_threads;
	unsigned long int i;

	explore_range_args *args;
	pthread_t *threads;
	int ret_val;

	unsigned long long int best = 0;
	unsigned int best_level = 0;

	/*
	 * checking args 
	 */
	if ((argc != 2) && (argc != 4)) {
		fprintf(stderr, "usage: %s min max threads OR %s number\n",
			argv[0], argv[0]);

		return EXIT_FAILURE;
	}

	if (argc == 2) {
		current = strtoull(argv[1], NULL, 10);
		printf("Number: %llu\nLevels: %u\n", current,
		       multiplicative_persistence(current));

		return EXIT_SUCCESS;
	}

	if (argc == 4) {
		/* parsing arguments */
		min = strtoull(argv[1], NULL, 10);
		max = strtoull(argv[2], NULL, 10);
		num_threads = strtoul(argv[3], NULL, 10);

		/* initializing arrays for threads */
		best_numbers =
		    calloc(num_threads, sizeof(unsigned long long int));
		best_levels = calloc(num_threads, sizeof(unsigned int));
		threads = malloc(num_threads * sizeof(pthread_t));

		/* creating threads */
		for (i = 0; i < num_threads; i++) {
			args =
			    init_explore_range_args(min +
						    (max / num_threads) * i,
						    (max / num_threads) * (i +
									   1),
						    i);
			if ((ret_val =
			     pthread_create(threads + i, NULL, explore_range,
					    args))) {
				fprintf(stderr, "Thread creation failed!\n");
				exit(EXIT_FAILURE);
			}
		}

		/* finding best result */
		for (i = 0; i < num_threads; i++) {
			pthread_join(threads[i], NULL);
			if (best_levels[i] > best_level) {
				best = best_numbers[i];
				best_level = best_levels[i];
			}
		}

	}

	/* freeing memory */
	free(best_numbers);
	free(best_levels);
	free(threads);

	printf("Best Number: %llu\nLevels: %u\n", best, best_level);

	return EXIT_SUCCESS;
}
