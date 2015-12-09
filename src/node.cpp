#include "yaml-cpp/node/node.h"
#include "yaml-cpp/exceptions.h"
#include "yaml-cpp/node/detail/node.h"
#include "yaml-cpp/node/detail/node_data.h"

#include "nodebuilder.h"
#include "nodeevents.h"

namespace YAML {
Node Clone(const Node& node) {
  NodeEvents events(node);
  NodeBuilder builder;
  events.Emit(builder);
  return builder.Root();
}

const std::string& Node::Scalar() const {
    if (!m_isValid)
        throw InvalidNode();
    return m_pNode ? m_pNode->scalar() : detail::node_data::empty_scalar;
}

const std::string& Node::Tag() const {
    if (!m_isValid)
        throw InvalidNode();
    return m_pNode ? m_pNode->tag() : detail::node_data::empty_scalar;
}
}
