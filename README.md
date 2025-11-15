# minimal-subsequential-transducer

This repository contains my course project for **Applications of Finite State Machines**, taught at Sofia University.

The project implements a **minimal subsequential transducer** constructed from a sorted dictionary, in **linear time with respect to the total length of the dictionary**. A subsequential transducer represents a function from strings to numbers in a highly compressed form. This type of algorithm is widely used in efficient software systems such as database engines and indexing structures.

The same idea can be applied to build **perfect hash functions** from strings to integers, where lookup time is linear in the length of the input word.

The algorithm is presented in detail in the paper available [here](https://link.springer.com/chapter/10.1007/3-540-44674-5_180).
.

My implementation has been extensively tested both by me and by Prof. Mihov, and it passes all tests while remaining efficient and fast. In one of the stress tests, the program successfully built a transducer with **10 million states**, representing **1 million words**, each with an average length of around 100 characters.
