/*!
 *  Copyright (c) 2016 by Contributors
 * \file node.h
 * \brief Graph node data structure.
 */
#ifndef NNVM_NODE_H_
#define NNVM_NODE_H_

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include "./base.h"
#include "./op.h"

namespace nnvm {

// Forward declare node.
class Node;

/*!
 * \brief we always used NodePtr for a reference pointer
 *  to the node, so this alias can be changed in case.
 *
 *  By default, NodePtr is a std::shared_ptr of node
 */
using NodePtr = std::shared_ptr<Node>;

/*! \brief an entry that represents output data from a node */
struct NodeEntry {
  /*! \brief the source node of this data */
  NodePtr node;
  /*! \brief index of output from the source. */
  uint32_t index;
  /*!
   * \brief version of input Variable.
   *  This field can only be nonzero when this->node is a Variable node.
   *  version is increased by one each time a Variable get composed to a mutation Op.
   *  This information can be helpful to decide order of operations when sequence of mutation happens.
   */
  uint32_t version;
};

/*!
 * \brief The attributes of the current operation node.
 *  Usually are additional parameters like axis,
 */
struct NodeAttrs {
  /*! \brief name of the node */
  std::string name;
  /*! \brief Vector representation of positional attributes */
  std::vector<double> scalars;
  /*! \brief The dictionary representation of attributes */
  std::unordered_map<std::string, std::string> dict;
  /*!
   * \brief A parsed version of attributes,
   * This is generated if OpProperty.attr_parser is registered.
   * The object can be used to quickly access attributes.
   */
  any parsed;
};

/*!
 * \brief Node represents an operation in a computation graph.
 */
class Node {
 public:
  /*!
   * \brief The operator this node uses.
   *  For place holder variable, op == nullptr.
   */
  const Op *op{nullptr};
  /*! \brief inputs to this node */
  std::vector<NodeEntry> inputs;
  /*!
   * \brief Optional control flow dependencies
   *  Gives operation must be performed before this operation.
   */
  std::vector<NodePtr> control_deps;
  /*! \brief The attributes in the node. */
  NodeAttrs attrs;
  /*! \brief destructor of node */
  ~Node();
  /*!
   * \brief return whether node is placeholder variable.
   *  This is equivalent to op == nullptr
   * \return whether node is placeholder input variable
   */
  inline bool is_variable() const;
  /*! \return number of outputs from this node */
  inline uint32_t num_outputs() const;
  /*! \return number of inputs from this node */
  inline uint32_t num_inputs() const;
  /*!
   * \brief create a new empty shared_ptr of Node.
   * \return a created empty node.
   */
  static NodePtr Create();
};

// implementation of functions.
inline bool Node::is_variable() const {
  return this->op == nullptr;
}

inline uint32_t Node::num_outputs() const {
  if (is_variable()) return 1;
  if (this->op->get_num_outputs == nullptr) {
    return this->op->num_outputs;
  } else {
    return this->op->get_num_outputs(this->attrs);
  }
}

inline uint32_t Node::num_inputs() const {
  if (is_variable()) return 1;
  if (this->op->get_num_inputs == nullptr) {
    return this->op->num_inputs;
  } else {
    return this->op->get_num_inputs(this->attrs);
  }
}

}  // namespace nnvm

#endif  // NNVM_NODE_H_
