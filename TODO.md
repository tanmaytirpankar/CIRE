* Add CHPC batch scripts for the benchmarks
* Get the C++ counterparts of satire benchmarks and modify them to work with CIRE
* Look into upping the simplification level
* Look into eliminating regeneration of the expression for the maxima after you generate for minima
* In generateExpr, remove the common statements like generating the Symbolic expression out of the switch statements
* Investigate memory leaks and think about reducing memory usage.
* Think about metrics to collect and add them to results file
  * Errors on abstracted nodes
* Add conditionals
* Get unhandled fpbench benchmarks to work
* Transfer the Ibex::Interval field from FreeVariable to Variable node and remove the FreeVariable node
* Add an LLVM Instruction field to node.
* Reformat entire codebase using clang-format

# Completed
* Fix another type cast rounding problem. The type cast rounding should not be multiplied by the backward derivative. You only determine the type cast rounding simultaneously with the backward deriv computation but it is used by adding to the operator rounding error during error expression generation.
* Fix the type cast rounding problem. type cast rounding should not be multiplied with the backward derivatives. Have a separate dictionary for the product.
* Handle fptrunc and fpext LLVM instructions
* Think about metrics to collect and add them to results file
  * Input related metrics
    * Num Operators
    * Height of tree
  * Execution related metrics
    * Time
    * Final Error
  * Scalability studies
    * Abstraction window
    * Num times abstracted
    * Operator Threshold
* Add support for abstraction and other flags to the LLVM frontend
* Add logging support
* Add rounding type to all the operators in Node.cpp
* Output the results of the analysis to a file in CIRE