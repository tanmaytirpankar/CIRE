#include "Cire.h"

void show_usage(std::string name) {
  std::cerr << "Usage: " << name << " <option(s)> <input_file>"
            << "Options:\n"
            << "\t-h,--help\t\tShow this help message\n"
            << "\t-a,--abstraction\t\tEnable abstraction\n"
            << "\t-m,--min-depth\t\tSet minimum depth for abstraction\n"
            << "\t-M,--max-depth\t\tSet maximum depth for abstraction\n"
            << "\t-c,--compare\t\tValidate the generated error expression with the one in the file\n"
            << "\t-o,--output\t\tSet the output file\n"
            << "\t-d,--debug-level\t\tSet the debug level\n"
            << "\t-l,--log-level\t\tSet the log level\n"
            << "\t-lo,--log-output\t\tSet the log output file\n"
            << "\t-cf,--csv-friendly\t\tEnable output to JSON file in a CSV friendly manner\n"
            << "\t-to,--global-opt-timeout\t\tSet the global optimization timeout\n"
            << "\t-op,--operator-threshold\t\tSet the threshold on numer of operators on the error expression\n"
            << "\t-cec,--concretize-error-components\t\tConcretize error components\n"
            << "\t-cecd,--collect-error-component-data\t\tCollect error component data\n"
            << std::endl;
}

int main(int argc, char *argv[]) {
  Cire cire;

  const auto start = std::chrono::high_resolution_clock::now();
  bool CSV_friendly = false;

  if (argc < 2) {
    show_usage(argv[0]);
    return 1;
  } else {
    for (int i = 1; i < argc; ++i) {
      std::string arg = argv[i];
      if ((arg == "-h") || (arg == "--help")) {
        show_usage(argv[0]);
        return 0;
      } else if ((arg == "-a") || (arg == "--abstraction")) {
        cire.setAbstraction(true);
      } else if ((arg == "-m") || (arg == "--min-depth")) {
        if (i + 1 < argc) {
          cire.setMinDepth(std::stoi(argv[++i]));
        } else {
          std::cerr << "--min-depth option requires one argument." << std::endl;
          return 1;
        }
      } else if ((arg == "-M") || (arg == "--max-depth")) {
        if (i + 1 < argc) {
          cire.setMaxDepth(std::stoi(argv[++i]));
        } else {
          std::cerr << "--max-depth option requires one argument." << std::endl;
          return 1;
        }
      } else if ((arg == "-c") || (arg == "--compare")) {
        if (i + 1 < argc) {
          cire.graph->setValidationFile(argv[++i]);
        } else {
          std::cerr << "--compare option requires one argument." << std::endl;
          return 1;
        }
      } else if ((arg == "-o" || (arg == "--output"))) {
        if (i + 1 < argc) {
          cire.results->setFile(argv[++i]);
        } else {
          std::cerr << "--output option requires one argument." << std::endl;
          return 1;
        }
      } else if ((arg == "-d" || (arg == "--debug-level"))) {
        if (i + 1 < argc) {
          cire.setDebugLevel(std::stoi(argv[++i]));
          cire.graph->debugLevel = cire.debugLevel;
          cire.graph->errorAnalyzer->debugLevel = cire.debugLevel;
          cire.graph->ibexInterface->debugLevel = cire.debugLevel;
          cire.results->debugLevel = cire.debugLevel;
        } else {
          std::cerr << "--debug-level option requires one argument." << std::endl;
          return 1;
        }
      } else if ((arg == "-l" || (arg == "--log-level"))) {
        if (i + 1 < argc) {
          cire.setLogLevel(std::stoi(argv[++i]));
          cire.graph->logLevel = cire.logLevel;
          cire.graph->errorAnalyzer->logLevel = cire.logLevel;
        } else {
          std::cerr << "--log-level option requires one argument." << std::endl;
          return 1;
        }
      } else if((arg == "-lo") || (arg == "--log-output")) {
        if (i + 1 < argc) {
          cire.graph->log.setFile(argv[++i]);
        } else {
          std::cerr << "--log-output option requires one argument." << std::endl;
          return 1;
        }
      } else if ((arg == "-cf" || (arg == "--csv-friendly"))) {
        CSV_friendly = true;
      } else if ((arg == "-to" || (arg == "--global-opt-timeout"))) {
        if (i + 1 < argc) {
          cire.graph->ibexInterface->optimizerTimeOut = std::stoi(argv[++i]);
        } else {
          std::cerr << "--global-opt-timeout option requires one argument. Default: 20 seconds" << std::endl;
          return 1;
        }
      } else if((arg == "-op" || (arg == "--operator-threshold"))) {
        if (i + 1 < argc) {
          cire.graph->errorAnalyzer->errorExpressionOperatorThreshold = std::stoi(argv[++i]);
        } else {
          std::cerr << "--operator-threshold option requires one argument." << std::endl;
          return 1;
        }
      } else if ((arg == "-cec") || (arg == "--concretize-error-components")) {
          cire.graph->concretize_error_components = true;
      } else if ((arg == "-cecd") || (arg == "--collect-error-component-data")) {
          cire.setCollectErrorComponentData(true);
      } else {
        cire.setFile(argv[i]);
      }
    }
  }

  cire.graph->log.openFile();

  if(cire.graph->parse(*cire.file.c_str()) != 0) {
    return 1;
  }

  const auto parse_end = std::chrono::high_resolution_clock::now();

  std::map<Node *, ErrorAnalysisResult> answer = cire.performErrorAnalysis();

  const auto error_analysis_end = std::chrono::high_resolution_clock::now();

  if(cire.debugLevel > 0) {
    // print the result of nodes corresponding nodes in the output list
    for (string &output: cire.graph->outputs) {
      Node *node = cire.graph->symbolTables[cire.graph->currentScope]->table[output];
      assert(answer.find(node) != answer.end());

      std::cout << "\nOutput Variable: " << output << "";
      if(cire.debugLevel > 2) {
        std::cout << *node;
      }
      std::cout << ": \tOutput: " << answer[node].outputExtrema << ","
                << "  \tError: " << answer[node].errorExtrema << ","
                << "  \n\tNum Optimizer Calls: " << answer[node].numOptimizationCalls << std::endl;
    }
  }

  if (cire.logLevel > 0) {
    // print the result of nodes corresponding nodes in the output list
    for (string &output: cire.graph->outputs) {
      Node *node = cire.graph->symbolTables[cire.graph->currentScope]->table[output];
      assert(answer.find(node) != answer.end());

      cire.graph->log.logFile << "\nOutput Variable: " << output << "";
      if(cire.debugLevel > 2) {
        cire.graph->log.logFile << *node;
      }
      cire.graph->log.logFile << ": \tOutput: " << answer[node].outputExtrema << ","
                              << "  \tError: " << answer[node].errorExtrema << ","
                              << "  \n\tNum Optimizer Calls: " << answer[node].numOptimizationCalls << std::endl;
    }
  }

  const auto total_end = std::chrono::high_resolution_clock::now();
  cire.time_map["Parsing"] = parse_end - start;
  cire.time_map["Error_Analysis"] = error_analysis_end - parse_end;
  cire.time_map["Total"] = total_end - start;

  if(cire.debugLevel > 0) {
    std::cout << "Parsing Time taken: " << cire.time_map["Parsing"].count() << " seconds" << std::endl;
    std::cout << "Error Analysis Time taken: " << cire.time_map["Error Analysis"].count() << " seconds" << std::endl;
    std::cout << "Time taken: " << cire.time_map["Total"].count() << " seconds" << std::endl;
  }
  if(cire.logLevel > 0) {
    cire.graph->log.logFile << "Parsing Time taken: " << cire.time_map["Parsing"].count() << " seconds" << std::endl;
    cire.graph->log.logFile << "Error Analysis Time taken: " << cire.time_map["Error Analysis"].count() << " seconds" << std::endl;
    cire.graph->log.logFile << "Time taken: " << cire.time_map["Total"].count() << " seconds" << std::endl;
    cire.graph->log.log("Writing results to " + cire.results->file + " ...\n");
  }

  if (CSV_friendly) {
    cire.results->writeResultsForCSV(cire.graph->outputs,
                                     cire.graph->numOperatorsOutput,
                                     cire.graph->depthTable.rbegin()->first,
                                     cire.graph->abstractionMetrics,
                                     cire.file, answer, cire.time_map);
  } else {
    cire.results->writeResults(cire.graph->outputs,
                               cire.graph->numOperatorsOutput,
                               cire.graph->depthTable.rbegin()->first,
                               cire.graph->abstractionMetrics,
                               cire.file, answer, cire.time_map);
  }

  if(cire.logLevel > 0) {
    cire.graph->log.logFile << "Results written to " << cire.results->file << "!" << std::endl;
  }

  return 0;
}