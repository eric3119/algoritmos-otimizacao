/*
 * SampleDecoder.cpp
 *
 * For more information, see SampleDecoder.h
 *
 * Created on : Nov 17, 2011 by rtoso
 * Authors    : Rodrigo Franco Toso <rtoso@cs.rutgers.edu>
 *              Mauricio G.C. Resende <mgcr@research.att.com>
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018
 * Rodrigo Franco Toso (rfrancotoso@gmail.com) and
 * Mauricio G.C. Resende
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is furnished to do
 * so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include "SampleDecoder.h"

SampleDecoder::SampleDecoder() { }

SampleDecoder::~SampleDecoder() { }

// Runs in O(n \log n):
double SampleDecoder::decode(const std::vector< double >& chromosome) const {
	// for(unsigned i = 0; i < chromosome.size(); ++i) {
	// 	std::cout << static_cast<int>(chromosome[i] * 360) << "\n";
	// }

	httplib::Client cli("http://localhost:8080");

	std::stringstream fmt;
    fmt << "/antenna/simulate?phi1=" << static_cast<int>(chromosome[0] * 360)
		<< "&theta1=" << static_cast<int>(chromosome[1] * 360)
		<< "&phi2=" << static_cast<int>(chromosome[2] * 360) 
		<< "&theta2=" << static_cast<int>(chromosome[3] * 360)
		<< "&phi3=" << static_cast<int>(chromosome[4] * 360)
		<< "&theta3=" << static_cast<int>(chromosome[5] * 360);

	auto res = cli.Get(fmt.str().c_str());

    std::string s = res->body;
    std::string delimiter = "\n";
    std::string fitness = s.substr(0, s.find(delimiter));

	// std::cout << "\nfitness: " << fitness << "\n";

	double myFitness = -std::stod(fitness);

	// Return the fitness:
	return myFitness;
}
