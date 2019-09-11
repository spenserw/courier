#ifndef UTIL_H
#define UTIL_H

// memcpy()
#include <string.h>
// std integer types
#include <stdint.h>

/*
 * Definition flags:
 * SW_IMPLEMENTATION: implement resources, should only be done once
 * SW_NO_SHORT_NAMES: don't define shorter aliases (collision avoidance)
 * SW_NO_CPP_WRAP: don't define CPP convenience wrapper classes (cpp only)
 */

#if !defined(sw_remalloc) && !defined(sw_free)
#include <stdlib.h>
#define sw_realloc(p, s) realloc(p, s)
#define sw_free(p) free(p)
#endif

#ifndef NDEBUG
#include <assert.h>
#define SW_ASSERT(expr) assert(expr)
#else
#define SW_ASSERT(expr)
#endif

/////////////////
// MACRO UTILS
/////////////////
#define X_DEF(x, type) type x;

#define NARGS_LAST(_1, _2, _3, _4, _5, _6, _7, _8, N, ...) N
#define NARGS(...) NARGS_LAST(__VA_ARGS__, 8, 7, 6, 5, 4, 3, 2, 1)
#define TOKEN_CAT(a, b) a ## b
// TOKEN_CAT_WRAPPER necessary to delay concatenation until macros have been expanded
#define TOKEN_CAT_WRAPPER(a, b) TOKEN_CAT(a, b)

#define APPLY(macro, ...) TOKEN_CAT_WRAPPER(APPLY_, NARGS(__VA_ARGS__))(macro, __VA_ARGS__)
#define APPLY_1(m, arg1) m(arg1)
#define APPLY_2(m, arg1, arg2) m(arg1); m(arg2)
#define APPLY_3(m, arg1, arg2, arg3) m(x1); m(x2); m(x3)
#define APPLY_4(m, arg1, arg2, arg3, arg4) m(arg1); m(arg2); m(arg3); m(arg4)
#define APPLY_5(m, arg1, arg2, arg3, arg4, arg5) m(arg1); m(arg2); m(arg3); m(arg4); m(arg5)
#define APPLY_6(m, arg1, arg2, arg3, arg4, arg5, arg6) m(arg1); m(arg2); m(arg3); m(arg4); m(arg5); m(arg6)
#define APPLY_7(m, arg1, arg2, arg3, arg4, arg5, arg6, arg7) m(arg1); m(arg2); m(arg3); m(arg4); m(arg5); m(arg6); m(arg7)
#define APPLY_8(m, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8) m(arg1); m(arg2); m(arg3); m(arg4); m(arg5); m(arg6); m(arg7); m(arg8)

// APPLY supporting a constant argument
#define APPLY_CARG(macro, carg, ...) TOKEN_CAT_WRAPPER(APPLY_CARG_, NARGS(__VA_ARGS__))(macro, carg, __VA_ARGS__)
#define APPLY_CARG_1(m, carg, arg1) m(carg, arg1)
#define APPLY_CARG_2(m, carg, arg1, arg2) m(carg, arg1); m(carg, arg2)
#define APPLY_CARG_3(m, carg, arg1, arg2, arg3) m(carg, arg1); m(carg, arg2); m(carg, arg3)
#define APPLY_CARG_4(m, carg, arg1, arg2, arg3, arg4) m(carg, arg1); m(carg, arg2); m(carg, arg3); m(carg, arg4)
#define APPLY_CARG_5(m, carg, arg1, arg2, arg3, arg4, arg5) m(carg, arg1); m(carg, arg2); m(carg, arg3); m(carg, arg4); m(carg, arg5)
#define APPLY_CARG_6(m, carg, arg1, arg2, arg3, arg4, arg5, arg6) m(carg, arg1); m(carg, arg2); m(carg, arg3); m(carg, arg4); m(carg, arg5); m(carg, arg6)
#define APPLY_CARG_7(m, carg, arg1, arg2, arg3, arg4, arg5, arg6, arg7) m(carg, arg1); m(carg, arg2); m(carg, arg3); m(carg, arg4); m(carg, arg5); m(carg, arg6); m(carg, arg7)
#define APPLY_CARG_8(m, carg, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8) m(carg, arg1); m(carg, arg2); m(carg, arg3); m(carg, arg4); m(carg, arg5); m(carg, arg6); m(carg, arg7); m(carg, arg8)

//////////////////
// STRING HELPERS
//////////////////
// TODO 9-6-2019: Support more than just std::string. We can then remove the C++ check
#ifdef __cplusplus
#include <string>
#define _str_concat(str, s) str.append(s);
#define str_concat(str, ...) APPLY_CARG(_str_concat, str, __VA_ARGS__)

bool str_starts_with(std::string str, std::string prefix);
bool str_ends_with(std::string str, std::string postfix);
#ifdef SW_IMPLEMENTATION
// // TODO 9-6-2019: would this be better done with `strcmp`?
bool str_starts_with (std::string str, std::string prefix) {
    if(str == prefix)
	return true;

    for(size_t i = 0; i < str.length(); i++) {
	if(str[i] == prefix[i]) {
	    if(i == prefix.length() - 1)
		return true;
	} else
	    break;
    }
    return false;
}

bool str_ends_with (std::string str, std::string postfix) {
    if(str == postfix)
	return true;

    for(size_t i = str.length() - 1; i > 0; i--) {
	if(str[i] == postfix[i]) {
	    if(i == str.length() - postfix.length())
		return true;
	} else
	    break;
    }
    return false;
}
#endif
#endif

//////////
// VECTOR
//////////
#define VEC_MEMBERS_2(_, ...) _(x __VA_OPT__(,) __VA_ARGS__) _(y __VA_OPT__(,) __VA_ARGS__)
#define VEC_MEMBERS_3(_, ...) _(x __VA_OPT__(,) __VA_ARGS__) _(y __VA_OPT__(,) __VA_ARGS__) _(z __VA_OPT__(,) __VA_ARGS__)
#define VEC_MEMBERS_4(_, ...) _(x __VA_OPT__(,) __VA_ARGS__) _(y __VA_OPT__(,) __VA_ARGS__) _(z __VA_OPT__(,) __VA_ARGS__) _(w __VA_OPT__(,) __VA_ARGS__)

#define X_ADD_MEMBER_X(x, o) x += o.x;
#define X_SUB_MEMBER_X(x, o) x -= o.x;

#define X_MEMBER_X_ADD_X(x, o) o.x += x;
#define X_MEMBER_X_SUB_X(x, o) o.x -= x;
#define X_MEMBER_X_EQUALS_X(x, o) o.x = x;

#ifdef VECTOR_GLM_CASTS
    #include <glm/vec2.hpp>
    #include <glm/vec3.hpp>
    #include <glm/vec4.hpp>
    #define GLM_CAST(count)					    \
	operator glm::vec##count () {		                    \
	    glm::vec##count vec;				    \
	    VEC_MEMBERS_##count(X_MEMBER_X_EQUALS_X, vec)	    \
	    return vec;						    \
	}
#else
    #define GLM_CAST(count)
#endif

#ifndef SW_NO_SHORT_NAMES
#define VEC_SHORT_NAME(postfix) typedef Vector##postfix Vec##postfix
#else
#define VEC_SHORT_NAME(postfix)
#endif

#define VEC_DEF(type, count, postfix)				    \
    struct Vector##postfix {					    \
	VEC_MEMBERS_##count(X_DEF, type)			    \
	Vector##postfix operator+= (Vector##postfix& vec) {	    \
	    VEC_MEMBERS_##count(X_ADD_MEMBER_X, vec);		    \
	    return *this;					    \
	}                                                           \
	Vector##postfix operator-= (Vector##postfix& vec) {	    \
	    VEC_MEMBERS_##count(X_SUB_MEMBER_X, vec);		    \
	    return *this;					    \
	}                                                           \
								    \
	Vector##postfix operator+ (Vector##postfix& vec) {	    \
	    Vector##postfix res;				    \
	    VEC_MEMBERS_##count(X_MEMBER_X_ADD_X, res);		    \
	    return res;						    \
	}                                                           \
	Vector##postfix operator- (Vector##postfix& vec) {	    \
	    Vector##postfix res;				    \
	    VEC_MEMBERS_##count(X_MEMBER_X_SUB_X, res);		    \
	    return res;						    \
	}                                                           \
	GLM_CAST(count)						    \
    };							            \
    VEC_SHORT_NAME(postfix);
 
VEC_DEF(int, 2, 2i); // Vector2i
VEC_DEF(float, 2, 2f); // Vector2f

VEC_DEF(int, 3, 3i); // Vector3i
VEC_DEF(float, 3, 3f); // Vector3f

VEC_DEF(int, 4, 4i); // Vector4i
VEC_DEF(float, 4, 4f); // Vector4f
 
inline Vec4i i32_by_vec4 (int32_t i) {
    return Vec4i{i >> 24 & 0xff,
		 i >> 16 & 0xff,
		 i >> 8 & 0xff,
		 i & 0xff};
}

///////
// MAP
///////
// TODO 8-31-2019: handle dup keys

#define MAP_DEF(type) \
    class type##Map : public Map {		\
    public:					\
        type##Map()					\
	: Map(sizeof(type)) {}				\
	~type##Map() {					\
	    for(size_t i = 0; i < _count; i++) {	\
		get_at_index(i).~type();		\
	    }						\
	}								\
	type& insert (const char* key, type obj) {			\
	    auto item = new(_insertion_cursor_for_key(key)) type(obj);	\
	    return *item;						\
        }								\
	type& get (const char* key) {					\
	    return *static_cast<type*>(_get(key));			\
	}								\
	type& get_at_index (const size_t index) {			\
	    return *static_cast<type*>(_get_at_index(index));		\
        }								\
    };

class Map {
public:

    const size_t count () {
	return _count;
    }

    const bool contains (const char* key) {
	return _index_of_key(key) != -1;
    }

protected:
    Map(const int obj_size) {
	_obj_size = obj_size;
	_capacity = _initial_size();
	_chunk_size = _initial_size();

	_data = static_cast<char*>(malloc(_obj_size * _initial_size()));
	_keys = static_cast<char**>(malloc(sizeof(char*) * _initial_size()));
	
	_count = 0;
	for(size_t i = 0; i < _capacity; i++) {
	    _keys[i] = static_cast<char*>(malloc(MAX_KEY_LENGTH));
	}
    }

    ~Map() {
	for(size_t i = 0; i < _capacity; i++) {
	    free(_keys[i]);
	}
	free(_keys);
    }

    const int _index_of_key (const char* key) {
	int index = -1;
	for(size_t i = 0; i < _count; i++) {
	    if(strcmp(key, _keys[i]) == 0)
		index = i;
	}

	return index;
    }

    bool _resize (const size_t capacity) {
	// XXX should we be asserting this?? or is failure an option?
	SW_ASSERT(realloc(_data, capacity * _obj_size) != NULL);
	_capacity = capacity;
	return true;
    }

    /*
     * NOTE: we assume the cursor will be used to fill in data after return
     *       and therefore mark it as occupied
     * NOTE: overrides on key collision
     */
    char* _insertion_cursor_for_key (const char* key) {
        const int index = _index_of_key(key);
        size_t offset = 0;

	// key doesn't already exist
        if(index == -1) {
            if(_count++ >= _capacity) {
		_resize(_capacity + _chunk_size);
            }

            memcpy(_keys[_count - 1], key, strlen(key));
            offset = (_count - 1) * _obj_size;
        }
	// key already exists, override
	else {
            memcpy(_keys[index], key, strlen(key));
            offset = index * _obj_size;
        }

        char* cursor = _data + offset;
        return cursor;
    }

    void* _get (const char* key) {
	const int index = _index_of_key(key);
	return index == -1 ?
	    nullptr
	    : (void*)(_data + (index * _obj_size));
    }

    void* _get_at_index (const size_t index) {
	    return (void*)(_data + (index * _obj_size));
    }

    const virtual inline int _initial_size () {
	    return 10;
    }

    const int MAX_KEY_LENGTH = 32;
    
    size_t _obj_size;
    size_t _chunk_size;

    size_t _capacity;
    size_t _count;

    char* _data;
    char** _keys;
};

/////////////////
// DYNAMIC ARRAY
// Inspired by/shamelessly ripped off of Sean Barrett
/////////////////

#ifndef SW_NO_SHORT_NAMES
#define arr_len sw_arr_len
#define arr_cap sw_arr_cap
#define arr_set_cap sw_arr_set_cap
#define arr_set_len sw_arr_set_len
#define arr_push sw_arr_push
#define arr_pop sw_arr_pop
#define arr_tail sw_arr_tail
#define arr_free sw_arr_free
#endif

template<class T> static T * sw_arr_growf_wrapper(T *a, size_t elemsize, size_t addlen, size_t min_cap);
extern void* sw_arr_growf(void* a, const size_t element_size, const size_t add_len, size_t min_cap);
void sw_copy_arrf(void* dest, const void* src, const size_t element_size);

typedef struct {
    size_t length;
    size_t capacity;
    size_t element_size;
} sw_arr_header;
#define sw_header(o) ((sw_arr_header*) (o) - sizeof(sw_arr_header))

#define sw_arr_cap(a) (a ? sw_header(a)->capacity : 0)
#define sw_arr_len(a) (a ? sw_header(a)->length: 0)
#define sw_arr_set_cap(a, n) (sw_arr_grow(a, 0, n))
#define sw_arr_set_len(a, n) ((sw_arr_cap(a) < n ? sw_arr_set_cap(a, n) : 0), (a ? sw_header(a)->length = n : 0))

#define sw_arr_grow(a, s, c) ((a) = sw_arr_growf_wrapper(a, sizeof(*a), s, c))
#define sw_arr_maybe_grow(a, n) (((!a) || sw_arr_len(a) + n > sw_arr_cap(a)) \
				 ? (sw_arr_grow(a, n, 0),0) : 0)
#define sw_arr_push(a, v) (sw_arr_maybe_grow(a,1), (a)[sw_header(a)->length++] = (v))
#define sw_arr_pop(a) (sw_header(a)->length--, (a)[sw_header(a)->length])

#define sw_arr_tail(a) ((a)[sw_header(a)->length-1])

#define sw_arr_copy(a, b) (sw_arr_copy(a, b, sizeof(*a)))

#define sw_arr_free(a) sw_free(a)

#ifdef SW_IMPLEMENTATION

#ifdef __cplusplus
template<class T> static T * sw_arr_growf_wrapper (T *a, size_t elemsize, size_t addlen, size_t min_cap) {
  return (T*)sw_arr_growf((void *)a, elemsize, addlen, min_cap);
}
#else
#define sw_arr_growf_wrapper sw_arr_growf
#endif

void* sw_arr_growf (void* a, const size_t element_size, const size_t add_len, size_t min_cap) {
    void* new_arr;
    const size_t new_length = sw_arr_len(a) + add_len;
    if(new_length > min_cap)
	min_cap = new_length;

    if(min_cap <= sw_arr_cap(a))
	return a;

    // ammortized O(1) by cap sitcking to constant proportions
    if(min_cap < 2 * sw_arr_cap(a))
	min_cap = 2 * sw_arr_cap(a);

    new_arr = (char*) sw_realloc(a ? sw_header(a) : NULL,
			  element_size * min_cap + sizeof(sw_arr_header));
    // put header in the boot
    new_arr = (char*) new_arr + sizeof(sw_arr_header);

    if(NULL == a) {
	sw_header(new_arr)->length = 0;
    }
    sw_header(new_arr)->capacity = min_cap;
    return new_arr;
}

void sw_copy_arrf (void* dest, const void* src, const size_t element_size) {
    const size_t src_cap = sw_arr_cap(src);
    if(sw_arr_cap(dest) < src_cap)
	sw_arr_growf(dest, element_size, 0, src_cap);

    const size_t src_len = sw_arr_len(src);
    sw_header(dest)->length = src_len;
    const size_t bytes = src_len * element_size;
    memcpy(dest, src, bytes);
}
#endif

#if defined(__cplusplus) && !defined(SW_NO_CPP_WRAP)
#define DEF_ARRAY_ITERATOR(type, name)					\
    struct name##Iterator {						\
	size_t index;							\
	type* array;							\
									\
	name##Iterator (size_t index, type* array)			\
	    : index(index), array(array) {}				\
									\
	type& value () {						\
	    return array[index];					\
	}								\
	type* operator-> () {						\
	    return array + index;					\
	}								\
									\
	void next () {							\
	    index++;							\
	}								\
        void operator++ () {						\
	    next();							\
	}								\
									\
        bool operator== (const name##Iterator& itr) {			\
	    return index == itr.index					\
		&& array == itr.array;					\
	}								\
        bool operator!= (const name##Iterator itr) {			\
	    return !(*this == itr);					\
	}								\
    };									\

#define DEF_ARRAY(type, name)						\
    DEF_ARRAY_ITERATOR(type, name);					\
    class name {							\
	type* _arr;							\
    public:								\
	name () {}							\
	name (const size_t initial_capacity) {				\
	    sw_arr_set_cap(_arr, initial_capacity);			\
	}								\
	name (const name& arr) {					\
	    sw_copy_arrf(_arr, arr.data(), sizeof(*_arr));		\
	}								\
									\
	~name () {							\
	    sw_arr_free(_arr);						\
	}								\
									\
	void reserve (const size_t capacity) {				\
	    sw_arr_set_cap(_arr, capacity);				\
	}								\
									\
	type& pop () {							\
	    return sw_arr_pop(_arr);					\
	}								\
	void push (type& val) {						\
	    sw_arr_push(_arr, val);					\
	}								\
	name& operator= (const name& arr) {				\
	    sw_copy_arrf(_arr, arr.data(), sizeof(*_arr));		\
	    return *this;						\
	}								\
									\
	type& at (const size_t index) {					\
	    return _arr[index];						\
	}								\
	type& operator[] (const size_t index) {				\
	    return at(index);						\
	}								\
	type& tail () {							\
	    return sw_arr_tail(_arr);					\
	}								\
									\
	const name##Iterator begin () {					\
	    return name##Iterator(0, _arr);				\
	}								\
	name##Iterator end () {						\
	    return name##Iterator(sw_arr_len(_arr) - 1, _arr);		\
	}								\
									\
	const type * const data () const {				\
	    return _arr;						\
	}								\
    };

DEF_ARRAY(double, DoubleArray);
DEF_ARRAY(float, FloatArray);

DEF_ARRAY(uint8_t, UInt8Array);
DEF_ARRAY(uint16_t, UInt16Array);
DEF_ARRAY(uint32_t, UInt32Array);
DEF_ARRAY(uint64_t, UInt64Array);
DEF_ARRAY(size_t, SizeArray);

DEF_ARRAY(int8_t, Int8Array);
DEF_ARRAY(int16_t, Int16Array);
DEF_ARRAY(int32_t, Int32Array);
DEF_ARRAY(int64_t, Int64Array);
#endif


/////////
// COLOR
/////////
typedef Vector4f color;
inline color rgba_to_color (uint32_t rgba) {
    Vec4i color_bytes = i32_by_vec4(rgba);
    return Vec4f{static_cast<float>(color_bytes.x) / 0xff,
		 static_cast<float>(color_bytes.y) / 0xff,
		 static_cast<float>(color_bytes.z) / 0xff,
	         static_cast<float>(color_bytes.w) / 0xff};
}

inline color rgb_to_color (uint32_t rgb) {
    Vec4i color_bytes = i32_by_vec4(rgb << 8);
    return Vec4f{static_cast<float>(color_bytes.x) / 0xff,
		 static_cast<float>(color_bytes.y) / 0xff,
		 static_cast<float>(color_bytes.z) / 0xff,
	         0xff};
}
 
#endif
