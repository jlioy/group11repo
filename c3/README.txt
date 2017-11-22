Solution to Concurrency 3: Problem 1 and Problem 2
CS 444 Fall Term 2017
Group 11: Brian Wiltse and Joshua Lioy




To build our solutions, please run 'make' in the directory in which the tar.bz2 file was unpacked.

To test problem1:

1. Run the command 'concurrency3_1 <seed>', where <seed> is an integer for the pseudo-random number generator.
2. Note the behavior:
    a. Print statements occur when threads become active, become inactive, and are waiting to use the resource.
    b. Three threads will become active immediately (for period of 2-6 seconds) and two threads will wait.
    c. Once all three become inactive, 3 more threads will become active.
3. The program will run indefinitely; Terminate the program with Ctrl-c.

