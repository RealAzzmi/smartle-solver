# Smartle Solver

Cracks the word-based puzzle game https://smartle.net

# Usage & Further Ideas

1. Change `TAKEN_COMBINATION_SOLUTION` to 100000-1000000 for better solutions. Higher is better but more slow. In most cases, 1000000 gives a near-optimal solution in less than 5 minutes.
2. It could be easily modified to use multithreading; starting a thread for every node in the first word only speeds up proportionally to the number of threads up to a certain point (until thrashing occurs).
3. The greedy `calculate_swaps` approximates an NP-Hard problem (given two strings, find the minimal number of swaps needed to convert one string to the other string). It is still very naive and could be further improved for instance, by detecting a cycle of length 3 in which after a +1 matched-swap, another +2 matched-swap can be done.

# Pics

![Screenshot from 2024-06-11 01-32-27](https://github.com/RealAzzmi/smartle-solver/assets/15536717/89648952-8097-4d63-b54c-047f71d018d6)
![Screenshot from 2024-06-11 23-13-11](https://github.com/RealAzzmi/smartle-solver/assets/15536717/dc1298b9-8a20-4f55-8a27-5ac9cd2e0ef6)
![Screenshot from 2024-06-11 01-13-56](https://github.com/RealAzzmi/smartle-solver/assets/15536717/e0e47467-6e05-413e-aff3-dfffa4f13dc4)
