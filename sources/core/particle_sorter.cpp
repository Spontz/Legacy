#include <algorithm>

#include "../main.h"

// aqui debes devolver si p1 esta mas cerca que p2
bool operator<(const particle_reference& p1, const particle_reference& p2)
	{
	return p1.m_TransformedPosition.z < p2.m_TransformedPosition.z;
	}

extern "C" {

void sort_particles(particle_reference* pFirst, unsigned int count)
	{
	std::sort(pFirst, pFirst+count);
	}

}
