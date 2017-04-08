#include "time2.h"
#include <iostream>
#include <map>

int main(int argc, char *argv[])
{
	Date	date;
	std::multimap<Date, int>	map;

	map.insert(std::pair<Date,int>(date, 1));
	date += Time(1000000);
	map.insert(std::pair<Date,int>(date, 2));
	date += Time(3000000);
	map.insert(std::pair<Date,int>(date, 4));
	date -= Time(2000000);
	map.insert(std::pair<Date,int>(date, 3));


	std::multimap<Date, int>::iterator it = map.begin();
	while(it != map.end())
	{
		std::cout << time_t(it->first) << " : " << it->second << std::endl;
		it++;	
	}

}
