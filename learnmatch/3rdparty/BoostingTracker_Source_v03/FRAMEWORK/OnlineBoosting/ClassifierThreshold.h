#pragma once

#include "EstimatedGaussDistribution.h"
#include <math.h>

namespace OnlineBoosting
{

class ClassifierThreshold 
{
public:

	ClassifierThreshold();
	virtual ~ClassifierThreshold();

	void update(float value, int target);
	int eval(float value);

	void* getDistribution(int target);

private:

	EstimatedGaussDistribution* m_posSamples;
	EstimatedGaussDistribution* m_negSamples;

	float m_threshold;
	int m_parity;
};

}
