#pragma once

#include "access.h"

#include <ryml.hpp>
#include <unordered_map>

namespace serialization
{
template<typename T, typename U, typename TContext>
void save(std::unordered_map<T, U>& data, const Serializer<TContext>& ser)
{
  ser.tag("map");
  ser.node |= ryml::SEQ;
  for(auto& [key, value] : data)
  {
    const auto tmp = ser.newChild();
    tmp(S_NV("key", const_cast<T&>(key)), S_NV("value", value));
  }
}

template<typename T, typename U, typename TContext>
void load(std::unordered_map<T, U>& data, const Serializer<TContext>& ser)
{
  ser.tag("map");
  data = std::unordered_map<T, U>();
  for(const auto& element : ser.node.children())
  {
    gsl_Assert(element.is_map());
    gsl_Assert(element.num_children() == 2);
    auto elemSer = ser.withNode(element);
    data.emplace(access<T>::callCreate(elemSer["key"]), access<U>::callCreate(elemSer["value"]));
  }
}
} // namespace serialization
