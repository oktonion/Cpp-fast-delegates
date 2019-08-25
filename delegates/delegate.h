
#ifndef DELEGATE_H
#define DELEGATE_H
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//wrapper for Fast Delegates of Don Clugston
//adds some jucy functionality
//now you could bind to some free functions that accept pointer to 'this' as first parameter (like member function but nah)

/******************************************************************************************************************************/
/* So you could now bind f.e. 'delegates::delegate<return_type, param_type1, param_type2>' to:                                */
/*                                                                                                                            */
/*   1) { return_type function(param_type1 param1, param_type2 param2); }                                                     */
/*   2) { this_type pthis, return_type function(this_type param1, param_type1 param2, param_type2 param3); }                  */
/*   3) { this_type pthis, return_type this_type::member_function(param_type1 param1, param_type2 param2); }                  */
/*   4) { this_type pthis, return_type function(const this_type param1, param_type1 param2, param_type2 param3); }            */
/*   5) { const this_type pthis, return_type this_type::member_function const (param_type1 param1, param_type2 param2); }     */
/*                                                                                                                            */
/******************************************************************************************************************************/

#include "FastDelegate.h"

#include <algorithm>
#include <utility>
#include <cstring>

namespace delegates
{
	namespace detail
	{
		typedef fastdelegate::detail::DefaultVoid DefaultVoid;

		struct DelegateMementoHack :
			public fastdelegate::DelegateMemento
		{
			static
			void copy_pthis(fastdelegate::DelegateMemento &memento_to, const fastdelegate::DelegateMemento &memento_from)
			{
				memento_to.*(&DelegateMementoHack::m_pthis) = memento_from.*(&DelegateMementoHack::m_pthis);
			}

			static
			bool is_equal_pFunction(const fastdelegate::DelegateMemento &memento1, const fastdelegate::DelegateMemento &memento2)
			{
				fastdelegate::DelegateMemento 
					lhs(memento1), rhs(memento2);
				lhs.*(&DelegateMementoHack::m_pthis) = NULL;
				rhs.*(&DelegateMementoHack::m_pthis) = NULL;
				return lhs.IsEqual(rhs);
			}

			static
			bool is_less_pFunction(const fastdelegate::DelegateMemento &memento1, const fastdelegate::DelegateMemento &memento2)
			{
				fastdelegate::DelegateMemento 
					lhs(memento1), rhs(memento2);
				lhs.*(&DelegateMementoHack::m_pthis) = NULL;
				rhs.*(&DelegateMementoHack::m_pthis) = NULL;
				return lhs.IsLess(rhs);
			}
		};
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
		class ParamUnusedT = detail::DefaultVoid
	>
	class delegate;

	template<class ReturnT>
	class delegate<ReturnT, detail::DefaultVoid, detail::DefaultVoid, detail::DefaultVoid, detail::DefaultVoid, detail::DefaultVoid, detail::DefaultVoid, detail::DefaultVoid, detail::DefaultVoid, detail::DefaultVoid> :
		public fastdelegate::FastDelegate0<ReturnT>
	{
		typedef ReturnT(*free_function_like_member_t)(void* );
		typedef ReturnT(*free_function_like_member_const_t)(const void* );
        
        typedef ReturnT(delegate::* f_proxy_type)() const;

        template<class Y>
        inline
        f_proxy_type get_proxy(Y*, ReturnT(*)(Y*)) const
        { return &delegate::f_proxy<Y>; }

        template<class Y>
        inline
        f_proxy_type get_proxy(Y*, ReturnT(*)(const Y*)) const
        { return &delegate::f_proxy_const<Y>; }

        template<class Y>
        inline
        f_proxy_type get_proxy(const Y*, ReturnT(*)(const Y*)) const
        { return &delegate::f_proxy_const<Y>; }

	public:
		typedef fastdelegate::FastDelegate0< ReturnT  > base_type;

		typedef delegate type;

		delegate() 
			: base_type(),
			m_pthis(NULL),
			m_free_func(NULL)  
		{ }

		template < class X, class Y >
		delegate(Y * pthis,
			ReturnT(X::* function_to_bind)( ))
			: base_type(pthis, function_to_bind),
			m_pthis(NULL),
			m_free_func(NULL)  
		{ }

		template < class X, class Y >
		delegate(const Y *pthis,
			ReturnT(X::* function_to_bind)( ) const)
			: base_type(pthis, function_to_bind),
			m_pthis(NULL),
			m_free_func(NULL)  
		{ }

		template < class Y >
		delegate(Y *pthis,
			ReturnT(*function_to_bind)(Y* ))
			: base_type(this, get_proxy(pthis, function_to_bind)),
			m_pthis(static_cast<void*>(pthis)),
			m_free_func(reinterpret_cast<free_function_like_member_t>(function_to_bind))
		{ }

		template < class Y >
		delegate(Y *pthis,
			ReturnT(*function_to_bind)(const Y* ))
			: base_type(this, get_proxy(pthis, function_to_bind)),
			m_pthis_const(static_cast<const void*>(pthis)),
			m_free_func_const(reinterpret_cast<free_function_like_member_const_t>(function_to_bind))
		{ }

		template < class Y >
		delegate(const Y *pthis,
			ReturnT(*function_to_bind)(const Y*))
			: base_type(this, get_proxy(pthis, function_to_bind)),
			m_pthis_const(static_cast<const void*>(pthis)),
			m_free_func_const(reinterpret_cast<free_function_like_member_const_t>(function_to_bind))
		{ }


		delegate(ReturnT(*function_to_bind)( ))
			: base_type(function_to_bind),
			m_pthis(NULL),
			m_free_func(NULL)  
		{ }

		delegate(const delegate &other)
		{ 
			{
				f_proxy_type proxy = &delegate::f_proxy<delegate>;
				base_type::bind(this, proxy);
				fastdelegate::DelegateMemento tmp = (base_type(other)).GetMemento();
				if(other.m_free_func || other.m_free_func_const)
					detail::DelegateMementoHack::copy_pthis(tmp, base_type::GetMemento());
				base_type::SetMemento(tmp);
			}
			using namespace std;
			memcpy(&m_pthis, &other.m_pthis, sizeof(m_pthis));
			memcpy(&m_pthis_const, &other.m_pthis_const, sizeof(m_pthis_const));
			memcpy(&m_free_func, &other.m_free_func, sizeof(m_free_func));
			memcpy(&m_free_func_const, &other.m_free_func_const, sizeof(m_free_func_const));
		}

		void operator=(const delegate &other)
		{
			{
				f_proxy_type proxy = &delegate::f_proxy<delegate>;
				base_type::bind(this, proxy);
				fastdelegate::DelegateMemento tmp = (base_type(other)).GetMemento();
				if(other.m_free_func || other.m_free_func_const)
					detail::DelegateMementoHack::copy_pthis(tmp, base_type::GetMemento());
				base_type::SetMemento(tmp);
			}
			using namespace std;
			memcpy(&m_pthis, &other.m_pthis, sizeof(m_pthis));
			memcpy(&m_pthis_const, &other.m_pthis_const, sizeof(m_pthis_const));
			memcpy(&m_free_func, &other.m_free_func, sizeof(m_free_func));
			memcpy(&m_free_func_const, &other.m_free_func_const, sizeof(m_free_func_const));
		}

		bool operator==(const delegate &other) const 
		{
			if(!m_free_func && !other.m_free_func)
				return ( static_cast<const base_type&>(*this) == static_cast<const base_type&>(other) );
			
			if(m_pthis == other.m_pthis && m_free_func == other.m_free_func)
				return detail::DelegateMementoHack::is_equal_pFunction((base_type(*this)).GetMemento(), (base_type(other)).GetMemento());
			else
				return false;
		}

		bool operator!=(const delegate &other) const 
		{
			return !(*this == other);
		}

		bool operator<(const delegate &other) const 
		{
			if(!m_free_func && !other.m_free_func)
				return ( static_cast<const base_type&>(*this) < static_cast<const base_type&>(other) );
			if(m_pthis != other.m_pthis)
				return m_pthis < other.m_pthis;
			if(m_free_func != other.m_free_func)
				return m_free_func < other.m_free_func;
			return detail::DelegateMementoHack::is_less_pFunction((base_type(*this)).GetMemento(), (base_type(other)).GetMemento());
		}

		bool operator>(const delegate &other) const 
		{
			if(!m_free_func && !other.m_free_func)
				return ( static_cast<const base_type&>(*this) > static_cast<const base_type&>(other) );
			if(m_pthis != other.m_pthis)
				return m_pthis > other.m_pthis;
			if(m_free_func != other.m_free_func)
				return m_free_func > other.m_free_func;
			return detail::DelegateMementoHack::is_less_pFunction((base_type(other)).GetMemento(), (base_type(*this)).GetMemento());
		}

		template < class Y >
		inline void bind(Y *pthis, ReturnT(*function_to_bind)(Y* )) {
			this->clear();
			m_pthis = static_cast<void*>(pthis);
			m_free_func = reinterpret_cast<free_function_like_member_t>(function_to_bind);
			bind(this, get_proxy(pthis, function_to_bind));
		}

		template < class Y >
		inline void bind(Y *pthis, ReturnT(*function_to_bind)(const Y* )) {
			this->clear();
			m_pthis_const = static_cast<const void*>(pthis);
			m_free_func_const = reinterpret_cast<free_function_like_member_const_t>(function_to_bind);
			bind(this, get_proxy(pthis, function_to_bind));
		}

		template < class Y >
		inline void bind(const Y *pthis, ReturnT(*function_to_bind)(const Y*)) {
			this->clear();
			m_pthis_const = static_cast<const void*>(pthis);
			m_free_func_const = reinterpret_cast<free_function_like_member_const_t>(function_to_bind);
			bind(this, get_proxy(pthis, function_to_bind));
		}

		template < class X, class Y >
		inline void bind(Y *pthis, ReturnT(X::* function_to_bind)()) {
			base_type::bind(pthis, function_to_bind);
		}

		template < class X, class Y >
		inline void bind(const Y *pthis, ReturnT(X::* function_to_bind)() const) {
			base_type::bind(pthis, function_to_bind);
		}

		inline void bind(ReturnT(*function_to_bind)()) {
			base_type::bind(function_to_bind);
		}

	private:

		union
		{
			const void *m_pthis_const;
			void *m_pthis;
		};

		union
		{
			free_function_like_member_t m_free_func;
			free_function_like_member_const_t m_free_func_const;
		};

		template< class Y >
		ReturnT f_proxy() const 
		{ 
			typedef ReturnT(*type_free_function_like_member_t)(Y* );

			return reinterpret_cast<type_free_function_like_member_t>(m_free_func)(static_cast<Y*>(m_pthis));
		}

		template< class Y >
		ReturnT f_proxy_const() const 
		{
			typedef ReturnT(*type_free_function_like_member_t)(const Y* );

			return reinterpret_cast<type_free_function_like_member_t>(m_free_func_const)(static_cast<const Y*>(m_pthis_const));
		}
	};

	template<class ReturnT, class Param1T>
	class delegate<ReturnT, Param1T, detail::DefaultVoid, detail::DefaultVoid, detail::DefaultVoid, detail::DefaultVoid, detail::DefaultVoid, detail::DefaultVoid, detail::DefaultVoid, detail::DefaultVoid> :
		public fastdelegate::FastDelegate1<Param1T, ReturnT>
	{
		typedef ReturnT(*free_function_like_member_t)(void*, Param1T);
		typedef ReturnT(*free_function_like_member_const_t)(const void*, Param1T);

		typedef ReturnT(delegate::* f_proxy_type)(Param1T) const;

        template<class Y>
        inline
        f_proxy_type get_proxy(Y*, ReturnT(*)(Y*, Param1T)) const
        { return &delegate::f_proxy<Y>; }

        template<class Y>
        inline
        f_proxy_type get_proxy(Y*, ReturnT(*)(const Y*, Param1T)) const
        { return &delegate::f_proxy_const<Y>; }

        template<class Y>
        inline
        f_proxy_type get_proxy(const Y*, ReturnT(*)(const Y*, Param1T)) const
        { return &delegate::f_proxy_const<Y>; }

	public:
		typedef fastdelegate::FastDelegate1<Param1T, ReturnT> base_type;

		typedef delegate type;

		delegate() 
			: base_type(),
			m_pthis(NULL),
			m_free_func(NULL)  
		{ }

		template < class X, class Y >
		delegate(Y * pthis,
			ReturnT(X::* function_to_bind)(Param1T))
			: base_type(pthis, function_to_bind),
			m_pthis(NULL),
			m_free_func(NULL)  
		{ }

		template < class X, class Y >
		delegate(const Y *pthis,
			ReturnT(X::* function_to_bind)(Param1T) const)
			: base_type(pthis, function_to_bind),
			m_pthis(NULL),
			m_free_func(NULL)  
		{ }

		template < class Y >
		delegate(Y *pthis,
			ReturnT(*function_to_bind)(Y*, Param1T))
			: base_type(this, get_proxy(pthis, function_to_bind)),
			m_pthis(static_cast<void*>(pthis)),
			m_free_func(reinterpret_cast<free_function_like_member_t>(function_to_bind))
		{ }

		template < class Y >
		delegate(Y *pthis,
			ReturnT(*function_to_bind)(const Y*, Param1T))
			: base_type(this, get_proxy(pthis, function_to_bind)),
			m_pthis_const(static_cast<const void*>(pthis)),
			m_free_func_const(reinterpret_cast<free_function_like_member_const_t>(function_to_bind))
		{ }

		template < class Y >
		delegate(const Y *pthis,
			ReturnT(*function_to_bind)(const Y*, Param1T))
			: base_type(this, get_proxy(pthis, function_to_bind)),
			m_pthis_const(static_cast<const void*>(pthis)),
			m_free_func_const(reinterpret_cast<free_function_like_member_const_t>(function_to_bind))
		{ }


		delegate(ReturnT(*function_to_bind)(Param1T))
			: base_type(function_to_bind),
			m_pthis(NULL),
			m_free_func(NULL)  
		{ }

		delegate(const delegate &other)
		{
			{
				f_proxy_type proxy = &delegate::f_proxy<delegate>;
				base_type::bind(this, proxy);
				fastdelegate::DelegateMemento tmp = (base_type(other)).GetMemento();
				if(other.m_free_func || other.m_free_func_const)
					detail::DelegateMementoHack::copy_pthis(tmp, base_type::GetMemento());
				base_type::SetMemento(tmp);
			}
			using namespace std;
			memcpy(&m_pthis, &other.m_pthis, sizeof(m_pthis));
			memcpy(&m_pthis_const, &other.m_pthis_const, sizeof(m_pthis_const));
			memcpy(&m_free_func, &other.m_free_func, sizeof(m_free_func));
			memcpy(&m_free_func_const, &other.m_free_func_const, sizeof(m_free_func_const));
		}

		void operator=(const delegate &other)
		{
			{
				f_proxy_type proxy = &delegate::f_proxy<delegate>;
				base_type::bind(this, proxy);
				fastdelegate::DelegateMemento tmp = (base_type(other)).GetMemento();
				if(other.m_free_func || other.m_free_func_const)
					detail::DelegateMementoHack::copy_pthis(tmp, base_type::GetMemento());
				base_type::SetMemento(tmp);
			}
			using namespace std;
			memcpy(&m_pthis, &other.m_pthis, sizeof(m_pthis));
			memcpy(&m_pthis_const, &other.m_pthis_const, sizeof(m_pthis_const));
			memcpy(&m_free_func, &other.m_free_func, sizeof(m_free_func));
			memcpy(&m_free_func_const, &other.m_free_func_const, sizeof(m_free_func_const));
		}
		
		bool operator==(const delegate &other) const 
		{
			if(!m_free_func && !other.m_free_func)
				return ( static_cast<const base_type&>(*this) == static_cast<const base_type&>(other) );
			
			if(m_pthis == other.m_pthis && m_free_func == other.m_free_func)
				return detail::DelegateMementoHack::is_equal_pFunction((base_type(*this)).GetMemento(), (base_type(other)).GetMemento());
			else
				return false;
		}

		bool operator!=(const delegate &other) const 
		{
			return !(*this == other);
		}

		bool operator<(const delegate &other) const 
		{
			if(!m_free_func && !other.m_free_func)
				return ( static_cast<const base_type&>(*this) < static_cast<const base_type&>(other) );
			if(m_pthis != other.m_pthis)
				return m_pthis < other.m_pthis;
			if(m_free_func != other.m_free_func)
				return m_free_func < other.m_free_func;
			return detail::DelegateMementoHack::is_less_pFunction((base_type(*this)).GetMemento(), (base_type(other)).GetMemento());
		}

		bool operator>(const delegate &other) const 
		{
			if(!m_free_func && !other.m_free_func)
				return ( static_cast<const base_type&>(*this) > static_cast<const base_type&>(other) );
			if(m_pthis != other.m_pthis)
				return m_pthis > other.m_pthis;
			if(m_free_func != other.m_free_func)
				return m_free_func > other.m_free_func;
			return detail::DelegateMementoHack::is_less_pFunction((base_type(other)).GetMemento(), (base_type(*this)).GetMemento());
		}

		template < class Y >
		inline void bind(Y *pthis, ReturnT(*function_to_bind)(Y*, Param1T)) {
			this->clear();
			m_pthis = static_cast<void*>(pthis);
			m_free_func = reinterpret_cast<free_function_like_member_t>(function_to_bind);
			bind(this, get_proxy(pthis, function_to_bind));
		}

		template < class Y >
		inline void bind(Y *pthis, ReturnT(*function_to_bind)(const Y*, Param1T)) {
			this->clear();
			m_pthis_const = static_cast<const void*>(pthis);
			m_free_func_const = reinterpret_cast<free_function_like_member_const_t>(function_to_bind);
			bind(this, get_proxy(pthis, function_to_bind));
		}

		template < class Y >
		inline void bind(const Y *pthis, ReturnT(*function_to_bind)(const Y*, Param1T)) {
			this->clear();
			m_pthis_const = static_cast<const void*>(pthis);
			m_free_func_const = reinterpret_cast<free_function_like_member_const_t>(function_to_bind);
			bind(this, get_proxy(pthis, function_to_bind));
		}

		template < class X, class Y >
		inline void bind(Y *pthis, ReturnT(X::* function_to_bind)(Param1T)) {
			base_type::bind(pthis, function_to_bind);
		}

		template < class X, class Y >
		inline void bind(const Y *pthis, ReturnT(X::* function_to_bind)(Param1T) const) {
			base_type::bind(pthis, function_to_bind);
		}

		inline void bind(ReturnT(*function_to_bind)(Param1T)) {
			base_type::bind(function_to_bind);
		}

	private:
		union
		{
			const void *m_pthis_const;
			void *m_pthis;
		};

		union
		{
			free_function_like_member_t m_free_func;
			free_function_like_member_const_t m_free_func_const;
		};

		template< class Y >
		ReturnT f_proxy(Param1T p1) const
		{
			typedef ReturnT(*type_free_function_like_member_t)(Y*, Param1T);

			return reinterpret_cast<type_free_function_like_member_t>(m_free_func)(static_cast<Y*>(m_pthis), p1);
		}

		template< class Y >
		ReturnT f_proxy_const(Param1T p1) const
		{
			typedef ReturnT(*type_free_function_like_member_t)(const Y*, Param1T);

			return reinterpret_cast<type_free_function_like_member_t>(m_free_func_const)(static_cast<const Y*>(m_pthis_const), p1);
		}
	};

	template<class ReturnT, class Param1T, class Param2T>
	class delegate<ReturnT, Param1T, Param2T, detail::DefaultVoid, detail::DefaultVoid, detail::DefaultVoid, detail::DefaultVoid, detail::DefaultVoid, detail::DefaultVoid, detail::DefaultVoid> :
		public fastdelegate::FastDelegate2<Param1T, Param2T, ReturnT>
	{
		typedef ReturnT(*free_function_like_member_t)(void*, Param1T, Param2T);
		typedef ReturnT(*free_function_like_member_const_t)(const void*, Param1T, Param2T);

		typedef ReturnT(delegate::* f_proxy_type)(Param1T, Param2T) const;

        template<class Y>
        inline
        f_proxy_type get_proxy(Y*, ReturnT(*)(Y*, Param1T, Param2T)) const
        { return &delegate::f_proxy<Y>; }

        template<class Y>
        inline
        f_proxy_type get_proxy(Y*, ReturnT(*)(const Y*, Param1T, Param2T)) const
        { return &delegate::f_proxy_const<Y>; }

        template<class Y>
        inline
        f_proxy_type get_proxy(const Y*, ReturnT(*)(const Y*, Param1T, Param2T)) const
        { return &delegate::f_proxy_const<Y>; }

	public:
		typedef fastdelegate::FastDelegate2<Param1T, Param2T, ReturnT> base_type;

		typedef delegate type;

		delegate() 
			: base_type(),
			m_pthis(NULL),
			m_free_func(NULL)  
		{ }

		template < class X, class Y >
		delegate(Y * pthis,
			ReturnT(X::* function_to_bind)(Param1T, Param2T))
			: base_type(pthis, function_to_bind),
			m_pthis(NULL),
			m_free_func(NULL)  
		{ }

		template < class X, class Y >
		delegate(const Y *pthis,
			ReturnT(X::* function_to_bind)(Param1T, Param2T) const)
			: base_type(pthis, function_to_bind),
			m_pthis(NULL),
			m_free_func(NULL)  
		{ }

		template < class Y >
		delegate(Y *pthis,
			ReturnT(*function_to_bind)(Y*, Param1T, Param2T))
			: base_type(this, get_proxy(pthis, function_to_bind)),
			m_pthis(static_cast<void*>(pthis)),
			m_free_func(reinterpret_cast<free_function_like_member_t>(function_to_bind))
		{ }

		template < class Y >
		delegate(Y *pthis,
			ReturnT(*function_to_bind)(const Y*, Param1T, Param2T))
			: base_type(this, get_proxy(pthis, function_to_bind)),
			m_pthis_const(static_cast<const void*>(pthis)),
			m_free_func_const(reinterpret_cast<free_function_like_member_const_t>(function_to_bind))
		{ }

		template < class Y >
		delegate(const Y *pthis,
			ReturnT(*function_to_bind)(const Y*, Param1T, Param2T))
			: base_type(this, get_proxy(pthis, function_to_bind)),
			m_pthis_const(static_cast<const void*>(pthis)),
			m_free_func_const(reinterpret_cast<free_function_like_member_const_t>(function_to_bind))
		{ }


		delegate(ReturnT(*function_to_bind)(Param1T, Param2T))
			: base_type(function_to_bind),
			m_pthis(NULL),
			m_free_func(NULL)  
		{ }

		delegate(const delegate &other)
		{
			{
				f_proxy_type proxy = &delegate::f_proxy<delegate>;
				base_type::bind(this, proxy);
				fastdelegate::DelegateMemento tmp = (base_type(other)).GetMemento();
				if(other.m_free_func || other.m_free_func_const)
					detail::DelegateMementoHack::copy_pthis(tmp, base_type::GetMemento());
				base_type::SetMemento(tmp);
			}
			using namespace std;
			memcpy(&m_pthis, &other.m_pthis, sizeof(m_pthis));
			memcpy(&m_pthis_const, &other.m_pthis_const, sizeof(m_pthis_const));
			memcpy(&m_free_func, &other.m_free_func, sizeof(m_free_func));
			memcpy(&m_free_func_const, &other.m_free_func_const, sizeof(m_free_func_const));
		}

		void operator=(const delegate &other)
		{
			{
				f_proxy_type proxy = &delegate::f_proxy<delegate>;
				base_type::bind(this, proxy);
				fastdelegate::DelegateMemento tmp = (base_type(other)).GetMemento();
				if(other.m_free_func || other.m_free_func_const)
					detail::DelegateMementoHack::copy_pthis(tmp, base_type::GetMemento());
				base_type::SetMemento(tmp);
			}
			using namespace std;
			memcpy(&m_pthis, &other.m_pthis, sizeof(m_pthis));
			memcpy(&m_pthis_const, &other.m_pthis_const, sizeof(m_pthis_const));
			memcpy(&m_free_func, &other.m_free_func, sizeof(m_free_func));
			memcpy(&m_free_func_const, &other.m_free_func_const, sizeof(m_free_func_const));
		}
		
		bool operator==(const delegate &other) const 
		{
			if(!m_free_func && !other.m_free_func)
				return ( static_cast<const base_type&>(*this) == static_cast<const base_type&>(other) );
			
			if(m_pthis == other.m_pthis && m_free_func == other.m_free_func)
				return detail::DelegateMementoHack::is_equal_pFunction((base_type(*this)).GetMemento(), (base_type(other)).GetMemento());
			else
				return false;
		}

		bool operator!=(const delegate &other) const 
		{
			return !(*this == other);
		}

		bool operator<(const delegate &other) const 
		{
			if(!m_free_func && !other.m_free_func)
				return ( static_cast<const base_type&>(*this) < static_cast<const base_type&>(other) );
			if(m_pthis != other.m_pthis)
				return m_pthis < other.m_pthis;
			if(m_free_func != other.m_free_func)
				return m_free_func < other.m_free_func;
			return detail::DelegateMementoHack::is_less_pFunction((base_type(*this)).GetMemento(), (base_type(other)).GetMemento());
		}

		bool operator>(const delegate &other) const 
		{
			if(!m_free_func && !other.m_free_func)
				return ( static_cast<const base_type&>(*this) > static_cast<const base_type&>(other) );
			if(m_pthis != other.m_pthis)
				return m_pthis > other.m_pthis;
			if(m_free_func != other.m_free_func)
				return m_free_func > other.m_free_func;
			return detail::DelegateMementoHack::is_less_pFunction((base_type(other)).GetMemento(), (base_type(*this)).GetMemento());
		}

		template < class Y >
		inline void bind(Y *pthis, ReturnT(*function_to_bind)(Y*, Param1T, Param2T)) {
			this->clear();
			m_pthis = static_cast<void*>(pthis);
			m_free_func = reinterpret_cast<free_function_like_member_t>(function_to_bind);
			bind(this, get_proxy(pthis, function_to_bind));
		}

		template < class Y >
		inline void bind(Y *pthis, ReturnT(*function_to_bind)(const Y*, Param1T, Param2T)) {
			this->clear();
			m_pthis_const = static_cast<const void*>(pthis);
			m_free_func_const = reinterpret_cast<free_function_like_member_const_t>(function_to_bind);
			bind(this, get_proxy(pthis, function_to_bind));
		}

		template < class Y >
		inline void bind(const Y *pthis, ReturnT(*function_to_bind)(const Y*, Param1T, Param2T)) {
			this->clear();
			m_pthis_const = static_cast<const void*>(pthis);
			m_free_func_const = reinterpret_cast<free_function_like_member_const_t>(function_to_bind);
			bind(this, get_proxy(pthis, function_to_bind));
		}

		template < class X, class Y >
		inline void bind(Y *pthis, ReturnT(X::* function_to_bind)(Param1T, Param2T)) {
			base_type::bind(pthis, function_to_bind);
		}

		template < class X, class Y >
		inline void bind(const Y *pthis, ReturnT(X::* function_to_bind)(Param1T, Param2T) const) {
			base_type::bind(pthis, function_to_bind);
		}

		inline void bind(ReturnT(*function_to_bind)(Param1T, Param2T)) {
			base_type::bind(function_to_bind);
		}

	private:
		union
		{
			const void *m_pthis_const;
			void *m_pthis;
		};

		union
		{
			free_function_like_member_t m_free_func;
			free_function_like_member_const_t m_free_func_const;
		};

		template< class Y >
		ReturnT f_proxy(Param1T p1, Param2T p2) const
		{
			typedef ReturnT(*type_free_function_like_member_t)(Y*, Param1T, Param2T);

			return reinterpret_cast<type_free_function_like_member_t>(m_free_func)(static_cast<Y*>(m_pthis), p1, p2);
		}

		template< class Y >
		ReturnT f_proxy_const(Param1T p1, Param2T p2) const
		{
			typedef ReturnT(*type_free_function_like_member_t)(const Y*, Param1T, Param2T);

			return reinterpret_cast<type_free_function_like_member_t>(m_free_func_const)(static_cast<const Y*>(m_pthis_const), p1, p2);
		}
	};

	template<class ReturnT, class Param1T, class Param2T, class Param3T>
	class delegate<ReturnT, Param1T, Param2T, Param3T, detail::DefaultVoid, detail::DefaultVoid, detail::DefaultVoid, detail::DefaultVoid, detail::DefaultVoid, detail::DefaultVoid> :
		public fastdelegate::FastDelegate3<Param1T, Param2T, Param3T, ReturnT>
	{
		typedef ReturnT(*free_function_like_member_t)(void*, Param1T, Param2T, Param3T);
		typedef ReturnT(*free_function_like_member_const_t)(const void*, Param1T, Param2T, Param3T);

		typedef ReturnT(delegate::* f_proxy_type)(Param1T, Param2T, Param3T) const;

        template<class Y>
        inline
        f_proxy_type get_proxy(Y*, ReturnT(*)(Y*, Param1T, Param2T, Param3T)) const
        { return &delegate::f_proxy<Y>; }

        template<class Y>
        inline
        f_proxy_type get_proxy(Y*, ReturnT(*)(const Y*, Param1T, Param2T, Param3T)) const
        { return &delegate::f_proxy_const<Y>; }

        template<class Y>
        inline
        f_proxy_type get_proxy(const Y*, ReturnT(*)(const Y*, Param1T, Param2T, Param3T)) const
        { return &delegate::f_proxy_const<Y>; }

	public:
		typedef fastdelegate::FastDelegate3<Param1T, Param2T, Param3T, ReturnT> base_type;

		typedef delegate type;

		delegate() 
			: base_type(),
			m_pthis(NULL),
			m_free_func(NULL)  
		{ }

		template < class X, class Y >
		delegate(Y * pthis,
			ReturnT(X::* function_to_bind)(Param1T, Param2T, Param3T))
			: base_type(pthis, function_to_bind),
			m_pthis(NULL),
			m_free_func(NULL)  
		{ }

		template < class X, class Y >
		delegate(const Y *pthis,
			ReturnT(X::* function_to_bind)(Param1T, Param2T, Param3T) const)
			: base_type(pthis, function_to_bind),
			m_pthis(NULL),
			m_free_func(NULL)  
		{ }

		template < class Y >
		delegate(Y *pthis,
			ReturnT(*function_to_bind)(Y*, Param1T, Param2T, Param3T))
			: base_type(this, get_proxy(pthis, function_to_bind)),
			m_pthis(static_cast<void*>(pthis)),
			m_free_func(reinterpret_cast<free_function_like_member_t>(function_to_bind))
		{ }

		template < class Y >
		delegate(Y *pthis,
			ReturnT(*function_to_bind)(const Y*, Param1T, Param2T, Param3T))
			: base_type(this, get_proxy(pthis, function_to_bind)),
			m_pthis_const(static_cast<const void*>(pthis)),
			m_free_func_const(reinterpret_cast<free_function_like_member_const_t>(function_to_bind))
		{ }

		template < class Y >
		delegate(const Y *pthis,
			ReturnT(*function_to_bind)(const Y*, Param1T, Param2T, Param3T))
			: base_type(this, get_proxy(pthis, function_to_bind)),
			m_pthis_const(static_cast<const void*>(pthis)),
			m_free_func_const(reinterpret_cast<free_function_like_member_const_t>(function_to_bind))
		{ }


		delegate(ReturnT(*function_to_bind)(Param1T, Param2T, Param3T))
			: base_type(function_to_bind),
			m_pthis(NULL),
			m_free_func(NULL)  
		{ }

		delegate(const delegate &other)
		{
			{
				f_proxy_type proxy = &delegate::f_proxy<delegate>;
				base_type::bind(this, proxy);
				fastdelegate::DelegateMemento tmp = (base_type(other)).GetMemento();
				if(other.m_free_func || other.m_free_func_const)
					detail::DelegateMementoHack::copy_pthis(tmp, base_type::GetMemento());
				base_type::SetMemento(tmp);
			}
			using namespace std;
			memcpy(&m_pthis, &other.m_pthis, sizeof(m_pthis));
			memcpy(&m_pthis_const, &other.m_pthis_const, sizeof(m_pthis_const));
			memcpy(&m_free_func, &other.m_free_func, sizeof(m_free_func));
			memcpy(&m_free_func_const, &other.m_free_func_const, sizeof(m_free_func_const));
		}

		void operator=(const delegate &other)
		{
			{
				f_proxy_type proxy = &delegate::f_proxy<delegate>;
				base_type::bind(this, proxy);
				fastdelegate::DelegateMemento tmp = (base_type(other)).GetMemento();
				if(other.m_free_func || other.m_free_func_const)
					detail::DelegateMementoHack::copy_pthis(tmp, base_type::GetMemento());
				base_type::SetMemento(tmp);
			}
			using namespace std;
			memcpy(&m_pthis, &other.m_pthis, sizeof(m_pthis));
			memcpy(&m_pthis_const, &other.m_pthis_const, sizeof(m_pthis_const));
			memcpy(&m_free_func, &other.m_free_func, sizeof(m_free_func));
			memcpy(&m_free_func_const, &other.m_free_func_const, sizeof(m_free_func_const));
		}
		
		bool operator==(const delegate &other) const 
		{
			if(!m_free_func && !other.m_free_func)
				return ( static_cast<const base_type&>(*this) == static_cast<const base_type&>(other) );
			
			if(m_pthis == other.m_pthis && m_free_func == other.m_free_func)
				return detail::DelegateMementoHack::is_equal_pFunction((base_type(*this)).GetMemento(), (base_type(other)).GetMemento());
			else
				return false;
		}

		bool operator!=(const delegate &other) const 
		{
			return !(*this == other);
		}

		bool operator<(const delegate &other) const 
		{
			if(!m_free_func && !other.m_free_func)
				return ( static_cast<const base_type&>(*this) < static_cast<const base_type&>(other) );
			if(m_pthis != other.m_pthis)
				return m_pthis < other.m_pthis;
			if(m_free_func != other.m_free_func)
				return m_free_func < other.m_free_func;
			return detail::DelegateMementoHack::is_less_pFunction((base_type(*this)).GetMemento(), (base_type(other)).GetMemento());
		}

		bool operator>(const delegate &other) const 
		{
			if(!m_free_func && !other.m_free_func)
				return ( static_cast<const base_type&>(*this) > static_cast<const base_type&>(other) );
			if(m_pthis != other.m_pthis)
				return m_pthis > other.m_pthis;
			if(m_free_func != other.m_free_func)
				return m_free_func > other.m_free_func;
			return detail::DelegateMementoHack::is_less_pFunction((base_type(other)).GetMemento(), (base_type(*this)).GetMemento());
		}

		template < class Y >
		inline void bind(Y *pthis, ReturnT(*function_to_bind)(Y*, Param1T, Param2T, Param3T)) {
			this->clear();
			m_pthis = static_cast<void*>(pthis);
			m_free_func = reinterpret_cast<free_function_like_member_t>(function_to_bind);
			bind(this, get_proxy(pthis, function_to_bind));
		}

		template < class Y >
		inline void bind(Y *pthis, ReturnT(*function_to_bind)(const Y*, Param1T, Param2T, Param3T)) {
			this->clear();
			m_pthis_const = static_cast<const void*>(pthis);
			m_free_func_const = reinterpret_cast<free_function_like_member_const_t>(function_to_bind);
			bind(this, get_proxy(pthis, function_to_bind));
		}

		template < class Y >
		inline void bind(const Y *pthis, ReturnT(*function_to_bind)(const Y*, Param1T, Param2T, Param3T)) {
			this->clear();
			m_pthis_const = static_cast<const void*>(pthis);
			m_free_func_const = reinterpret_cast<free_function_like_member_const_t>(function_to_bind);
			bind(this, get_proxy(pthis, function_to_bind));
		}

		template < class X, class Y >
		inline void bind(Y *pthis, ReturnT(X::* function_to_bind)(Param1T, Param2T, Param3T)) {
			base_type::bind(pthis, function_to_bind);
		}

		template < class X, class Y >
		inline void bind(const Y *pthis, ReturnT(X::* function_to_bind)(Param1T, Param2T, Param3T) const) {
			base_type::bind(pthis, function_to_bind);
		}

		inline void bind(ReturnT(*function_to_bind)(Param1T, Param2T, Param3T)) {
			base_type::bind(function_to_bind);
		}

	private:
		union
		{
			const void *m_pthis_const;
			void *m_pthis;
		};

		union
		{
			free_function_like_member_t m_free_func;
			free_function_like_member_const_t m_free_func_const;
		};

		template< class Y >
		ReturnT f_proxy(Param1T p1, Param2T p2, Param3T p3) const
		{
			typedef ReturnT(*type_free_function_like_member_t)(Y*, Param1T, Param2T, Param3T);

			return reinterpret_cast<type_free_function_like_member_t>(m_free_func)(static_cast<Y*>(m_pthis), p1, p2, p3);
		}

		template< class Y >
		ReturnT f_proxy_const(Param1T p1, Param2T p2, Param3T p3) const
		{
			typedef ReturnT(*type_free_function_like_member_t)(const Y*, Param1T, Param2T, Param3T);

			return reinterpret_cast<type_free_function_like_member_t>(m_free_func_const)(static_cast<const Y*>(m_pthis_const), p1, p2, p3);
		}
	};

	template<class ReturnT, class Param1T, class Param2T, class Param3T, class Param4T>
	class delegate<ReturnT, Param1T, Param2T, Param3T, Param4T, detail::DefaultVoid, detail::DefaultVoid, detail::DefaultVoid, detail::DefaultVoid, detail::DefaultVoid> :
		public fastdelegate::FastDelegate4<Param1T, Param2T, Param3T, Param4T, ReturnT>
	{
		typedef ReturnT(*free_function_like_member_t)(void*, Param1T, Param2T, Param3T, Param4T);
		typedef ReturnT(*free_function_like_member_const_t)(const void*, Param1T, Param2T, Param3T, Param4T);

		typedef ReturnT(delegate::* f_proxy_type)(Param1T, Param2T, Param3T, Param4T) const;

        template<class Y>
        inline
        f_proxy_type get_proxy(Y*, ReturnT(*)(Y*, Param1T, Param2T, Param3T, Param4T)) const
        { return &delegate::f_proxy<Y>; }

        template<class Y>
        inline
        f_proxy_type get_proxy(Y*, ReturnT(*)(const Y*, Param1T, Param2T, Param3T, Param4T)) const
        { return &delegate::f_proxy_const<Y>; }

        template<class Y>
        inline
        f_proxy_type get_proxy(const Y*, ReturnT(*)(const Y*, Param1T, Param2T, Param3T, Param4T)) const
        { return &delegate::f_proxy_const<Y>; }

	public:
		typedef fastdelegate::FastDelegate4<Param1T, Param2T, Param3T, Param4T, ReturnT> base_type;

		typedef delegate type;

		delegate() 
			: base_type(),
			m_pthis(NULL),
			m_free_func(NULL)  
		{ }

		template < class X, class Y >
		delegate(Y * pthis,
			ReturnT(X::* function_to_bind)(Param1T, Param2T, Param3T, Param4T))
			: base_type(pthis, function_to_bind),
			m_pthis(NULL),
			m_free_func(NULL)  
		{ }

		template < class X, class Y >
		delegate(const Y *pthis,
			ReturnT(X::* function_to_bind)(Param1T, Param2T, Param3T, Param4T) const)
			: base_type(pthis, function_to_bind),
			m_pthis(NULL),
			m_free_func(NULL)  
		{ }

		template < class Y >
		delegate(Y *pthis,
			ReturnT(*function_to_bind)(Y*, Param1T, Param2T, Param3T, Param4T))
			: base_type(this, get_proxy(pthis, function_to_bind)),
			m_pthis(static_cast<void*>(pthis)),
			m_free_func(reinterpret_cast<free_function_like_member_t>(function_to_bind))
		{ }

		template < class Y >
		delegate(Y *pthis,
			ReturnT(*function_to_bind)(const Y*, Param1T, Param2T, Param3T, Param4T))
			: base_type(this, get_proxy(pthis, function_to_bind)),
			m_pthis_const(static_cast<const void*>(pthis)),
			m_free_func_const(reinterpret_cast<free_function_like_member_const_t>(function_to_bind))
		{ }

		template < class Y >
		delegate(const Y *pthis,
			ReturnT(*function_to_bind)(const Y*, Param1T, Param2T, Param3T, Param4T))
			: base_type(this, get_proxy(pthis, function_to_bind)),
			m_pthis_const(static_cast<const void*>(pthis)),
			m_free_func_const(reinterpret_cast<free_function_like_member_const_t>(function_to_bind))
		{ }


		delegate(ReturnT(*function_to_bind)(Param1T, Param2T, Param3T, Param4T))
			: base_type(function_to_bind),
			m_pthis(NULL),
			m_free_func(NULL)  
		{ }

		delegate(const delegate &other)
		{
			{
				f_proxy_type proxy = &delegate::f_proxy<delegate>;
				base_type::bind(this, proxy);
				fastdelegate::DelegateMemento tmp = (base_type(other)).GetMemento();
				if(other.m_free_func || other.m_free_func_const)
					detail::DelegateMementoHack::copy_pthis(tmp, base_type::GetMemento());
				base_type::SetMemento(tmp);
			}
			using namespace std;
			memcpy(&m_pthis, &other.m_pthis, sizeof(m_pthis));
			memcpy(&m_pthis_const, &other.m_pthis_const, sizeof(m_pthis_const));
			memcpy(&m_free_func, &other.m_free_func, sizeof(m_free_func));
			memcpy(&m_free_func_const, &other.m_free_func_const, sizeof(m_free_func_const));
		}

		void operator=(const delegate &other)
		{
			{
				f_proxy_type proxy = &delegate::f_proxy<delegate>;
				base_type::bind(this, proxy);
				fastdelegate::DelegateMemento tmp = (base_type(other)).GetMemento();
				if(other.m_free_func || other.m_free_func_const)
					detail::DelegateMementoHack::copy_pthis(tmp, base_type::GetMemento());
				base_type::SetMemento(tmp);
			}
			using namespace std;
			memcpy(&m_pthis, &other.m_pthis, sizeof(m_pthis));
			memcpy(&m_pthis_const, &other.m_pthis_const, sizeof(m_pthis_const));
			memcpy(&m_free_func, &other.m_free_func, sizeof(m_free_func));
			memcpy(&m_free_func_const, &other.m_free_func_const, sizeof(m_free_func_const));
		}
		
		bool operator==(const delegate &other) const 
		{
			if(!m_free_func && !other.m_free_func)
				return ( static_cast<const base_type&>(*this) == static_cast<const base_type&>(other) );
			
			if(m_pthis == other.m_pthis && m_free_func == other.m_free_func)
				return detail::DelegateMementoHack::is_equal_pFunction((base_type(*this)).GetMemento(), (base_type(other)).GetMemento());
			else
				return false;
		}

		bool operator!=(const delegate &other) const 
		{
			return !(*this == other);
		}

		bool operator<(const delegate &other) const 
		{
			if(!m_free_func && !other.m_free_func)
				return ( static_cast<const base_type&>(*this) < static_cast<const base_type&>(other) );
			if(m_pthis != other.m_pthis)
				return m_pthis < other.m_pthis;
			if(m_free_func != other.m_free_func)
				return m_free_func < other.m_free_func;
			return detail::DelegateMementoHack::is_less_pFunction((base_type(*this)).GetMemento(), (base_type(other)).GetMemento());
		}

		bool operator>(const delegate &other) const 
		{
			if(!m_free_func && !other.m_free_func)
				return ( static_cast<const base_type&>(*this) > static_cast<const base_type&>(other) );
			if(m_pthis != other.m_pthis)
				return m_pthis > other.m_pthis;
			if(m_free_func != other.m_free_func)
				return m_free_func > other.m_free_func;
			return detail::DelegateMementoHack::is_less_pFunction((base_type(other)).GetMemento(), (base_type(*this)).GetMemento());
		}

		template < class Y >
		inline void bind(Y *pthis, ReturnT(*function_to_bind)(Y*, Param1T, Param2T, Param3T, Param4T)) {
			this->clear();
			m_pthis = static_cast<void*>(pthis);
			m_free_func = reinterpret_cast<free_function_like_member_t>(function_to_bind);
			bind(this, get_proxy(pthis, function_to_bind));
		}

		template < class Y >
		inline void bind(Y *pthis, ReturnT(*function_to_bind)(const Y*, Param1T, Param2T, Param3T, Param4T)) {
			this->clear();
			m_pthis_const = static_cast<const void*>(pthis);
			m_free_func_const = reinterpret_cast<free_function_like_member_const_t>(function_to_bind);
			bind(this, get_proxy(pthis, function_to_bind));
		}

		template < class Y >
		inline void bind(const Y *pthis, ReturnT(*function_to_bind)(const Y*, Param1T, Param2T, Param3T, Param4T)) {
			this->clear();
			m_pthis_const = static_cast<const void*>(pthis);
			m_free_func_const = reinterpret_cast<free_function_like_member_const_t>(function_to_bind);
			bind(this, get_proxy(pthis, function_to_bind));
		}

		template < class X, class Y >
		inline void bind(Y *pthis, ReturnT(X::* function_to_bind)(Param1T, Param2T, Param3T, Param4T)) {
			base_type::bind(pthis, function_to_bind);
		}

		template < class X, class Y >
		inline void bind(const Y *pthis, ReturnT(X::* function_to_bind)(Param1T, Param2T, Param3T, Param4T) const) {
			base_type::bind(pthis, function_to_bind);
		}

		inline void bind(ReturnT(*function_to_bind)(Param1T, Param2T, Param3T, Param4T)) {
			base_type::bind(function_to_bind);
		}

	private:
		union
		{
			const void *m_pthis_const;
			void *m_pthis;
		};

		union
		{
			free_function_like_member_t m_free_func;
			free_function_like_member_const_t m_free_func_const;
		};

		template< class Y >
		ReturnT f_proxy(Param1T p1, Param2T p2, Param3T p3, Param4T p4) const
		{
			typedef ReturnT(*type_free_function_like_member_t)(Y*, Param1T, Param2T, Param3T, Param4T);

			return reinterpret_cast<type_free_function_like_member_t>(m_free_func)(static_cast<Y*>(m_pthis), p1, p2, p3, p4);
		}

		template< class Y >
		ReturnT f_proxy_const(Param1T p1, Param2T p2, Param3T p3, Param4T p4) const
		{
			typedef ReturnT(*type_free_function_like_member_t)(const Y*, Param1T, Param2T, Param3T, Param4T);

			return reinterpret_cast<type_free_function_like_member_t>(m_free_func_const)(static_cast<const Y*>(m_pthis_const), p1, p2, p3, p4);
		}
	};

	template<class ReturnT, class Param1T, class Param2T, class Param3T, class Param4T, class Param5T>
	class delegate<ReturnT, Param1T, Param2T, Param3T, Param4T, Param5T, detail::DefaultVoid, detail::DefaultVoid, detail::DefaultVoid, detail::DefaultVoid> :
		public fastdelegate::FastDelegate5<Param1T, Param2T, Param3T, Param4T, Param5T, ReturnT>
	{
		typedef ReturnT(*free_function_like_member_t)(void*, Param1T, Param2T, Param3T, Param4T, Param5T);
		typedef ReturnT(*free_function_like_member_const_t)(const void*, Param1T, Param2T, Param3T, Param4T, Param5T);

		typedef ReturnT(delegate::* f_proxy_type)(Param1T, Param2T, Param3T, Param4T, Param5T) const;

        template<class Y>
        inline
        f_proxy_type get_proxy(Y*, ReturnT(*)(Y*, Param1T, Param2T, Param3T, Param4T, Param5T)) const
        { return &delegate::f_proxy<Y>; }

        template<class Y>
        inline
        f_proxy_type get_proxy(Y*, ReturnT(*)(const Y*, Param1T, Param2T, Param3T, Param4T, Param5T)) const
        { return &delegate::f_proxy_const<Y>; }

        template<class Y>
        inline
        f_proxy_type get_proxy(const Y*, ReturnT(*)(const Y*, Param1T, Param2T, Param3T, Param4T, Param5T)) const
        { return &delegate::f_proxy_const<Y>; }

	public:
		typedef fastdelegate::FastDelegate5<Param1T, Param2T, Param3T, Param4T, Param5T, ReturnT> base_type;

		typedef delegate type;

		delegate() 
			: base_type(),
			m_pthis(NULL),
			m_free_func(NULL)  
		{ }

		template < class X, class Y >
		delegate(Y * pthis,
			ReturnT(X::* function_to_bind)(Param1T, Param2T, Param3T, Param4T, Param5T))
			: base_type(pthis, function_to_bind),
			m_pthis(NULL),
			m_free_func(NULL)  
		{ }

		template < class X, class Y >
		delegate(const Y *pthis,
			ReturnT(X::* function_to_bind)(Param1T, Param2T, Param3T, Param4T, Param5T) const)
			: base_type(pthis, function_to_bind),
			m_pthis(NULL),
			m_free_func(NULL)  
		{ }

		template < class Y >
		delegate(Y *pthis,
			ReturnT(*function_to_bind)(Y*, Param1T, Param2T, Param3T, Param4T, Param5T))
			: base_type(this, get_proxy(pthis, function_to_bind)),
			m_pthis(static_cast<void*>(pthis)),
			m_free_func(reinterpret_cast<free_function_like_member_t>(function_to_bind))
		{ }

		template < class Y >
		delegate(Y *pthis,
			ReturnT(*function_to_bind)(const Y*, Param1T, Param2T, Param3T, Param4T, Param5T))
			: base_type(this, get_proxy(pthis, function_to_bind)),
			m_pthis_const(static_cast<const void*>(pthis)),
			m_free_func_const(reinterpret_cast<free_function_like_member_const_t>(function_to_bind))
		{ }

		template < class Y >
		delegate(const Y *pthis,
			ReturnT(*function_to_bind)(const Y*, Param1T, Param2T, Param3T, Param4T, Param5T))
			: base_type(this, get_proxy(pthis, function_to_bind)),
			m_pthis_const(static_cast<const void*>(pthis)),
			m_free_func_const(reinterpret_cast<free_function_like_member_const_t>(function_to_bind))
		{ }


		delegate(ReturnT(*function_to_bind)(Param1T, Param2T, Param3T, Param4T, Param5T))
			: base_type(function_to_bind),
			m_pthis(NULL),
			m_free_func(NULL)  
		{ }

		delegate(const delegate &other)
		{
			{
				f_proxy_type proxy = &delegate::f_proxy<delegate>;
				base_type::bind(this, proxy);
				fastdelegate::DelegateMemento tmp = (base_type(other)).GetMemento();
				if(other.m_free_func || other.m_free_func_const)
					detail::DelegateMementoHack::copy_pthis(tmp, base_type::GetMemento());
				base_type::SetMemento(tmp);
			}
			using namespace std;
			memcpy(&m_pthis, &other.m_pthis, sizeof(m_pthis));
			memcpy(&m_pthis_const, &other.m_pthis_const, sizeof(m_pthis_const));
			memcpy(&m_free_func, &other.m_free_func, sizeof(m_free_func));
			memcpy(&m_free_func_const, &other.m_free_func_const, sizeof(m_free_func_const));
		}

		void operator=(const delegate &other)
		{
			{
				f_proxy_type proxy = &delegate::f_proxy<delegate>;
				base_type::bind(this, proxy);
				fastdelegate::DelegateMemento tmp = (base_type(other)).GetMemento();
				if(other.m_free_func || other.m_free_func_const)
					detail::DelegateMementoHack::copy_pthis(tmp, base_type::GetMemento());
				base_type::SetMemento(tmp);
			}
			using namespace std;
			memcpy(&m_pthis, &other.m_pthis, sizeof(m_pthis));
			memcpy(&m_pthis_const, &other.m_pthis_const, sizeof(m_pthis_const));
			memcpy(&m_free_func, &other.m_free_func, sizeof(m_free_func));
			memcpy(&m_free_func_const, &other.m_free_func_const, sizeof(m_free_func_const));
		}
		
		bool operator==(const delegate &other) const 
		{
			if(!m_free_func && !other.m_free_func)
				return ( static_cast<const base_type&>(*this) == static_cast<const base_type&>(other) );
			
			if(m_pthis == other.m_pthis && m_free_func == other.m_free_func)
				return detail::DelegateMementoHack::is_equal_pFunction((base_type(*this)).GetMemento(), (base_type(other)).GetMemento());
			else
				return false;
		}

		bool operator!=(const delegate &other) const 
		{
			return !(*this == other);
		}

		bool operator<(const delegate &other) const 
		{
			if(!m_free_func && !other.m_free_func)
				return ( static_cast<const base_type&>(*this) < static_cast<const base_type&>(other) );
			if(m_pthis != other.m_pthis)
				return m_pthis < other.m_pthis;
			if(m_free_func != other.m_free_func)
				return m_free_func < other.m_free_func;
			return detail::DelegateMementoHack::is_less_pFunction((base_type(*this)).GetMemento(), (base_type(other)).GetMemento());
		}

		bool operator>(const delegate &other) const 
		{
			if(!m_free_func && !other.m_free_func)
				return ( static_cast<const base_type&>(*this) > static_cast<const base_type&>(other) );
			if(m_pthis != other.m_pthis)
				return m_pthis > other.m_pthis;
			if(m_free_func != other.m_free_func)
				return m_free_func > other.m_free_func;
			return detail::DelegateMementoHack::is_less_pFunction((base_type(other)).GetMemento(), (base_type(*this)).GetMemento());
		}

		template < class Y >
		inline void bind(Y *pthis, ReturnT(*function_to_bind)(Y*, Param1T, Param2T, Param3T, Param4T, Param5T)) {
			this->clear();
			m_pthis = static_cast<void*>(pthis);
			m_free_func = reinterpret_cast<free_function_like_member_t>(function_to_bind);
			bind(this, get_proxy(pthis, function_to_bind));
		}

		template < class Y >
		inline void bind(Y *pthis, ReturnT(*function_to_bind)(const Y*, Param1T, Param2T, Param3T, Param4T, Param5T)) {
			this->clear();
			m_pthis_const = static_cast<const void*>(pthis);
			m_free_func_const = reinterpret_cast<free_function_like_member_const_t>(function_to_bind);
			bind(this, get_proxy(pthis, function_to_bind));
		}

		template < class Y >
		inline void bind(const Y *pthis, ReturnT(*function_to_bind)(const Y*, Param1T, Param2T, Param3T, Param4T, Param5T)) {
			this->clear();
			m_pthis_const = static_cast<const void*>(pthis);
			m_free_func_const = reinterpret_cast<free_function_like_member_const_t>(function_to_bind);
			bind(this, get_proxy(pthis, function_to_bind));
		}

		template < class X, class Y >
		inline void bind(Y *pthis, ReturnT(X::* function_to_bind)(Param1T, Param2T, Param3T, Param4T, Param5T)) {
			base_type::bind(pthis, function_to_bind);
		}

		template < class X, class Y >
		inline void bind(const Y *pthis, ReturnT(X::* function_to_bind)(Param1T, Param2T, Param3T, Param4T, Param5T) const) {
			base_type::bind(pthis, function_to_bind);
		}

		inline void bind(ReturnT(*function_to_bind)(Param1T, Param2T, Param3T, Param4T, Param5T)) {
			base_type::bind(function_to_bind);
		}

	private:
		union
		{
			const void *m_pthis_const;
			void *m_pthis;
		};

		union
		{
			free_function_like_member_t m_free_func;
			free_function_like_member_const_t m_free_func_const;
		};

		template< class Y >
		ReturnT f_proxy(Param1T p1, Param2T p2, Param3T p3, Param4T p4, Param5T p5) const
		{
			typedef ReturnT(*type_free_function_like_member_t)(Y*, Param1T, Param2T, Param3T, Param4T, Param5T);

			return reinterpret_cast<type_free_function_like_member_t>(m_free_func)(static_cast<Y*>(m_pthis), p1, p2, p3, p4, p5);
		}

		template< class Y >
		ReturnT f_proxy_const(Param1T p1, Param2T p2, Param3T p3, Param4T p4, Param5T p5) const
		{
			typedef ReturnT(*type_free_function_like_member_t)(const Y*, Param1T, Param2T, Param3T, Param4T, Param5T);

			return reinterpret_cast<type_free_function_like_member_t>(m_free_func_const)(static_cast<const Y*>(m_pthis_const), p1, p2, p3, p4, p5);
		}
	};

	template<class ReturnT, class Param1T, class Param2T, class Param3T, class Param4T, class Param5T, class Param6T>
	class delegate<ReturnT, Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, detail::DefaultVoid, detail::DefaultVoid, detail::DefaultVoid> :
		public fastdelegate::FastDelegate6<Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, ReturnT>
	{
		typedef ReturnT(*free_function_like_member_t)(void*, Param1T, Param2T, Param3T, Param4T, Param5T, Param6T);
		typedef ReturnT(*free_function_like_member_const_t)(const void*, Param1T, Param2T, Param3T, Param4T, Param5T, Param6T);

		typedef ReturnT(delegate::* f_proxy_type)(Param1T, Param2T, Param3T, Param4T, Param5T, Param6T) const;

        template<class Y>
        inline
        f_proxy_type get_proxy(Y*, ReturnT(*)(Y*, Param1T, Param2T, Param3T, Param4T, Param5T, Param6T)) const
        { return &delegate::f_proxy<Y>; }

        template<class Y>
        inline
        f_proxy_type get_proxy(Y*, ReturnT(*)(const Y*, Param1T, Param2T, Param3T, Param4T, Param5T, Param6T)) const
        { return &delegate::f_proxy_const<Y>; }

        template<class Y>
        inline
        f_proxy_type get_proxy(const Y*, ReturnT(*)(const Y*, Param1T, Param2T, Param3T, Param4T, Param5T, Param6T)) const
        { return &delegate::f_proxy_const<Y>; }

	public:
		typedef fastdelegate::FastDelegate6<Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, ReturnT> base_type;

		typedef delegate type;

		delegate() 
			: base_type(),
			m_pthis(NULL),
			m_free_func(NULL)  
		{ }

		template < class X, class Y >
		delegate(Y * pthis,
			ReturnT(X::* function_to_bind)(Param1T, Param2T, Param3T, Param4T, Param5T, Param6T))
			: base_type(pthis, function_to_bind),
			m_pthis(NULL),
			m_free_func(NULL)  
		{ }

		template < class X, class Y >
		delegate(const Y *pthis,
			ReturnT(X::* function_to_bind)(Param1T, Param2T, Param3T, Param4T, Param5T, Param6T) const)
			: base_type(pthis, function_to_bind),
			m_pthis(NULL),
			m_free_func(NULL)  
		{ }

		template < class Y >
		delegate(Y *pthis,
			ReturnT(*function_to_bind)(Y*, Param1T, Param2T, Param3T, Param4T, Param5T, Param6T))
			: base_type(this, get_proxy(pthis, function_to_bind)),
			m_pthis(static_cast<void*>(pthis)),
			m_free_func(reinterpret_cast<free_function_like_member_t>(function_to_bind))
		{ }

		template < class Y >
		delegate(Y *pthis,
			ReturnT(*function_to_bind)(const Y*, Param1T, Param2T, Param3T, Param4T, Param5T, Param6T))
			: base_type(this, get_proxy(pthis, function_to_bind)),
			m_pthis_const(static_cast<const void*>(pthis)),
			m_free_func_const(reinterpret_cast<free_function_like_member_const_t>(function_to_bind))
		{ }

		template < class Y >
		delegate(const Y *pthis,
			ReturnT(*function_to_bind)(const Y*, Param1T, Param2T, Param3T, Param4T, Param5T, Param6T))
			: base_type(this, get_proxy(pthis, function_to_bind)),
			m_pthis_const(static_cast<const void*>(pthis)),
			m_free_func_const(reinterpret_cast<free_function_like_member_const_t>(function_to_bind))
		{ }


		delegate(ReturnT(*function_to_bind)(Param1T, Param2T, Param3T, Param4T, Param5T, Param6T))
			: base_type(function_to_bind),
			m_pthis(NULL),
			m_free_func(NULL)  
		{ }

		delegate(const delegate &other)
		{
			{
				f_proxy_type proxy = &delegate::f_proxy<delegate>;
				base_type::bind(this, proxy);
				fastdelegate::DelegateMemento tmp = (base_type(other)).GetMemento();
				if(other.m_free_func || other.m_free_func_const)
					detail::DelegateMementoHack::copy_pthis(tmp, base_type::GetMemento());
				base_type::SetMemento(tmp);
			}
			using namespace std;
			memcpy(&m_pthis, &other.m_pthis, sizeof(m_pthis));
			memcpy(&m_pthis_const, &other.m_pthis_const, sizeof(m_pthis_const));
			memcpy(&m_free_func, &other.m_free_func, sizeof(m_free_func));
			memcpy(&m_free_func_const, &other.m_free_func_const, sizeof(m_free_func_const));
		}

		void operator=(const delegate &other)
		{
			{
				f_proxy_type proxy = &delegate::f_proxy<delegate>;
				base_type::bind(this, proxy);
				fastdelegate::DelegateMemento tmp = (base_type(other)).GetMemento();
				if(other.m_free_func || other.m_free_func_const)
					detail::DelegateMementoHack::copy_pthis(tmp, base_type::GetMemento());
				base_type::SetMemento(tmp);
			}
			using namespace std;
			memcpy(&m_pthis, &other.m_pthis, sizeof(m_pthis));
			memcpy(&m_pthis_const, &other.m_pthis_const, sizeof(m_pthis_const));
			memcpy(&m_free_func, &other.m_free_func, sizeof(m_free_func));
			memcpy(&m_free_func_const, &other.m_free_func_const, sizeof(m_free_func_const));
		}
		
		bool operator==(const delegate &other) const 
		{
			if(!m_free_func && !other.m_free_func)
				return ( static_cast<const base_type&>(*this) == static_cast<const base_type&>(other) );
			
			if(m_pthis == other.m_pthis && m_free_func == other.m_free_func)
				return detail::DelegateMementoHack::is_equal_pFunction((base_type(*this)).GetMemento(), (base_type(other)).GetMemento());
			else
				return false;
		}

		bool operator!=(const delegate &other) const 
		{
			return !(*this == other);
		}

		bool operator<(const delegate &other) const 
		{
			if(!m_free_func && !other.m_free_func)
				return ( static_cast<const base_type&>(*this) < static_cast<const base_type&>(other) );
			if(m_pthis != other.m_pthis)
				return m_pthis < other.m_pthis;
			if(m_free_func != other.m_free_func)
				return m_free_func < other.m_free_func;
			return detail::DelegateMementoHack::is_less_pFunction((base_type(*this)).GetMemento(), (base_type(other)).GetMemento());
		}

		bool operator>(const delegate &other) const 
		{
			if(!m_free_func && !other.m_free_func)
				return ( static_cast<const base_type&>(*this) > static_cast<const base_type&>(other) );
			if(m_pthis != other.m_pthis)
				return m_pthis > other.m_pthis;
			if(m_free_func != other.m_free_func)
				return m_free_func > other.m_free_func;
			return detail::DelegateMementoHack::is_less_pFunction((base_type(other)).GetMemento(), (base_type(*this)).GetMemento());
		}

		template < class Y >
		inline void bind(Y *pthis, ReturnT(*function_to_bind)(Y*, Param1T, Param2T, Param3T, Param4T, Param5T, Param6T)) {
			this->clear();
			m_pthis = static_cast<void*>(pthis);
			m_free_func = reinterpret_cast<free_function_like_member_t>(function_to_bind);
			bind(this, get_proxy(pthis, function_to_bind));
		}

		template < class Y >
		inline void bind(Y *pthis, ReturnT(*function_to_bind)(const Y*, Param1T, Param2T, Param3T, Param4T, Param5T, Param6T)) {
			this->clear();
			m_pthis_const = static_cast<const void*>(pthis);
			m_free_func_const = reinterpret_cast<free_function_like_member_const_t>(function_to_bind);
			bind(this, get_proxy(pthis, function_to_bind));
		}

		template < class Y >
		inline void bind(const Y *pthis, ReturnT(*function_to_bind)(const Y*, Param1T, Param2T, Param3T, Param4T, Param5T, Param6T)) {
			this->clear();
			m_pthis_const = static_cast<const void*>(pthis);
			m_free_func_const = reinterpret_cast<free_function_like_member_const_t>(function_to_bind);
			bind(this, get_proxy(pthis, function_to_bind));
		}

		template < class X, class Y >
		inline void bind(Y *pthis, ReturnT(X::* function_to_bind)(Param1T, Param2T, Param3T, Param4T, Param5T, Param6T)) {
			base_type::bind(pthis, function_to_bind);
		}

		template < class X, class Y >
		inline void bind(const Y *pthis, ReturnT(X::* function_to_bind)(Param1T, Param2T, Param3T, Param4T, Param5T, Param6T) const) {
			base_type::bind(pthis, function_to_bind);
		}

		inline void bind(ReturnT(*function_to_bind)(Param1T, Param2T, Param3T, Param4T, Param5T, Param6T)) {
			base_type::bind(function_to_bind);
		}

	private:
		union
		{
			const void *m_pthis_const;
			void *m_pthis;
		};

		union
		{
			free_function_like_member_t m_free_func;
			free_function_like_member_const_t m_free_func_const;
		};

		template< class Y >
		ReturnT f_proxy(Param1T p1, Param2T p2, Param3T p3, Param4T p4, Param5T p5, Param6T p6) const
		{
			typedef ReturnT(*type_free_function_like_member_t)(Y*, Param1T, Param2T, Param3T, Param4T, Param5T, Param6T);

			return reinterpret_cast<type_free_function_like_member_t>(m_free_func)(static_cast<Y*>(m_pthis), p1, p2, p3, p4, p5, p6);
		}

		template< class Y >
		ReturnT f_proxy_const(Param1T p1, Param2T p2, Param3T p3, Param4T p4, Param5T p5, Param6T p6) const
		{
			typedef ReturnT(*type_free_function_like_member_t)(const Y*, Param1T, Param2T, Param3T, Param4T, Param5T, Param6T);

			return reinterpret_cast<type_free_function_like_member_t>(m_free_func_const)(static_cast<const Y*>(m_pthis_const), p1, p2, p3, p4, p5, p6);
		}
	};

	template<class ReturnT, class Param1T, class Param2T, class Param3T, class Param4T, class Param5T, class Param6T, class Param7T>
	class delegate<ReturnT, Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, Param7T, detail::DefaultVoid, detail::DefaultVoid> :
		public fastdelegate::FastDelegate7<Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, Param7T, ReturnT>
	{
		typedef ReturnT(*free_function_like_member_t)(void*, Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, Param7T);
		typedef ReturnT(*free_function_like_member_const_t)(const void*, Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, Param7T);

		typedef ReturnT(delegate::* f_proxy_type)(Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, Param7T) const;

        template<class Y>
        inline
        f_proxy_type get_proxy(Y*, ReturnT(*)(Y*, Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, Param7T)) const
        { return &delegate::f_proxy<Y>; }

        template<class Y>
        inline
        f_proxy_type get_proxy(Y*, ReturnT(*)(const Y*, Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, Param7T)) const
        { return &delegate::f_proxy_const<Y>; }

        template<class Y>
        inline
        f_proxy_type get_proxy(const Y*, ReturnT(*)(const Y*, Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, Param7T)) const
        { return &delegate::f_proxy_const<Y>; }

	public:
		typedef fastdelegate::FastDelegate7<Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, Param7T, ReturnT> base_type;

		typedef delegate type;

		delegate() 
			: base_type(),
			m_pthis(NULL),
			m_free_func(NULL)  
		{ }

		template < class X, class Y >
		delegate(Y * pthis,
			ReturnT(X::* function_to_bind)(Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, Param7T))
			: base_type(pthis, function_to_bind),
			m_pthis(NULL),
			m_free_func(NULL)  
		{ }

		template < class X, class Y >
		delegate(const Y *pthis,
			ReturnT(X::* function_to_bind)(Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, Param7T) const)
			: base_type(pthis, function_to_bind),
			m_pthis(NULL),
			m_free_func(NULL)  
		{ }

		template < class Y >
		delegate(Y *pthis,
			ReturnT(*function_to_bind)(Y*, Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, Param7T))
			: base_type(this, get_proxy(pthis, function_to_bind)),
			m_pthis(static_cast<void*>(pthis)),
			m_free_func(reinterpret_cast<free_function_like_member_t>(function_to_bind))
		{ }

		template < class Y >
		delegate(Y *pthis,
			ReturnT(*function_to_bind)(const Y*, Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, Param7T))
			: base_type(this, get_proxy(pthis, function_to_bind)),
			m_pthis_const(static_cast<const void*>(pthis)),
			m_free_func_const(reinterpret_cast<free_function_like_member_const_t>(function_to_bind))
		{ }

		template < class Y >
		delegate(const Y *pthis,
			ReturnT(*function_to_bind)(const Y*, Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, Param7T))
			: base_type(this, get_proxy(pthis, function_to_bind)),
			m_pthis_const(static_cast<const void*>(pthis)),
			m_free_func_const(reinterpret_cast<free_function_like_member_const_t>(function_to_bind))
		{ }


		delegate(ReturnT(*function_to_bind)(Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, Param7T))
			: base_type(function_to_bind),
			m_pthis(NULL),
			m_free_func(NULL)  
		{ }

		delegate(const delegate &other)
		{
			{
				f_proxy_type proxy = &delegate::f_proxy<delegate>;
				base_type::bind(this, proxy);
				fastdelegate::DelegateMemento tmp = (base_type(other)).GetMemento();
				if(other.m_free_func || other.m_free_func_const)
					detail::DelegateMementoHack::copy_pthis(tmp, base_type::GetMemento());
				base_type::SetMemento(tmp);
			}
			using namespace std;
			memcpy(&m_pthis, &other.m_pthis, sizeof(m_pthis));
			memcpy(&m_pthis_const, &other.m_pthis_const, sizeof(m_pthis_const));
			memcpy(&m_free_func, &other.m_free_func, sizeof(m_free_func));
			memcpy(&m_free_func_const, &other.m_free_func_const, sizeof(m_free_func_const));
		}

		void operator=(const delegate &other)
		{
			{
				f_proxy_type proxy = &delegate::f_proxy<delegate>;
				base_type::bind(this, proxy);
				fastdelegate::DelegateMemento tmp = (base_type(other)).GetMemento();
				if(other.m_free_func || other.m_free_func_const)
					detail::DelegateMementoHack::copy_pthis(tmp, base_type::GetMemento());
				base_type::SetMemento(tmp);
			}
			using namespace std;
			memcpy(&m_pthis, &other.m_pthis, sizeof(m_pthis));
			memcpy(&m_pthis_const, &other.m_pthis_const, sizeof(m_pthis_const));
			memcpy(&m_free_func, &other.m_free_func, sizeof(m_free_func));
			memcpy(&m_free_func_const, &other.m_free_func_const, sizeof(m_free_func_const));
		}
		
		bool operator==(const delegate &other) const 
		{
			if(!m_free_func && !other.m_free_func)
				return ( static_cast<const base_type&>(*this) == static_cast<const base_type&>(other) );
			
			if(m_pthis == other.m_pthis && m_free_func == other.m_free_func)
				return detail::DelegateMementoHack::is_equal_pFunction((base_type(*this)).GetMemento(), (base_type(other)).GetMemento());
			else
				return false;
		}

		bool operator!=(const delegate &other) const 
		{
			return !(*this == other);
		}

		bool operator<(const delegate &other) const 
		{
			if(!m_free_func && !other.m_free_func)
				return ( static_cast<const base_type&>(*this) < static_cast<const base_type&>(other) );
			if(m_pthis != other.m_pthis)
				return m_pthis < other.m_pthis;
			if(m_free_func != other.m_free_func)
				return m_free_func < other.m_free_func;
			return detail::DelegateMementoHack::is_less_pFunction((base_type(*this)).GetMemento(), (base_type(other)).GetMemento());
		}

		bool operator>(const delegate &other) const 
		{
			if(!m_free_func && !other.m_free_func)
				return ( static_cast<const base_type&>(*this) > static_cast<const base_type&>(other) );
			if(m_pthis != other.m_pthis)
				return m_pthis > other.m_pthis;
			if(m_free_func != other.m_free_func)
				return m_free_func > other.m_free_func;
			return detail::DelegateMementoHack::is_less_pFunction((base_type(other)).GetMemento(), (base_type(*this)).GetMemento());
		}

		template < class Y >
		inline void bind(Y *pthis, ReturnT(*function_to_bind)(Y*, Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, Param7T)) {
			this->clear();
			m_pthis = static_cast<void*>(pthis);
			m_free_func = reinterpret_cast<free_function_like_member_t>(function_to_bind);
			bind(this, get_proxy(pthis, function_to_bind));
		}

		template < class Y >
		inline void bind(Y *pthis, ReturnT(*function_to_bind)(const Y*, Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, Param7T)) {
			this->clear();
			m_pthis_const = static_cast<const void*>(pthis);
			m_free_func_const = reinterpret_cast<free_function_like_member_const_t>(function_to_bind);
			bind(this, get_proxy(pthis, function_to_bind));
		}

		template < class Y >
		inline void bind(const Y *pthis, ReturnT(*function_to_bind)(const Y*, Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, Param7T)) {
			this->clear();
			m_pthis_const = static_cast<const void*>(pthis);
			m_free_func_const = reinterpret_cast<free_function_like_member_const_t>(function_to_bind);
			bind(this, get_proxy(pthis, function_to_bind));
		}

		template < class X, class Y >
		inline void bind(Y *pthis, ReturnT(X::* function_to_bind)(Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, Param7T)) {
			base_type::bind(pthis, function_to_bind);
		}

		template < class X, class Y >
		inline void bind(const Y *pthis, ReturnT(X::* function_to_bind)(Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, Param7T) const) {
			base_type::bind(pthis, function_to_bind);
		}

		inline void bind(ReturnT(*function_to_bind)(Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, Param7T)) {
			base_type::bind(function_to_bind);
		}

	private:
		union
		{
			const void *m_pthis_const;
			void *m_pthis;
		};

		union
		{
			free_function_like_member_t m_free_func;
			free_function_like_member_const_t m_free_func_const;
		};

		template< class Y >
		ReturnT f_proxy(Param1T p1, Param2T p2, Param3T p3, Param4T p4, Param5T p5, Param6T p6, Param7T p7) const
		{
			typedef ReturnT(*type_free_function_like_member_t)(Y*, Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, Param7T);

			return reinterpret_cast<type_free_function_like_member_t>(m_free_func)(static_cast<Y*>(m_pthis), p1, p2, p3, p4, p5, p6, p7);
		}

		template< class Y >
		ReturnT f_proxy_const(Param1T p1, Param2T p2, Param3T p3, Param4T p4, Param5T p5, Param6T p6, Param7T p7) const
		{
			typedef ReturnT(*type_free_function_like_member_t)(const Y*, Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, Param7T);

			return reinterpret_cast<type_free_function_like_member_t>(m_free_func_const)(static_cast<const Y*>(m_pthis_const), p1, p2, p3, p4, p5, p6, p7);
		}
	};

	template<class ReturnT, class Param1T, class Param2T, class Param3T, class Param4T, class Param5T, class Param6T, class Param7T, class Param8T>
	class delegate<ReturnT, Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, Param7T, Param8T, detail::DefaultVoid> :
		public fastdelegate::FastDelegate8<Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, Param7T, Param8T, ReturnT>
	{
		typedef ReturnT(*free_function_like_member_t)(void*, Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, Param7T, Param8T);
		typedef ReturnT(*free_function_like_member_const_t)(const void*, Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, Param7T, Param8T);

		typedef ReturnT(delegate::* f_proxy_type)(Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, Param7T, Param8T) const;

        template<class Y>
        inline
        f_proxy_type get_proxy(Y*, ReturnT(*)(Y*, Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, Param7T)) const
        { return &delegate::f_proxy<Y>; }

        template<class Y>
        inline
        f_proxy_type get_proxy(Y*, ReturnT(*)(const Y*, Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, Param7T)) const
        { return &delegate::f_proxy_const<Y>; }

        template<class Y>
        inline
        f_proxy_type get_proxy(const Y*, ReturnT(*)(const Y*, Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, Param7T)) const
        { return &delegate::f_proxy_const<Y>; }

	public:
		typedef fastdelegate::FastDelegate8<Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, Param7T, Param8T, ReturnT> base_type;

		typedef delegate type;

		delegate() 
			: base_type(),
			m_pthis(NULL),
			m_free_func(NULL)  
		{ }

		template < class X, class Y >
		delegate(Y * pthis,
			ReturnT(X::* function_to_bind)(Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, Param7T, Param8T))
			: base_type(pthis, function_to_bind),
			m_pthis(NULL),
			m_free_func(NULL)  
		{ }

		template < class X, class Y >
		delegate(const Y *pthis,
			ReturnT(X::* function_to_bind)(Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, Param7T, Param8T) const)
			: base_type(pthis, function_to_bind),
			m_pthis(NULL),
			m_free_func(NULL)  
		{ }

		template < class Y >
		delegate(Y *pthis,
			ReturnT(*function_to_bind)(Y*, Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, Param7T, Param8T))
			: base_type(this, get_proxy(pthis, function_to_bind)),
			m_pthis(static_cast<void*>(pthis)),
			m_free_func(reinterpret_cast<free_function_like_member_t>(function_to_bind))
		{ }

		template < class Y >
		delegate(Y *pthis,
			ReturnT(*function_to_bind)(const Y*, Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, Param7T, Param8T))
			: base_type(this, get_proxy(pthis, function_to_bind)),
			m_pthis_const(static_cast<const void*>(pthis)),
			m_free_func_const(reinterpret_cast<free_function_like_member_const_t>(function_to_bind))
		{ }

		template < class Y >
		delegate(const Y *pthis,
			ReturnT(*function_to_bind)(const Y*, Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, Param7T, Param8T))
			: base_type(this, get_proxy(pthis, function_to_bind)),
			m_pthis_const(static_cast<const void*>(pthis)),
			m_free_func_const(reinterpret_cast<free_function_like_member_const_t>(function_to_bind))
		{ }


		delegate(ReturnT(*function_to_bind)(Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, Param7T, Param8T))
			: base_type(function_to_bind) { }

		delegate(const delegate &other)
		{
			{
				f_proxy_type proxy = &delegate::f_proxy<delegate>;
				base_type::bind(this, proxy);
				fastdelegate::DelegateMemento tmp = (base_type(other)).GetMemento();
				if(other.m_free_func || other.m_free_func_const)
					detail::DelegateMementoHack::copy_pthis(tmp, base_type::GetMemento());
				base_type::SetMemento(tmp);
			}
			using namespace std;
			memcpy(&m_pthis, &other.m_pthis, sizeof(m_pthis));
			memcpy(&m_pthis_const, &other.m_pthis_const, sizeof(m_pthis_const));
			memcpy(&m_free_func, &other.m_free_func, sizeof(m_free_func));
			memcpy(&m_free_func_const, &other.m_free_func_const, sizeof(m_free_func_const));
		}

		void operator=(const delegate &other)
		{
			{
				f_proxy_type proxy = &delegate::f_proxy<delegate>;
				base_type::bind(this, proxy);
				fastdelegate::DelegateMemento tmp = (base_type(other)).GetMemento();
				if(other.m_free_func || other.m_free_func_const)
					detail::DelegateMementoHack::copy_pthis(tmp, base_type::GetMemento());
				base_type::SetMemento(tmp);
			}
			using namespace std;
			memcpy(&m_pthis, &other.m_pthis, sizeof(m_pthis));
			memcpy(&m_pthis_const, &other.m_pthis_const, sizeof(m_pthis_const));
			memcpy(&m_free_func, &other.m_free_func, sizeof(m_free_func));
			memcpy(&m_free_func_const, &other.m_free_func_const, sizeof(m_free_func_const));
		}
		
		bool operator==(const delegate &other) const 
		{
			if(!m_free_func && !other.m_free_func)
				return ( static_cast<const base_type&>(*this) == static_cast<const base_type&>(other) );
			
			if(m_pthis == other.m_pthis && m_free_func == other.m_free_func)
				return detail::DelegateMementoHack::is_equal_pFunction((base_type(*this)).GetMemento(), (base_type(other)).GetMemento());
			else
				return false;
		}

		bool operator!=(const delegate &other) const 
		{
			return !(*this == other);
		}

		bool operator<(const delegate &other) const 
		{
			if(!m_free_func && !other.m_free_func)
				return ( static_cast<const base_type&>(*this) < static_cast<const base_type&>(other) );
			if(m_pthis != other.m_pthis)
				return m_pthis < other.m_pthis;
			if(m_free_func != other.m_free_func)
				return m_free_func < other.m_free_func;
			return detail::DelegateMementoHack::is_less_pFunction((base_type(*this)).GetMemento(), (base_type(other)).GetMemento());
		}

		bool operator>(const delegate &other) const 
		{
			if(!m_free_func && !other.m_free_func)
				return ( static_cast<const base_type&>(*this) > static_cast<const base_type&>(other) );
			if(m_pthis != other.m_pthis)
				return m_pthis > other.m_pthis;
			if(m_free_func != other.m_free_func)
				return m_free_func > other.m_free_func;
			return detail::DelegateMementoHack::is_less_pFunction((base_type(other)).GetMemento(), (base_type(*this)).GetMemento());
		}

		template < class Y >
		inline void bind(Y *pthis, ReturnT(*function_to_bind)(Y*, Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, Param7T, Param8T)) {
			this->clear();
			m_pthis = static_cast<void*>(pthis);
			m_free_func = reinterpret_cast<free_function_like_member_t>(function_to_bind);
			bind(this, get_proxy(pthis, function_to_bind));
		}

		template < class Y >
		inline void bind(Y *pthis, ReturnT(*function_to_bind)(const Y*, Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, Param7T, Param8T)) {
			this->clear();
			m_pthis_const = static_cast<const void*>(pthis);
			m_free_func_const = reinterpret_cast<free_function_like_member_const_t>(function_to_bind);
			bind(this, get_proxy(pthis, function_to_bind));
		}

		template < class Y >
		inline void bind(const Y *pthis, ReturnT(*function_to_bind)(const Y*, Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, Param7T, Param8T)) {
			this->clear();
			m_pthis_const = static_cast<const void*>(pthis);
			m_free_func_const = reinterpret_cast<free_function_like_member_const_t>(function_to_bind);
			bind(this, get_proxy(pthis, function_to_bind));
		}

		template < class X, class Y >
		inline void bind(Y *pthis, ReturnT(X::* function_to_bind)(Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, Param7T, Param8T)) {
			base_type::bind(pthis, function_to_bind);
		}

		template < class X, class Y >
		inline void bind(const Y *pthis, ReturnT(X::* function_to_bind)(Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, Param7T, Param8T) const) {
			base_type::bind(pthis, function_to_bind);
		}

		inline void bind(ReturnT(*function_to_bind)(Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, Param7T, Param8T)) {
			base_type::bind(function_to_bind);
		}

	private:
		union
		{
			const void *m_pthis_const;
			void *m_pthis;
		};

		union
		{
			free_function_like_member_t m_free_func;
			free_function_like_member_const_t m_free_func_const;
		};

		template< class Y >
		ReturnT f_proxy(Param1T p1, Param2T p2, Param3T p3, Param4T p4, Param5T p5, Param6T p6, Param7T p7, Param8T p8) const
		{
			typedef ReturnT(*type_free_function_like_member_t)(Y*, Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, Param7T, Param8T);

			return reinterpret_cast<type_free_function_like_member_t>(m_free_func)(static_cast<Y*>(m_pthis), p1, p2, p3, p4, p5, p6, p7, p8);
		}

		template< class Y >
		ReturnT f_proxy_const(Param1T p1, Param2T p2, Param3T p3, Param4T p4, Param5T p5, Param6T p6, Param7T p7, Param8T p8) const
		{
			typedef ReturnT(*type_free_function_like_member_t)(const Y*, Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, Param7T, Param8T);

			return reinterpret_cast<type_free_function_like_member_t>(m_free_func_const)(static_cast<const Y*>(m_pthis_const), p1, p2, p3, p4, p5, p6, p7, p8);
		}
	};

	template < class X, class Y, class ReturnT >
	delegate<ReturnT>
		bind(Y * pthis,
			ReturnT(X::* function_to_bind)())
	{
		return delegate<ReturnT>(pthis, function_to_bind);
	}

	template < class X, class Y, class ReturnT >
	delegate<ReturnT>
		bind(const Y *pthis,
			ReturnT(X::* function_to_bind)() const)
	{
		return delegate<ReturnT>(pthis, function_to_bind);
	}

	template < class Y, class ReturnT >
	delegate<ReturnT>
		bind(Y *pthis,
			ReturnT(*function_to_bind)(Y*))
	{
		return delegate<ReturnT>(pthis, function_to_bind);
	}

	template < class Y, class ReturnT >
	delegate<ReturnT>
		bind(Y *pthis,
			ReturnT(*function_to_bind)(const Y*))
	{
		return delegate<ReturnT>(pthis, function_to_bind);
	}

	template < class ReturnT >
	delegate<ReturnT>
		bind(ReturnT(*function_to_bind)())
	{
		return delegate<ReturnT>(function_to_bind);
	}

	template < class X, class Y, class ReturnT, class Param1T >
	delegate<ReturnT, Param1T>
		bind(Y * pthis,
			ReturnT(X::* function_to_bind)(Param1T))
	{
		return delegate<ReturnT, Param1T>(pthis, function_to_bind);
	}

	template < class X, class Y, class ReturnT, class Param1T >
	delegate<ReturnT, Param1T>
		bind(const Y *pthis,
			ReturnT(X::* function_to_bind)(Param1T) const)
	{
		return delegate<ReturnT, Param1T>(pthis, function_to_bind);
	}

	template < class Y, class ReturnT, class Param1T >
	delegate<ReturnT, Param1T>
		bind(Y *pthis,
			ReturnT(*function_to_bind)(Y*, Param1T))
	{
		return delegate<ReturnT, Param1T>(pthis, function_to_bind);
	}

	template < class Y, class ReturnT, class Param1T >
	delegate<ReturnT, Param1T>
		bind(Y *pthis,
			ReturnT(*function_to_bind)(const Y*, Param1T))
	{
		return delegate<ReturnT, Param1T>(pthis, function_to_bind);
	}

	template < class ReturnT, class Param1T >
	delegate<ReturnT, Param1T>
		bind(ReturnT(*function_to_bind)(Param1T))
	{
		return delegate<ReturnT, Param1T>(function_to_bind);
	}

	template < class X, class Y, class ReturnT, class Param1T, class Param2T >
	delegate<ReturnT, Param1T, Param2T>
		bind(Y * pthis,
			ReturnT(X::* function_to_bind)(Param1T, Param2T))
	{
		return delegate<ReturnT, Param1T, Param2T>(pthis, function_to_bind);
	}

	template < class X, class Y, class ReturnT, class Param1T, class Param2T >
	delegate<ReturnT, Param1T, Param2T>
		bind(const Y *pthis,
			ReturnT(X::* function_to_bind)(Param1T, Param2T) const)
	{
		return delegate<ReturnT, Param1T, Param2T>(pthis, function_to_bind);
	}

	template < class Y, class ReturnT, class Param1T, class Param2T >
	delegate<ReturnT, Param1T, Param2T>
		bind(Y *pthis,
			ReturnT(*function_to_bind)(Y*, Param1T, Param2T))
	{
		return delegate<ReturnT, Param1T, Param2T>(pthis, function_to_bind);
	}

	template < class Y, class ReturnT, class Param1T, class Param2T >
	delegate<ReturnT, Param1T, Param2T>
		bind(Y *pthis,
			ReturnT(*function_to_bind)(const Y*, Param1T, Param2T))
	{
		return delegate<ReturnT, Param1T, Param2T>(pthis, function_to_bind);
	}

	template < class ReturnT, class Param1T, class Param2T >
	delegate<ReturnT, Param1T, Param2T>
		bind(ReturnT(*function_to_bind)(Param1T, Param2T))
	{
		return delegate<ReturnT, Param1T, Param2T>(function_to_bind);
	}

	template < class X, class Y, class ReturnT, class Param1T, class Param2T, class Param3T >
	delegate<ReturnT, Param1T, Param2T, Param3T>
		bind(Y * pthis,
			ReturnT(X::* function_to_bind)(Param1T, Param2T, Param3T))
	{
		return delegate<ReturnT, Param1T, Param2T, Param3T>(pthis, function_to_bind);
	}

	template < class X, class Y, class ReturnT, class Param1T, class Param2T, class Param3T >
	delegate<ReturnT, Param1T, Param2T, Param3T>
		bind(const Y *pthis,
			ReturnT(X::* function_to_bind)(Param1T, Param2T, Param3T) const)
	{
		return delegate<ReturnT, Param1T, Param2T, Param3T>(pthis, function_to_bind);
	}

	template < class Y, class ReturnT, class Param1T, class Param2T, class Param3T >
	delegate<ReturnT, Param1T, Param2T, Param3T>
		bind(Y *pthis,
			ReturnT(*function_to_bind)(Y*, Param1T, Param2T, Param3T))
	{
		return delegate<ReturnT, Param1T, Param2T, Param3T>(pthis, function_to_bind);
	}

	template < class Y, class ReturnT, class Param1T, class Param2T, class Param3T >
	delegate<ReturnT, Param1T, Param2T, Param3T>
		bind(Y *pthis,
			ReturnT(*function_to_bind)(const Y*, Param1T, Param2T, Param3T))
	{
		return delegate<ReturnT, Param1T, Param2T, Param3T>(pthis, function_to_bind);
	}

	template < class ReturnT, class Param1T, class Param2T, class Param3T >
	delegate<ReturnT, Param1T, Param2T, Param3T>
		bind(ReturnT(*function_to_bind)(Param1T, Param2T, Param3T))
	{
		return delegate<ReturnT, Param1T, Param2T, Param3T>(function_to_bind);
	}

	template < class X, class Y, class ReturnT, class Param1T, class Param2T, class Param3T, class Param4T >
	delegate<ReturnT, Param1T, Param2T, Param3T, Param4T>
		bind(Y * pthis,
			ReturnT(X::* function_to_bind)(Param1T, Param2T, Param3T, Param4T))
	{
		return delegate<ReturnT, Param1T, Param2T, Param3T, Param4T>(pthis, function_to_bind);
	}

	template < class X, class Y, class ReturnT, class Param1T, class Param2T, class Param3T, class Param4T >
	delegate<ReturnT, Param1T, Param2T, Param3T, Param4T>
		bind(const Y *pthis,
			ReturnT(X::* function_to_bind)(Param1T, Param2T, Param3T, Param4T) const)
	{
		return delegate<ReturnT, Param1T, Param2T, Param3T, Param4T>(pthis, function_to_bind);
	}

	template < class Y, class ReturnT, class Param1T, class Param2T, class Param3T, class Param4T >
	delegate<ReturnT, Param1T, Param2T, Param3T, Param4T>
		bind(Y *pthis,
			ReturnT(*function_to_bind)(Y*, Param1T, Param2T, Param3T, Param4T))
	{
		return delegate<ReturnT, Param1T, Param2T, Param3T, Param4T>(pthis, function_to_bind);
	}

	template < class Y, class ReturnT, class Param1T, class Param2T, class Param3T, class Param4T >
	delegate<ReturnT, Param1T, Param2T, Param3T, Param4T>
		bind(Y *pthis,
			ReturnT(*function_to_bind)(const Y*, Param1T, Param2T, Param3T, Param4T))
	{
		return delegate<ReturnT, Param1T, Param2T, Param3T, Param4T>(pthis, function_to_bind);
	}

	template < class ReturnT, class Param1T, class Param2T, class Param3T, class Param4T >
	delegate<ReturnT, Param1T, Param2T, Param3T, Param4T>
		bind(ReturnT(*function_to_bind)(Param1T, Param2T, Param3T, Param4T))
	{
		return delegate<ReturnT, Param1T, Param2T, Param3T, Param4T>(function_to_bind);
	}

	template < class X, class Y, class ReturnT, class Param1T, class Param2T, class Param3T, class Param4T, class Param5T >
	delegate<ReturnT, Param1T, Param2T, Param3T, Param4T, Param5T>
		bind(Y * pthis,
			ReturnT(X::* function_to_bind)(Param1T, Param2T, Param3T, Param4T, Param5T))
	{
		return delegate<ReturnT, Param1T, Param2T, Param3T, Param4T, Param5T>(pthis, function_to_bind);
	}

	template < class X, class Y, class ReturnT, class Param1T, class Param2T, class Param3T, class Param4T, class Param5T >
	delegate<ReturnT, Param1T, Param2T, Param3T, Param4T, Param5T>
		bind(const Y *pthis,
			ReturnT(X::* function_to_bind)(Param1T, Param2T, Param3T, Param4T, Param5T) const)
	{
		return delegate<ReturnT, Param1T, Param2T, Param3T, Param4T, Param5T>(pthis, function_to_bind);
	}

	template < class Y, class ReturnT, class Param1T, class Param2T, class Param3T, class Param4T, class Param5T >
	delegate<ReturnT, Param1T, Param2T, Param3T, Param4T, Param5T>
		bind(Y *pthis,
			ReturnT(*function_to_bind)(Y*, Param1T, Param2T, Param3T, Param4T, Param5T))
	{
		return delegate<ReturnT, Param1T, Param2T, Param3T, Param4T, Param5T>(pthis, function_to_bind);
	}

	template < class Y, class ReturnT, class Param1T, class Param2T, class Param3T, class Param4T, class Param5T >
	delegate<ReturnT, Param1T, Param2T, Param3T, Param4T, Param5T>
		bind(Y *pthis,
			ReturnT(*function_to_bind)(const Y*, Param1T, Param2T, Param3T, Param4T, Param5T))
	{
		return delegate<ReturnT, Param1T, Param2T, Param3T, Param4T, Param5T>(pthis, function_to_bind);
	}

	template < class ReturnT, class Param1T, class Param2T, class Param3T, class Param4T, class Param5T >
	delegate<ReturnT, Param1T, Param2T, Param3T, Param4T, Param5T>
		bind(ReturnT(*function_to_bind)(Param1T, Param2T, Param3T, Param4T, Param5T))
	{
		return delegate<ReturnT, Param1T, Param2T, Param3T, Param4T, Param5T>(function_to_bind);
	}

	template < class X, class Y, class ReturnT, class Param1T, class Param2T, class Param3T, class Param4T, class Param5T, class Param6T >
	delegate<ReturnT, Param1T, Param2T, Param3T, Param4T, Param5T, Param6T>
		bind(Y * pthis,
			ReturnT(X::* function_to_bind)(Param1T, Param2T, Param3T, Param4T, Param5T, Param6T))
	{
		return delegate<ReturnT, Param1T, Param2T, Param3T, Param4T, Param5T, Param6T>(pthis, function_to_bind);
	}

	template < class X, class Y, class ReturnT, class Param1T, class Param2T, class Param3T, class Param4T, class Param5T, class Param6T >
	delegate<ReturnT, Param1T, Param2T, Param3T, Param4T, Param5T, Param6T>
		bind(const Y *pthis,
			ReturnT(X::* function_to_bind)(Param1T, Param2T, Param3T, Param4T, Param5T, Param6T) const)
	{
		return delegate<ReturnT, Param1T, Param2T, Param3T, Param4T, Param5T, Param6T>(pthis, function_to_bind);
	}

	template < class Y, class ReturnT, class Param1T, class Param2T, class Param3T, class Param4T, class Param5T, class Param6T >
	delegate<ReturnT, Param1T, Param2T, Param3T, Param4T, Param5T, Param6T>
		bind(Y *pthis,
			ReturnT(*function_to_bind)(Y*, Param1T, Param2T, Param3T, Param4T, Param5T, Param6T))
	{
		return delegate<ReturnT, Param1T, Param2T, Param3T, Param4T, Param5T, Param6T>(pthis, function_to_bind);
	}

	template < class Y, class ReturnT, class Param1T, class Param2T, class Param3T, class Param4T, class Param5T, class Param6T >
	delegate<ReturnT, Param1T, Param2T, Param3T, Param4T, Param5T, Param6T>
		bind(Y *pthis,
			ReturnT(*function_to_bind)(const Y*, Param1T, Param2T, Param3T, Param4T, Param5T, Param6T))
	{
		return delegate<ReturnT, Param1T, Param2T, Param3T, Param4T, Param5T, Param6T>(pthis, function_to_bind);
	}

	template < class ReturnT, class Param1T, class Param2T, class Param3T, class Param4T, class Param5T, class Param6T >
	delegate<ReturnT, Param1T, Param2T, Param3T, Param4T, Param5T, Param6T>
		bind(ReturnT(*function_to_bind)(Param1T, Param2T, Param3T, Param4T, Param5T, Param6T))
	{
		return delegate<ReturnT, Param1T, Param2T, Param3T, Param4T, Param5T, Param6T>(function_to_bind);
	}

	template < class X, class Y, class ReturnT, class Param1T, class Param2T, class Param3T, class Param4T, class Param5T, class Param6T, class Param7T >
	delegate<ReturnT, Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, Param7T>
		bind(Y * pthis,
			ReturnT(X::* function_to_bind)(Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, Param7T))
	{
		return delegate<ReturnT, Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, Param7T>(pthis, function_to_bind);
	}

	template < class X, class Y, class ReturnT, class Param1T, class Param2T, class Param3T, class Param4T, class Param5T, class Param6T, class Param7T >
	delegate<ReturnT, Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, Param7T>
		bind(const Y *pthis,
			ReturnT(X::* function_to_bind)(Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, Param7T) const)
	{
		return delegate<ReturnT, Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, Param7T>(pthis, function_to_bind);
	}

	template < class Y, class ReturnT, class Param1T, class Param2T, class Param3T, class Param4T, class Param5T, class Param6T, class Param7T >
	delegate<ReturnT, Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, Param7T>
		bind(Y *pthis,
			ReturnT(*function_to_bind)(Y*, Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, Param7T))
	{
		return delegate<ReturnT, Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, Param7T>(pthis, function_to_bind);
	}

	template < class Y, class ReturnT, class Param1T, class Param2T, class Param3T, class Param4T, class Param5T, class Param6T, class Param7T >
	delegate<ReturnT, Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, Param7T>
		bind(Y *pthis,
			ReturnT(*function_to_bind)(const Y*, Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, Param7T))
	{
		return delegate<ReturnT, Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, Param7T>(pthis, function_to_bind);
	}

	template < class ReturnT, class Param1T, class Param2T, class Param3T, class Param4T, class Param5T, class Param6T, class Param7T >
	delegate<ReturnT, Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, Param7T>
		bind(ReturnT(*function_to_bind)(Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, Param7T))
	{
		return delegate<ReturnT, Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, Param7T>(function_to_bind);
	}

	template < class X, class Y, class ReturnT, class Param1T, class Param2T, class Param3T, class Param4T, class Param5T, class Param6T, class Param7T, class Param8T >
	delegate<ReturnT, Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, Param7T, Param8T>
		bind(Y * pthis,
			ReturnT(X::* function_to_bind)(Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, Param7T, Param8T))
	{
		return delegate<ReturnT, Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, Param7T, Param8T>(pthis, function_to_bind);
	}

	template < class X, class Y, class ReturnT, class Param1T, class Param2T, class Param3T, class Param4T, class Param5T, class Param6T, class Param7T, class Param8T >
	delegate<ReturnT, Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, Param7T, Param8T>
		bind(const Y *pthis,
			ReturnT(X::* function_to_bind)(Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, Param7T, Param8T) const)
	{
		return delegate<ReturnT, Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, Param7T, Param8T>(pthis, function_to_bind);
	}

	template < class Y, class ReturnT, class Param1T, class Param2T, class Param3T, class Param4T, class Param5T, class Param6T, class Param7T, class Param8T >
	delegate<ReturnT, Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, Param7T, Param8T>
		bind(Y *pthis,
			ReturnT(*function_to_bind)(Y*, Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, Param7T, Param8T))
	{
		return delegate<ReturnT, Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, Param7T, Param8T>(pthis, function_to_bind);
	}

	template < class Y, class ReturnT, class Param1T, class Param2T, class Param3T, class Param4T, class Param5T, class Param6T, class Param7T, class Param8T >
	delegate<ReturnT, Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, Param7T, Param8T>
		bind(Y *pthis,
			ReturnT(*function_to_bind)(const Y*, Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, Param7T, Param8T))
	{
		return delegate<ReturnT, Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, Param7T, Param8T>(pthis, function_to_bind);
	}

	template < class ReturnT, class Param1T, class Param2T, class Param3T, class Param4T, class Param5T, class Param6T, class Param7T, class Param8T >
	delegate<ReturnT, Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, Param7T, Param8T>
		bind(ReturnT(*function_to_bind)(Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, Param7T, Param8T))
	{
		return delegate<ReturnT, Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, Param7T, Param8T>(function_to_bind);
	}
}


#endif