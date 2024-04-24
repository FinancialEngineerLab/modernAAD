#include <iostream>

inline std::vector<std::vector<double> > mcSimul(const Product<double>& prd,
                                                const Model<double>& mdl,
                                                const RNG& rng,
                                                const size_t nPath)
                                                {
                                                    auto cMdl = mdl.clone();
                                                    auto cRng = rng.clone();
                                                    
                                                    const size_t nPay = prd.payoffLabels().size();
                                                    std::vector<std::vector<double> > results(nPath, std::vector<double>(nPay));
                                                    
                                                    cMdl->allocate(prd.timeline(), prd.defline());
                                                    cMdl->init(prd.timeline(), prd.defline());
                                                    cRng->init(cMdl->simDim());
                                                    
                                                    std::vector<double> gaussVec(cMdl->simDim());
                                                    Scenario<double> path;
                                                    allocatePath(prd.defline(), path);
                                                    initializePath(path);
                                                    
                                                    for(size_t i = 0; i<nPath;i++)
                                                    {
                                                        cRng->nextG(gaussVec);
                                                        cMdl->generatePath(gaussVec, path);
                                                        prd.payoffs(path, results[i]);
                                                    };
                                                    
                                                    return results;
                                                    
                                                };

