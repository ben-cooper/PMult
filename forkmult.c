#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

unsigned long long int best = 0;
unsigned int best_level = 0;

unsigned long long int multiply_digits(unsigned long long int n)
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

unsigned int multiplicative_persistence(unsigned long long int n)
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

void fork_workload(unsigned long long int min,
					unsigned long long int max,
					unsigned long int max_threads,
					unsigned long int current_thread)
{
	int fd[2];
	pid_t child;
	
	unsigned long long int local_best = 0;
	unsigned int local_best_level = 0;
	
	unsigned long long int local_min;
	unsigned long long int local_max;
	
	unsigned long long int i;
	unsigned int i_level;
	
	pipe(fd);
	
	if ((child = fork()) == -1)
	{
		perror("fork");
		exit(EXIT_FAILURE);
	}
	
	if (child != 0)
	{
		close(fd[1]);
		
		// parent keeps forking if necessary
		if (current_thread + 1 < max_threads)
		{
			fork_workload(min, max, max_threads, current_thread + 1);
		}
		
		// receives best values from child
		read(fd[0], &local_best, sizeof(local_best));
		read(fd[0], &local_best_level, sizeof(local_best_level));
		
		// updating best values
		if (local_best_level > best_level)
		{
			best = local_best;
			best_level = local_best_level;
		}
		
		close(fd[0]);

	} else {
		// calculating range
		local_min = min + ((max - min) / max_threads) * current_thread;
		local_max = ((current_thread +1) == max_threads) ? max:
				min + ((max - min) / max_threads) * (current_thread + 1);
		
		// child finds best value in range
		for (i = local_min; i <= local_max; i++)
		{
			i_level = multiplicative_persistence(i);
			
			// if number found with more levels or number hasn't been set
			if ((i_level > local_best_level) || (local_best < local_min)) {
				local_best = i;
				local_best_level = i_level;
			}
		}
		
		printf("Min: %llu Max: %llu PID: %d\n", local_min, local_max, getpid());
		
		// sending best value back to parent
		write(fd[1], &local_best, sizeof(local_best));
		write(fd[1], &local_best_level, sizeof(local_best_level));
		
		close(fd[1]);
		exit(EXIT_SUCCESS);
	}
}


int main(int argc, char **argv)
{
	unsigned long long int max;
	unsigned long long int min;
	unsigned long long int current;
	unsigned long int max_threads;

	// checking arguments
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
		// parsing arguments
		min = strtoull(argv[1], NULL, 10);
		max = strtoull(argv[2], NULL, 10);
		max_threads = strtoul(argv[3], NULL, 10);
		
		fork_workload(min, max, max_threads, 0);
		
		printf("\nBest number: %llu\nLevels: %u\n", best, best_level);
		
	}
	
	return EXIT_SUCCESS;
}
