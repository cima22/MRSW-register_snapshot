# MRSW-register snapshot
Final project for the "Advanced Multiprocessor Programming" @ TU Wien. <br>

# Wait-free snapshot
A *wait-free snapshot* is a concurrent object taken from the theory of shared-memory multiprocessor programming. An atomic snapshot constructs an instantaneous view of an array of atomic registers. It is wait-free, meaning that a thread can take an instantaneous snapshot of memory without delaying any other thread. Atomic snapshots might be useful for backups or checkpoints (Chapter no. 4 of "The art of multiprocessor programming"[[1](https://dl.acm.org/doi/book/10.5555/2385452)]).

# Goal of the project
- Implement the Multiple Reader Single Writer wait-free snapshot presented in the Chapter 4 of "The Art of Multiprocessor Programming" book [1]
- Implement Multiple Reader Single Writer wait-free snapshot extension from Damien Imbs et al. [add reference]
- Test both the implementations for correctness
- Benchmark the methods exposed by the object, with different combinations of operations and increasing number of threads

# Description

# How to compile

# How to run

# Dependencies and prerequisites

## Authors and acknowledgment

Project carried out by Cimador Gabriele, Gunter Kambiz, Ionescu Serban.

## License

Repository licensed with the MIT license. See the [LICENSE](LICENSE) for rights and limitations.
