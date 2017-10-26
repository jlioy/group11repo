Solution to Concurrency 2: The Dining Philosophers Problem
CS 444 Fall Term 2017
Group 11: Brian Wiltse and Joshua Lioy

We obtained this solution from The Little Book of Semaphores by Allen B. Downey Section 4.4.4 (Solution 2)
http://greenteapress.com/semaphores/LittleBookOfSemaphores.pdf
 

To test this program, please:

1. Run 'make' in the directory in which the tar.bz2 file was unpacked.
2. Run the command dining_phil <seed>, where seed is an integer for the pseudo-random number generator.
3. Note the behavior of each of the philosophers.
    a. All 5 philosophers will begin thinking.
    b. Output occurs when philosophers begin thinking, begin eating, and discontinue eating.
    c. When they begin and finish eating, they will each indicate the forks they are picking up or putting down.
4. You will not see deadlock or starvation occur; it can be verified that they will never occur
   by noting that we implemented a known solution that has been proven to avoid deadlock and starvation.
