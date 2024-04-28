#include <iostream>

#include "mcBase.hpp"

#define ONE_HOUR 0.000114469

template<class T>
class UOC: public Product<T>
{
    double myStrike;
    double myBarrier;
    Time myMaturity;
    double mySmooth;
    
    std::vector<Time> myTimeline;
    std::vector<SampleDef> myDefline;
    std::vector<std::string> myLabels;
    
    public:
    UOC(const double strike, const double barrier, const Time maturity, const Time monitorFreq, const double smooth) :
        myStrike(strike), myBarrier(barrier), myMaturity(maturity), mySmooth(smooth), maLabels(2)
        {
            myTimeline.push_back(systemTime);
            Time t = systemTime + monitorFreq;
            
            while(myMaturity - t > ONE_HOUR)
            {
                myTimeline.push_back(t);
                t += monitorFreq;
            }
            
            myTimeline.push_back(myMaturity);
            
            const size_t n = myTimeline.size();
            myDefline.resize(n);
            
            for(size_t i =0; i<n; ++i)
            {
                myDefline[i].numeraire = false;
                myDefline[i].forwardMats.push_back(myTimeline[i]);
            }
            
            myDefline.back().numeraire = true;
            
            ostringstream ost;
            ost.precision(2);
            ost << fixed;
            
            ost << "call " << myMaturity << " " << myStrike;
            myLabels[1] = ost.str();
            ost << " up and out "
                << myBarrier << " monitoring freq " << monitorFreq
                << " smooth " << mySmooth;
            myLabels[0] = ost.str();
        }
        
        std::unique_ptr<Product<T> > clone() const override
        {
            return std::make_unique< UOC<T> >(*this);
        }
        
        const std::vector<Time>& timeline() const override
        {
            return myTimeline;
        }
        
        const std::vector<SampleDef>& defline() const override
        {
            return myDefline;
        }
        
        const std::vector<std::string>& payoffLabels() const override
        {
            return myLabels;
        }
        
        void payoffs(cosnt Scenario<T>& path, std::vector<T>& payoffs) const override
        {
            const double smooth = double(path[0].forwards[0] * mySmooth), twoSmooth= 2 * smooth, barSmooth = myBarrier + smooth;
        };
        
        
        T alive(1.0);
        
        for(const auto& sample: path)
        {
            if (sample.forwards[0] > barSmooth)
            {
                alive = T(0.0);
                break;
            }
            
            if(sample.forwards[0] > myBarrier - smooth)
            {
                alive *= (barSmooth - sample.forwards[0]) / twoSmooth;
            }
        };
        
        // European
        payoffs[1] = max(path.back().forwards[0] - myStrike, 0.0) / path.back().numeraire;
        
        // barrier
        payoffs[0] = alive* payoffs[1];
        
        
}

int main()
{
    std::cout<<"Hello World";

    return 0;
}
