#include "series_operations.hpp"

class SeriesCos
	:public Series
{
public:
	SeriesCos() {}

	virtual double a(int n) override
	{
		if(n%2==1){
			return 0;
		}else{
			return pow(-1, n)/factorial(2*n);
		}
	}
};


Series *series_cos(Series *s)
{
    assert(s->is_variable());
    return new SeriesCos;
}
