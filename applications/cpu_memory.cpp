// cpu_memory.cpp: Monte Carlo Simulation to estimate the performance of different machine organizations
//
// Copyright (C) 2017-2020 Stillwater Supercomputing, Inc.
//
// This file is part of the HPR-MCMC project, which is released under an MIT Open Source license.
#include <iostream>
#include <iomanip>
#include <vector>
// include a number system of choice 
#include <universal/posit/posit>
#include <hpr-mcmc.hpp>


// readRate/writeRate give us some control over the workload model
template<typename Real>
Real vonNeumann_v1(Real readRate, Real writeRate) {
	constexpr Real instr = 1'000'000;
	constexpr Real memLat = 15;  // clocks
	const     Real reads = instr * readRate;
	const     Real writes = instr * writeRate;
	return instr +
		instr * memLat +
		reads * memLat +
	    writes * memLat;
}

template<typename Real>
Real harvard_v1(Real readRate, Real writeRate, Real l1InstrHitRate, Real l1DataHitRate) {
	constexpr Real instr  = 1'000'000;
	constexpr Real l1Lat  = 3;  // clocks
	constexpr Real memLat = 15;  // clocks
	const     Real reads  = instr * readRate;
	const     Real writes = instr * writeRate;
	return instr +
		instr * l1InstrHitRate*l1Lat +
		instr * (1.0f - l1InstrHitRate)*memLat +
		reads * l1DataHitRate*l1Lat +
		// we are assuming that writes are non-blocking
		reads * (1.0f - l1DataHitRate)*memLat;
}

template<typename Real>
Real cambridge_v1(Real readRate, Real writeRate, Real l1InstrHitRate, Real l1DataHitRate) {
	constexpr Real instr = 1'000'000;
	constexpr Real l1Lat = 3;  // clocks
	constexpr Real memLat = 15;  // clocks
	const     Real reads = instr * readRate;
	const     Real writes = instr * writeRate;
	return instr +
		instr * l1InstrHitRate*l1Lat +
		instr * (1.0f - l1InstrHitRate)*memLat +
		reads * l1DataHitRate*l1Lat +
		// we are assuming that writes are non-blocking
		reads * (1.0f - l1DataHitRate)*memLat;
}

int main()
{
	using namespace std;
	using namespace sw::mcmc;

	const int nrolls = 10000;  // number of experiments
	const int nstars = 100;    // maximum number of stars to distribute

	constexpr uint64_t nrTrails = 10;
	constexpr uint64_t nrDraws = 5; // 1'000'000;

	using Real = float;
	using Generator = std::mt19937_64;

	for (uint64_t i = 0; i < nrTrails; ++i) {
		Seed<Generator> seed;
		Generator rng(seed);
		std::normal_distribution<float> distr(0.8f, 0.05f);

		for (uint64_t j = 0; j < nrDraws; ++j) {
			cout << "vonNeumann : " << vonNeumann_v1(0.2f, 0.2f) << endl;
			Real l1InstrHitRate = distr(rng);
			Real l1DataHitRate = distr(rng);
			cout << "harvard    : " << harvard_v1(0.2f, 0.2f, l1InstrHitRate, l1DataHitRate) << endl;
			cout << "cambridge  : " << cambridge_v1(0.2f, 0.2f, l1InstrHitRate, l1DataHitRate) << endl;
		}
	}
}