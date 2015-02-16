# Enumerators

STL iterators are overly ambitious. They try to be useful for all sorts of algorithms
and end up being overly complicated. Enumerators are less enterprising. They are just
input iterators with an 'operator bool() const' for detecting the end of data.

We assume data generated via algorithms or database queries is immutable and just
waiting to be enumerated.  