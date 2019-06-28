#ifndef SYMPHONY4_CALLCHAIN_H
#define SYMPHONY4_CALLCHAIN_H

struct CallChain {
	typedef bool result_type;

	template<typename InputIterator>
	result_type operator()(InputIterator first, InputIterator last) const
	{
		while (first != last) {
			if (*first)
				return true;
			++first;
		}
		return false;
	}
};

#endif