#include "netvar.h"

#include <cassert>
#include <cstdio>
// strtok
#include <cstring>

NetVarFlagsIterator NetVarFlagsIterator::next () {
	*this = _flags->_get_next_itr(*this);
	return *this;
}

NetVarFlagsIterator NetVarFlags::at (const size_t index) {
	if(index < _flag_count) {
		return NetVarFlagsIterator(this,
		                           _net_vars[index],
		                           index,
		                           flag_at(index));
	}

	return end();
}

NetVarFlagsIterator NetVarFlags::begin () {
	return at(0);
}

NetVarFlagsIterator NetVarFlags::end () {
	return NetVarFlagsIterator(this, nullptr, _flag_count, false);
}

bool NetVarFlags::flag_at (const size_t index) {
	return (_flags >> index) & 0x1;
}

NetVarFlagsIterator NetVarFlags::_get_next_itr (const NetVarFlagsIterator& itr) {
	const size_t index = itr.index;
	if(index < _flag_count) {
		return at(index + 1);
	}

	return end();
}

NetVarRaiseFlagCb NetVarFlags::add_var (BaseNetVar* var) {
	_net_vars.push_back(var);
	const size_t index = _flag_count++;
	return [=](){ _raise(index); };
}

void NetVarFlags::_raise (const size_t index) {
	assert(index < _flag_count);
	_flags |= (1 << index);
}

void NetVarFlags::reset_flags () {
	_flags = 0;
}

const std::string NetworkEntity::serialize_changed_state () {
	std::string serial_data = serial_header();

	// We only need to insert S_NETVAR_SEP
	bool var_flagged = false;
	for(auto itr = _flags.begin(); itr != _flags.end(); itr.next()) {
		if(itr.flagged) {
			if(!var_flagged)
				var_flagged = true;
			else
				serial_data += S_NETVAR_SEP;

			str_concat(serial_data,
			           std::to_string(itr.index),
			           S_NETVAR_SEP,
			           itr.net_var->serialize());
		}
	}

	serial_data += S_ENTITY_END;
	return serial_data;
}

#define MAX_SERIALIZATION_SIZE 256
const bool NetworkEntity::deserialize_state (const std::string& data) {
	SW_ASSERT(data.size() <= MAX_SERIALIZATION_SIZE);
	char d[MAX_SERIALIZATION_SIZE];
	strncpy(d, data.c_str(), MAX_SERIALIZATION_SIZE);
	char* token = strtok(d, S_NETVAR_SEP);

	do {
		size_t index = atoi(token);

		// get value to deserialize
		token = strtok(NULL, S_NETVAR_SEP);
		if(!_flags.at(index).net_var->deserialize(std::string(token)))
			return false;
		// get next index
		token = strtok(NULL, S_NETVAR_SEP);
	} while(NULL != token);

	_flags.reset_flags();

	return true;
}

const std::string NetworkEntity::serial_header () {
	std::string header;
	str_concat(header,
	           "NE",
	           S_ENTITY_HEADER_SEP,
	           std::to_string((size_t)type()),
	           S_ENTITY_HEADER_SEP,
	           std::to_string(_instance_id),
	           S_ENTITY_START);
	return header;
}
