#ifndef PSO_H
#define PSO_H
#include "StochOptimization.h"
#include <algorithm>

namespace stochopt {

	struct PSOBorders : public Borders
	{
		//PSOBorders() {} : Borders()
		PSOBorders(int nParam_, int min_, int max_, int vmax_);
		PSOBorders(int nParam_, std::vector<double> bound_, int vmax_);
		PSOBorders(int nParam_, std::vector<std::vector<double>> bound_, double vmax_);
		std::vector<int> flags; // 0 - no round-off, 1 - round-off, 2 - transform to binary
		std::vector<std::vector<double>> bound;
		std::vector<double> vmax; // 0 - no velocity clamping
		std::vector<double> bc; // boundary conditions
	};

	void discretize(std::vector<double>& x, const PSOBorders& borders);

	class PSO : public Algorithm
	{
	public:
		PSO(int nDim, int callsm, CostObject* costobj, PSOBorders bo, int nPart = 0, int snNeighborhoods = 1,
			double sw = 0.7298, double sc1 = 1.4962, double sc2 = 1.4962,
			int usesolution = 0, std::vector<double> solution = {},
			bool usegr = false, double sc3 = 0.1, std::vector<double>(*gG)(std::vector<double>&) = nullptr);

		void solve();

		void setAR(bool &a) { useAR = a; }
	protected:
		class particle;
		struct neighborhood;

		class particle : public entity
		{
		public:
			particle(int n, PSOBorders& borders, CostObject *costObject);
			particle(int n, PSOBorders& borders, CostObject *costObject, std::vector<double> initSol, int k, int nUseSol);

			std::vector<double> xBest;
			std::vector<double> v;
			double costBest = std::numeric_limits<double>::infinity();
			neighborhood *nhp; // pointer to the neighborhood in which this particle updates its velocity
		};

		struct neighborhood
		{
			neighborhood(std::vector<particle*> &p);
			void updateNeighborhoodsBP();

			std::vector<particle*> part; // particles in this neighborhood
			particle* bestParticle = nullptr; // best (according to history) particle in this neighborhood
		};

		std::vector<double> calculateMeanX();
		double calculateDiversity();
		void moveToBoundary(particle& p);
		void permuteSwarm();
		void setLocalNeighborhoods();
		void initialization();

		bool useGrad = false;
		bool useAR = false; // attractive-repulsive PSO
		double c1, c2, w, c3;
		int nNeighborhoods, pin;
		PSOBorders borders;

		std::vector<double>(*getGradient)(std::vector<double>&);
		std::vector<particle> swarm;
		std::vector<neighborhood> nh;

	private:

	};

}
#endif // PSO_H

