#include <iostream>

template<class T>
class European: public Product<T>
{
    double myStrike;
    Time myExerciseDate;
    Time mySettlementDate;
    
    std::vector<Time> myTimeline;
    std::vector<SampleDef> myDefline;
    std::vector<string> myLabels;
  
    public:
    European(const double strike, const Time exerciseDate, const Time settlementDate):
        myStrike(strike), myExerciseDate(exerciseDate), mySettlementDate(settlementDate), myLabels(1)
        {
            myTimeline.push_back(exerciseDate);
            myDefline.resize(1);
            myDefline[0].numeraire = true;
            myDefline[0].forwardMats.push_back(settlementDate);
            myDefline[0].discountMats.push_back(settlementDate);
            
            ostringstream ost;
            ost.precision(2);
            ost << fixed;
            if (settlementDate == exerciseDate)
            {
                ost << "call" << myStrike << " " << exerciseDate;
            }
            else
            {
                ost << "call" << myStrike << " " << exerciseDate << " " << settlementDate;
            }
            myLabels[0] = ost.str();
        }
        
        European(const double strike, cosnt Time exerciseDate): 
            European(strike, exerciseDate, exerciseDate) {}
            
        unique_ptr<Product<T> > clone() const override
        {
            return make_unique<European<T> >(*this);
        };
        
        const std::vector<Time>& timeline() const override
        {
            return myTimeline;
        };
        
        const std::vector<SampleDef>& defline() const override
        {
            return myDefline;
        };
        
        const std::vector<std::string>& payoffLabels() const override
        {
            return myLabels;
        }
        
        void payoffs(const Scenario<T>& path, std::vector<T>& payoffs) const override
        {
            payoffs[0] = std::max(path[0].forwards[0] - myStrike, 0.0) * path[0].discounts[0] / path[0].numeraire;
        }
};

