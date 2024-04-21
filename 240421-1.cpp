#include <iostream>

template<class T>
struct Sample
{
    T numeraire;
    std::vector<T> forwards;
    std::vector<T> discounts;
    std::vector<T> libors;
    
    void allocate(const SampleDef& data)
    {
        forwards.reszie(data.forwardMats.size());
        discounts.reszie(data.discountMats.size());
        libors.reszie(data.liborDefs.size());
    }
    
    void initialize()
    {
        numeraire = T(1.0);
        fill(forwards.begin(), forwards.end(), T(100.0));
        fill(discounts.begin(), discounts.end(), T(1.0));
        fill(libors.begin(), libors.end(), T(0.0));
        
    }
};

template<class T>
inline void allocatePath(const std::vector<SampleDef>& defline, Scenario<T>& path)
{
    path.resize(defline.size());
    for(size_t i = 0; i< defline.size(); ++i)
    {
        path[i].allocate(defline[i]);
    };
};

template<class T>
inline void initializePath(Scenario<T>& path)
{
    for(auto& scen : path)
    {
        scen.initialize();
    };
};

template<class T>
class Product
{
    public:
    virtual const std::vector<Time>& timeline() const = 0;
    virtual const std::vector<SampleDef>& defline() const = 0;
    virtual const std::vector<std::string>& payoffLabels() const = 0;
    virtual void payoffs(const Scenario<T>& path, std::vector<T>& payoffs) const = 0;
    virtual unique_ptr<Product<T> > clone() const = 0;
    virtual ~Product() {}
};
