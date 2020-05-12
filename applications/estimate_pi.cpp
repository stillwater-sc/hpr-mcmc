// estimate_pi.cpp: Monte Carlo Simulation to estimate the value of PI
//
// Copyright (C) 2017-2020 Stillwater Supercomputing, Inc.
//
// This file is part of the HPR-MCMC project, which is released under an MIT Open Source license.
#include <iostream>
#include <iomanip>
// include a number system of choice 
#include <universal/posit/posit>
// include the MCMC library
#include <hpr-mcmc.hpp>

// Author: Jerry Coffin
// https://codereview.stackexchange.com/questions/166447/monte-carlo-simulation-to-approximate-the-value-of-pi

template <typename Real>
class Point {
public:
	Point(Real x, Real y) :x(x), y(y) {}

	Real mag() { 
		return hypot(x, y); 
	}
private:
	Real x;
	Real y;
};

template <typename Real>
Real approximatePI(const Real& inCircle, const Real& total) noexcept {
    return ((inCircle / total) * Real(4.0));
}

template <typename Real>
Real percentError(const Real& computed, const Real& actual) noexcept {
     return Real(100.0) * ((computed - actual) / actual);
}


template <class Real>
void report(unsigned long long inCircle, unsigned long long total) {
    std::cout << "Points in circle: " << inCircle << '\n';
    std::cout << "Total points: " << total << '\n';
    std::cout << "Estimated Pi: " << std::setprecision(10) << approximatePI<Real>(Real(inCircle), Real(total)) << "\n";
}

int main()
{
	using namespace std;
	using namespace sw::mcmc;

    // Test Parameters
    constexpr int M = 10;			// Simulations per program execution
    constexpr int N = 2'000'000;	// Points per simulation

    unsigned long long overallPointsInCircle = 0;
    unsigned long long overallPoints = 0;

 	//using Real = sw::unum::posit<16, 1>;
	using Real = float;
    Real actualPI = 4 * atan(Real(1.0));
	using Generator = std::mt19937_64;

    for (int i = 0; i < M; ++i) {
        int pointsInCircle = 0;
        int totalPoints = 0;

		// generate a uniform distribution of doubles, which we'll cast to our own Reals
        Seed<Generator> seed;
		Generator rng(seed);
        std::uniform_real_distribution<float> uid(-1.0, 1.0);

        for (int j = 0; j < N; ++j) {
            Point<Real> p(uid(rng), uid(rng));

            ++totalPoints;
            ++overallPoints;

            if (p.mag() <= 1.0) {
                ++pointsInCircle;
                ++overallPointsInCircle;
            }
        }

        std::cout << "\nIteration: " << i + 1 << std::endl;
        report<Real>(pointsInCircle, totalPoints);
    }

    std::cout << "\n\nFinal values:\n";
    report<Real>(overallPointsInCircle, overallPoints);

    auto computedPi = approximatePI<Real>(Real(overallPointsInCircle), Real(overallPoints));
    std::cout << "Percent Error: " << std::setprecision(10) << percentError(computedPi, actualPI);
}