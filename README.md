Multithreaded Log Processor (C++17)

STATUS
In development. This README describes the intended design and scope of the project.

OVERVIEW
A small C++17 program that reads a log file and processes its lines in parallel. One reader thread feeds lines
into a thread-safe bounded queue, several worker threads take lines from the queue, validate and filter them,
and write the results to output files. The project is a hands-on exercise in the producer-consumer pattern, mutexes, condition variables, backpressure and clean thread shutdown.

HOW IT WORKS

    [Reader thread] --> (Bounded queue) --> [Worker threads 1..N] --> [Output files]

- Reader: reads the input file line by line and pushes each line into the queue. When the file ends, it closes
  the queue.
- Bounded queue: a template class, BoundedQueue<T>, built on std::mutex and std::condition_variable. It has a
  fixed capacity. When it is full, the reader waits until a worker makes space (backpressure). When it is
  empty, workers wait for new lines. After the queue is closed and empty, workers stop.
- Workers: each worker takes a line, checks that it has the expected fields and a valid severity, and then
  either writes it to the output file for its severity or records it as malformed. Lines with DEBUG severity
  are filtered out.
- Output: all workers write through one mutex-protected writer, so lines in a file are never interleaved.
  Malformed lines are written to dead_letter.log and are never silently dropped.
- Counters: std::atomic counters track lines read, filtered, malformed and written.

INPUT FORMAT
One record per line, comma-separated: timestamp,severity,source,message
Valid severities: DEBUG, INFO, WARN, ERROR
Example: 2026-10-24T10:15:30,ERROR,auth-service,token validation failed
A small generator program creates a sample input file, including some deliberately malformed lines.

USAGE
Build:  g++ -std=c++17 -pthread -O2 src/main.cpp -Iinclude -o pipeline
Run:    ./pipeline <input_file> <num_workers> <queue_capacity>
Example: ./pipeline sample_logs.csv 4 100

OUTPUT
- output/INFO.log, output/WARN.log, output/ERROR.log : valid records by severity
- dead_letter.log : malformed lines
- A summary printed to the console with the four counters

CORRECTNESS RULE
Every input line must end up in exactly one place, so the program checks that
filtered + malformed + written = lines read
and prints a warning if it does not hold. Running with different worker counts should always give the same
totals, although the order of lines inside an output file may differ.

PROJECT STRUCTURE
include/BoundedQueue.h   thread-safe bounded queue
src/main.cpp             reader, workers, output writer and shutdown logic
src/generate_logs.cpp    sample input generator

SCOPE
Included: one reader thread, N worker threads, one bounded queue, mutex-protected output, atomic counters,
clean shutdown.
Not included: networking, multiple processing stages, retries, a thread pool, performance benchmarking,
and a database.

WHAT THIS PROJECT DEMONSTRATES
- The producer-consumer pattern with a hand-written thread-safe queue
- Correct use of mutexes, condition variables with predicates, and atomics
- Backpressure through a bounded queue, and orderly shutdown by closing the queue
- Defensive handling of malformed input