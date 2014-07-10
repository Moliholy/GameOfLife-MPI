AUTHOR: José Molina Colmenero
COURSE: Parallel Programming for Multicore-based Systems
DATE: 02/18/2014

ASSIGNMENT 3


To compile the program in Linux please go into the folder Debug, and use the following programs:

$ make clean
$ make

In order to do that I have used mpic++ compiler, basically because the code is written in C++.


After this step executable file should have been generated. If not, check the includes in both files /src/main.cpp and /src/GameOfLife.h as well a the compiler you are using.

To execute the program simply do it this way:

$ mpirun -np <number of processes> ./parallel_ass3 <rows> <columns> <aliveProbability> <numTimeSteps>


For example, for the execution
$ mpirun -np 8 ./parallel_ass3 400 500 0.85 21


I get the output:

Process     1: 21448   734   651   715   676   622   646   630   608   620   616 
Process     5: 21530   729   676   718   684   645   673   649   633   638   629 
Process     3: 21434   723   648   712   669   636   664   636   631   649   618 
Process     7: 21412   342   349   377   336   319   364   315   321   336   316 
Process     4: 21422   719   663   704   677   626   645   632   610   617   613 
Process     6: 21413   726   637   714   670   623   651   655   596   639   620 
Process     2: 21463   717   655   725   685   634   667   646   619   642   626 
Process     0: 21448   442   356   356   346   296   309   305   293   299   300 
TOTAL:         171570  5132  4635  5021  4743  4401  4619  4468  4311  4440  4338 

Process     3:   618   616   626   612   608   616   604   604   602   600   596 
Process     5:   629   621   625   617   615   611   613   611   613   611   613 
Process     7:   316   301   328   297   303   300   298   283   292   277   287 
Process     1:   616   601   606   602   597   599   601   596   598   596   598 
Process     2:   626   606   625   616   603   612   608   600   602   599   598 
Process     6:   620   602   609   621   582   611   597   595   588   602   582 
Process     0:   300   292   294   292   293   292   293   292   293   292   293 
Process     4:   613   602   606   606   602   602   602   602   602   602   602 
TOTAL:          4338  4241  4319  4263  4203  4243  4216  4183  4190  4179  4169 

Process     7:   287   276 
Process     3:   596   595 
Process     1:   598   596 
Process     5:   613   611 
Process     2:   598   598 
Process     0:   293   292 
Process     4:   602   602 
Process     6:   582   599 
TOTAL:          4169  4169
