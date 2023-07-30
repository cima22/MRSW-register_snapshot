import ctypes
import os
import datetime

class cBenchResult(ctypes.Structure):
    '''
    This has to match the returned struct in library.c
    '''
    _fields_ = [
        ("scan_throughput", ctypes.c_float),
        ("update_throughput", ctypes.c_float),
        ("mixed_throughput", ctypes.c_float),
        ("avg_indep_scan_latency", ctypes.c_float),
        ("avg_mixed_latency", ctypes.c_float),
        ("avg_update_latency", ctypes.c_float)
    ]
class Benchmark:
    '''
    Class representing a benchmark. It assumes any benchmark sweeps over some
    parameter xrange using the fixed set of inputs for every point. It provides
    two ways of averaging over the given amount of repetitions:
    - represent everything in a boxplot, or
    - average over the results.
    '''
    def __init__(self, bench_function, parameters,
                 repetitions_per_point, xrange, basedir, name):
        self.bench_function = bench_function
        self.parameters = parameters
        self.repetitions_per_point = repetitions_per_point
        self.xrange = xrange
        self.basedir = basedir
        self.name = name

        self.data = {i: {} for i in range(6)}
        self.now = None

    def run(self):
        '''
        Runs the benchmark with the given parameters. Collects
        repetitions_per_point data points and writes them back to the data
        dictionary to be processed later.
        '''
        self.now = datetime.datetime.now().strftime("%Y-%m-%dT%H:%M:%S")
        print(f"Starting Benchmark run at {self.now}")

        for x in self.xrange:
            for r in range(6):
                self.data[r][x] = []
            for r in range(0,self.repetitions_per_point):
                result = self.bench_function(x)
                self.data[0][x].append(result.avg_indep_scan_latency*1000)
                self.data[1][x].append(result.avg_mixed_latency*1000)
                self.data[2][x].append(result.avg_update_latency*1000)
                self.data[3][x].append(result.scan_throughput*1000)
                self.data[4][x].append(result.update_throughput*1000)
                self.data[5][x].append(result.mixed_throughput*1000)

    def write_avg_data(self, folder, Date):
        '''
        Writes averages for each point measured into a dataset in the data
        folder timestamped when the run was started.
        '''
        if self.now is None:
            raise Exception("Benchmark was not run. Run before writing data.")

        try:
            os.makedirs(f"{self.basedir}/{folder}/data/{Date}/avg")
        except FileExistsError:
            pass
        for idx, data_dict in self.data.items():
            with open(f"{self.basedir}/{folder}/data/{Date}/avg/{self.name}_{idx}.data", "w") as datafile:
                datafile.write(f"x datapoint\n")
                for x, box in data_dict.items():
                    datafile.write(f"{x} {sum(box)/len(box)}\n")


def benchmark(folder,num_threads):
    '''
    Requires the binary to also be present as a shared library.
    '''
    basedir = os.path.dirname(os.path.abspath(__file__))
    binary = ctypes.CDLL( f"{basedir}/{folder}/snapshotBench.so" )
    # Set the result type for each benchmark function
    binary.small_bench.restype = cBenchResult

    # Parameters for the benchmark are passed in a tuple, here (1000,). To pass
    # just one parameter, we cannot write (1000) because that would not parse
    # as a tuple, instead python understands a trailing comma as a tuple with
    # just one entry.
    smallbench_10 = Benchmark(binary.small_bench, (10,), 3,
                              num_threads, basedir, "smallbench_10")

    Date = datetime.datetime.now().strftime("%Y-%m-%dT%H:%M:%S")
    smallbench_10.run()
    smallbench_10.write_avg_data(folder,Date)
    # smallbench_100.run()
    # smallbench_100.write_avg_data(Date)

if __name__ == "__main__":
    num_threads = [1,2,4,8,16,32]
    benchmark("WFSnapshot",num_threads)
    num_threads.append(64)
    benchmark("PSnapshot",num_threads);
