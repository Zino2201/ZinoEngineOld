#pragma once

#include <memory>
#include <boost/smart_ptr/intrusive_ptr.hpp>
#include <boost/smart_ptr/intrusive_ref_counter.hpp>

/**
 * Declare a raw pointer that own its resource
 */
template<typename T>
using TOwnerPtr = T*;