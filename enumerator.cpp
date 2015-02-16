// enumerator.cpp
#include <cassert>
#include <list>
#include <vector>
#include "enumerator.h"

using namespace std;

template<class I>
void test_enumerator()
{
	I a = {1, 2, 3};
	auto b = begin(a);
	auto e = end(a);

	auto i = enumerator(b,e);
	auto i2(i);
	assert (i2 == i);
	assert (!(i2 != i));
//	++i2;
	assert (++i2 != i);
	++i;
	assert (i2 == i);

	i = enumerator(b,e);
	i2 = i;
	assert (i2 == i);
	assert (!(i2 != i));
	++i2;
	assert (i2 != i);
	++i;
	assert (i2 == i);

	i = enumerator(b,e);
	assert (i);
	assert (*i == 1);
	assert (*++i == 2);
	assert (*++i == 3);
	assert (!++i);

	i = enumerator(a);
	assert (i);
	assert (*i == 1);
	assert (*++i == 2);
	assert (*++i == 3);
	assert (!++i);
}
void test_apply_enumerator()
{
	std::vector<int> a = {1,2,3};
	auto ea = enumerator(a.begin(), a.end());
	auto f = [](int i) { return i*i; };
	auto fa = make_apply_enumerator(f, ea);
	
	assert (*fa == 1);
	assert (*++fa == 4);
	assert (*++fa == 9);
	assert (!++fa);

}
void test_concatenate_enumerator()
{
	vector<int> a = {1,2,3};
	auto a_ = enumerator(a);
	std::reverse(a.begin(), a.end());
	auto aa = make_concatenate_enumerator(a_, enumerator(a));
	// a_ points at reversed vector now
	assert (*aa == 3);
	assert (*++aa == 2);
	assert (*++aa == 1);
	assert (aa && *++aa == 3);
	assert (*++aa == 2);
	assert (*++aa == 1);
	assert (!++aa);
}

int main()
{
//	test_enumerator<int[]>();
	test_enumerator<list<int>>();
	test_enumerator<vector<int>>();

	test_drop();
	test_take();

	test_enumerator_sequence();

	test_apply_enumerator();
	test_concatenate_enumerator();

	return 0;
}