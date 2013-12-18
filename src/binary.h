#include <math.h>
#include <inttypes.h>

size_t min_power_of_two(size_t n) {
	return pow(2, ceil(log(n)/log(2)));
}
