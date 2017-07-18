
#include "FastDelegate.h"

#ifndef DELEGATE_H
#define DELEGATE_H
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//wrapper for Fast Delegates of Don Clugston
//adds some jucy functionality
//now you could bind to some free functions that accept pointer to 'this' as first parameter (like member function but nah)

namespace delegates
{
	namespace detail
	{
		typedef fastdelegate::detail::DefaultVoid DefaultVoid;
	}

	template <class ReturnT = detail::DefaultVoid, class Param1T = detail::DefaultVoid, class Param2T = detail::DefaultVoid, class Param3T = detail::DefaultVoid, class Param4T = detail::DefaultVoid, class Param5T = detail::DefaultVoid, class Param6T = detail::DefaultVoid, class Param7T = detail::DefaultVoid, class Param8T = detail::DefaultVoid>
	class delegate;

	template<class ReturnT>
	class delegate<ReturnT, detail::DefaultVoid, detail::DefaultVoid, detail::DefaultVoid, detail::DefaultVoid, detail::DefaultVoid, detail::DefaultVoid, detail::DefaultVoid, detail::DefaultVoid> :
		public fastdelegate::FastDelegate0<ReturnT>
	{
		typedef ReturnT(*free_function_like_member_t)(void* );
		typedef ReturnT(*free_function_like_member_const_t)(const void* );

	public:
		typedef fastdelegate::FastDelegate0< ReturnT  > base_type;

		typedef delegate type;

		delegate() : base_type() { }

		template < class X, class Y >
		delegate(Y * pthis,
			ReturnT(X::* function_to_bind)( ))
			: base_type(pthis, function_to_bind) { }

		template < class X, class Y >
		delegate(const Y *pthis,
			ReturnT(X::* function_to_bind)( ) const)
			: base_type(pthis, function_to_bind)
		{  }

		template < class Y >
		delegate(Y *pthis,
			ReturnT(*function_to_bind)(Y* ))
			: base_type(this, &delegate::f_proxy<Y>),
			m_pthis(static_cast<void*>(pthis)),
			m_free_func(reinterpret_cast<free_function_like_member_t>(function_to_bind))
		{  	}

		template < class Y >
		delegate(Y *pthis,
			ReturnT(*function_to_bind)(const Y* ))
			: base_type(this, &delegate::f_proxy_const<Y>),
			m_pthis_const(static_cast<const void*>(pthis)),
			m_free_func_const(reinterpret_cast<free_function_like_member_const_t>(function_to_bind))
		{  	}

		template < class Y >
		delegate(const Y *pthis,
			ReturnT(*function_to_bind)(const Y*))
			: base_type(this, &delegate::f_proxy_const<Y>),
			m_pthis_const(static_cast<const void*>(pthis)),
			m_free_func_const(reinterpret_cast<free_function_like_member_const_t>(function_to_bind))
		{  	}


		delegate(ReturnT(*function_to_bind)( ))
			: base_type(function_to_bind) { }

		void operator = (const base_type &x) {
			*static_cast<base_type*>(this) = x;
		}

		template < class Y >
		inline void bind(Y *pthis, ReturnT(*function_to_bind)(Y* )) {
			this->clear();
			m_pthis = static_cast<void*>(pthis);
			m_free_func = reinterpret_cast<free_function_like_member_t>(function_to_bind);
			bind(this, &delegate::f_proxy<Y>);
		}

		template < class Y >
		inline void bind(Y *pthis, ReturnT(*function_to_bind)(const Y* )) {
			this->clear();
			m_pthis_const = static_cast<const void*>(pthis);
			m_free_func_const = reinterpret_cast<free_function_like_member_const_t>(function_to_bind);
			bind(this, &delegate::f_proxy_const<Y>);
		}

		template < class Y >
		inline void bind(const Y *pthis, ReturnT(*function_to_bind)(const Y*)) {
			this->clear();
			m_pthis_const = static_cast<const void*>(pthis);
			m_free_func_const = reinterpret_cast<free_function_like_member_const_t>(function_to_bind);
			bind(this, &delegate::f_proxy_const<Y>);
		}

		using base_type::bind;

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
		ReturnT f_proxy() 
		{ 
			typedef ReturnT(*type_free_function_like_member_t)(Y* );

			return reinterpret_cast<type_free_function_like_member_t>(m_free_func)(static_cast<Y*>(m_pthis));
		}

		template< class Y >
		ReturnT f_proxy_const() 
		{
			typedef ReturnT(*type_free_function_like_member_t)(const Y* );

			return reinterpret_cast<type_free_function_like_member_t>(m_free_func_const)(static_cast<const Y*>(m_pthis_const));
		}
	};

	template<class ReturnT, class Param1T>
	class delegate<ReturnT, Param1T, detail::DefaultVoid, detail::DefaultVoid, detail::DefaultVoid, detail::DefaultVoid, detail::DefaultVoid, detail::DefaultVoid, detail::DefaultVoid> :
		public fastdelegate::FastDelegate1<Param1T, ReturnT>
	{
		typedef ReturnT(*free_function_like_member_t)(void*, Param1T);
		typedef ReturnT(*free_function_like_member_const_t)(const void*, Param1T);

	public:
		typedef fastdelegate::FastDelegate1<Param1T, ReturnT> base_type;

		typedef delegate type;

		delegate() : base_type() { }

		template < class X, class Y >
		delegate(Y * pthis,
			ReturnT(X::* function_to_bind)(Param1T))
			: base_type(pthis, function_to_bind) { }

		template < class X, class Y >
		delegate(const Y *pthis,
			ReturnT(X::* function_to_bind)(Param1T) const)
			: base_type(pthis, function_to_bind)
		{  }

		template < class Y >
		delegate(Y *pthis,
			ReturnT(*function_to_bind)(Y*, Param1T))
			: base_type(this, &delegate::f_proxy<Y>),
			m_pthis(static_cast<void*>(pthis)),
			m_free_func(reinterpret_cast<free_function_like_member_t>(function_to_bind))
		{  	}

		template < class Y >
		delegate(Y *pthis,
			ReturnT(*function_to_bind)(const Y*, Param1T))
			: base_type(this, &delegate::f_proxy_const<Y>),
			m_pthis_const(static_cast<const void*>(pthis)),
			m_free_func_const(reinterpret_cast<free_function_like_member_const_t>(function_to_bind))
		{  	}

		template < class Y >
		delegate(const Y *pthis,
			ReturnT(*function_to_bind)(const Y*, Param1T))
			: base_type(this, &delegate::f_proxy_const<Y>),
			m_pthis_const(static_cast<const void*>(pthis)),
			m_free_func_const(reinterpret_cast<free_function_like_member_const_t>(function_to_bind))
		{  	}


		delegate(ReturnT(*function_to_bind)(Param1T))
			: base_type(function_to_bind) { }

		void operator = (const base_type &x) {
			*static_cast<base_type*>(this) = x;
		}

		template < class Y >
		inline void bind(Y *pthis, ReturnT(*function_to_bind)(Y*, Param1T)) {
			this->clear();
			m_pthis = static_cast<void*>(pthis);
			m_free_func = reinterpret_cast<free_function_like_member_t>(function_to_bind);
			bind(this, &delegate::f_proxy<Y>);
		}

		template < class Y >
		inline void bind(Y *pthis, ReturnT(*function_to_bind)(const Y*, Param1T)) {
			this->clear();
			m_pthis_const = static_cast<const void*>(pthis);
			m_free_func_const = reinterpret_cast<free_function_like_member_const_t>(function_to_bind);
			bind(this, &delegate::f_proxy_const<Y>);
		}

		template < class Y >
		inline void bind(const Y *pthis, ReturnT(*function_to_bind)(const Y*, Param1T)) {
			this->clear();
			m_pthis_const = static_cast<const void*>(pthis);
			m_free_func_const = reinterpret_cast<free_function_like_member_const_t>(function_to_bind);
			bind(this, &delegate::f_proxy_const<Y>);
		}

		using base_type::bind;

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
		ReturnT f_proxy(Param1T p1)
		{
			typedef ReturnT(*type_free_function_like_member_t)(Y*, Param1T);

			return reinterpret_cast<type_free_function_like_member_t>(m_free_func)(static_cast<Y*>(m_pthis), p1);
		}

		template< class Y >
		ReturnT f_proxy_const(Param1T p1)
		{
			typedef ReturnT(*type_free_function_like_member_t)(const Y*, Param1T);

			return reinterpret_cast<type_free_function_like_member_t>(m_free_func_const)(static_cast<const Y*>(m_pthis_const), p1);
		}
	};

	template<class ReturnT, class Param1T, class Param2T>
	class delegate<ReturnT, Param1T, Param2T, detail::DefaultVoid, detail::DefaultVoid, detail::DefaultVoid, detail::DefaultVoid, detail::DefaultVoid, detail::DefaultVoid> :
		public fastdelegate::FastDelegate2<Param1T, Param2T, ReturnT>
	{
		typedef ReturnT(*free_function_like_member_t)(void*, Param1T, Param2T);
		typedef ReturnT(*free_function_like_member_const_t)(const void*, Param1T, Param2T);

	public:
		typedef fastdelegate::FastDelegate2<Param1T, Param2T, ReturnT> base_type;

		typedef delegate type;

		delegate() : base_type() { }

		template < class X, class Y >
		delegate(Y * pthis,
			ReturnT(X::* function_to_bind)(Param1T, Param2T))
			: base_type(pthis, function_to_bind) { }

		template < class X, class Y >
		delegate(const Y *pthis,
			ReturnT(X::* function_to_bind)(Param1T, Param2T) const)
			: base_type(pthis, function_to_bind)
		{  }

		template < class Y >
		delegate(Y *pthis,
			ReturnT(*function_to_bind)(Y*, Param1T, Param2T))
			: base_type(this, &delegate::f_proxy<Y>),
			m_pthis(static_cast<void*>(pthis)),
			m_free_func(reinterpret_cast<free_function_like_member_t>(function_to_bind))
		{  	}

		template < class Y >
		delegate(Y *pthis,
			ReturnT(*function_to_bind)(const Y*, Param1T, Param2T))
			: base_type(this, &delegate::f_proxy_const<Y>),
			m_pthis_const(static_cast<const void*>(pthis)),
			m_free_func_const(reinterpret_cast<free_function_like_member_const_t>(function_to_bind))
		{  	}

		template < class Y >
		delegate(const Y *pthis,
			ReturnT(*function_to_bind)(const Y*, Param1T, Param2T))
			: base_type(this, &delegate::f_proxy_const<Y>),
			m_pthis_const(static_cast<const void*>(pthis)),
			m_free_func_const(reinterpret_cast<free_function_like_member_const_t>(function_to_bind))
		{  	}


		delegate(ReturnT(*function_to_bind)(Param1T, Param2T))
			: base_type(function_to_bind) { }

		void operator = (const base_type &x) {
			*static_cast<base_type*>(this) = x;
		}

		template < class Y >
		inline void bind(Y *pthis, ReturnT(*function_to_bind)(Y*, Param1T, Param2T)) {
			this->clear();
			m_pthis = static_cast<void*>(pthis);
			m_free_func = reinterpret_cast<free_function_like_member_t>(function_to_bind);
			bind(this, &delegate::f_proxy<Y>);
		}

		template < class Y >
		inline void bind(Y *pthis, ReturnT(*function_to_bind)(const Y*, Param1T, Param2T)) {
			this->clear();
			m_pthis_const = static_cast<const void*>(pthis);
			m_free_func_const = reinterpret_cast<free_function_like_member_const_t>(function_to_bind);
			bind(this, &delegate::f_proxy_const<Y>);
		}

		template < class Y >
		inline void bind(const Y *pthis, ReturnT(*function_to_bind)(const Y*, Param1T, Param2T)) {
			this->clear();
			m_pthis_const = static_cast<const void*>(pthis);
			m_free_func_const = reinterpret_cast<free_function_like_member_const_t>(function_to_bind);
			bind(this, &delegate::f_proxy_const<Y>);
		}

		using base_type::bind;

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
		ReturnT f_proxy(Param1T p1, Param2T p2)
		{
			typedef ReturnT(*type_free_function_like_member_t)(Y*, Param1T);

			return reinterpret_cast<type_free_function_like_member_t>(m_free_func)(static_cast<Y*>(m_pthis), p1, p2);
		}

		template< class Y >
		ReturnT f_proxy_const(Param1T p1, Param2T p2)
		{
			typedef ReturnT(*type_free_function_like_member_t)(const Y*, Param1T);

			return reinterpret_cast<type_free_function_like_member_t>(m_free_func_const)(static_cast<const Y*>(m_pthis_const), p1, p2);
		}
	};
}


#endif