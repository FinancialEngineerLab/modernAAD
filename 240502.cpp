#include <iostream>

#define ONE_DAY 0.003773585

template<class T>
class ContingentBond: public Product<T>
{
    Time myMaturity;
    double myCpn;
    double mySmooth;
    std::vector<Time> myTimeline;
    std::vector<SampleDef> myDefline;
    std::vector<std::string> myLabels;
    std::vector<double> myDt;
    
    public:
    ContingentBond(const Time maturity, const double cpn, const Time payFreq, const double smooth) :
        myMaturity(maturity), myCpn(cpn), mySmooth(smooth), myLabels(1)
        {
            myTimeline.push_back(systemTime);
            Time t = systemTime + payFreq;
            
            while(myMaturity - t > ONE_DAY)
            {
                myDt.push_back(t - myTimeline.back());
                myTimeline.push_back(t);
                t += payFreq;
            }
            
            myDt.push_back(myMaturity - myTimeline.back());
            myTimeline.push_back(myMaturity);
            
            const size_t n = myTimeline.size();
            myDefline.resize(n);
            for(size_t i = 0; i<n;++i)
            {
                myDefline[i].forwardMats.push_back(myTimeline[i]);
                if(i < n -1)
                {
                    myDefline[i].liborDefs.push_back(SampleDef::RateDef(myTimeline[i], myTimeline[i+1], "libor"));
                }
                myDefline[i].numeraire = i > 0;
            }
            ostringstream ost;
            ost.precision(2);
            ost << fixed;
            ost << "contingent bond " << myMaturity << " " << myCpn;
            myLabels[0] = ost.str();
        }
        
        unique_ptr<Product<T> > clone() const override
        {
            return make_unique<ContingentBond<T> >(*this);
        }
        
        const std::vector<Time>& timeline() const override
        {
            return myTimeline;
        }
        
        const std::vector<SampleDef>& defline() const override
        {
            return myDefline;
        }
        
        const std::vector<string>& payoffLabels() const override
        {
            return myLabels;
        }
        
        void payoffs(const Scenario<T>& path, std::vector<T>& payoffs) const override
        {
            const double smooth = double(path[0].forwards[0] * mySmooth), twoSmooth = 2 * smooth;
            
            const size_t n = path.size() - 1;
            payoffs[0] = 0;
            for(size_t i = 0; i<n; ++i)
            {
                const auto& start = path[i];
                const auto& end = path[i+1];
                const T s0 = start.forwards[0];
                const T s1 = end.forwards[0];
                
                T digital;
                
                if(s1 - s0 > smooth)
                {
                    digital = T(1.0);
                }
                else if (s1 - s0 < -smooth)
                {
                    digital = T(0.0);
                }
                else
                {
                    digital = (s1 - s0 + smooth) / twoSmooth;
                }
                
                payoffs[0] += digital * (start.libors[0] + myCpn) * myDt[i] / end.numeraire;
                
            }
            payoffs[0] += 1.0 / path.back().numeraire;
        }
};
