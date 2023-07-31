# MRSW-register snapshot
Final project for the course [Advanced Multiprocessor Programming](https://tiss.tuwien.ac.at/course/courseDetails.xhtml?courseNr=184726&semester=2023S&dswid=4539&dsrid=714) @ [TU Wien](https://www.tuwien.at/en/). <br>

# Wait-free snapshot
A *wait-free snapshot* is a concurrent object taken from the theory of shared-memory multiprocessor programming. An atomic snapshot constructs an instantaneous view of an array of atomic registers. It is wait-free, meaning that a thread can take an instantaneous snapshot of memory without delaying any other thread. Atomic snapshots might be useful for backups or checkpoints (Chapter no. 4 of "The art of multiprocessor programming"[[1]](https://dl.acm.org/doi/book/10.5555/2385452).

# Goal of the project
- Implement the Multiple Reader Single Writer wait-free snapshot presented in the Chapter 4 of "The Art of Multiprocessor Programming" book [[1]](https://dl.acm.org/doi/book/10.5555/2385452)
- Implement Multiple Reader Single Writer wait-free partial snapshot extension taken from "Help when needed, but no more: Efficient read/write partial snapshot" by Damien Imbs et al. [[2]](https://www.sciencedirect.com/science/article/abs/pii/S0743731511001626)
- Test both the implementations for correctness
- Benchmark the methods exposed by the object, with different combinations of operations and increasing number of threads

# Description
In this repository there are present:
- `Project-3_Report.pdf`, a report of the entire project in PDF format
- `Makefile` to compile the code
- The two main directories: `WFSnapshot` which contains the wait-free implementation from the book, and `PSnapshot` which contains the implementation from the cited paper. The two directories share the same structure, which is made of:
  - `src` directory containing the source code
  - `build` directory containing the `.o` files built after compilation
  - `test` directory containing the source code for testing the implementations
  - `data` directory containing the data which would be generated after running the banchmarks

- `plots` directory containting the `tex` code to produce plots out of the benchmark data
- `benchmark.py` a Python script to responsible to run the benchmarks

# How to compile
## Fast Compile
Just type
```bash
make
```
To compile and build the two executables of the benchmarks, which will be present in the two directories `WFSnapshot` and `PSnapshot`.
## Small benchmark
Just type
```bash
make small-bench
```
To compile and execute the Python benchmark, which will generate the data gathered into the `data` directory. The two metrics measured are:
- Average latency: average amount of milliseconds to execute the operations exposed by the implementations
- Throughput: number of operations execute in a certain amount of time
## Small plot
Just type
```bash
make small-plot
```
To generate `.png` images out of the data, just type
## Test
Just type
```bash
make test
```
To generate, into the two `test` directories present into the two main ones, the executable containing some tests about the correctness of the implementations.
# Dependencies and prerequisites
Make sure to have the ``gcc`` compiler installed alongside with the *OpenMp* library.

## Authors and acknowledgment

Project carried out by Cimador Gabriele, Gunter Kambiz, Ionescu Serban.

## License

Repository licensed with the MIT license. See the [LICENSE](LICENSE) for rights and limitations.
