// enumerator.h - read only forward iterators
// reversible?
#pragma once
#include <cassert>
#include <functional>
#include <initializer_list>

template<class T>
class enumerator_base {
public:
	typedef T value_type;
	enumerator_base& operator++()
	{
		return _incr();
	}
	T operator*() const
	{
		return _star();
	}
	explicit operator bool() const
	{
		return _bool();
	}
private:
	virtual enumerator_base& _incr() = 0;
	virtual T _star() const = 0;
	virtual bool _bool() const = 0;
};

template<class I>
class enumerator_iterator : public enumerator_base<typename std::iterator_traits<I>::value_type> {
	I i, b, e;
public:
	typedef typename typename std::iterator_traits<I>::value_type value_type;
	enumerator_iterator(I& b, I& e)
		: i(b), b(b), e(e)
	{ }
	void reset()
	{
		i = b;
	}
	I begin() const
	{
		return b;
	}
	I end() const
	{
		return e;
	}
	bool operator==(const enumerator_iterator& ei) const
	{
		return i == ei.i && b == ei.b && e == ei.e;
	}
	bool operator!=(const enumerator_iterator& e) const
	{
		return !operator==(e);
	}

	enumerator_iterator& _incr()
	{
		++i;

		return *this;
	}
	enumerator_iterator& operator++()
	{
		++i;

		return *this;
	}
	value_type _star() const
	{
		return *i;
	}
	bool _bool() const
	{
		return i != e;
	}
};
template<class I>
inline enumerator_iterator<I> make_enumerator_iterator(I& b, I& e)
{
	return enumerator_iterator<I>(b, e);
}
template<class I>
inline enumerator_iterator<I> enumerator(I& b, I& e)
{
	return enumerator_iterator<I>(b, e);
}
template<class C>
inline enumerator_iterator<typename C::iterator> make_enumerator_iterator(C& c)
{
	return enumerator_iterator<typename C::iterator>(std::begin(c), std::end(c));
}
template<class C>
inline enumerator_iterator<typename C::iterator> enumerator(C& c)
{
	return enumerator_iterator<typename C::iterator>(std::begin(c), std::end(c));
}

// drop n elements from front
template<class I>
inline I drop(size_t n, I& i)
{
	while (n-- && i)
		++i;

	return i;
}
#ifdef _DEBUG
inline void test_drop()
{
	std::vector<int> a{1,2,3};
	auto i = enumerator(a);

	assert (*i == 1);
	drop(1, i);
	assert (*i == 2);
	assert (!drop(2, i));

	i.reset();
	assert (*drop(2,i) == 3);

	i.reset();
	assert (!drop(100, i));
}
#endif

// take first n elements
template<class I>
class take_enumerator : public I {
	size_t n;
public:
	take_enumerator(size_t n, I& i)
		: I(i), n(n)
	{ }
	take_enumerator& _incr()
	{
		if (n > 0) {
			--n;
			I::operator++();
		}

		return *this;
	}
	take_enumerator& operator++()
	{
		if (n > 0) {
			--n;
			I::operator++();
		}

		return *this;
	}
	typename I::value_type _star() const
	{
		return I::_star();
	}
	bool _bool() const
	{
		return n != 0 && I::_bool();
	}
};
template<class I>
inline take_enumerator<I> make_take_enumerator(size_t n, I& i)
{
	return take_enumerator<I>(n, i);
}
template<class I>
inline take_enumerator<I> take(size_t n, I& i)
{
	return take_enumerator<I>(n, i);
}
#ifdef _DEBUG

void test_take()
{
	std::vector<int> a {1, 2, 3};
	auto i = take(2, enumerator(a));
	assert (*i == 1);
	assert (*++i == 2);
	assert (!++i);
}

#endif // _DEBUG

template<class F, class I>
class apply_enumerator : public enumerator_base<typename I::value_type> {
	F f;
	I t;
public:
	apply_enumerator(F f, I t)
		: f(f), t(t)
	{ }
	apply_enumerator& _incr()
	{
		++t;

		return *this;
	}
	typename I::value_type _star() const
	{
		return f(*t);
	}
	bool _bool() const
	{
		return t.operator bool();
	}
};
template<class F, class I>
inline apply_enumerator<F,I> make_apply_enumerator(F f, I t)
{
	return apply_enumerator<F,I>(f, t);
}

template<class I>
class concatenate_enumerator : public enumerator_base<typename I::value_type> {
	I e,f;
public:
	concatenate_enumerator(I e, I f)
		: e(e), f(f)
	{ }
	concatenate_enumerator& _incr()
	{
		e ? ++e : ++f;

		return *this;
	}
	concatenate_enumerator& operator++()
	{
		return _incr();
	}
	typename I::value_type _star() const
	{
		return e ? *e : *f;
	}
	bool _bool() const
	{
		return e || f;
	}
};
template<class I>
inline concatenate_enumerator<I> make_concatenate_enumerator(I e, I f)
{
	return concatenate_enumerator<I>(e,f);
}

template<class I, class J>
class pair_enumerator : public enumerator_base<std::pair<typename I::value_type, typename J::value_type>> {
	I i;
	J j;
public:
	pair_enumerator(I& i, J& j)
		: i(i), j(j)
	{ }
	pair_enumerator& _incr()
	{
		++i; ++j;

		return *this;
	}
	pair_enumerator& operator++()
	{
		return _incr();
	}
	std::pair<typename I::value_type, typename J::value_type> _star() const
	{
		return std::make_pair(*i, *j);
	}
	bool _bool() const
	{
		assert (i && j || (!i && !j));

		return i.operator bool();
	}
};
template<class I, class J>
inline pair_enumerator<I,J> make_pair_enumerator(I& i, J& j)
{
	reutrn pair_enumerator<I,J>(i, j);
}

template<class T>
class enumerator_sequence : public enumerator_base<T> {
	T i, b, s, e;
public:
	enumerator_sequence(const T& b = 0, const T& s = 1, const T& e = std::numeric_limits<T>::max())
		: i(b), b(b), s(s), e(e)
	{ }
	bool operator==(const enumerator_sequence& i_) const
	{
		return i == i_.i && b == i_.b && s == i_.s && e == i_.e;
	}
	bool operator!=(const enumerator_sequence& i_) const
	{
		return !operator==(i_);
	}

	// input iterator implementation
	enumerator_sequence& _incr()
	{
		i += s;

		return *this;
	}
	enumerator_sequence& operator++()
	{
		return _incr();
	}
	enumerator_sequence operator++(int)
	{
		enumerator_sequence i_(*this);

		_incr();

		return i_;
	}
	T _star() const
	{
		return i;
	}
	bool _bool() const
	{
		return i < e;
	}
};
template<class T>
inline enumerator_sequence<T> make_enumerator_sequence(const T& b = 0, const T& e = std::numeric_limits<T>::max(); const T& s = 1)
{
	return enumerator_sequence<T>(b, e, s);
}
template<class T>
inline enumerator_sequence<T> sequence(const T& b = 0, const T& e = std::numeric_limits<T>::max(); const T& s = 1)
{
	return enumerator_sequence<T>(b, e, s);
}

#ifdef _DEBUG

void test_enumerator_sequence()
{
	enumerator_sequence<int> i(1,2), i2;
	i2 = i;
	assert (i2 == i);
	auto i3(i);
	assert (i3 == i);

	assert (*i == 1);
	assert (*++i == 3);
}

#endif // _DEBUG

#if 0
template<class U, class T>
inline enumerator<U> apply(const std::function<U(T)>& f)
{
	return [](
}


template<class I> 
struct enumerator_base : public std::iterator<
	typename std::iterator_traits<I>::iterator_category,
	typename std::iterator_traits<I>::value_type,
	typename std::iterator_traits<I>::difference_type,
	typename std::iterator_traits<I>::pointer,
	typename std::iterator_traits<I>::reference>
{
	operator bool() const
	{
		return false;
	}
};

template<class I> 
class enumerator : public enumerator_base<I> {
	I i, b, e;
public:
	enumerator(I i, I e)
		: i(i), b(i), e(e)
	{ }
	enumerator(const enumerator&) = default;
	enumerator& operator=(const enumerator&) = default;
	~enumerator()
	{ }

	// input iterator implementation
	enumerator& operator++()
	{
		++i;

		return *this;
	}
	enumerator operator++(int)
	{
		enumerator i_(*this);

		++i;

		return i_;
	}
	reference operator*() const
	{
		return *i;
	}
    pointer operator->() const
	{
		return i.operator->();
	}
	bool operator==(const enumerator& i_) const
	{
		return i == i_.i && b == i_.b && e == i_.e;
	}
	bool operator!=(const enumerator& i_) const
	{
		return !operator==(i_);
	}

	// enumerator termination
	operator bool() const
	{
		return i != e;
	}

};
template<class I>
inline enumerator<I> make_enumerator(I b, I e)
{
	return enumerator<I>(b, e);
}

template<class T>
class enumerator_sequence : public enumerator_base<const T*> {
	T i, di;
public:
	enumerator_sequence(const T& i = 0, const T& di = 1)
		: i(i), di(di)
	{ }

	// input iterator implementation
	enumerator_sequence& operator++()
	{
		i += di;

		return *this;
	}
	enumerator_sequence operator++(int)
	{
		enumerator_sequence i_(*this);

		i += di;

		return i_;
	}
	reference operator*() const
	{
		return i;
	}
	bool operator==(const enumerator_sequence& i_) const
	{
		return i == i_.i && di == i_.di;
	}
	bool operator!=(const enumerator_sequence& i_) const
	{
		return !operator==(i_);
	}

};

// apply function to enumerator
template<class F, class E>
class apply_enumerator : public E {
	F f;
public:
	apply_enumerator(const F& f, const E& e)
		: f(f), E(e)
	{ }
	apply_enumerator(const apply_enumerator&) = default;
	apply_enumerator& operator=(const apply_enumerator&) = default;
	~apply_enumerator()
	{ }
	apply_enumerator& operator++()
	{
		E::operator++();

		return *this;
	}
	int operator*() const
	{
		return f(E::operator*());
	}
	operator bool() const
	{
		return E::operator bool();
	}
};

// enumerator from array
template<class T>
class enumerator_array : public std::iterator<std::input_iterator_tag,T> {
	size_t i, n;
	const T* t;
public:
	enumerator_array(size_t n = 0, const T* t = nullptr)
		: i(0), n(n), t(t)
	{ }
	enumerator_array(const enumerator_array&) = default;
	enumerator_array& operator=(const enumerator_array&) = default;
	~enumerator_array()
	{ }

	enumerator_array& operator++()
	{
		assert (i < n);

		++i;

		return *this;
	}
	const T& operator*() const 
	{
		return t[i];
	}
	operator bool() const
	{
		return i != n;
	}
};
// enumerator from iterator
template<class I>
class enumerator_iterator : public std::iterator<std::iterator_traits<I>::iterator_category,T> {
	I i, b, e;
public:
	enumerator_iterator(I b, I e)
		: i(b), b(b), e(e)
	{}
	enumerator_iterator(const enumerator_iterator&) = default;
	enumerator_iterator& operator=(const enumerator_iterator&) = default;
	~enumerator_iterator()
	{ }
	enumerator_iterator& operator++()
	{
		assert (i != e);

		++i;

		return *this;
	}
	typename std::iterator_traits<I>::reference operator*() const
	{
		return *i;
	}
	operator bool() const
	{
		return i != e;
	}
};
template<class I>
inline enumerator_iterator<I> make_enumerator_iterator(I b, I e)
{
	return enumerator_iterator<I>(b, e);
}


template<class I, class J>
class concatenate_enumerator {
	I i;
	J j;
public:
	typedef typename std::iterator_traits<I>::reference RI;
	typedef typename std::iterator_traits<J>::reference RJ;

	concatenate_enumerator(I i, J j)
		: i(i), j(j)
	{ }
	concatenate_enumerator(const concatenate_enumerator&) = default;
	concatenate_enumerator& operator=(const concatenate_enumerator&) = default;
	~concatenate_enumerator()
	{ }

	concatenate_enumerator& operator++()
	{
		i ? ++i : ++j;

		return *this;
	}
	typename std::common_type<RI,RJ>::type operator*() const
	{
		return i ? *i : *j;
	}
	operator bool() const
	{
		return i.operator bool() || j.operator bool();
	}
};
template<class E, class F>
inline concatenate_enumerator<E,F> make_concatenate_enumerator(E e, F f)
{
	return concatenate_enumerator<E,F>(e, f);
}
#endif // 0