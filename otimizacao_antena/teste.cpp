#include <iostream>
#include <iomanip>
#include "httplib.h"

#define FIXED_FLOAT(x) std::fixed <<std::setprecision(15)<<(x) 

int main(int argc, char const *argv[])
{
    httplib::Client cli("http://localhost:8080");

    std::stringstream fmt;
    fmt << "/antenna/simulate?phi1=" << 90 << "&theta1=" << 90 << "&phi2=" << 90 << "&theta2=" << 90 << "&phi3=" << 90 << "&theta3=" << 90;

	auto res = cli.Get(fmt.str().c_str());

	// std::cout << res->status<< "\n"; // 200
	// std::cout << res->body << "\n";
    std::string s = res->body;
    std::string delimiter = "\n";
    std::string token = s.substr(0, s.find(delimiter)); // token is "scott"
    std::cout << FIXED_FLOAT(std::stod(token)) << "\n";

    return 0;
}
