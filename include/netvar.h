/**
 * NetVar implementation. The goal is to enable a simple means of identifying
 *     when the state of a given Entity has changed, while still prividing a 
 *     usable interface for individual variables, and providing a simple
 *     means of fetching the changed values.
 *
 * Author: spenserw<spenser@w2cs.com>
 *
 * Sources & inspiration:
 *    - Getters & setters: https://github.com/ValveSoftware/source-sdk-2013/
 */

#pragma once

#include <vector>
#include <cstdint>
#include <functional>
#include <cstring>
#include <string>

#include "entity.h"

// TODO 9-6-2019: this should be more robust
#define SERIALIZE_FUNC(type, value)		\
    const std::string serialize () {		\
	return std::to_string(value);		\
    }

#define DESERIALIZE_FUNC(type, value)					\
    const bool deserialize (const std::string& data) {			\
	const type new_value =						\
	    TOKEN_CAT(DESERIALIZE_, type)(data.c_str());		\
	value = new_value;						\
	return true;							\
    }

#define DESERIALIZE_float(value) atof(value)
#define DESERIALIZE_int(value) atoi(value)

#define NET_VAR_DEF(type)						\
    class NetworkVariable_##type : public BaseNetVar {			\
    public:								\
        type value;								\
        NetworkVariable_##type (NetVarFlags& flags, const float initial_value) { \
	    _raise_flag_func = flags.add_var(this);				\
	    value = initial_value;						\
	}								\
									\
	/* SETTERS / MODIFIERS */					\
	const type& set (const type& new_value) {			\
	    if(memcmp(&value, &new_value, sizeof(type))) {		\
		_raise_flag_func();					\
		value = new_value;					\
	    }								\
	    return value;						\
	}								\
	template<typename T>						\
	const type& operator=(const T& new_value) {			\
	    return set((const type) new_value);				\
	}								\
	template<typename T>						\
	const type& operator+=(const T& new_value) {			\
	    return set(value + (const type) new_value);			\
	}								\
	template<typename T>						\
	const type& operator-=(const T& new_value) {			\
	    return set(value - (const type) new_value);			\
	}								\
	template<typename T>						\
	const type& operator*=(const T& new_value) {			\
	    return set(value * (const type) new_value);			\
	}								\
	template<typename T>						\
	const type& operator/=(const T& new_value) {			\
	    return set(value / (const type) new_value);			\
	}							        \
									\
	const type& operator++() {				        \
	    return (*this += 1);					\
	}							        \
	const type& operator--() {				        \
	    return (*this -= 1);					\
	}								\
									\
	/* GETTERS */							\
	type& get_to_modify () {					\
	    _raise_flag_func();						\
	    return value;						\
	}								\
									\
	type& get () {							\
	    return value;						\
	}								\
	operator type&() {						\
	    return get();						\
	}								\
	const type* operator->() {					\
	    return &get();						\
	}								\
									\
	SERIALIZE_FUNC(type, value)					\
	DESERIALIZE_FUNC(type, value)					\
    }

#define NET_VAR_DEC(type, name, initial_value) \
    NetworkVariable_##type name = NetworkVariable_##type(_flags, initial_value);

typedef std::function<void()> NetVarRaiseFlagCb;

class BaseNetVar {
public:
    virtual const std::string serialize() = 0;
    virtual const bool deserialize(const std::string& data) = 0;
protected:
    NetVarRaiseFlagCb _raise_flag_func;
};

class NetVarFlags; // fwd dec
struct NetVarFlagsIterator {
    NetVarFlagsIterator(NetVarFlags* flags, BaseNetVar* net_var, size_t index, bool flagged)
	: net_var(net_var), index(index), flagged(flagged), _flags(flags) {}

    BaseNetVar* net_var;
    size_t index;
    bool flagged;

    NetVarFlagsIterator next();

private:
    NetVarFlags* _flags;
};

inline bool operator!= (const NetVarFlagsIterator& lhs, const NetVarFlagsIterator& rhs) {
    return
	lhs.net_var != rhs.net_var
	|| lhs.index != rhs.index
	|| lhs.flagged != rhs.flagged;
}

/*
 * NetVarFlags tracks a collection of network variables for changes.
 * A change is denoted via a flag being flipped at the given vars specified index.
 * NOTE: This only holds 64 flags.
 */
class NetVarFlags {
public:
    NetVarFlags()
	: _flag_count(0), _flags(0) {}

    NetVarFlagsIterator at(const size_t index);
    NetVarFlagsIterator begin();
    NetVarFlagsIterator end();

    bool flag_at(const size_t index);

    // returns cb to raise flag
    NetVarRaiseFlagCb add_var(BaseNetVar* net_var);

    void reset_flags();

    const size_t count();

protected:
    friend class NetVarFlagsIterator;
    NetVarFlagsIterator _get_next_itr(const NetVarFlagsIterator& itr);

    void _reset_flag(const NetVarFlagsIterator& itr);

private:
    void _raise(const size_t index);

    size_t _flag_count;

    std::vector<BaseNetVar*> _net_vars;
    uint64_t _flags;
};

/////////////////////////
// NetVar declarations //
/////////////////////////
NET_VAR_DEF(float);
NET_VAR_DEF(int);

//////////////////
// Network Entity
//////////////////

// Serialization separators
#define S_ENTITY_HEADER_SEP ":"
#define S_ENTITY_START "["
#define S_ENTITY_END "]"
#define S_NETVAR_SEP "_"

#define NET_ENTITY() NetVarFlags _flags;

class NetworkEntity : public Entity {
public:
    NET_ENTITY();
    NET_VAR_DEC(float, test, 32.0f);
    NET_VAR_DEC(int, test2, 555);
    NET_VAR_DEC(float, test3, -9999.0f);

    const std::string serialize();
    const std::string serialize_changed_state();

    const bool deserialize_state(const std::string& data);

private:
    const std::string serial_header();
};
