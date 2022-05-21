# ID_BIMAP

A custom map datastructure that was written as a home assigment for my Advanced C++ subject.

The id_bimap is a custom template datastructure that uses integral types as key. The default key type is long long.
The map is written to store values of different type than that of the key.
The values are stored in the map using std::optional.
The map is not allowed to store duplicate values.
The map can be created using an initializer list and it also has copy and move constructors.
The = operator realizes copy and move operations as well and checks if the parameter id_bimap contains values that can be copied.
The [] operator can be used to find pairs based on value or key, optionally throwing std::out_of_range and std::domain_error exceptions.
When inserting a new value, the map finds the first unused key and inserts the value at this position. If no available key is found, the map creates a new one at the end.
It is possible to querry if all of the keys are used and determine the next free key.
A variadic template function allows to create new pairs right inside the map based on passed arguments.
Elements of the map can be erased based on key or value.
Elements can be found and deleted based on unary predicates.
Additional capacity can be reserved for the map or the capacity can be decreased as long as the desired capacity does not affect the already stored elements.
A custom type string_id_bimap is provided that uses std::string as the type of the values.
A template alias kchar_id_bimap is provided that uses char as the type of the key.
