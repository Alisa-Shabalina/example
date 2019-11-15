#include "..\include\pso.h"

namespace stochopt {


	PSOBorders::PSOBorders(int nParam_, int min_, int max_, int vmax_)
	{
		std::vector<double> xmin1(nParam_, min_);
		std::vector<double> xmax1(nParam_, max_);
		std::vector<int> flags1(nParam_, 0);
		std::vector<double> vmax1(nParam_, vmax_);
		std::vector<double> bc1(nParam_, -0.75);

		xmin = xmin1;
		xmax = xmax1;
		flags = flags1;
		vmax = vmax1;
		bc = bc1;
	}
	PSOBorders::PSOBorders(int nParam_, std::vector<double> bound_, int vmax_)
	{
		double min_ = bound_.front();
		double max_ = bound_.back();

		std::vector<double> xmin1(nParam_, min_);
		std::vector<double> xmax1(nParam_, max_);
		std::vector<int> flags1(nParam_, 1);
		std::vector<std::vector<double>> bound1(nParam_, bound_);
		std::vector<double> vmax1(nParam_, vmax_);
		std::vector<double> bc1(nParam_, -0.75);

		xmin = xmin1;
		xmax = xmax1;
		flags = flags1;
		bound = bound1;
		vmax = vmax1;
		bc = bc1;
	}
	PSOBorders::PSOBorders(int nParam_, std::vector<std::vector<double>> bound_, double vmax_)
	{
		std::vector<double> xmin1(nParam_, 0.0);
		std::vector<double> xmax1(nParam_, 0.0);
		std::vector<int> flags1(nParam_, 1);
		std::vector<std::vector<double>> bound1(nParam_, { 0.0,1.0 });
		std::vector<double> vmax1(nParam_, vmax_);
		std::vector<double> bc1(nParam_, -0.75);
		int ind = 1;
		//if (ColeGl == 1) {
		//	ind = 5;
		//}
		//if (ColeGl == 0) {
		//	ind = 2;
		//}
		for (int i = 0; i <= int(nParam_ / ind); i++) {
			if (i != int(nParam_ / ind)) {
				for (int k = 0; k < ind; k++) {
					xmin1[ind*i + k] = bound_[ind*i + k][0];
					xmax1[ind*i + k] = bound_[ind*i + k][1];
					bound1[ind*i + k] = bound_[ind*i + k];
				}
			}
			if (i == int(nParam_ / ind)) {
				for (int k = 0; k < ind - 1; k++) {
					xmin1[ind*i + k] = bound_[ind*i + k][0];
					xmax1[ind*i + k] = bound_[ind*i + k][1];
					bound1[ind*i + k] = bound_[ind*i + k];
				}

			}



		}

		xmin = xmin1;
		xmax = xmax1;
		flags = flags1;
		bound = bound1;
		vmax = vmax1;
		bc = bc1;
	}

	void discretize(std::vector<double>& x, const PSOBorders& borders) {
		// round-offs continious elements from x to discrete set of parameters
		// (i.e. price-list of pipes' diameters)
		double x0;

		unsigned int nDim = x.size();
		for (unsigned int i = 0; i < nDim; ++i) {
			x0 = x[i];

			if (x0 > borders.xmax[i]) {
				x0 = borders.xmax[i];
			}
			else if (x0 < borders.xmin[i]) {
				x0 = borders.xmin[i];
			}

			if (borders.flags[i] == 1) {
				std::vector<double> discarray = borders.bound[i];
				for (unsigned int j = 0; j < discarray.size() - 1; ++j) {
					double d1 = x0 - discarray[j];
					double d2 = x0 - discarray[j + 1];
					if (d1*d2 <= 0) {
						(abs(d1) < abs(d2)) ? (x0 = discarray[j]) : (x0 = discarray[j + 1]);
					}
				}
			}

			x[i] = x0;
		}
	}

	PSO::PSO(int nDim, int callsm, CostObject* costobj, PSOBorders bo, int nPart, int snNeighborhoods,
		double sw, double sc1, double sc2,
		int usesolution, std::vector<double> solution,
		bool usegr, double sc3, std::vector<double>(*gG)(std::vector<double>&)) :
		Algorithm(nDim, callsm, costobj),
		useGrad(usegr), useAR(false), c1(sc1), c2(sc2), w(sw), c3(sc3),
		nNeighborhoods(snNeighborhoods), pin(0), borders(bo)
	{
		getGradient = gG;
		nParticles = nPart;
		costBestSolution = std::numeric_limits<double>::infinity();
		calls = 0;
		useSol = usesolution;
		initSol = solution;
		useStop = false;

		if (nPart == 0)
		{
			nParticles = nDimensions;
		}
		initialization();
	}

	void PSO::solve()  // PSO iterations
	{
		int t = 0; //iteration number
		std::srand((unsigned int)time(0));

		while (!isConverged())
		{
			++t;
			// swarm permutation
			permuteSwarm();
			for (auto &ni : nh)
			{
				ni.updateNeighborhoodsBP();
			}

			if (useAR == true) {
				double div = calculateDiversity();
				if (c1 > 0 && div < 5e-6) {
					c1 *= -1;
					c2 *= -1;
				}
				else if (c1 < 0 && div > 0.25) {
					c1 *= -1;
					c2 *= -1;
				}
			}

			// updating velocities
	//#pragma omp parallel for
			for (int i = 0; i < nParticles; ++i) {
				particle &p = swarm[i];
				std::vector<double> &socialDirection = p.nhp->bestParticle->xBest;
				double add1, add2;
				for (int i = 0; i < nDimensions; ++i) {
					add1 = c1 * stochopt::random()*(p.xBest[i] - p.x[i]);
					add2 = c2 * stochopt::random()*(socialDirection[i] - p.x[i]);
					//add3 = c3*r3*...

					p.v[i] = w * p.v[i] + add1 + add2;
				}

			}

			//updating x
	//#pragma omp parallel for
			for (int i = 0; i < nParticles; ++i)
			{
				particle &p = swarm[i];
				moveToBoundary(p);

				p.cost = costObject->getCost(p.x);

				if (p.cost <= p.costBest)
				{
					p.costBest = p.cost;
					p.xBest = p.x;
				}
				++calls;
			}

			for (auto &p : swarm)
			{
				double costp = p.costBest;
				if (costp <= costBestSolution)
				{
					costBestSolution = costp;
					xBestSolution = p.xBest;
				}
			}

			costGraph.push_back(-costBestSolution);
			xCostGraph.push_back(xBestSolution);
			callsGraph.push_back(calls);
			if (costGraph.size() >= 3) {
				double residual = std::abs(costGraph.back() - 
					*std::next(costGraph.begin(), 2 * costGraph.size() / 3));
				residuals.push_back(std::move(residual));
			}

			if (useStop == true) {
				auto mid = std::next(costGraph.begin(), costGraph.size() / 2);
				if ((abs(costBestSolution - *mid) < 1e-5 * abs(costBestSolution)) && 
					(t > 30)) {
					break;
				}
			}

		} // end of calculations

	}

	PSO::particle::particle(int n, PSOBorders& borders, CostObject *costObject)
	{
		x.resize(n);
		v.resize(n);
		for (int i = 0; i < n; ++i)
		{
			double bxmax = borders.xmax[i];
			double bxmin = borders.xmin[i];
			double bvmax = borders.vmax[i];
			x[i] = stochopt::random() * (bxmax - bxmin) + bxmin;
			if (borders.vmax[i] != 0)
			{
				v[i] = 2 * (stochopt::random() - 0.5) * bvmax;
			}
			else
			{
				v[i] = 2 * (stochopt::random() - 0.5) * (bxmax - bxmin) / 10;
			}
		}
		xBest = x;
		cost = costObject->getCost(x);
		costBest = cost;
	}
	PSO::particle::particle(int n, PSOBorders& borders, CostObject *costObject, std::vector<double> initSol, int k, int nUseSol)
	{
		x.resize(n);
		v.resize(n);
		for (int i = 0; i < n; ++i)
		{
			double x0 = initSol[i];
			double bxmax = borders.xmax[i];
			double bxmin = borders.xmin[i];
			double bvmax = borders.vmax[i];
			double delta = (std::abs(x0 - bxmax) < std::abs(x0 - bxmin)) ? std::abs(x0 - bxmax) : std::abs(x0 - bxmin);

			if (nUseSol == 1) {
				x[i] = x0;
			}
			else {
				x[i] = x0 + 0.1*(stochopt::random() - 0.5) * 2 * delta * k / (nUseSol - 1.0);
			}

			if (bvmax != 0)
			{
				v[i] = 2 * (stochopt::random() - 0.5) * bvmax;
			}
			else
			{
				v[i] = 2 * (stochopt::random() - 0.5) * (bxmax - bxmin) / 10;
			}
		}

		xBest = x;
		cost = costObject->getCost(x);

		while (!isEqual(xBest, x)) {
			x = xBest;
			cost = costObject->getCost(x);
		}
		costBest = cost;
	}

	PSO::neighborhood::neighborhood(std::vector<particle*> &p)
	{
		part = p;
		updateNeighborhoodsBP();
	}
	void PSO::neighborhood::updateNeighborhoodsBP()
	{ // updates neighborhood's best particle
		double costBest = std::numeric_limits<double>::infinity();
		for (auto a : part)
		{
			if (a->costBest <= costBest)
			{
				bestParticle = a;
				costBest = a->costBest;
			}
		}
	}

	std::vector<double> PSO::calculateMeanX() {
		// calculates mean vector of all particles's current positions
		std::vector<double> mean(nDimensions);

		for (int i = 0; i < nDimensions; ++i) {
			double sum = 0;
			for (auto &p : swarm) {
				sum += p.x.at(i);
			}
			mean.at(i) = sum / nDimensions;
		}
		return mean;
	}

	double PSO::calculateDiversity() {
		// calculates diversity parameter (for ARPSO)
		std::vector<double> mean = calculateMeanX();

		double diversity = 0;

		for (auto &p : swarm) {
			double add = 0;
			for (int i = 0; i < nDimensions; ++i) {
				add += (p.x.at(i) - mean.at(i))*(p.x.at(i) - mean.at(i));
			}
			diversity += sqrt(add);
		}

		// calculating L2 norm of parameter space
		// consider calculating it once and storing for speed-up
		double borderNorm = 0;
		for (int i = 0; i < nDimensions; ++i) {
			borderNorm += (borders.xmax.at(i) - borders.xmin.at(i))*(borders.xmax.at(i) - borders.xmin.at(i));
		}
		borderNorm = sqrt(borderNorm);

		diversity /= (nParticles * borderNorm);

		return diversity;
	}

	void PSO::moveToBoundary(particle& p) {
		double x0;
		// checking boundary conditions
		for (int i = 0; i < nDimensions; ++i) {
			x0 = p.x[i] + p.v[i];

			if (x0 > borders.xmax[i]) {
				if (borders.bc[i] <= 0) { // reflective boundary condition
					x0 = borders.xmax[i];
				}
				else { // periodic boundary condition
					x0 = borders.xmin[i];
				}
				p.v[i] = borders.bc[i] * p.v[i];
			}
			else if (x0 < borders.xmin[i]) {
				if (borders.bc[i] <= 0) { // reflective boundary condition
					x0 = borders.xmin[i];
				}
				else { // periodic boundary condition
					x0 = borders.xmax[i];
				}
				p.v[i] = borders.bc[i] * p.v[i];
			}

			p.x[i] = x0;
		}
	}

	void PSO::permuteSwarm()
	{
		//std::random_device rd;
		//std::mt19937 g(rd());
		//std::shuffle(swarm.begin(), swarm.end(), g);
		//setLocalNeighborhoods();
		random_shuffle(swarm.begin(), swarm.end());
		setLocalNeighborhoods();
	}

	void PSO::setLocalNeighborhoods()
	{
		for (int i = 0; i < nNeighborhoods; ++i)
		{
			for (int j = i * pin; j < (i + 1)*pin; ++j)
			{
				swarm[j].nhp = &nh[i];
			}
		}

		// with following code some particles (<pin-1) remain invisible during update
		// other particles from their respective swarm[..].nhp neighborhoods don't "see" them
		// these particles are different on every iteration due to swarm permutation
		// in practice, it even could be beneficial in terms of stochastic behavior
		// moreover, every neighborhood has the same size

		if (nNeighborhoods*pin < nParticles) {
			for (int k = nParticles; k > nNeighborhoods*pin; --k) {
				swarm[k - 1].nhp = &nh[nNeighborhoods - 1];
			}
		}
	}

	void PSO::initialization()
	{
		swarm.reserve(nParticles);
		if (useSol != 0) {
			for (int i = 0; i < useSol; ++i)
			{
				swarm.push_back(particle(nDimensions, borders, costObject, initSol, i, useSol));
			}
		}
		for (int i = 0; i < nParticles - useSol; ++i)
		{
			swarm.push_back(particle(nDimensions, borders, costObject));
		}
		// Building up neighborhoods according to topology type
		// local topology
		nh.clear();

		pin = nParticles / nNeighborhoods; // particles in neighborhood
		for (int i = 0; i < nNeighborhoods; ++i)
		{ // number of the neighborhood
			std::vector<particle*> particles;
			for (int j = i * pin; j < (i + 1)*pin; ++j)
			{
				particles.push_back(&swarm.at(j));
			}
			nh.push_back(neighborhood(particles));
		}

		setLocalNeighborhoods();

	}

}
