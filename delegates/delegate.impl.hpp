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

        enum comparison_type {
            less, equal, greater
        } ;

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

            std::memcpy(&func, &that.union_[0], sizeof(func));

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

        template<class T>
        inline 
        static bool compare(const T &lhs, const T &rhs, comparison_type op)
        {
            switch (op)
            {
            case less: return std::less<T>()(lhs, rhs);
            case greater: return std::greater<T>()(lhs, rhs);
            case equal: return (lhs == rhs);
            }
            return false;
        }

        inline
        static bool simple_function_comparator(const delegate& lhs, const delegate& rhs, comparison_type op)
        { 
            free_func_type lhs_func, rhs_func;

            std::memcpy(&lhs_func, &lhs.union_[0], sizeof(lhs_func));
            std::memcpy(&rhs_func, &rhs.union_[0], sizeof(rhs_func));

            return compare(&lhs_func, &rhs_func, op);
        }

        template< class Y, class real_free_func_like_member_type >
        inline
        static bool function_comparator(const delegate& lhs, const delegate& rhs, comparison_type op)
        {
            member_func_call lhs_call, rhs_call;

            std::memcpy(&lhs_call, lhs.union_, sizeof(lhs_call));
            std::memcpy(&rhs_call, rhs.union_, sizeof(rhs_call));

            if (!compare(lhs_call.pthis_, rhs_call.pthis_, op))
                return false;

            real_free_func_like_member_type lhs_func, rhs_func;
            std::memcpy(&lhs_func, lhs_call.union_, sizeof(lhs_func));
            std::memcpy(&rhs_func, rhs_call.union_, sizeof(rhs_func));

            return compare(lhs_func, rhs_func, op);
        }

        template< class Y, class real_member_func_type >
        inline
        static bool mfunction_comparator(const delegate& lhs, const delegate& rhs, comparison_type op)
        {
            member_func_call lhs_call, rhs_call;

            std::memcpy(&lhs_call, lhs.union_, sizeof(lhs_call));
            std::memcpy(&rhs_call, rhs.union_, sizeof(rhs_call));

            if (!compare(lhs_call.pthis_, rhs_call.pthis_, op))
                return false;

            const int result = 
                std::memcmp(lhs_call.union_, rhs_call.union_, sizeof(lhs_call.union_));

            switch (op)
            {
            case less: return (result < 0);
            case greater: return (result > 0);
            case equal: return (0 == result);
            }
            return false;
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
            return &function_comparator<Y, func_type>;
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
            return &mfunction_comparator<X, func_type>;
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
            std::memset(&union_[0], 0, sizeof(union_));
        }

        template < class X, class Y >
        delegate(Y * pthis,
            ReturnT(X::* function_to_bind)(DELEGATE_TEMPLATE_ARGS))
            : caller_(get_caller(pthis, function_to_bind))
            , comparator_(get_comparator(pthis, function_to_bind))
        { 

            std::memset(&union_[0], 0, sizeof(union_));

            typename
            detail::disable_if_not_convertable
            <member_func_call, Y*, X*>::type f_call;
            
            std::memset(&f_call, 0, sizeof(f_call));
            f_call.pthis_ = pthis;
            std::memcpy(&f_call.union_[0], &function_to_bind, sizeof(function_to_bind));
            
            std::memcpy(&union_[0], &f_call, sizeof(f_call));

            assert(NULL != pthis);
            assert(NULL != function_to_bind);
        }

        template < class X, class Y >
        delegate(const Y *pthis,
            ReturnT(X::* function_to_bind)(DELEGATE_TEMPLATE_ARGS) const)
            : caller_(get_caller(pthis, function_to_bind))
            , comparator_(get_comparator(pthis, function_to_bind))
        { 
            std::memset(&union_[0], 0, sizeof(union_));

            typename
            detail::disable_if_not_convertable
            <member_func_call, Y*, X*>::type f_call;
            
            std::memset(&f_call, 0, sizeof(f_call));
            f_call.pthis_ = pthis;
            std::memcpy(&f_call.union_[0], &function_to_bind, sizeof(function_to_bind));

            std::memcpy(&union_[0], &f_call, sizeof(f_call));

            assert(NULL != pthis);
            assert(NULL != function_to_bind);
        }

        template < class Y >
        delegate(Y *pthis,
            ReturnT(*function_to_bind)(Y* DELEGATE_COMMA DELEGATE_TEMPLATE_ARGS))
            : caller_(get_caller(pthis, function_to_bind))
            , comparator_(get_comparator(pthis, function_to_bind))
        {
            std::memset(&union_[0], 0, sizeof(union_));

            member_func_call f_call;
            
            std::memset(&f_call, 0, sizeof(f_call));
            f_call.pthis_ = pthis;
            std::memcpy(&f_call.union_[0], &function_to_bind, sizeof(function_to_bind));

            std::memcpy(&union_[0], &f_call, sizeof(f_call));

            assert(NULL != function_to_bind);
        }

        template < class Y >
        delegate(Y *pthis,
            ReturnT(*function_to_bind)(const Y* DELEGATE_COMMA DELEGATE_TEMPLATE_ARGS))
            : caller_(get_caller(pthis, function_to_bind))
            , comparator_(get_comparator(pthis, function_to_bind))
        {
            std::memset(&union_[0], 0, sizeof(union_));

            member_func_call f_call;
            
            std::memset(&f_call, 0, sizeof(f_call));
            f_call.pthis_ = pthis;
            std::memcpy(&f_call.union_[0], &function_to_bind, sizeof(function_to_bind));

            std::memcpy(&union_[0], &f_call, sizeof(f_call));

            assert(NULL != function_to_bind);
        }

        template < class Y >
        delegate(const Y *pthis,
            ReturnT(*function_to_bind)(const Y* DELEGATE_COMMA DELEGATE_TEMPLATE_ARGS))
            : caller_(get_caller(pthis, function_to_bind))
            , comparator_(get_comparator(pthis, function_to_bind))
        {
            std::memset(&union_[0], 0, sizeof(union_));

            member_func_call f_call;
            
            std::memset(&f_call, 0, sizeof(f_call));
            f_call.pthis_ = pthis;
            std::memcpy(&f_call.union_[0], &function_to_bind, sizeof(function_to_bind));

            std::memcpy(&union_[0], &f_call, sizeof(f_call));

            assert(NULL != function_to_bind);
        }


        delegate(free_func_type function_to_bind)
            : caller_(get_caller(function_to_bind))
            , comparator_(get_comparator(function_to_bind))
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
                // swap for comparator
                {
                    comparator_type tmp = comparator_;
                    comparator_ = other.comparator_;
                    other.comparator_ = tmp;
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

        friend void swap(delegate& lhs, delegate& rhs)
        {
            lhs.swap(rhs);
        }

        delegate(const delegate &other)
        { 
            if (this == &other)
                return;

            if (caller_ || other.caller_)
            {
                caller_ = (other.caller_);
                comparator_ = (other.comparator_);
                std::memcpy(&union_[0], &other.union_[0], sizeof(union_));
            }
        }

        delegate& operator=(const delegate &other)
        {
            if (this != &other)
            {
                delegate(other).swap(*this);
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
            if (comparator_ != other.comparator_)
                return false;
            if(NULL != comparator_)
                return comparator_(*this, other, equal);
            return true;
        }

        bool operator!=(const delegate &other) const 
        {
            return !(*this == other);
        }

        bool operator<(const delegate &other) const 
        {
            if(caller_ != other.caller_)
                return std::less<caller_type>()(caller_, other.caller_);
            if (comparator_ != other.comparator_)
                return std::less<comparator_type>()(comparator_, other.comparator_);
            if (NULL != comparator_)
                return comparator_(*this, other, less);
            return false;
        }

        bool operator>(const delegate &other) const 
        {
            if (caller_ != other.caller_)
                return std::greater<caller_type>()(caller_, other.caller_);
            if (comparator_ != other.comparator_)
                return std::greater<comparator_type>()(comparator_, other.comparator_);
            if (NULL != comparator_)
                return comparator_(*this, other, greater);
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

            std::memcpy(&call, &union_[0], sizeof(call));

            return (call.pthis_);
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
