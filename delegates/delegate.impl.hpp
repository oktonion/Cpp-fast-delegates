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
        typedef ReturnT(delegate::*member_func_type)(DELEGATE_TEMPLATE_ARGS);
        typedef ReturnT(*free_func_type)(DELEGATE_TEMPLATE_ARGS);
        typedef ReturnT(*free_func_like_member_type)(pthis_type DELEGATE_COMMA DELEGATE_TEMPLATE_ARGS);
        typedef ReturnT(*caller_type)(const delegate& DELEGATE_COMMA DELEGATE_TEMPLATE_ARGS);

        template<class Y>
        inline
        caller_type get_caller(Y*, ReturnT(*)(Y* DELEGATE_COMMA DELEGATE_TEMPLATE_ARGS)) const
        { return &delegate::function_caller<Y>; }

        template<class Y>
        inline
        caller_type get_caller(const Y*, ReturnT(*)(const Y* DELEGATE_COMMA DELEGATE_TEMPLATE_ARGS)) const
        { return &delegate::function_caller_const<Y>; }

        template<class X, class Y>
        inline
        caller_type get_caller(X*, ReturnT(Y::*)(DELEGATE_TEMPLATE_ARGS)) const
        { return &delegate::mfunction_caller<X>; }

        template<class X, class Y>
        inline
        caller_type get_caller(const X*, ReturnT(Y::*)(DELEGATE_TEMPLATE_ARGS) const) const
        { return &delegate::mfunction_caller_const<X>; }

        inline
        caller_type get_caller(ReturnT(*)(DELEGATE_TEMPLATE_ARGS)) const
        { return &delegate::simple_function_caller; }

    public:
        typedef delegate type;

        delegate() 
            : pthis_(NULL)
            , caller_(NULL)
            , free_func_like_member_(NULL)  
            , member_func_(NULL)
            , free_func_(NULL)
        { }

        template < class X, class Y >
        delegate(Y * pthis,
            ReturnT(X::* function_to_bind)(DELEGATE_TEMPLATE_ARGS))
            : pthis_(pthis)
            , caller_(get_caller(pthis, function_to_bind))
            , free_func_like_member_(NULL)
            , member_func_(reinterpret_cast<member_func_type>(function_to_bind))
            , free_func_(NULL)
        { 
            assert(NULL != pthis);
            assert(NULL != function_to_bind);
        }

        template < class X, class Y >
        delegate(const Y *pthis,
            ReturnT(X::* function_to_bind)(DELEGATE_TEMPLATE_ARGS) const)
            : pthis_(pthis)
            , caller_(get_caller(pthis, function_to_bind))
            , free_func_like_member_(NULL)
            , member_func_(reinterpret_cast<member_func_type>(function_to_bind)) 
            , free_func_(NULL)
        { 
            assert(NULL != pthis);
            assert(NULL != function_to_bind);
        }

        template < class Y >
        delegate(Y *pthis,
            ReturnT(*function_to_bind)(Y* DELEGATE_COMMA DELEGATE_TEMPLATE_ARGS))
            : pthis_(pthis)
            , caller_(get_caller(pthis, function_to_bind))
            , free_func_like_member_(reinterpret_cast<free_func_like_member_type>(function_to_bind))
            , member_func_(NULL) 
            , free_func_(NULL)
        { }

        template < class Y >
        delegate(Y *pthis,
            ReturnT(*function_to_bind)(const Y* DELEGATE_COMMA DELEGATE_TEMPLATE_ARGS))
            : pthis_(pthis)
            , caller_(get_caller(pthis, function_to_bind))
            , free_func_like_member_(reinterpret_cast<free_func_like_member_type>(function_to_bind))
            , member_func_(NULL) 
            , free_func_(NULL)
        { }

        template < class Y >
        delegate(const Y *pthis,
            ReturnT(*function_to_bind)(const Y* DELEGATE_COMMA DELEGATE_TEMPLATE_ARGS))
            : pthis_(pthis)
            , caller_(get_caller(pthis, function_to_bind))
            , free_func_like_member_(reinterpret_cast<free_func_like_member_type>(function_to_bind))
            , member_func_(NULL) 
            , free_func_(NULL)
        { }


        delegate(free_func_type function_to_bind)
            : pthis_(NULL)
            , caller_(get_caller(function_to_bind))
            , free_func_like_member_(NULL)
            , member_func_(NULL)   
            , free_func_(function_to_bind)
        {
            assert(NULL != function_to_bind);
        }

        void swap(delegate &other)
        {
            using std::swap;
            swap(pthis_                , other.pthis_);
            swap(caller_               , other.caller_);
            swap(free_func_like_member_, other.free_func_like_member_);
            swap(free_func_            , other.free_func_);
            swap(member_func_          , other.member_func_);
        }

        delegate(const delegate &other)
        { 
            pthis_                 = (other.pthis_);
            caller_                = (other.caller_);
            free_func_like_member_ = (other.free_func_like_member_);
            free_func_             = (other.free_func_);
            member_func_           = (other.member_func_);
        }

        void operator=(const delegate &other)
        {
            delegate tmp = other;
            swap(tmp);
        }

        bool operator!() const
        {
            if(pthis_                 != NULL)
                return false;
            if(caller_                != NULL)
                return false;
            if(free_func_like_member_ != NULL)
                return false;
            if(free_func_             != NULL)
                return false;
            if(member_func_           != NULL)
                return false;      

            return true;     
        }

        bool operator==(const delegate &other) const 
        {
            if(pthis_                 != other.pthis_)
                return false;
            if(caller_                != other.caller_)
                return false;
            if(free_func_like_member_ != other.free_func_like_member_)
                return false;
            if(free_func_             != other.free_func_)
                return false;
            if(member_func_           != other.member_func_)
                return false;
            return true;
        }

        bool operator!=(const delegate &other) const 
        {
            return !(*this == other);
        }

        bool operator<(const delegate &other) const 
        {
            using namespace std;

            if(pthis_                 != other.pthis_)
                return memcmp(&pthis_, &other.pthis_, sizeof(pthis_)) < 0;
            if(caller_                != other.caller_)
                return memcmp(&caller_, &other.caller_, sizeof(caller_)) < 0;
            if(free_func_like_member_ != other.free_func_like_member_)
                return memcmp(&free_func_like_member_, &other.free_func_like_member_, sizeof(free_func_like_member_)) < 0;
            if(free_func_             != other.free_func_)
                return memcmp(&free_func_, &other.free_func_, sizeof(free_func_)) < 0;
            if(member_func_           != other.member_func_)
                return memcmp(&member_func_, &other.member_func_, sizeof(member_func_)) < 0;
            return false;
        }

        bool operator>(const delegate &other) const 
        {
            using namespace std;

            if(pthis_                 != other.pthis_)
                return memcmp(&pthis_, &other.pthis_, sizeof(pthis_)) >= 0;
            if(caller_                != other.caller_)
                return memcmp(&caller_, &other.caller_, sizeof(caller_)) >= 0;
            if(free_func_like_member_ != other.free_func_like_member_)
                return memcmp(&free_func_like_member_, &other.free_func_like_member_, sizeof(free_func_like_member_)) >= 0;
            if(free_func_             != other.free_func_)
                return memcmp(&free_func_, &other.free_func_, sizeof(free_func_)) >= 0;
            if(member_func_           != other.member_func_)
                return memcmp(&member_func_, &other.member_func_, sizeof(member_func_)) >= 0;
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

        pthis_type pthis_;
        caller_type caller_;
        free_func_type free_func_;
        free_func_like_member_type free_func_like_member_;
        member_func_type member_func_;

        static ReturnT simple_function_caller(const delegate &that DELEGATE_COMMA DELEGATE_PARAMS)
        { 
            return that.free_func_(DELEGATE_ARGS);
        }

        template< class Y >
        static ReturnT function_caller(const delegate &that DELEGATE_COMMA DELEGATE_PARAMS)
        { 
            typedef ReturnT(*real_free_func_like_member_type)(Y* DELEGATE_COMMA DELEGATE_TEMPLATE_ARGS );

            return 
                reinterpret_cast<real_free_func_like_member_type>(that.free_func_like_member_)
                (static_cast<Y*>(const_cast<void*>(that.pthis_)) DELEGATE_COMMA DELEGATE_ARGS);
        }

        template< class Y >
        static ReturnT function_caller_const(const delegate &that DELEGATE_COMMA DELEGATE_PARAMS) 
        {
            typedef ReturnT(*real_free_func_like_member_type)(const Y* DELEGATE_COMMA DELEGATE_TEMPLATE_ARGS);

            return 
                reinterpret_cast<real_free_func_like_member_type>(that.free_func_like_member_)
                (static_cast<const Y*>(const_cast<const void*>(that.pthis_)) DELEGATE_COMMA DELEGATE_ARGS);
        }

        template< class Y >
        static ReturnT mfunction_caller(const delegate &that DELEGATE_COMMA DELEGATE_PARAMS) 
        { 
            typedef ReturnT(Y::*real_member_func_type)(DELEGATE_TEMPLATE_ARGS);

            return 
                (static_cast<Y*>(const_cast<void*>(that.pthis_))->*reinterpret_cast<real_member_func_type>(that.member_func_))
                (DELEGATE_ARGS);
        }

        template< class Y >
        static ReturnT mfunction_caller_const(const delegate &that DELEGATE_COMMA DELEGATE_PARAMS) 
        {
            typedef ReturnT(Y::*real_member_func_type)(DELEGATE_TEMPLATE_ARGS) const;

            return 
                (static_cast<const Y*>(const_cast<const void*>(that.pthis_))->*reinterpret_cast<real_member_func_type>(that.member_func_))
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
