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

        typedef detail::comparison_type comparison_type;
        typedef bool(*comparator_type)(const delegate&, const delegate&, comparison_type);

        struct member_func_call
        {
            unsigned char union_[
                (sizeof(free_func_like_member_type) > sizeof(member_func_type) ?
                    sizeof(free_func_like_member_type) : sizeof(member_func_type))];
            pthis_type pthis_;
        };

        inline
        static ReturnT simple_function_caller(const delegate &that DELEGATE_COMMA DELEGATE_PARAMS)
        { 
            free_func_type func;

            std::memcpy(&func, that.union_, sizeof(func));

            return func(DELEGATE_ARGS);
        }

        template< class Y, class real_free_func_like_member_type >
        inline
        static ReturnT function_caller(const delegate &that DELEGATE_COMMA DELEGATE_PARAMS)
        { 
            member_func_call call;

            std::memcpy(&call, that.union_, sizeof(call));

            real_free_func_like_member_type func;
            Y* pthis = static_cast<Y*>(const_cast<void*>(call.pthis_));

            std::memcpy(&func, call.union_, sizeof(func));

            return 
                func
                (pthis DELEGATE_COMMA DELEGATE_ARGS);
        }

        template< class Y, class real_free_func_like_member_type >
        inline
        static ReturnT function_caller_const(const delegate &that DELEGATE_COMMA DELEGATE_PARAMS) 
        {
            member_func_call call;

            std::memcpy(&call, that.union_, sizeof(call));

            real_free_func_like_member_type func;
            const Y* pthis = static_cast<const Y*>(const_cast<const void*>(call.pthis_));

            std::memcpy(&func, call.union_, sizeof(func));

            return 
                func
                (pthis DELEGATE_COMMA DELEGATE_ARGS);
        }

        template< class Y, class real_member_func_type >
        inline
        static ReturnT mfunction_caller(const delegate &that DELEGATE_COMMA DELEGATE_PARAMS) 
        { 
            member_func_call call;

            std::memcpy(&call, that.union_, sizeof(call));

            real_member_func_type func;
            Y* pthis = static_cast<Y*>(const_cast<void*>(call.pthis_));

            std::memcpy(&func, call.union_, sizeof(func));

            return 
                (pthis->*func)
                (DELEGATE_ARGS);
        }

        template< class Y, class real_member_func_type >
        inline
        static ReturnT mfunction_caller_const(const delegate &that DELEGATE_COMMA DELEGATE_PARAMS) 
        {
            member_func_call call;

            std::memcpy(&call, that.union_, sizeof(call));

            real_member_func_type func;
            const Y* pthis = static_cast<const Y*>(const_cast<const void*>(call.pthis_));

            std::memcpy(&func, call.union_, sizeof(func));

            return 
                (pthis->*func)
                (DELEGATE_ARGS);
        }

        template<class Y>
        inline
        caller_type get_caller(Y*, ReturnT(*)(Y* DELEGATE_COMMA DELEGATE_TEMPLATE_ARGS)) const
        { 
            typedef ReturnT(*func_type)(Y* DELEGATE_COMMA DELEGATE_TEMPLATE_ARGS);
            return &function_caller<Y, func_type>;
        }

        template<class Y>
        inline
        caller_type get_caller(const Y*, ReturnT(*)(const Y* DELEGATE_COMMA DELEGATE_TEMPLATE_ARGS)) const
        { 
            typedef ReturnT(*func_type)(const Y * DELEGATE_COMMA DELEGATE_TEMPLATE_ARGS);
            return &function_caller_const<Y, func_type>;
        }

        template<class X, class Y>
        inline
        caller_type get_caller(X*, ReturnT(Y::*)(DELEGATE_TEMPLATE_ARGS)) const
        { 
            typedef ReturnT(Y::*func_type)(DELEGATE_TEMPLATE_ARGS);
            return &mfunction_caller<X, func_type>;
        }

        template<class X, class Y>
        inline
        caller_type get_caller(const X*, ReturnT(Y::*)(DELEGATE_TEMPLATE_ARGS) const) const
        { 
            typedef ReturnT(Y::* func_type)(DELEGATE_TEMPLATE_ARGS) const;
            return &mfunction_caller_const<X, func_type>;
        }

        inline
        caller_type get_caller(ReturnT(*)(DELEGATE_TEMPLATE_ARGS)) const
        { return &simple_function_caller; }

        inline
        static bool simple_function_comparator(const delegate& lhs, const delegate& rhs, comparison_type op)
        { 
            free_func_type lhs_func, rhs_func;

            std::memcpy(&lhs_func, lhs.union_, sizeof(lhs_func));
            std::memcpy(&rhs_func, rhs.union_, sizeof(rhs_func));

            return detail::compare(lhs_func, rhs_func, op);
        }

        template< class Y, class real_free_func_like_member_type >
        inline
        static bool function_comparator(const delegate& lhs, const delegate& rhs, comparison_type op)
        {
            member_func_call lhs_call, rhs_call;

            std::memcpy(&lhs_call, lhs.union_, sizeof(lhs_call));
            std::memcpy(&rhs_call, rhs.union_, sizeof(rhs_call));

            switch (op)
            {
            case detail::less:
                if (detail::compare(lhs_call.pthis_, rhs_call.pthis_, detail::less))
                {
                    return true;
                }
                else if (!detail::compare(rhs_call.pthis_, lhs_call.pthis_, detail::less))
                {
                    real_free_func_like_member_type lhs_func, rhs_func;
                    std::memcpy(&lhs_func, lhs_call.union_, sizeof(lhs_func));
                    std::memcpy(&rhs_func, rhs_call.union_, sizeof(rhs_func));
                    return
                        detail::compare(lhs_func, rhs_func, detail::less);
                }break;
            case detail::greater:
                if (detail::compare(rhs_call.pthis_, lhs_call.pthis_, detail::less))
                {
                    return true;
                }
                else if (!detail::compare(lhs_call.pthis_, rhs_call.pthis_, detail::less))
                {
                    real_free_func_like_member_type lhs_func, rhs_func;
                    std::memcpy(&lhs_func, lhs_call.union_, sizeof(lhs_func));
                    std::memcpy(&rhs_func, rhs_call.union_, sizeof(rhs_func));
                    return
                        detail::compare(rhs_func, lhs_func, detail::less);
                }break;
            case detail::equal:
                if (detail::compare(rhs_call.pthis_, lhs_call.pthis_, detail::equal))
                {
                    real_free_func_like_member_type lhs_func, rhs_func;
                    std::memcpy(&lhs_func, lhs_call.union_, sizeof(lhs_func));
                    std::memcpy(&rhs_func, rhs_call.union_, sizeof(rhs_func));
                    return
                        detail::compare(lhs_func, rhs_func, detail::equal);
                }break;
            }

            return false;
        }

        template< class Y, class real_free_func_like_member_type >
        inline
        static bool function_comparator_const(const delegate& lhs, const delegate& rhs, comparison_type op)
        {
            return function_comparator<Y, real_free_func_like_member_type>(lhs, rhs, op);
        }

        template< class Y, class real_member_func_type >
        inline
        static bool mfunction_comparator(const delegate& lhs, const delegate& rhs, comparison_type op)
        {
            member_func_call lhs_call, rhs_call;

            std::memcpy(&lhs_call, lhs.union_, sizeof(lhs_call));
            std::memcpy(&rhs_call, rhs.union_, sizeof(rhs_call));

            switch (op)
            {
            case detail::less: 
                return detail::compare(lhs_call.pthis_, rhs_call.pthis_, detail::less) ||
                    (!detail::compare(rhs_call.pthis_, lhs_call.pthis_, detail::less) &&
                    std::memcmp(lhs_call.union_, rhs_call.union_, sizeof(lhs_call.union_) < 0)); break;
            case detail::greater:
                return detail::compare(rhs_call.pthis_, lhs_call.pthis_, detail::less) ||
                    (!detail::compare(lhs_call.pthis_, rhs_call.pthis_, detail::less) &&
                        std::memcmp(rhs_call.union_, lhs_call.union_, sizeof(lhs_call.union_) < 0)); break;
            case detail::equal: 
                return detail::compare(lhs_call.pthis_, rhs_call.pthis_, detail::equal) &&
                    (0 == std::memcmp(lhs_call.union_, rhs_call.union_, sizeof(lhs_call.union_))); break;
            }
            return false;
        }

        template< class Y, class real_member_func_type >
        inline
        static bool mfunction_comparator_const(const delegate& lhs, const delegate& rhs, comparison_type op)
        {
            return mfunction_comparator<Y, real_member_func_type>(lhs, rhs, op);
        }

        template<class Y>
        inline
        comparator_type get_comparator(Y*, ReturnT(*)(Y* DELEGATE_COMMA DELEGATE_TEMPLATE_ARGS)) const
        { 
            typedef ReturnT(*func_type)(Y* DELEGATE_COMMA DELEGATE_TEMPLATE_ARGS);
            return &function_comparator<Y, func_type>;
        }
        
        template<class Y>
        inline
        comparator_type get_comparator(const Y*, ReturnT(*)(const Y* DELEGATE_COMMA DELEGATE_TEMPLATE_ARGS)) const
        { 
            typedef ReturnT(*func_type)(const Y * DELEGATE_COMMA DELEGATE_TEMPLATE_ARGS);
            return &function_comparator_const<Y, func_type>;
        }
        
        template<class X, class Y>
        inline
        comparator_type get_comparator(X*, ReturnT(Y::*)(DELEGATE_TEMPLATE_ARGS)) const
        { 
            typedef ReturnT(Y::*func_type)(DELEGATE_TEMPLATE_ARGS);
            return &mfunction_comparator<X, func_type>;
        }
        
        template<class X, class Y>
        inline
        comparator_type get_comparator(const X*, ReturnT(Y::*)(DELEGATE_TEMPLATE_ARGS) const) const
        { 
            typedef ReturnT(Y::* func_type)(DELEGATE_TEMPLATE_ARGS) const;
            return &mfunction_comparator_const<X, func_type>;
        }

        inline
        comparator_type get_comparator(ReturnT(*)(DELEGATE_TEMPLATE_ARGS)) const
        { return &simple_function_comparator; }

    public:
        typedef delegate type;

        delegate() 
            : caller_(NULL)
            , comparator_(NULL)
        { 
            std::memset(union_, 0, sizeof(union_));
        }

        template < class X, class Y >
        delegate(Y * pthis,
            ReturnT(X::* function_to_bind)(DELEGATE_TEMPLATE_ARGS))
            : caller_(get_caller(pthis, function_to_bind))
            , comparator_(get_comparator(pthis, function_to_bind))
        { 

            std::memset(union_, 0, sizeof(union_));

            typename
            detail::disable_if_not_convertable
            <member_func_call, Y*, X*>::type f_call;
            
            std::memset(&f_call, 0, sizeof(f_call));
            f_call.pthis_ = pthis;
            std::memcpy(f_call.union_, &function_to_bind, sizeof(function_to_bind));
            
            std::memcpy(union_, &f_call, sizeof(f_call));

            assert(NULL != pthis);
            assert(NULL != function_to_bind);
        }

        template < class X, class Y >
        delegate(const Y *pthis,
            ReturnT(X::* function_to_bind)(DELEGATE_TEMPLATE_ARGS) const)
            : caller_(get_caller(pthis, function_to_bind))
            , comparator_(get_comparator(pthis, function_to_bind))
        { 
            std::memset(union_, 0, sizeof(union_));

            typename
            detail::disable_if_not_convertable
            <member_func_call, Y*, X*>::type f_call;
            
            std::memset(&f_call, 0, sizeof(f_call));
            f_call.pthis_ = pthis;
            std::memcpy(f_call.union_, &function_to_bind, sizeof(function_to_bind));

            std::memcpy(union_, &f_call, sizeof(f_call));

            assert(NULL != pthis);
            assert(NULL != function_to_bind);
        }

        template < class Y >
        delegate(Y *pthis,
            ReturnT(*function_to_bind)(Y* DELEGATE_COMMA DELEGATE_TEMPLATE_ARGS))
            : caller_(get_caller(pthis, function_to_bind))
            , comparator_(get_comparator(pthis, function_to_bind))
        {
            std::memset(union_, 0, sizeof(union_));

            member_func_call f_call;
            
            std::memset(&f_call, 0, sizeof(f_call));
            f_call.pthis_ = pthis;
            std::memcpy(f_call.union_, &function_to_bind, sizeof(function_to_bind));

            std::memcpy(union_, &f_call, sizeof(f_call));

            assert(NULL != function_to_bind);
        }

        template < class Y >
        delegate(Y *pthis,
            ReturnT(*function_to_bind)(const Y* DELEGATE_COMMA DELEGATE_TEMPLATE_ARGS))
            : caller_(get_caller(pthis, function_to_bind))
            , comparator_(get_comparator(pthis, function_to_bind))
        {
            std::memset(union_, 0, sizeof(union_));

            member_func_call f_call;
            
            std::memset(&f_call, 0, sizeof(f_call));
            f_call.pthis_ = pthis;
            std::memcpy(f_call.union_, &function_to_bind, sizeof(function_to_bind));

            std::memcpy(union_, &f_call, sizeof(f_call));

            assert(NULL != function_to_bind);
        }

        template < class Y >
        delegate(const Y *pthis,
            ReturnT(*function_to_bind)(const Y* DELEGATE_COMMA DELEGATE_TEMPLATE_ARGS))
            : caller_(get_caller(pthis, function_to_bind))
            , comparator_(get_comparator(pthis, function_to_bind))
        {
            std::memset(union_, 0, sizeof(union_));

            member_func_call f_call;
            
            std::memset(&f_call, 0, sizeof(f_call));
            f_call.pthis_ = pthis;
            std::memcpy(f_call.union_, &function_to_bind, sizeof(function_to_bind));

            std::memcpy(union_, &f_call, sizeof(f_call));

            assert(NULL != function_to_bind);
        }


        delegate(free_func_type function_to_bind)
            : caller_(get_caller(function_to_bind))
            , comparator_(get_comparator(function_to_bind))
        {
            std::memset(union_, 0, sizeof(union_));

            std::memcpy(union_, &function_to_bind, sizeof(function_to_bind));

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
                // swap for comparator
                {
                    comparator_type tmp = comparator_;
                    comparator_ = other.comparator_;
                    other.comparator_ = tmp;
                }
                // swap for arrays
                {
                    unsigned char tmp[sizeof(union_)];
                    std::memcpy(tmp, union_, sizeof(union_));
                    std::memcpy(union_, other.union_, sizeof(union_));
                    std::memcpy(other.union_, tmp, sizeof(union_));
                }
            }
            
        }

        friend void swap(delegate& lhs, delegate& rhs)
        {
            lhs.swap(rhs);
        }

        delegate(const delegate &other)
        { 
            if (other.caller_)
            {
                caller_ = (other.caller_);
                comparator_ = (other.comparator_);
                std::memcpy(union_, other.union_, sizeof(union_));
            }
            else
            {
                caller_ = NULL;
                comparator_ = NULL;
                std::memset(union_, 0, sizeof(union_));
            }
        }

        delegate& operator=(const delegate &other)
        {
            if (this != &other)
            {
                type(other).swap(*this);
            }

            return *this;
        }

        bool operator!() const
        {
            return NULL == caller_;
        }

        bool operator==(const delegate &other) const 
        {
            if (!detail::compare_equal(caller_, other.caller_))
                return false;
            if (!detail::compare_equal(comparator_, other.comparator_))
                return false;
            if(NULL != comparator_)
                return comparator_(*this, other, detail::equal);
            return true;
        }

        bool operator!=(const delegate &other) const 
        {
            return !(*this == other);
        }

        bool operator<(const delegate &other) const 
        {
            if (!detail::compare_equal(caller_, other.caller_))
                return detail::compare_less(caller_, other.caller_);
            if (!detail::compare_equal(comparator_, other.comparator_))
                return detail::compare_less(comparator_, other.comparator_);
            if (NULL != comparator_)
                return comparator_(*this, other, detail::less);
            return false;
        }

        bool operator>(const delegate &other) const 
        {
            if (!detail::compare_equal(caller_, other.caller_))
                return detail::compare_greater(caller_, other.caller_);
            if (!detail::compare_equal(comparator_, other.comparator_))
                return detail::compare_greater(comparator_, other.comparator_);
            if (NULL != comparator_)
                return comparator_(*this, other, detail::greater);
            return false;
        }

        inline
        ReturnT operator()(DELEGATE_PARAMS) const
        {
            return caller_(*this DELEGATE_COMMA DELEGATE_ARGS);
        }
        
        pthis_type ptr() const
        {
            member_func_call call;

            std::memcpy(&call, union_, sizeof(call));

            return (call.pthis_);
        }

        template < class Y >
        inline void bind(Y *pthis, ReturnT(*function_to_bind)(Y* DELEGATE_COMMA DELEGATE_TEMPLATE_ARGS)) {
            type(pthis, function_to_bind).swap(*this);
        }

        template < class Y >
        inline void bind(Y *pthis, ReturnT(*function_to_bind)(const Y* DELEGATE_COMMA DELEGATE_TEMPLATE_ARGS)) {
            type(pthis, function_to_bind).swap(*this);
        }

        template < class Y >
        inline void bind(const Y *pthis, ReturnT(*function_to_bind)(const Y* DELEGATE_COMMA DELEGATE_TEMPLATE_ARGS)) {
            type(pthis, function_to_bind).swap(*this);
        }

        template < class X, class Y >
        inline void bind(Y *pthis, ReturnT(X::* function_to_bind)(DELEGATE_TEMPLATE_ARGS)) {
            type(pthis, function_to_bind).swap(*this);
        }

        template < class X, class Y >
        inline void bind(const Y *pthis, ReturnT(X::* function_to_bind)(DELEGATE_TEMPLATE_ARGS) const) {
            type(pthis, function_to_bind).swap(*this);
        }

        inline void bind(ReturnT(*function_to_bind)(DELEGATE_TEMPLATE_ARGS)) {
            type(function_to_bind).swap(*this);
        }

    private:

        // caller function to proper cast "union_" and call function stored in it
        caller_type caller_;

        // comparator function to proper cast "union_" and call comparison function
        comparator_type comparator_;

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
    };
    
    template<DELEGATE_TEMPLATE_PARAMS>
    class delegate<detail::DefaultVoid DELEGATE_COMMA DELEGATE_TEMPLATE_ARGS>
        : delegate<void DELEGATE_COMMA DELEGATE_TEMPLATE_ARGS> {};

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
