// seed.hpp: seed generator for sampling experiments
//
// Copyright (C) 2017-2020 Stillwater Supercomputing, Inc.
//
// This file is part of the HPR-MCMC project, which is released under an MIT Open Source license.
#include <array>
#include <algorithm>
#include <iterator>
#include <random>

namespace sw {
namespace mcmc {

	// Generate a seed set for sampling experiments
	template <typename PseudoRandomNumberGenerator>
	class Seed {
		class seeder {
			std::array < std::random_device::result_type, PseudoRandomNumberGenerator::state_size > rand_data;
		public:
			seeder() {
				std::random_device rd;
				std::generate(rand_data.begin(), rand_data.end(), std::ref(rd));
			}

			typename std::array < std::random_device::result_type, PseudoRandomNumberGenerator::state_size >::iterator begin() { return rand_data.begin(); }
			typename std::array < std::random_device::result_type, PseudoRandomNumberGenerator::state_size >::iterator end() { return rand_data.end(); }
		} seed;

	public:
		Seed() : s(seed.begin(), seed.end()) { }

		template <typename I>
		auto generate(I a, I b) { return s.generate(std::forward<I>(a), std::forward<I>(b)); }

	private:
		std::seed_seq s;

	};
}
}
