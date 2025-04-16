# Code Quality Notes

Welcome to the codebase!

Note that as a C project, it only makes sense to define a module as a `.c` source file and its corresponding `.h` header file.


## Note 1: Module Fan-out

The module fan-out metric is significantly more restrictive than the class coupling equivalent used to analyse OOP projects. Class coupling counts the number of classes referenced from a certain class, while module fan-out counts the number of function calls. 

The metric specified in the Code Quality Assessment document does not count function calls correctly. It counts any variable or constant reference outside of a function as a function call. This means if you have a constant defined in the header file for a source file, the tool will report any reference to that constant as a separate function call, which is not correct.

We believe that this overly restrictive code quality metric is not what was intended, considering its origin in class coupling. For a deeply interconnected and stateful system like an embedded system, attempting to abide by this metrics forces us to decompose modules in unnatural ways, or redefine global constants (like an IP address) in each file, which significantly worsens the quality of our code.

Rather than relying on the metric in the Code Quality Assessment document, we propose an alternative. Module fan-out is a replacement for class coupling, so we would like to bring the metric a bit closer to its initial intention - we can count the number of included modules at the top of a file, mimicking class coupling.


## Note 2: Average Cyclomatic Complexity

During testing, we have noticed that the Understand software sometimes reports the average cyclomatic complexity as the SUM of the cyclomatic complexities of all functions in the file, reporting it as even higher than the MAX cyclomatic complexity.

We do not know exactly, why this happens, but most likely because it interprets function implementation as just function definitions with no implementations, and therefore does not divide the sum by the number of functions properly to calculate the average.

Should this error occur, we ask that you try to rename the folders from "source" and "elementary_functions" to something different. For us, this gave us proper results but only on the first time we ran the quality checker. 

If even then you cannot find a proper reading, please try to look over the files manually.
