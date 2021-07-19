
#ifndef DELEGATE_H
#define DELEGATE_H
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <algorithm>
#include <utility>
#include <cstring>
#include <cassert>

namespace delegates
{
    namespace detail
    {
        struct DefaultVoid;
    }

    template <
        class ReturnT = detail::DefaultVoid,
        class Param1T = detail::DefaultVoid,
        class Param2T = detail::DefaultVoid,
        class Param3T = detail::DefaultVoid,
        class Param4T = detail::DefaultVoid,
        class Param5T = detail::DefaultVoid,
        class Param6T = detail::DefaultVoid,
        class Param7T = detail::DefaultVoid,
        class Param8T = detail::DefaultVoid,
        class ParamPamPam = detail::DefaultVoid
    >
    class delegate;
}

#define DELEGATE_PARAM_COUNT 0
#define DELEGATE_TEMPLATE_PARAMS
#define DELEGATE_TEMPLATE_ARGS
#define DELEGATE_PARAMS
#define DELEGATE_ARGS

    #include "delegate.impl.hpp"

#undef DELEGATE_PARAM_COUNT
#undef DELEGATE_TEMPLATE_PARAMS
#undef DELEGATE_TEMPLATE_ARGS
#undef DELEGATE_PARAMS
#undef DELEGATE_ARGS

#define DELEGATE_PARAM_COUNT 1
#define DELEGATE_TEMPLATE_PARAMS class Param1T
#define DELEGATE_TEMPLATE_ARGS Param1T
#define DELEGATE_PARAMS Param1T a1
#define DELEGATE_ARGS a1

    #include "delegate.impl.hpp"

#undef DELEGATE_PARAM_COUNT
#undef DELEGATE_TEMPLATE_PARAMS
#undef DELEGATE_TEMPLATE_ARGS
#undef DELEGATE_PARAMS
#undef DELEGATE_ARGS

#define DELEGATE_PARAM_COUNT 2
#define DELEGATE_TEMPLATE_PARAMS class Param1T, class Param2T
#define DELEGATE_TEMPLATE_ARGS Param1T, Param2T
#define DELEGATE_PARAMS Param1T a1, Param2T a2
#define DELEGATE_ARGS a1, a2

    #include "delegate.impl.hpp"

#undef DELEGATE_PARAM_COUNT
#undef DELEGATE_TEMPLATE_PARAMS
#undef DELEGATE_TEMPLATE_ARGS
#undef DELEGATE_PARAMS
#undef DELEGATE_ARGS

#define DELEGATE_PARAM_COUNT 3
#define DELEGATE_TEMPLATE_PARAMS class Param1T, class Param2T, class Param3T
#define DELEGATE_TEMPLATE_ARGS Param1T, Param2T, Param3T
#define DELEGATE_PARAMS Param1T a1, Param2T a2, Param3T a3
#define DELEGATE_ARGS a1, a2, a3

    #include "delegate.impl.hpp"

#undef DELEGATE_PARAM_COUNT
#undef DELEGATE_TEMPLATE_PARAMS
#undef DELEGATE_TEMPLATE_ARGS
#undef DELEGATE_PARAMS
#undef DELEGATE_ARGS

#define DELEGATE_PARAM_COUNT 4
#define DELEGATE_TEMPLATE_PARAMS class Param1T, class Param2T, class Param3T, class Param4T
#define DELEGATE_TEMPLATE_ARGS Param1T, Param2T, Param3T, Param4T
#define DELEGATE_PARAMS Param1T a1, Param2T a2, Param3T a3, Param4T a4
#define DELEGATE_ARGS a1, a2, a3, a4

#include "delegate.impl.hpp"

#undef DELEGATE_PARAM_COUNT
#undef DELEGATE_TEMPLATE_PARAMS
#undef DELEGATE_TEMPLATE_ARGS
#undef DELEGATE_PARAMS
#undef DELEGATE_ARGS

#define DELEGATE_PARAM_COUNT 5
#define DELEGATE_TEMPLATE_PARAMS class Param1T, class Param2T, class Param3T, class Param4T, class Param5T
#define DELEGATE_TEMPLATE_ARGS Param1T, Param2T, Param3T, Param4T, Param5T
#define DELEGATE_PARAMS Param1T a1, Param2T a2, Param3T a3, Param4T a4, Param5T a5
#define DELEGATE_ARGS a1, a2, a3, a4, a5

#include "delegate.impl.hpp"

#undef DELEGATE_PARAM_COUNT
#undef DELEGATE_TEMPLATE_PARAMS
#undef DELEGATE_TEMPLATE_ARGS
#undef DELEGATE_PARAMS
#undef DELEGATE_ARGS

#define DELEGATE_PARAM_COUNT 6
#define DELEGATE_TEMPLATE_PARAMS class Param1T, class Param2T, class Param3T, class Param4T, class Param5T, class Param6T
#define DELEGATE_TEMPLATE_ARGS Param1T, Param2T, Param3T, Param4T, Param5T, Param6T
#define DELEGATE_PARAMS Param1T a1, Param2T a2, Param3T a3, Param4T a4, Param5T a5, Param6T a6
#define DELEGATE_ARGS a1, a2, a3, a4, a5, a6

#include "delegate.impl.hpp"

#undef DELEGATE_PARAM_COUNT
#undef DELEGATE_TEMPLATE_PARAMS
#undef DELEGATE_TEMPLATE_ARGS
#undef DELEGATE_PARAMS
#undef DELEGATE_ARGS

#define DELEGATE_PARAM_COUNT 7
#define DELEGATE_TEMPLATE_PARAMS class Param1T, class Param2T, class Param3T, class Param4T, class Param5T, class Param6T, class Param7T
#define DELEGATE_TEMPLATE_ARGS Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, Param7T
#define DELEGATE_PARAMS Param1T a1, Param2T a2, Param3T a3, Param4T a4, Param5T a5, Param6T a6, Param7T a7
#define DELEGATE_ARGS a1, a2, a3, a4, a5, a6, a7

#include "delegate.impl.hpp"

#undef DELEGATE_PARAM_COUNT
#undef DELEGATE_TEMPLATE_PARAMS
#undef DELEGATE_TEMPLATE_ARGS
#undef DELEGATE_PARAMS
#undef DELEGATE_ARGS

#define DELEGATE_PARAM_COUNT 8
#define DELEGATE_TEMPLATE_PARAMS class Param1T, class Param2T, class Param3T, class Param4T, class Param5T, class Param6T, class Param7T, class Param8T
#define DELEGATE_TEMPLATE_ARGS Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, Param7T, Param8T
#define DELEGATE_PARAMS Param1T a1, Param2T a2, Param3T a3, Param4T a4, Param5T a5, Param6T a6, Param7T a7, Param8T a8
#define DELEGATE_ARGS a1, a2, a3, a4, a5, a6, a7, a8

#include "delegate.impl.hpp"

#undef DELEGATE_PARAM_COUNT
#undef DELEGATE_TEMPLATE_PARAMS
#undef DELEGATE_TEMPLATE_ARGS
#undef DELEGATE_PARAMS
#undef DELEGATE_ARGS


#endif // DELEGATE_H
