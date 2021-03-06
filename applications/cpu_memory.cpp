// cpu_memory.cpp: Monte Carlo Simulation to estimate the performance of different machine organizations
//
// Copyright (C) 2017-2020 Stillwater Supercomputing, Inc.
//
// This file is part of the HPR-MCMC project, which is released under an MIT Open Source license.
#include <iostream>
#include <iomanip>
#include <numeric>
#include <vector>
// include a number system of choice 
#include <universal/posit/posit>
#include <hpr-mcmc.hpp>

// System parameters
constexpr uint64_t instr = 1'000'000;
constexpr uint64_t l1Lat = 3;    // clocks
constexpr uint64_t l2Lat = 6;    // clocks
constexpr uint64_t l3Lat = 9;    // clocks
constexpr uint64_t memLat = 15;  // clocks

// readRate/writeRate give us some control over the workload model
template<typename Real>
Real vonNeumann_v1(Real readRate, Real writeRate) {
	const Real reads = instr * readRate;
	const Real writes = instr * writeRate;
	return instr +
		instr * memLat +
		reads * memLat +
	    writes * memLat;
}

template<typename Real>
Real harvard_v1(Real readRate, Real writeRate, Real l1InstrHitRate, Real l1DataHitRate, Real writeStallRate) {
	const Real reads  = instr * readRate;
	const Real writes = instr * writeRate;
	return instr +
		instr * l1InstrHitRate*l1Lat +
		instr * (1.0f - l1InstrHitRate)*memLat +
		reads * l1DataHitRate*l1Lat +
		writes * writeStallRate * memLat +
		reads * (1.0f - l1DataHitRate)*memLat;
}

template<typename Real>
Real cambridge_v1(Real readRate, Real writeRate, Real l1InstrHitRate, Real l1DataHitRate, Real writeStallRate) {
	const Real reads = instr * readRate;
	const Real writes = instr * writeRate;
	return instr +
		instr * l1InstrHitRate*l1Lat +
		instr * (1.0f - l1InstrHitRate)*memLat +
		reads * l1Lat +
		writes * writeStallRate * memLat;
}

// calculate the summary statistics of the samples
template<typename Real>
void summaryStats(const std::string& tag, const std::vector<Real>& samples) {
	using namespace std;
	Real mean(0.0f), sumOfSquares(0.0f), deviation(0.0f);
	Real sum(0.0f);
	sum = accumulate(samples.begin(), samples.end(), sum);
	mean = sum / Real(samples.size());
	for (auto v : samples) {
		sumOfSquares += (v - mean) * (v - mean);
	}
	deviation = sumOfSquares / Real(samples.size());
	cout << tag << " : " << mean << " +-" << deviation << "  total time\n";
	cout << "                       : " << mean / Real(instr) << " +-" << deviation / (Real(instr * instr)) << " clocks per instruction" << endl;
}

int main()
{
	using namespace std;
	using namespace sw::mcmc;

	constexpr uint64_t nrTrails = 10;
	constexpr uint64_t nrDraws = 5; // 1'000'000;

	using Real = float;
	using Generator = std::mt19937_64;

	// workload parameters
	constexpr Real readRate = 0.4f;  // a read rate of 40% implies that for every 2.5 instructions there is a read
	constexpr Real writeRate = 0.2f; // a write rate of 20% implies 1 write for every 5 instructions

	for (uint64_t i = 0; i < nrTrails; ++i) {
		Seed<Generator> seed;
		Generator rng(seed);
		std::normal_distribution<float> l1InstructionHitRateDistribution(0.95f, 0.05f);
		std::normal_distribution<float> l1DataHitRateDistribution(0.875f, 0.1f);
		std::normal_distribution<float> procWriteStallRateDistribution(0.875f, 0.1f);

		std::vector<Real> hdc(nrDraws);
		std::vector<Real> cam(nrDraws);
		for (uint64_t j = 0; j < nrDraws; ++j) {
			Real l1InstrHitRate = l1InstructionHitRateDistribution(rng);
			Real l1DataHitRate = l1DataHitRateDistribution(rng);
			Real writeStallRate = procWriteStallRateDistribution(rng);
			l1InstrHitRate = (l1InstrHitRate > 1.0f ? 1.0f : l1InstrHitRate);
			l1DataHitRate = (l1DataHitRate > 1.0f ? 1.0f : l1DataHitRate);
			writeStallRate = (writeStallRate > 1.0f ? 1.0f : writeStallRate);

			hdc[j] = harvard_v1(readRate, writeRate, l1InstrHitRate, l1DataHitRate, writeStallRate); // hdc: Harvard Dual Cache
			cam[j] = cambridge_v1(readRate, writeRate, l1InstrHitRate, l1DataHitRate, writeStallRate);

#ifdef DEBUG
			cout << "vonNeumann : " << spm[j] << endl;
			cout << "harvard    : " << hdc[j] << endl;
			cout << "cambridge  : " << cam[j] << endl;
#endif
		}
		std::vector<Real> spm(1);
		spm[0] = vonNeumann_v1(readRate, writeRate);
		// summarize
		summaryStats("Stored Program Machine", spm);
		summaryStats("Harvard Architecture  ", hdc);
		summaryStats("Cambridge Architecture", cam);
	}
}