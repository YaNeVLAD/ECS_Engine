#pragma once

#include "SimpleType.h"

using Float = details::SimpleType<std::float_t>;

using Int = details::SimpleType<std::int32_t>;

using Long = details::SimpleType<std::uint64_t>;

using Boolean = details::SimpleType<bool>;
