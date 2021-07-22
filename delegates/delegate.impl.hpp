#if !defined(DELEGATE_PARAM_COUNT)
    || defined(DELEGATE_TEMPLATE_PARAMS)
    || defined(DELEGATE_TEMPLATE_ARGS)
    || defined(DELEGATE_PARAMS)
    || defined(DELEGATE_ARGS)

#error "delegate not declared"
#endif

#if DELEGATE_PARAM_COUNT > 0
    #define DELEGATE_COMMA ,
#else
    #define DELEGATE_COMMA
#endif

namespace delegates
{
    template<class ReturnT DELEGATE_COMMA DELEGATE_TEMPLATE_PARAMS>
    class delegate<ReturnT DELEGATE_COMMA DELEGATE_TEMPLATE_ARGS>
    {
        typedef const void* pthis_type;
        typedef ReturnT(detail::virtual_class_child::*member_func_type)(DELEGATE_TEMPLATE_ARGS);
        typedef ReturnT(*free_func_type)(DELEGATE_TEMPLATE_ARGS);
        typedef ReturnT(*free_func_like_member_type)(pthis_type DELEGATE_COMMA DELEGATE_TEMPLATE_ARGS);
        typedef ReturnT(*caller_type)(const delegate& DELEGATE_COMMA DELEGATE_TEMPLATE_ARGS);

        struct member_func_call
        {
            pthis_type pthis_;
            unsigned char union_[
                (sizeof(free_func_like_member_type) > sizeof(member_func_type) ?
                    sizeof(free_func_like_member_type) : sizeof(member_func_type)) * 2];
        };

        template<class Y>
        inline
        caller_type get_caller(Y*, ReturnT(*)(Y* DELEGATE_COMMA DELEGATE_TEMPLATE_ARGS)) const
        { 
            typedef ReturnT(*type)(Y* DELEGATE_COMMA DELEGATE_TEMPLATE_ARGS);
            return &delegate::function_caller<Y, type>;
        }

        template<class Y>
        inline
        caller_type get_caller(const Y*, ReturnT(*)(const Y* DELEGATE_COMMA DELEGATE_TEMPLATE_ARGS)) const
        { 
            typedef ReturnT(*type)(const Y * DELEGATE_COMMA DELEGATE_TEMPLATE_ARGS)
            return &delegate::function_caller_const<Y, type>;
        }

        template<class X, class Y>
        inline
        caller_type get_caller(X*, ReturnT(Y::*)(DELEGATE_TEMPLATE_ARGS)) const
        { 
            typedef ReturnT(Y::*type)(DELEGATE_TEMPLATE_ARGS);
            return &delegate::mfunction_caller<X, type>;
        }

        template<class X, class Y>
        inline
        caller_type get_caller(const X*, ReturnT(Y::*)(DELEGATE_TEMPLATE_ARGS) const) const
        { 
            typedef ReturnT(Y::* type)(DELEGATE_TEMPLATE_ARGS) const;
            return &delegate::mfunction_caller_const<X, type>;
        }

        inline
        caller_type get_caller(ReturnT(*)(DELEGATE_TEMPLATE_ARGS)) const
        { return &delegate::simple_function_caller; }

    public:
        typedef delegate type;

        delegate() 
            : caller_(NULL)
        { 
            std::memset(&union_[0], 0, sizeof(union_));
        }

        template < class X, class Y >
        delegate(Y * pthis,
            ReturnT(X::* function_to_bind)(DELEGATE_TEMPLATE_ARGS))
            : caller_(get_caller(pthis, function_to_bind))
        { 

            std::memset(&union_[0], 0, sizeof(union_));

            typename
            detail::disable_if_not_convertable
            <member_func_call, Y*, X*>::type f_call;

            f_call.pthis_ = pthis;
            std::memset(&f_call.union_[0], 0, sizeof(f_call.union_));
            std::memcpy(&f_call.union_[0], &function_to_bind, sizeof(function_to_bind));
            
            std::memcpy(&union_[0], &f_call, sizeof(f_call));

            assert(NULL != pthis);
            assert(NULL != function_to_bind);
        }

        template < class X, class Y >
        delegate(const Y *pthis,
            ReturnT(X::* function_to_bind)(DELEGATE_TEMPLATE_ARGS) const)
            : caller_(get_caller(pthis, function_to_bind))
        { 
            std::memset(&union_[0], 0, sizeof(union_));

            typename
            detail::disable_if_not_convertable
            <member_func_call, Y*, X*>::type f_call;

            f_call.pthis_ = pthis;
            std::memset(&f_call.union_[0], 0, sizeof(f_call.union_));
            std::memcpy(&f_call.union_[0], &function_to_bind, sizeof(function_to_bind));

            std::memcpy(&union_[0], &f_call, sizeof(f_call));

            assert(NULL != pthis);
            assert(NULL != function_to_bind);
        }

        template < class Y >
        delegate(Y *pthis,
            ReturnT(*function_to_bind)(Y* DELEGATE_COMMA DELEGATE_TEMPLATE_ARGS))
            : caller_(get_caller(pthis, function_to_bind))
        {
            std::memset(&union_[0], 0, sizeof(union_));

            member_func_call f_call;

            f_call.pthis_ = pthis;
            std::memset(&f_call.union_[0], 0, sizeof(f_call.union_));
            std::memcpy(&f_call.union_[0], &function_to_bind, sizeof(function_to_bind));

            std::memcpy(&union_[0], &f_call, sizeof(f_call));

            assert(NULL != function_to_bind);
        }

        template < class Y >
        delegate(Y *pthis,
            ReturnT(*function_to_bind)(const Y* DELEGATE_COMMA DELEGATE_TEMPLATE_ARGS))
            : caller_(get_caller(pthis, function_to_bind))
        {
            std::memset(&union_[0], 0, sizeof(union_));

            member_func_call f_call;
            f_call.pthis_ = pthis;
            std::memset(&f_call.union_[0], 0, sizeof(f_call.union_));
            std::memcpy(&f_call.union_[0], &function_to_bind, sizeof(function_to_bind));

            std::memcpy(&union_[0], &f_call, sizeof(f_call));

            assert(NULL != function_to_bind);
        }

        template < class Y >
        delegate(const Y *pthis,
            ReturnT(*function_to_bind)(const Y* DELEGATE_COMMA DELEGATE_TEMPLATE_ARGS))
            : caller_(get_caller(pthis, function_to_bind))
        {
            std::memset(&union_[0], 0, sizeof(union_));

            member_func_call f_call;
            f_call.pthis_ = pthis;
            std::memset(&f_call.union_[0], 0, sizeof(f_call.union_));
            std::memcpy(&f_call.union_[0], &function_to_bind, sizeof(function_to_bind));

            std::memcpy(&union_[0], &f_call, sizeof(f_call));

            assert(NULL != function_to_bind);
        }


        delegate(free_func_type function_to_bind)
            : caller_(get_caller(function_to_bind))
        {
            std::memset(&union_[0], 0, sizeof(union_));

            std::memcpy(&union_[0], &function_to_bind, sizeof(function_to_bind));

            assert(NULL != function_to_bind);
        }

        void swap(delegate &other)
        {
            if (this == &other)
                return;

            if (caller_ || other.caller_)
            {
                // swap for caller
                {
                    caller_type tmp = caller_;
                    caller_ = other.caller_;
                    other.caller_ = tmp;
                }
                // swap for arrays
                {
                    unsigned char tmp[sizeof(union_)];
                    std::memcpy(&tmp[0], &union_[0], sizeof(union_));
                    std::memcpy(&union_[0], &other.union_[0], sizeof(union_));
                    std::memcpy(&other.union_[0], &tmp[0], sizeof(union_));
                }
            }
            
        }

        delegate(const delegate &other)
        { 
            if (this == &other)
                return;

            if (caller_ || other.caller_)
            {
                caller_ = (other.caller_);
                std::memcpy(&union_[0], &other.union_[0], sizeof(union_));
            }
        }

        delegate& operator=(const delegate &other)
        {
            if (this != &other)
            {
                delegate tmp = other;
                swap(tmp);
            }

            return *this;
        }

        bool operator!() const
        {
            return caller_ == NULL;
        }

        bool operator==(const delegate &other) const 
        {
            if(caller_ != other.caller_)
                return false;
            if(NULL != caller_)
                return (0 == std::memcmp(&union_[0], &other.union_[0], sizeof(union_)));
            return true;
        }

        bool operator!=(const delegate &other) const 
        {
            return !(*this == other);
        }

        bool operator<(const delegate &other) const 
        {
            if(caller_ != other.caller_)
                return std::memcmp(&caller_, &other.caller_, sizeof(caller_)) < 0;
            if (NULL != caller_)
                return (0 < std::memcmp(&union_[0], &other.union_[0], sizeof(union_)));
            return false;
        }

        bool operator>(const delegate &other) const 
        {
            if (caller_ != other.caller_)
                return std::memcmp(&caller_, &other.caller_, sizeof(caller_)) < 0;
            if (NULL != caller_)
                return (0 > std::memcmp(&union_[0], &other.union_[0], sizeof(union_)));
            return false;
        }

        ReturnT operator()(DELEGATE_PARAMS) const
        {
            return caller_(*this DELEGATE_COMMA DELEGATE_ARGS);
        }

        template < class Y >
        inline void bind(Y *pthis, ReturnT(*function_to_bind)(Y* DELEGATE_COMMA DELEGATE_TEMPLATE_ARGS)) {
            delegate tmp(pthis, function_to_bind);
            swap(tmp);
        }

        template < class Y >
        inline void bind(Y *pthis, ReturnT(*function_to_bind)(const Y* DELEGATE_COMMA DELEGATE_TEMPLATE_ARGS)) {
            delegate tmp(pthis, function_to_bind);
            swap(tmp);
        }

        template < class Y >
        inline void bind(const Y *pthis, ReturnT(*function_to_bind)(const Y* DELEGATE_COMMA DELEGATE_TEMPLATE_ARGS)) {
            delegate tmp(pthis, function_to_bind);
            swap(tmp);
        }

        template < class X, class Y >
        inline void bind(Y *pthis, ReturnT(X::* function_to_bind)(DELEGATE_TEMPLATE_ARGS)) {
            delegate tmp(pthis, function_to_bind);
            swap(tmp);
        }

        template < class X, class Y >
        inline void bind(const Y *pthis, ReturnT(X::* function_to_bind)(DELEGATE_TEMPLATE_ARGS) const) {
            delegate tmp(pthis, function_to_bind);
            swap(tmp);
        }

        inline void bind(ReturnT(*function_to_bind)(DELEGATE_TEMPLATE_ARGS)) {
            delegate tmp(function_to_bind);
            swap(tmp);
        }

    private:

        // caller function to proper cast "union_" and call function stored in it
        caller_type caller_;

        // C++ union imitation
        // there we store free function like 'return_type free_function(arguments_type)'
        // or 
        // member_func_call struct with:
        //   member function like 'return_type some_class_type::member_function(arguments_type)'
        //   or 
        //   free function accepting class like 'return_type free_function(some_class_type*, arguments_type)'
        unsigned char union_[
            sizeof(member_func_call) > sizeof(free_func_type) ? 
                sizeof(member_func_call) : sizeof(free_func_type)];

        static ReturnT simple_function_caller(const delegate &that DELEGATE_COMMA DELEGATE_PARAMS)
        { 
            free_func_type func;

            std::memcpy(&func, &that.union_[0], sizeof(func));

            return func(DELEGATE_ARGS);
        }

        template< class Y, class real_free_func_like_member_type >
        static ReturnT function_caller(const delegate &that DELEGATE_COMMA DELEGATE_PARAMS)
        { 
            member_func_call call;

            std::memcpy(&call, &that.union_[0], sizeof(call));

            real_free_func_like_member_type func;
            Y* pthis = static_cast<Y*>(const_cast<void*>(call.pthis_));

            std::memcpy(&func, &call.union_[0], sizeof(func));

            return 
                func
                (pthis DELEGATE_COMMA DELEGATE_ARGS);
        }

        template< class Y, class real_free_func_like_member_type >
        static ReturnT function_caller_const(const delegate &that DELEGATE_COMMA DELEGATE_PARAMS) 
        {
            member_func_call call;

            std::memcpy(&call, &that.union_[0], sizeof(call));

            real_free_func_like_member_type func;
            const Y* pthis = static_cast<const Y*>(const_cast<const void*>(call.pthis_));

            std::memcpy(&func, &call.union_[0], sizeof(func));

            return 
                func
                (pthis DELEGATE_COMMA DELEGATE_ARGS);
        }

        template< class Y, class real_member_func_type >
        static ReturnT mfunction_caller(const delegate &that DELEGATE_COMMA DELEGATE_PARAMS) 
        { 
            member_func_call call;

            std::memcpy(&call, &that.union_[0], sizeof(call));

            real_member_func_type func;
            Y* pthis = static_cast<Y*>(const_cast<void*>(call.pthis_));

            std::memcpy(&func, &call.union_[0], sizeof(func));

            return 
                (pthis->*func)
                (DELEGATE_ARGS);
        }

        template< class Y, class real_member_func_type >
        static ReturnT mfunction_caller_const(const delegate &that DELEGATE_COMMA DELEGATE_PARAMS) 
        {
            member_func_call call;

            std::memcpy(&call, &that.union_[0], sizeof(call));

            real_member_func_type func;
            const Y* pthis = static_cast<const Y*>(const_cast<const void*>(call.pthis_));

            std::memcpy(&func, &call.union_[0], sizeof(func));

            return 
                (pthis->*func)
                (DELEGATE_ARGS);
        }
    };

    template < class X, class Y, class ReturnT DELEGATE_COMMA DELEGATE_TEMPLATE_PARAMS>
    delegate<ReturnT DELEGATE_COMMA DELEGATE_TEMPLATE_ARGS>
        bind(X *pthis,
            ReturnT(Y::* function_to_bind)(DELEGATE_TEMPLATE_ARGS))
    {
        return delegate<ReturnT DELEGATE_COMMA DELEGATE_TEMPLATE_ARGS>(pthis, function_to_bind);
    }

    template < class X, class Y, class ReturnT DELEGATE_COMMA DELEGATE_TEMPLATE_PARAMS>
    delegate<ReturnT DELEGATE_COMMA DELEGATE_TEMPLATE_ARGS>
        bind(const X *pthis,
            ReturnT(Y::* function_to_bind)(DELEGATE_TEMPLATE_ARGS) const)
    {
        return delegate<ReturnT DELEGATE_COMMA DELEGATE_TEMPLATE_ARGS>(pthis, function_to_bind);
    }

    template < class X, class ReturnT DELEGATE_COMMA DELEGATE_TEMPLATE_PARAMS>
    delegate<ReturnT DELEGATE_COMMA DELEGATE_TEMPLATE_ARGS>
        bind(X *pthis,
            ReturnT(*function_to_bind)(X* DELEGATE_COMMA DELEGATE_TEMPLATE_ARGS))
    {
        return delegate<ReturnT DELEGATE_COMMA DELEGATE_TEMPLATE_ARGS>(pthis, function_to_bind);
    }

    template < class X, class ReturnT DELEGATE_COMMA DELEGATE_TEMPLATE_PARAMS>
    delegate<ReturnT DELEGATE_COMMA DELEGATE_TEMPLATE_ARGS>
        bind(X *pthis,
            ReturnT(*function_to_bind)(const X* DELEGATE_COMMA DELEGATE_TEMPLATE_ARGS))
    {
        return delegate<ReturnT>(pthis, function_to_bind);
    }

    template < class ReturnT DELEGATE_COMMA DELEGATE_TEMPLATE_PARAMS>
    delegate<ReturnT DELEGATE_COMMA DELEGATE_TEMPLATE_ARGS>
        bind(ReturnT(*function_to_bind)(DELEGATE_TEMPLATE_ARGS))
    {
        return delegate<ReturnT DELEGATE_COMMA DELEGATE_TEMPLATE_ARGS>(function_to_bind);
    }

}

#undef DELEGATE_COMMA
