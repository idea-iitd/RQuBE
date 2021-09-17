# RQuBE

## Introduction
Regular path query (RPQ) on labelled graphs is one of the core operators in graph analytics. In the decision version of an RPQ,the input is a pair of source-destination nodes, a labelled graph, and a regular expression. The goal is to determine if there exists a simple path from the source to destination, such that the label sequence in the path satisfies the input regular expression. However, identifying this regular expression is a tedious task and the goal of our work isto remove the barrier imposed by regular expressions without compromising on their expressive power. 
Thus We use query by example paradigm where the user provides an exemplar source-destination pair. The exemplar pair acts as a proxy for the regex constraint and communicates to the query evaluation engine the constraints in a more user-friendly manner. However,to make sense of this query, the query evaluation engine needs to execute the following tasks:
(1) Infer the regular expression that characterizes the paths between the source and the destination, and
(2) Identify nodes in the graph that are connected in a similar manner to the source as characterized by the regex inferred from the exemplar pair.

## Datasets
Datasets can be downloaded from here: https://drive.google.com/drive/folders/10odFg2fhUzrXc6symOcWQf9-abcW1n3k?usp=sharing

## Running experiments
To run a dataset with edge labels use x_edge file otherwise x_node file.

### Running RQuBE for a query
To compile: g++ -O3 src/RQuBE_(node/edge).cpp -o out

To run: ./out path_to/edges.txt path_to/labels.txt source_node destination_node path_length support_threshold samples_to_take_in_each_iteration retain_percentage directed/undirected topK_ranks_required eta number_of_hops

Example: ./out freebase/edges.txt freebase/labels.txt 1234 5678 4 0.3 2000 0.8 1 10 0.01 2

It created 2 files (topk.csv and nfa.csv) where topk.csv contains the topk ranks found by the algo and nfa.csv stores the nfa learned by the BF algorithm.

### BBFS
Create a folder where the results of bbfs will be stored as shown below in the example.

To compile: g++ -O3 src/bbfs_(node/edge).cpp -o out

To run: ./out path_to/edges.txt path_to/labels.txt ground_truth_directory start_file_number end_file_number path_length

Example: ./out freebase/edges.txt freebase/labels.txt groundTruth/freebase 0 2 4

Creates files 0.csv,1.csv in grundTruth/freebase and each file contains the ground truth data for a randomly chosen pair.

### Running experiments on values of parameters
To run the experiments it is necessary that we have created some example in the ground truth by bbfs for the calculation of the discussed metrics.

To compile: g++ -O3 src/runExperiment_(node/edge).cpp -o out

To run: ./out path_to/edges.txt path_to/labels.txt ground_truth_directory start_file_number end_file_number support_threshold samples_to_take_in_each_iteration retain_percentage directed/undirected topK_ranks_required eta number_of_hops path_length (supp/retain/k/z/eta/d)

Example: ./out freebase/edges.txt freebase/labels.txt groundTruth/freebase 0 2 0.3 2000 0.8 1 10 0.01 2 4 supp

Creates a single file with value.experiment_name given as output. For the above example 3.supp will be created which will store the value of metrics and time taken by the algorithm.

## Survey of the BF Algorithm
User survey graphs are available here: https://1drv.ms/b/s!AoQBaOgKVpLvibpFtyrrL64fsb8czA?e=Q3A8p9
