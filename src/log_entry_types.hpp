#ifndef LOG_ENTRY_TYPES_HPP
#define LOG_ENTRY_TYPES_HPP

enum class log_entry_types : uint8_t {
	gravity_vector = 1,
	photo_event = 2,
	startup = 10
};

using gravity_vector_t = int16_t[3];

#endif // LOG_ENTRY_TYPES_HPP
