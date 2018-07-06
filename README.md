# adjacent_range
Iterator and range classes for iterating over groups of adjacent items

These classes are inspired by this [Stack Overflow answer](https://stackoverflow.com/a/14829088) and this [lightning talk](https://youtu.be/aghnpqkvdCg). The result is an adjacent_iterator to loop over N consecutive items. For the special case where `N=2`, the iterators are stored in a `std::pair`, so access is possible via the members `first` and `second`. For the other cases, `N!=2` , access is required to use the `std::get<>` function.

###### Example
```
for (const auto& p : make_adjacent_range<2>(v)) {
  do_something(p.first, p.second);
}
```
