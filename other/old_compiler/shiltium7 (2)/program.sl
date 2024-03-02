/*
factorial(u n:0..20 -> u) memoize {n: 0..20} {
	if !n {
		return 1;
	}
	return n * factorial(n - 1);
}
*/

f x = 0.3 - 4.0 + 5.0 * 3.0 - 1.0 + 5.65 * 2.0 - 3.0;
output x;
