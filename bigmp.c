#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <gmp.h>

// arrays to be populated by each thread
mpz_t *best_numbers;
unsigned int *best_levels;

// mutex for printing thread info
pthread_mutex_t print_mutex = PTHREAD_MUTEX_INITIALIZER;

// argument structure for explore_range function
typedef struct {
	mpz_t min;
	mpz_t max;
	unsigned int thread;
} explore_range_args;

explore_range_args *init_explore_range_args(char *min,
											char *max,
											unsigned int thread,
											unsigned int num_threads)
{
	// returns pointer to explore_range_args struct
	explore_range_args *args = malloc(sizeof(explore_range_args));

	// temporary variables for calculations
	mpz_t temp1;
	mpz_t temp2;
	mpz_t temp3;

	mpz_init(temp1);
	mpz_init(temp2);
	mpz_init(temp3);
	
	// dividing the range into equal parts for each thread
	mpz_init(args->min);
	mpz_init(args->max);

	mpz_set_str(args->min, min, 10);
	mpz_set_str(args->max, max, 10);

	// calculating min
	mpz_set_ui(temp2, num_threads);
	mpz_sub(temp1, args->max, args->min);
	mpz_fdiv_q(temp1, temp1, temp2);
	mpz_mul_ui(temp1, temp1, thread);
	mpz_add(temp1, temp1, args->min);

	mpz_set(temp3, args->min);
	mpz_set(args->min, temp1);

	// calculating max if not max thread
	if ((thread+1) != num_threads)
	{
		mpz_sub(temp1, args->max, temp3);
		mpz_fdiv_q(temp1, temp1, temp2);
		mpz_mul_ui(temp1, temp1, thread + 1);
		mpz_add(temp1, temp1, temp3);
		mpz_set(args->max, temp1);
	}

	args->thread = thread;
	
	// freeing temporary values
	mpz_clear(temp1);
	mpz_clear(temp2);
	mpz_clear(temp3);

	return args;
}

void multiply_digits(mpz_t input, mpz_t result)
{
	/*
	 * return the product of all digits in n into result
	 */

	mpz_t n;
	mpz_t r;
	
	mpz_init(n);
	mpz_init(r);
	
	mpz_set(n, input);
	mpz_set_ui(result, 1);

	while (mpz_cmp_ui(n, 0) > 0) {
		mpz_fdiv_r_ui(r, n, 10);
		mpz_mul(result, result, r);
		mpz_fdiv_q_ui(n, n, 10);
	}

	mpz_clear(n);
	mpz_clear(r);
}

unsigned int multiplicative_persistence(mpz_t input)
{
	/*
	 * return the number of rounds n can pass 
	 */

	unsigned int levels = 0;
	mpz_t n;

	mpz_init(n);
	mpz_set(n, input);

	while (mpz_cmp_ui(n, 9) > 0) {
		levels++;
		multiply_digits(n, n);
	}

	mpz_clear(n);

	return levels;
}

void *explore_range(void *thread_args)
{
	// runs in pthread and populates best_numbers and best_levels arrays
	mpz_t current;
	unsigned int current_levels;

	// casting arguments
	explore_range_args *args = (explore_range_args *) thread_args;

	mpz_init(current);
	mpz_init(best_numbers[args->thread]);
	mpz_set(current, args->min);

	// printing args
	pthread_mutex_lock(&print_mutex);
	printf("Min: ");
	mpz_out_str(stdout, 10, args->min);
	printf(" Max: ");
	mpz_out_str(stdout, 10, args->max);
	printf(" Thread: %u\n", args->thread);
	pthread_mutex_unlock(&print_mutex);

	// finding number with highest multiplicative persistence within given range
	while (mpz_cmp(current, args->max) <= 0)
	{
		current_levels = multiplicative_persistence(current);

		// if number found with more levels or number hasn't been set
		if ((current_levels > best_levels[args->thread]) || (mpz_cmp(best_numbers[args->thread], args->min) < 0)) {
			mpz_set(best_numbers[args->thread], current);
			best_levels[args->thread] = current_levels;
		}

		mpz_add_ui(current, current, 1);
	}	

	// freeing
	mpz_clear(current);
	mpz_clear(args->max);
	mpz_clear(args->min);
	free(args);

	return NULL;
}

int main(int argc, char **argv)
{
	mpz_t current;
	unsigned int num_threads;
	unsigned int i;

	explore_range_args *args;
	pthread_t *threads;
	int ret_val;

	mpz_t best;
	unsigned int best_level = 0;

	// checking args
	if ((argc != 2) && (argc != 4)) {
		fprintf(stderr, "usage: %s min max threads OR %s number\n",
			argv[0], argv[0]);

		return EXIT_FAILURE;
	}

	if (argc == 2) {
		mpz_init(current);
		mpz_set_str(current, argv[1], 10);

		printf("Number: ");
		mpz_out_str(stdout, 10, current);
		printf("\nLevels: %u\n", multiplicative_persistence(current));
		
		mpz_clear(current);

		return EXIT_SUCCESS;
	}

	if (argc == 4) {
		mpz_init(best);
		mpz_set_str(best, argv[1], 10);
		
		// parsing arguments
		num_threads = strtoul(argv[3], NULL, 10);

		// initializing arrays for threads
		best_numbers = calloc(num_threads, sizeof(mpz_t));
		best_levels = calloc(num_threads, sizeof(unsigned int));
		threads = malloc(num_threads * sizeof(pthread_t));

		// creating threads
		for (i = 0; i < num_threads; i++) {
			args = init_explore_range_args(argv[1], argv[2], i, num_threads);
			if ((ret_val =
			     pthread_create(threads + i, NULL, explore_range,
					    args))) {
				fprintf(stderr, "Thread creation failed!\n");
				exit(EXIT_FAILURE);
			}
		}

		// finding best result
		for (i = 0; i < num_threads; i++) {
			pthread_join(threads[i], NULL);
			printf("Thread %u complete!\n", i);

			if (best_levels[i] > best_level)
			{
				mpz_set(best, best_numbers[i]);
				best_level = best_levels[i];
			}

			mpz_clear(best_numbers[i]);
		}

		// printing results
		printf("\n\nBest Number: ");
		mpz_out_str(stdout, 10, best);
		printf("\nLevels: %u\n", best_level);

		// freeing memory
		mpz_clear(best);
		free(best_numbers);
		free(best_levels);
		free(threads);
	}

	return EXIT_SUCCESS;
}
