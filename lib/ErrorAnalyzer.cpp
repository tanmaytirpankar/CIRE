#include "../include/ErrorAnalyzer.h"

ErrorAnalyzer::ErrorAnalyzer(Logging *log): log(*log) {

}

bool ErrorAnalyzer::parentsVisited(Node *node) {
  return numParentsOfNode[node] >= parentsOfNode[node].size();
}

void ErrorAnalyzer::derivativeComputingDriver() {
  if (debugLevel > 1) {
    std::cout << "Computing Derivatives..." << std::endl;
  }
  if (logLevel > 0) {
    assert(log.logFile.is_open());
    log.log("Computing Derivatives...\n");
  }

  int next_depth = -1;

  // Iterate all nodes in the worklist
  while(!workList.empty()) {
    Node *node = *workList.begin();
    workList.erase(node);

    int current_depth = node->depth;
    next_depth = current_depth-1;
    // If node contains a constant, add it to completed list as you cannot
    // compute its derivative.
    if(derivativeComputedNodes[current_depth].find(node) != derivativeComputedNodes[current_depth].end()) {
      // If derivative of node has already been computed, move on.
    }
    // These are constants and their derivatives are 0
    else if(node->type == NodeType::INTEGER ||
            node->type == NodeType::FLOAT ||
            node->type == NodeType::DOUBLE) {
      derivativeComputedNodes[current_depth].insert(node);
    }
    // If all parents of node have been visited, compute derivative of node
    else if(parentsVisited(node)) {
      derivativeComputing(node);
    }
    // This means that requirements for computing derivative of node are not met
    //  and we need to add it to the worklist again
    // Requirements are that all parents of node have been visited and node is not a constant
    else {
      workList.insert(node);
    }

    // If all nodes at current depth have been processed, move to next depth and add nodes at that depth to worklist
    if(workList.empty() && !nextWorkList.empty() && next_depth >= 0) {
      std::copy_if(nextWorkList.begin(), nextWorkList.end(), std::inserter(workList, workList.end()), [&next_depth](
              Node *node) { return node->depth == next_depth;});
    }
  }

  if (debugLevel > 4) {
    printBwdDerivativesIbexExprs();
    std::cout << std::endl;
  }
  if (logLevel > 4) {
    assert(log.logFile.is_open());
    logBwdDerivativesIbexExprs();
    log.logFile << std::endl;
  }

  if (debugLevel > 1) {
    std::cout << "Derivatives computed!" << std::endl;
  }
  if (logLevel > 1) {
    assert(log.logFile.is_open());
    log.log("Derivatives computed!\n");
  }
}

// Compute the Backward derivative of outVar with respect to node's children by using the chain rule
void ErrorAnalyzer::derivativeComputing(Node *node) {
  // TODO: Type rounding handled only for FL64 --> FL32. Handle for other cases.
  std::vector<Node *> outputList = keys(BwdDerivatives[node]);
  for (Node *outVar : outputList) {
    assert(BwdDerivatives[node][outVar] != nullptr && "Derivative of output wrt node should have been computed\n");

    Node *Operand, *leftOperand, *rightOperand;
    ibex::ExprNode *derivThroughNode, *derivLeftThroughNode, *derivRightThroughNode;
    ibex::ExprNode *typeCastRndVal;
    switch (node->type) {
      case DEFAULT:
      case INTEGER:
      case FLOAT:
      case DOUBLE:
      case FREE_VARIABLE:
      case VARIABLE:
        break;
      case UNARY_OP:
        Operand = ((UnaryOp *) node)->Operand;
        derivThroughNode = (ibex::ExprNode *) &product(*BwdDerivatives[node][outVar],
                                                      *getDerivativeWRTChildNode(node, 0)).simplify(0);

        if(node->OpRndType == Node::FL32 && Operand->OpRndType == Node::FL64) {
          typeCastRndVal = (ibex::ExprNode *) &ibex::ExprConstant::new_scalar(node->RoundingAmount[node->OpRndType]);
        } else {
          typeCastRndVal = (ibex::ExprNode *) &ibex::ExprConstant::new_scalar(0);
        }


        if (contains(BwdDerivatives[Operand], outVar)) {
          BwdDerivatives[Operand][outVar] = (ibex::ExprNode *) &(*BwdDerivatives[Operand][outVar] + *derivThroughNode);
        } else {
          BwdDerivatives[Operand][outVar] = (ibex::ExprNode *) &(*derivThroughNode);
        }

        if (contains(typeCastRnd[Operand], outVar)) {
          typeCastRnd[Operand][outVar] = (ibex::ExprNode *) &(*typeCastRnd[Operand][outVar] + *typeCastRndVal);
        } else {
          typeCastRnd[Operand][outVar] = (ibex::ExprNode *) &(*typeCastRndVal);
        }

        if (debugLevel > 4) {
          std::cout << *node->getExprNode() << " wrt "
                    << *Operand->getExprNode() << " : "
                    << *derivThroughNode << std::endl;
          std::cout << "Derivative so far of " << *outVar->getExprNode() << " wrt "
                    << *Operand->getExprNode() << " : "
                    << *BwdDerivatives[Operand][outVar] << std::endl;
        }
        if (logLevel > 4) {
          log.logFile << *node->getExprNode() << " wrt "
                      << *Operand->getExprNode() << " : "
                      << *derivThroughNode << std::endl;
          log.logFile << "Derivative so far of " << *outVar->getExprNode() << " wrt "
                      << *Operand->getExprNode() << " : "
                      << *BwdDerivatives[Operand][outVar] << std::endl;
        }

        // Add child to nextWorkList
        nextWorkList.insert(Operand);
        // Increment number of parents of child that have been processed
        numParentsOfNode[Operand]++;
        break;
      case BINARY_OP:
        // Computing the backward derivative of outVar with respect to node's children
        leftOperand = ((BinaryOp *) node)->leftOperand;
        derivLeftThroughNode = (ibex::ExprNode *) &product(*BwdDerivatives[node][outVar],
                                                          *getDerivativeWRTChildNode(node, 0)).simplify(0);
        if(node->OpRndType == Node::FL32 && leftOperand->OpRndType == Node::FL64) {
          typeCastRndVal = (ibex::ExprNode *) &ibex::ExprConstant::new_scalar(node->RoundingAmount[node->OpRndType]);
        } else {
          typeCastRndVal = (ibex::ExprNode *) &ibex::ExprConstant::new_scalar(0);
        }

        if (contains(BwdDerivatives[leftOperand], outVar)) {
          BwdDerivatives[leftOperand][outVar] = (ibex::ExprNode *) &(*BwdDerivatives[leftOperand][outVar] +
                                                                     *derivLeftThroughNode);
        } else {
          BwdDerivatives[leftOperand][outVar] = (ibex::ExprNode *) &(*derivLeftThroughNode);
        }

        if (contains(typeCastRnd[leftOperand], outVar)) {
          typeCastRnd[leftOperand][outVar] = (ibex::ExprNode *) &(*typeCastRnd[leftOperand][outVar] + *typeCastRndVal);
        } else {
          typeCastRnd[leftOperand][outVar] = (ibex::ExprNode *) &(*typeCastRndVal);
        }

        if (debugLevel > 4) {
          std::cout << *node->getExprNode() << " wrt "
                    << *leftOperand->getExprNode() << " : "
                    << *derivLeftThroughNode << std::endl;
          std::cout << "Derivative so far of " << *outVar->getExprNode() << " wrt "
                    << *leftOperand->getExprNode() << " : "
                    << *BwdDerivatives[leftOperand][outVar] << std::endl;
        }
        if (logLevel > 4) {
          log.logFile << *node->getExprNode() << " wrt "
                      << *leftOperand->getExprNode() << " : "
                      << *derivLeftThroughNode << std::endl;
          log.logFile << "Derivative so far of " << *outVar->getExprNode() << " wrt "
                      << *leftOperand->getExprNode() << " : "
                      << *BwdDerivatives[leftOperand][outVar] << std::endl;
        }

        rightOperand = ((BinaryOp *) node)->rightOperand;
        derivRightThroughNode = (ibex::ExprNode *) &product(*BwdDerivatives[node][outVar],
                                                           *getDerivativeWRTChildNode(node, 1)).simplify(0);
        if(node->OpRndType == Node::FL32 && rightOperand->OpRndType == Node::FL64) {
          typeCastRndVal = (ibex::ExprNode *) &ibex::ExprConstant::new_scalar(node->RoundingAmount[node->OpRndType]);
        } else {
          typeCastRndVal = (ibex::ExprNode *) &ibex::ExprConstant::new_scalar(0);
        }


        if (contains(BwdDerivatives[rightOperand], outVar)) {
          BwdDerivatives[rightOperand][outVar] = (ibex::ExprNode *) &(*BwdDerivatives[rightOperand][outVar] +
                                                                      *derivRightThroughNode);
        } else {
          BwdDerivatives[rightOperand][outVar] = (ibex::ExprNode *) &(*derivRightThroughNode);
        }

        if (contains(typeCastRnd[rightOperand], outVar)) {
          typeCastRnd[rightOperand][outVar] = (ibex::ExprNode *) &(*typeCastRnd[rightOperand][outVar] + *typeCastRndVal);
        } else {
          typeCastRnd[rightOperand][outVar] = (ibex::ExprNode *) &(*typeCastRndVal);
        }

        if (debugLevel > 4) {
          std::cout << *node->getExprNode() << " wrt "
                    << *rightOperand->getExprNode() << " : "
                    << *derivRightThroughNode << std::endl;
          std::cout << "Derivative so far of " << *outVar->getExprNode() << " wrt "
                    << *rightOperand->getExprNode() << " : "
                    << *BwdDerivatives[rightOperand][outVar] << std::endl;
        }
        if (logLevel > 4) {
          log.logFile << *node->getExprNode() << " wrt "
                      << *rightOperand->getExprNode() << " : "
                      << *derivRightThroughNode << std::endl;
          log.logFile << "Derivative so far of " << *outVar->getExprNode() << " wrt "
                      << *rightOperand->getExprNode() << " : "
                      << *BwdDerivatives[rightOperand][outVar] << std::endl;
        }

        // Add children to nextWorkList
        nextWorkList.insert(leftOperand);
        nextWorkList.insert(rightOperand);

        // Increment number of parents of children that have been processed
        numParentsOfNode[leftOperand]++;
        numParentsOfNode[rightOperand]++;
        break;
      case TERNARY_OP:
        // TODO: Complete this on adding ternary operations
        break;
    }
  }

  derivativeComputedNodes[node->depth].insert(node);
}

void ErrorAnalyzer::printBwdDerivative(Node *outNode, Node *WRTNode) {
  std::cout << *outNode->getExprNode() << " wrt "
            << *WRTNode->getExprNode() << " : "
            << *this->BwdDerivatives[WRTNode][outNode] << std::endl;
}

void ErrorAnalyzer::printBwdDerivativesIbexExprs() {
  std::cout << "Backward Derivatives: " << std::endl;
  for (auto &wrtNode : this->BwdDerivatives) {
    for (auto &outputNode : wrtNode.second) {
      printBwdDerivative(outputNode.first, wrtNode.first);
    }
  }
}

void ErrorAnalyzer::logBwdDerivative(Node *outNode, Node *WRTNode) {
  log.logFile << *outNode->getExprNode() << " wrt "
              << *WRTNode->getExprNode() << " : "
              << *this->BwdDerivatives[WRTNode][outNode] << std::endl;
}

void ErrorAnalyzer::logBwdDerivativesIbexExprs() {
  log.logFile << "Backward Derivatives: " << std::endl;
  for (auto &wrtNode : this->BwdDerivatives) {
    for (auto &outputNode : wrtNode.second) {
      logBwdDerivative(outputNode.first, wrtNode.first);
    }
  }
}

void ErrorAnalyzer::errorComputingDriver(const std::set<Node*> &candidate_nodes, IBEXInterface *ibexInterface) {
  if (debugLevel > 1) {
    std::cout << "Computing Error..." << std::endl;
  }
  if (logLevel > 1) {
    assert(log.logFile.is_open() && "Log file not open");
    log.logFile << "Computing Error..." << std::endl;
  }

  for(auto &output : candidate_nodes) {
    if(errorComputedNodes[output->depth].find(output) ==
       errorComputedNodes[output->depth].end()) {
      errorComputing(output, ibexInterface);
    }

    ErrAccumulator[output] = (ibex::ExprNode*) &(*ErrAccumulator[output]
   // The power term is the value of double ULP. We dont multiply by it here so optimizer can function better
   // AND we get the optimal value in terms of number of ULPs.
   // If uncommenting, comment the power term in the error computation
//            * pow(2, -53)
            );
  }

  if (debugLevel > 1) {
    std::cout << "Error Expressions generated!" << std::endl;
  }
  if (logLevel > 1) {
    assert(log.logFile.is_open() && "Log file not open");
    log.logFile << "Error Expressions generated!" << std::endl;
  }
}

void ErrorAnalyzer::errorComputing(Node *node, IBEXInterface *ibexInterface) {
  Node *Operand, *leftOperand, *middleOperand, *rightOperand;

  switch (node->type) {
    case DEFAULT:
    case INTEGER:
    case FLOAT:
    case DOUBLE:
    case FREE_VARIABLE:
    case VARIABLE:
      break;
    case UNARY_OP:
      Operand = ((UnaryOp *) node)->Operand;
      if(errorComputedNodes[Operand->depth].find(Operand) ==
         errorComputedNodes[Operand->depth].end()) {
        errorComputing(Operand, ibexInterface);
      }
      break;
    case BINARY_OP:
      leftOperand = ((BinaryOp *) node)->leftOperand;
      if(errorComputedNodes[leftOperand->depth].find(leftOperand) ==
         errorComputedNodes[leftOperand->depth].end()) {
        errorComputing(leftOperand, ibexInterface);
      }

      rightOperand = ((BinaryOp *) node)->rightOperand;
      if(errorComputedNodes[rightOperand->depth].find(rightOperand) ==
         errorComputedNodes[rightOperand->depth].end()) {
        errorComputing(rightOperand, ibexInterface);
      }
      break;
    case TERNARY_OP:
      leftOperand = ((TernaryOp *) node)->leftOperand;
      if(errorComputedNodes[leftOperand->depth].find(leftOperand) ==
         errorComputedNodes[leftOperand->depth].end()) {
        errorComputing(leftOperand, ibexInterface);
      }

      middleOperand = ((TernaryOp *) node)->middleOperand;
      if(errorComputedNodes[middleOperand->depth].find(middleOperand) ==
         errorComputedNodes[middleOperand->depth].end()) {
        errorComputing(middleOperand, ibexInterface);
      }

      rightOperand = ((TernaryOp *) node)->rightOperand;
      if(errorComputedNodes[rightOperand->depth].find(rightOperand) ==
         errorComputedNodes[rightOperand->depth].end()) {
        errorComputing(rightOperand, ibexInterface);
      }
      break;
  }

  if(errorComputedNodes[node->depth].find(node) == errorComputedNodes[node->depth].end()) {
    propagateError(node, ibexInterface);
  }
  errorComputedNodes[node->depth].insert(node);
}

void ErrorAnalyzer::propagateError(Node *node, IBEXInterface *ibexInterface) {
  std::vector<Node *> outputList = keys(BwdDerivatives[node]);

  for (Node *outVar : outputList) {
    if (debugLevel > 3) {
      std::cout << "Propagating error for " << outVar->id << " through node " << node->id << std::endl;
      if (debugLevel > 4) {
        printBwdDerivative(outVar, node);
        std::cout << "absolute error:" << node->getAbsoluteError() << std::endl;
        std::cout << "OpRounding:" << node->getRounding() << std::endl;
        std::cout << "Type Cast Rounding:" << *typeCastRnd[node][outVar] << std::endl;
      }
    }
    if(logLevel > 3) {
      log.logFile << "Propagating error for " << outVar->id << " through node " << node->id << std::endl;

      if (logLevel > 4) {
        logBwdDerivative(outVar, node);
        log.logFile << "absolute error:" << node->getAbsoluteError() << std::endl;
        log.logFile << "OpRounding:" << node->getRounding() << std::endl;
        log.logFile << "Type Cast Rounding:" << *typeCastRnd[node][outVar] << std::endl;
      }
    }

    // Generate the error expression by computing the product of the Backward derivative of outVar wrt node and
    // the local_error (product of the expression corresponding the node and (the operator rounding + type cast rounding)
    // Add the type cast rounding to the nodes rounding amount
    // rounding is the amount to round at ULP level whereas rounding error is the absolute amount of error introduced
    auto total_rounding = (ibex::ExprNode *) &(node->getRounding() + *typeCastRnd[node][outVar]);
    auto local_plus_type_cast_error = (ibex::ExprNode *) &product(node->getAbsoluteError(),
                                                                  *total_rounding).simplify(0);
    auto expr = (ibex::ExprNode *) &product(*BwdDerivatives[node][outVar],
                                           *local_plus_type_cast_error).simplify(0);

    if (contains(ErrAccumulator, outVar)) {
      ErrAccumulator[outVar] = (ibex::ExprNode *) &(*ErrAccumulator[outVar] + *expr);
    } else {
      ErrAccumulator[outVar] = (ibex::ExprNode *) &(*expr);
    }

    if(ErrAccumulator[outVar]->size > errorExpressionOperatorThreshold) {
      OptResult max_err = ibexInterface->FindAbsMax(*ErrAccumulator[outVar]);
      ErrAccumulator[outVar] = (ibex::ExprNode *) &ibex::ExprConstant::new_scalar((-max_err.result).mag());
      nodeNumOptCallsMap[outVar]++;
      if(debugLevel > 1) {
        std::cout << "Error Accumulator size exceeded " << errorExpressionOperatorThreshold << ". Concretizing error." << std::endl;
      }
      if(logLevel > 1) {
        log.logFile << "Error Accumulator size exceeded " << errorExpressionOperatorThreshold << ". Concretizing error." << std::endl;
      }
    }

    if (debugLevel > 4) {
      std::cout << "Error Accumulator for " << *outVar->getExprNode() << " : " << *ErrAccumulator[outVar] << std::endl;
      std::cout << std::endl;
    }
    if (logLevel > 4) {
      log.logFile << "Error Accumulator for " << *outVar->getExprNode() << " : " << *ErrAccumulator[outVar] << std::endl;
      log.logFile << std::endl;
    }
  }
}

ibex::ExprNode *getDerivativeWRTChildNode(Node *node, int index) {
  Node *child = node->getChildNode(index);

  switch (node->type) {
    case NodeType::INTEGER:
    case NodeType::FLOAT:
    case NodeType::DOUBLE:
    case NodeType::FREE_VARIABLE:
    case NodeType::VARIABLE:
      return (ibex::ExprNode *) &ibex::ExprConstant::new_scalar(0.0);
    case NodeType::UNARY_OP:
      switch (((UnaryOp*) node)->op) {
        case Node::NEG:
          return (ibex::ExprNode *) &ibex::ExprConstant::new_scalar(-1);
        case Node::SIN:
          return (ibex::ExprNode *) &cos(*child->getExprNode());
        case Node::COS:
          return (ibex::ExprNode *) &sin(-*child->getExprNode());
        case Node::TAN:
          return (ibex::ExprNode *) &(1.0/sqr(cos(*child->getExprNode())));
        case Node::SINH:
          return (ibex::ExprNode *) &(exp(*child->getExprNode())-exp(-*child->getExprNode())/2.0);
        case Node::COSH:
          return (ibex::ExprNode *) &(exp(*child->getExprNode())+exp(-*child->getExprNode())/2.0);
        case Node::TANH:
          return (ibex::ExprNode *) &(sinh(*child->getExprNode())/cosh(*child->getExprNode()));
        case Node::ASIN:
          return (ibex::ExprNode *) &(1.0/sqrt(1.0-sqr(*child->getExprNode())));
        case Node::ACOS:
          return (ibex::ExprNode *) &(-1.0/sqrt(1.0-sqr(*child->getExprNode())));
        case Node::ATAN:
          return (ibex::ExprNode *) &(1.0/(1.0+sqr(*child->getExprNode())));
        case Node::LOG:
          return (ibex::ExprNode *) &(1.0/(*child->getExprNode()*log(10.0)));
        case Node::SQRT:
          return (ibex::ExprNode *) &(1.0/(2.0*sqrt(*child->getExprNode())));
        case Node::EXP:
          return child->getExprNode();
        case Node::FPTRUNC:
        case Node::FPEXT:
          return (ibex::ExprNode *) &ibex::ExprConstant::new_scalar(1);
        default:
          std::cout << "Error: Should not be here. Unknown unary operator or not a unary operator." << std::endl;
          break;
      }
      break;
    case NodeType::BINARY_OP:
      switch (((BinaryOp*) node)->op) {
        case Node::ADD:
          return (ibex::ExprNode *) &ibex::ExprConstant::new_scalar(1);
        case Node::SUB:
          if (index == 0) {
            return (ibex::ExprNode *) &ibex::ExprConstant::new_scalar(1);
          }
          else {
            return (ibex::ExprNode *) &ibex::ExprConstant::new_scalar(-1);
          }
        case Node::MUL:
          if (index == 0) {
            return ((BinaryOp*) node)->rightOperand->getExprNode();
          }
          else {
            return ((BinaryOp*) node)->leftOperand->getExprNode();
          }
        case Node::DIV:
          if (index == 0) {
            return (ibex::ExprNode *) &(1.0 / *((BinaryOp*) node)->rightOperand->getExprNode());
          } else if (index == 1) {
            return (ibex::ExprNode *) &(-*((BinaryOp*) node)->leftOperand->getExprNode() /
                                        sqr(*((BinaryOp*) node)->rightOperand->getExprNode()));
          }
        default:
          std::cout << "Error: Should not be here. Unknown binary operator or not a binary operator." << std::endl;
          break;
      }
      break;
    case NodeType::TERNARY_OP:
      break;
    default:
      std::cout << "Unknown node type" << std::endl;
      exit(1);
  }

  return nullptr;
}

template<class T1, class T2>
std::vector<T1> keys(std::map<T1, T2> map) {
  std::vector<T1> keys;
  for (auto &key_value : map) {
    keys.push_back(key_value.first);
  }
  return keys;
}

template<class T1, class T2>
bool contains(std::map<T1, T2> map, T1 key) {
  return map.find(key) != map.end();
}

template<class T1, class T2>
T2 findWithDefaultInsertion(std::map<T1, T2> map, T1 key, T2 defaultVal) {
  auto it = map.find(key);
  if (it != map.end()) {
    return it->second;
  }
  else {
    map[key] = defaultVal;
    return defaultVal;
  }
}