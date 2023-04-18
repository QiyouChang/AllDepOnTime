AllDepOnTime 

Proposed by George Chang and Lijie Yang. 
Section 0 URL:
https://qiyouchang.github.io/AllDepOnTime/

Section 1 Summary: 
we will implement Optimized time-based thread assignment on simulated human networks 

Section 2: Background:  
Man is by nature a social animal. Everyone may like or dislike another person and at the same time, experiences like or dislike from another person. Thus the relationship between humans can be simulated by a human network (an undirected weighted graph in CS terms).

Let’s define the following rules for the network: 
People are nodes and have weighted edges to another person showing how much they like/dislike the other person on a scale from -1 to 1.  
Every once in a while, everyone will evaluate how healthy his/her living environment is by asking people within 2 degrees of separation to report their average like/dislikes rate and calculate based on this.
Upon getting the result of calculation in the last step, everyone will delete the edge with x friends based on their individual evaluation of how healthy the society is and add y new friends randomly. If the person thinks the society is healthy, then the person will drop x of his pessimistic friends. If the person thinks the society is not healthy, then the person will drop x of his optimistic friends. (x, y are tunable parameters)
After a constant number of iterations, we will gather the evaluation data (to another person or to the society) in general.
 
Speaking in technical terms, the system looks like this: 
System is an undirected, weighted, randomly initialized graph where the values of edges are between -1 and 1. 
In each iteration of simulation, communication between connected nodes are needed in order to send the average preference list. Then each processor/node will calculate the overall average and report to the master which stores all the evaluations.
When the calculation is done, x number of edges of each node are dropped and y number of new edges for each node are created. X,y are tunable parameters. 
In the end all evaluations from each node are gathered to the master processor and the final result will be calculated. 

This system will benefit a lot from parallelism because each node can calculate its evaluation independently. This system requires at least 2*(number of edges) messages to be sent between nodes so it will involve really heavy computation if no parallelism is used. 

Section 3: Challenge 
The problem is challenging because the network changes drastically as we add and drop new edges from each node and take 2 degrees of separation (friend and friend of a friend) so the calculation time may increase or decrease dramatically in two adjacent iterations. Thus, it would require us to reschedule which people are assigned to each processor to balance the workload. We will need to make predictions on how much work each processor will take (how many friends of a friend is not known) and that will be difficult and distinct from previous work we have done in this course. We want to learn more about MPI, and implement more efficient processor task assignments based on accurate predictions on the time cost of each node. 

The workflow will look something like this: 
The initial network is loaded to the master processor.
Nodes are assigned to processors based on our prediction algorithm on the time cost of each node. The respective nodes will be sent by the master processor to the other processors.
Each processor will communicate only with the processor recorded in the preference list to get their evaluation score. 
Calculations of new evaluation score will be done on each processor in parallel 
The edges of each node then change: drop some edges based on evaluation score and add some edges randomly to create some randomness. 
Summarizing the workload of each processor to the master processor. If the workload is very imbalanced, repeat step 2. Else start with step 3 above.
 
Each processor is dependent on the information sent by the other processor and is dependent on the node list sent by the master processor. There is memory accessed by the master at the start and end of the program and there might be one master copy of nodes’ preferences lists/ evaluation lists in the master process (but that will depend on the tradeoff between the number of communications and the size of each communication). The diverge execution happens in step 3,4 above where each processor separately makes evaluation. 

As mentioned earlier, the most challenging part is that the workload of each processor is not completely known to the processor itself: the number of friends of friends is not known. Thus, we need prediction algorithms to predict how much nodes are connected to our neighbor nodes to avoid too much communication. The workload imbalance between processors and the reduction of communication cost are the two most important challenges we will face. 
 
Section 4: Resources 
We will start from scratch with no outside data/previous code. We may do research on the books/papers related to human network, computer network constructions, message passing, processor communication which we are unsure which exactly we will reference as of now. We will consult modules like MPI homepage and C++ help pages for syntax/function usage. We will also need to access PSC machines + GHC machines to benefit from the multiprocessors system that it has.

Section 5: Goals 
PLAN to ACHIEVE: 
Develop the system we have described in the earlier section (background/challenge section) complete with no error. 
Expect a decent speedup from a sequential version of our system. Not sure how decent speedup will be because it will depend on the dataset we use and the number of edges each node will have. However, it will be indicative of the effectiveness of MPI in improving performance. 

HOPE to ACHIEVE:
Extend the 2nd degree of separation to a higher degree of separation. This will require a much more sophisticated prediction on the time cost of each node and communication cost will be more. This will be hard because we will need to handle more message sending / synchronization of messages. 
Or experience different communication strategies when the hyperparameters/dataset we created are different. This will be interesting because from testing it, we may be able to get a balance point to indicate when it will be good to have more communication and when it will be better to just do some extra work in each processor. 

Demo planned at the poster session: 
we will prepare a poster that includes the performance of our network model run on different numbers of cores with various iteration values. In the optimal case, our model will provide a significant speedup while parameters are changing.

Our background problem tends to be an optimization problem due to the fact that the network is almost completely randomized, the workload among different processors could be significantly imbalanced. Moreover, each processor is dependent on the information sent by the other processors and is dependent on the node list sent by the master processor. There is memory accessed by the master at the start and end of the program and there might be one master copy of nodes’ preferences lists/ evaluation lists in the master process (but that will depend on the tradeoff between the number of communications and the size of each communication).  Thus, we will construct our parallelized algorithm to predict how many nodes are connected to our neighbor nodes to avoid too much communication. The workload imbalance between processors and the reduction of communication cost are the two most important challenges we will face. In this case, hopefully our algorithm will achieve at least 35x compared to single-core sequential algorithm. 

Section 6: Platform Choice:
GHC and PSC with multicores will be used as our machine to run the program since they can provide at maximum 128 cores, which provides us with the capability to measure the performance of the program on various configurations. We use C++ as main programming language since we need to MPI to apply multicore parallelism and C++ will best support those resources that we are going to use on our model

Section 7: Schedule
The course schedule is organized as:

—-------------------------------------—--------------------------------------

Description Due When
Discuss Ideas with Instructors Mon/Wed, March 27/29, 9:30-11:00 am 
Project Proposal Due Fri, March 31, 11:59pm 
Milestone Report Due Wed, April 19, 9:00am 
Final Report Due Thu, May 4, 11:59pm 
Poster Session Fri, May 5, 8:30am or 1:00pm

—------------------------------------------------------------------------------

Based on the course schedule, we decide to work on our final project following such timeline:


Meeting 1 (Mon.)

Meeting 2 (Thurs.)

Instructor Communication (Wed.)

W1(Mar 31st to Apr. 7th)

Finish the final project proposal, decide main focus of the project, and assign work to each week

Search for resources (papers/ relevant open-source project information) and perform different ideations and decide which one to work on

Discuss various ideations with instructors during OH and ask for their advice (which model could be better to implement and if any other resources/APIs could be considered when implementing the model

W2(Apr.10th to Apr. 14th)

Implement basic structure of the model with available resources and discuss obstacles encountered, and prepare to implement the whole model and start Milestone Report

Done with the general model implementation as first prototype and recheck the defects of the model, think about possible ways of optimization while starting performance testing 

Show the first prototype to instructors and ask for their advice and possible ways to optimize. Again, think about any more resources that can be added to the prototype during future optimization periods. Moreover, ask about any other aspects of program could be tested but not considered yet

W3(Apr. 17th to Apr. 21th)

Optimizing the first prototype and complete second version of model with performance test and brainstorm other possible ways of optimization and submit Milestone Report

Finish second prototype and refine the model to final version with advice from instructors and start writing poster and final report

Ask instructors for help on optimizing the second prototype and maybe more ways of testing and optimization

W4(Apr. 24th to Apr. 28th)

Test final performance/ improvements brought by our model and write the report and design poster, finishing it before meeting instructors

Refine final report based on advice given by instructors and redesign the poster according to final report of new version

Check details of report and poster with instructors and ask for advice on refining 

W5(May 1st to May 5th)

Ready to submit the final report 

Finish the poster

Final advice on poster and check if anything needs to be changed



#################################################################################################################
MileStone Report:

Section 1 Schedule So-far:

We have tightly followed the schedule made in Final Project Proposal and have completed the sequential version of our code. Based on the course schedule, we decide to work on our final project following such timeline before the poster session:

Exact Schedule see here: https://docs.google.com/document/d/1hYbyfkilOPddejwr8oipkFsvZ3YKK_xDqYdiAA0nXrE/edit

Section 2 Work Summary:

As a project that we start from scratch, an important part of our work is to build up the network which includes a random data generator, all the structures needed for our algorithm, input/output file managing codes and testing codes. Thus, for the first three weeks, we completed these parts as mentioned in our schedule. More specifically they are: 
Basic structures:  adjacency matrix input/output files,  structures for people, connection between people, read/write files functions, makefile
Codes: random generator for adjacency matrix input data, network randomization (changes in each iteration of the loop of our simulation steps)
Besides, we have finished implementing all the sequential versions of our algorithm:
Network_simulator_seq.cpp is a script where one degree of friendship is considered (people only gather information from their direct friend and make evaluation based on these data) and no randomization of friends took place in the iterations.
Network_simulator_seq2.cpp is a script where n>1 degrees of friendship is considered (people not only gather information from their direct friend, but also gather information from the friends of friends multiplying a discounting factor) and uses randomization of connections to simulate a more realistic human network connection. 


Section 3 Goals and Deliverables:
In our Final project proposal, we state that the goal of the system is to parallelize the computation of social networks with multiple degrees and compare the speedup with the corresponding sequential version of the code. At the same time, since the like-value of each person changes completely randomly (implemented with a random number generated algorithm) and then we will always test how different scheduling approaches will affect the performance of the system. For now, we have successfully implemented the sequential version of the algorithm that runs with different degrees and are working on implementing MPI version of code and trying to achieve better speedup with parallelism. Our current is coherent to the purpose stated before and will incorporate performances of different scheduling approaches. 
Overall, the new list of goals will be similar to what we have before but with slightly different modifications:
Speedup comparison between sequential and parallel version (implemented by MPI)
Incorporate with different scheduling approaches 
Analyze the reason behind the speedup brought by parallelism under different scenarios (dense/sparse graph, intense/mild randomness, etc)


Section 4: Demo presentations:
In our final demo session, we plan to show a speedup chart which describes the speedup we achieve with parallelism using MPI in comparison with the sequential version we have. We will demonstrate how and why these speedups are achieved and how these results are meaningful to our understanding of human networking and the usage of parallelism in network computations. 


Section 5 Preliminary Results:
As we have only implemented sequential versions of our code and are still working on parallelizing the algorithm using MPI, the performance analysis can merely describe how runtime changes when the degree increases and will not provide too much useful information toward our final result or speedup. Therefore, we leave our current preliminary result as blank and will fill this out immediately after we have done the whole project. 


Section 6 Concerns:

As we have not yet completed the parallel algorithm, here are some of the concerns that we have: 
Figuring out commands that would allow us to run our scripts in multi-core: makeFile 
what and how the system overall is able to benefit from the parallelism: we may encounter large overhead because the messaging is huge in this project 
Exact ways to parallelize the computation and how we can send messages to each processor (detailed implementation): each person may expect a different number of connections/messages.
In what ways can we best distribute work evenly to the workers: we have ideas about predicting the individual workload but how that will benefit the time cost is still unknown. 


Section 7 Instructor Meetup:
We have signed up for a meeting with the instructor on Wednesday Morning to discuss our current progress and any modifications needed to better complete the project.


