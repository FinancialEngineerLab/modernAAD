#include <iostream>

template<class T>
class Europeans : public Product<T>
{
    std::vector<Time> myMaturities;
    std::vector<std::vector<double> > myStrikes;
    std::vector<SampleDef> myDefline;
    std::vector<std::string> myLabels;
    
    public:
    Europeans(const std::map<Time, std::vector<double> >& options)
    {
        const size_t n = options.size();
        for(const std::pair<Time, std::vector<double> >& p : options)
        {
            myMaturities.push_back(p.first);
            myStrikes.push_back(p.second);
        }
        
        myDefline.resize(n);
        
        for(size_t i =0; i<n;++i)
        {
            myDefline[i].numeraire = true;
            myDefline[i].forwardMats.push_back(myMaturities[i]);
        }
        
        for(const auto& option : options)
        {
            for(const auto& strike : option.second)
            {
                ostringstream ost;
                ost.precision(2);
                ost << fixed;
                ost << "call " << option.first << " " << strike;
                myLabels.push_back(ost.str());
            }
        }
    }
    
    // overriding functions //
    std::unique_ptr<Product<T> > clone() const override
    {
        return std::make_unique<Euroepeans<T> >(*this);
    }
    
    const std::vector<Time>& maturities() const
    {
        return myMaturities;
    }
    
    const std::vector<std::vector<double> >& stirkes() const
    {
        return myStrikes;
    }
    
    const std::vector<Time>& timeline() const override
    {
        return myMaturities;
    }
    
    const std::vector<SampleDef>& defline() const override
    {
        return myDefline;
    }
    
    const std::vector<std::string>& payoffLabels() const override
    {
        return myLabels;
    }
    
    void payoffs(const Scenario<T>& path, std::vector<T>& payoffs) const override
    {
        const size_t numT = myMaturities.size();
        auto payoffIt = payoffs.begin();
        for(size_t i = 0; i < numT; ++i)
        {
            std::transform(myStrikes[i].begin(), myStrikes[i].end(), payoffIt,
                        [spot = path[i].forwards[0], num = path[i].numeraire](const double& k)
                        {
                            return std::max(spot - k, 0.0) / num;
                        }
                        );
        payoffIt += myStrikes[i].size();
        }
    }
};
