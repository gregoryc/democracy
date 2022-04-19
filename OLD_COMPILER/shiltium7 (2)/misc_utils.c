#include "headers.h"

#undef unsigned
#undef double

double pow(double significand_, const double exponent) {
	unsigned int e;
	long double return_value;
	
	if (significand_ == 0.0) {
		if (exponent > 0.0) {
			return 0.0;
		}
		
		if (exponent == 0.0) {
			return 1.0;
		}
		
		return 1.0 / significand_;
	}
	
	e = (int)exponent;
	if (exponent == (int)e) {
		if ((int)e < 0) {
			e = -e;
			significand_ = 1.0 / significand_;
		}
		
		return_value = 1.0;
		
		while (1) {
			if (e & 1) {
				return_value *= significand_;
			}
			
			if (!(e >>= 1)) {
				break;
			}
			
			significand_ *= significand_;
		}
		
		return return_value;
	}
	
	return exp(log(significand_) * exponent);
}
