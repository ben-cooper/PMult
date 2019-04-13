#!/usr/bin/python3

import sys
from multiprocessing import Pool


def multiply_digits(n):
    """
    (int) -> int

    Return the product of all digits in n
    """

    product = 1

    while n > 0:
        product *= n % 10
        n //= 10

    return product


def multiplicative_persistence(n):
    """
    (int) -> int

    Return the multiplicative peristence of n.
    """

    levels = 0

    while n > 9:
        n = multiply_digits(n)
        levels += 1

    return levels


def best_multiplicative_persistence_in_range(r):
    """
    (int, int) -> (int, int)

    Return a 2-tuple containing the number with the best
    multiplicative persistence (within the range r) and
    the levels of multiplicative persistence it has, respectively.
    """

    best_number = -1
    best_levels = -1

    for i in r:
        i_persistence = multiplicative_persistence(i)

        if i_persistence > best_levels:
            # updating best number
            best_number = i
            best_levels = i_persistence

    return best_number, best_levels


def get_ranges(i, j, n):
    """
    (int, int, int) -> list of ranges

    Return a list of n equal ranges from i to j
    """

    return [range(i + k, j, n) for k in range(n)]


if __name__ == "__main__":
    if len(sys.argv) == 2:
        # checking a single number
        number = int(sys.argv[1])
        level = multiplicative_persistence(number)

        print("Number: {0}\nLevels: {1}".format(number, level))

    elif len(sys.argv) == 3:
        # checking a range of numbers
        minimum = int(sys.argv[1])
        maximum = int(sys.argv[2])
        search_range = range(minimum, maximum)

        result = best_multiplicative_persistence_in_range(search_range)

        print("Best Number: {0}\nLevels: {1}".format(result[0], result[1]))

    elif len(sys.argv) == 4:
        # check a range of numbers with multiple processes
        minimum = int(sys.argv[1])
        maximum = int(sys.argv[2])
        threads = int(sys.argv[3])

        ranges = get_ranges(minimum, maximum, threads)

        # creating child processes
        p = Pool(threads)
        result = p.map(best_multiplicative_persistence_in_range, ranges)

        # finding the highest number of levels found
        best_levels = max(result, key=lambda x: x[1])[1]

        # finding the smallest number with the highest number of levels
        best_numbers = [e[0] for e in result if e[1] == best_levels]
        best_number = min(best_numbers)

        print("Best Number: {0}\nLevels: {1}".format(best_number, best_levels))

    else:
        # invalid input
        print(("usages:\n\t" +
               "{0} number\n\t" +
               "{0} min max\n\t" +
               "{0} min max threads").format(sys.argv[0]))
