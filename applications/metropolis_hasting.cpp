/*
 * RWMH with simple normal model
 */

#include <hprblas>

template<typename Real>
struct norm_data {
    Real sigma;
    mtl::dense_vector<Real> x;

    Real mu_0;
    Real sigma_0;
};

template<typename Real>
Real ll_dens(const mtl::dense_vector<Real>& vals_inp, norm_data<Real>& ll_data)
{
    const Real mu = vals_inp(0);
    const Real pi = 3.14 // arma::datum::pi;

    const Real sigma = ll_data.sigma;
    const mtl::dense_vector<Real> x = ll_data.x;

    const int nrElements = size(x);

    const Real ret = - Real(nrElements) * (0.5*std::log(2*pi) + std::log(sigma)) \
                       - std::accumulate( std::pow(x - mu,2) / (2*sigma*sigma) );

    return ret;
}

template<typename Real>
Real log_pr_dens(const mtl::dense_vector<Real>& vals_inp, norm_data<Real>& ll_data)
{
    const Real mu_0 = ll_data.mu_0;
    const Real sigma_0 = ll_data.sigma_0;
	const Real pi = 3.14; // arma::datum::pi;

    const Real x = vals_inp[0];

    const Real ret = - 0.5*std::log(2*pi) - std::log(sigma_0) - std::pow(x - mu_0,2) / (2*sigma_0*sigma_0);

    return ret;
}

double log_target_dens(const mtl::dense_vector<Real>& vals_inp, norm_data<Real>& ll_data)
{
    return ll_dens(vals_inp,ll_data) + log_pr_dens(vals_inp,ll_data);
}

int main()
{
	using Real = double;
    const int n_data = 100;
    const Real mu = 2.0;

    norm_data<Real> dta;
    dta.sigma = 1.0;
    dta.mu_0 = 1.0;
    dta.sigma_0 = 2.0;

	mtl::dense_vector<Real> x_dta = mu + arma::randn(n_data,1);
    dta.x = x_dta;

    mcmc::algo_settings_t settings;
    settings.rwmh_par_scale = 0.5;

	mtl::dense_vector<Real> initial_val(1);
    initial_val(0) = 1.0;

	mtl::dense_vector<Real> lb(1);
    // lb(0) = -arma::datum::inf;
    lb(0) = 0.0;

	mtl::dense_vector<Real> ub(1);
    ub(0) = 4.0;
    // ub(0) = arma::datum::inf;

    settings.vals_bound = true;
    settings.lower_bounds = lb;
    settings.upper_bounds = ub;

    mtl::dense2D<Real> draws_out;
    mcmc::rwmh(initial_val,draws_out,log_target_dens,&dta,settings);

    std::cout << "draws:\n" << draws_out.rows(0,9) << std::endl;

    std::cout << "acceptance rate = " << settings.rwmh_accept_rate << std::endl;

    std::cout << "mcmc mean = " << arma::mean(draws_out) << std::endl;

    return 0;
}