Solution to Concurrency 3: Problem 1 and Problem 2
CS 444 Fall Term 2017
Group 11: Brian Wiltse and Joshua Lioy
NOTE: Citations can be found in the source code for both programs.


To build our solutions, please run 'make' in the directory in which the tar.bz2 file was unpacked.

To test problem 1 (shared resources):

  1. Run the command 'concurrency3_1 <numProcesses> <seed>', where <seed> is an integer for the pseudo-random number generator,
     and numProcesses is the number of processes sharing the resource. 6 is a good number for readable output.
  2. Note the behavior:
      a. Print statements occur when threads become active, become inactive, and are waiting to use the resource.
      b. Three threads will use the resource immediately (for period of 2-6 seconds) and the others will wait.
      c. Once all three become inactive, 3 more threads will become active.
  3. The program will run indefinitely. Terminate the program with Ctrl-c.

To test problem 2 (search insert delete):

  1. Run the command 'concurrency3_2 <numSearchers> <numInserters> <numDeleters> <seed>', where seed is an integer
     for the pseudorandom number generator.
     NOTE: 3 searchers, 4 inserters, and 2 deleters tends to give good output.

  2. Note the behavior. 
      a. Print statements occur when any of the thread types perform their action.
          i.   Inserters will print what data they insert into the list.
          ii.  Searchers will print what they are looking for, if they found it, and, where they found it if they did.
          iii. Deleters will print what position in the list they are deleting from, and then print the list.
      b. Only 1 inserter can insert at a time, but searchers can search while an inserter is inserting. 
         Only one deleter can delete at a time. Searchers and Inserters cannot be active on the list during deletion.
      c. The output will provide evidence that our solution works. For example, when we were implementing inserters at first,
         we forgot to make them mutually exclusive. We found this out by noting that the list output did not contain all
         the values that were inserted. You will find that the list is now consistent when running the program.
      d. Note also that we implemented a known solution to the problem. Please see the citation in our source code. 

  3. The program will run indefinitely. Terminate the program with Ctrl-c.
