#pragma once

template<class T, class V>
void integrate(T &seed, std::function<T(const V&)> &func, const V &x1, const V &x2, double step)
{
	V dist = x2 - x1;
	double len = dist.getLength();
	V normal = dist / len;
	int steps = (int)ceil(len / step);
	double remainder = len - step * (steps - 1);
	V p1, p2;
	p2 = x1;
	for(int i = 1; i <= steps; i++)
	{
		p1 = p2;
		p2 = (i < steps) ? x1 + normal * (i * step): x2;
		double ds = (i < steps) ? step: remainder;
		seed += (func(p1) + func(p2)) * 0.5 * ds;
	}
}

template<class T, class V>
void integrate(T &seed, std::function<void(T&, const V&)> &func, const V &x1, const V &x2, double step)
{
	V dist = x2 - x1;
	double len = dist.getLength();
	V normal = dist / len;
	int steps = (int)ceil(len / step);
	double remainder = len - step * (steps - 1);
	V p1, p2;
	T v1, v2;
	p2 = x1;
	func(v2, p2);
	for(int i = 1; i <= steps; i++)
	{
		p1 = p2;
		func(v1, p1);
		p2 = (i < steps) ? x1 + normal * (i * step): x2;
		double ds = (i < steps) ? step: remainder;
		seed += (v1 + v2) * 0.5 * ds;
	}
}