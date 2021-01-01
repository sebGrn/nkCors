// nkCor.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <random>
#include <chrono>

int main()
{
    std::cout << "starting\n";
    auto beginTime = std::chrono::system_clock::now();

    // 1000 * 1000 ~2sec
    // 5000 * 5000 ~3min
    // 10000 * 10000 ~30min

    const unsigned int count(5000);
    const unsigned int rows(count);
    const unsigned int cols(count);
    const unsigned int size(rows * cols);

    std::cout << "using matrix of " << rows << " * " << cols << " elements" << std::endl;

    std::vector<Eigen::Triplet<int>> tripleList;
    tripleList.resize(size);
    {
        // generate vector of random tuples (row index, col index, value (0/1))
        auto gen = std::bind(std::uniform_int_distribution<>(0, 1), std::default_random_engine(time(0)));
        unsigned int index = 0;
        for_each(tripleList.begin(), tripleList.end(), [&gen, &index, rows, cols](Eigen::Triplet<int>& triple) {
            triple = Eigen::Triplet<int>(static_cast<unsigned int>(index / rows), index % cols, gen());
            index++;
            });

        auto duration = (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - beginTime).count()) / 1000.0;
        std::cout << "triples created " << duration << " sec" << std::endl;
    }

    // create sparse matrix from generated tuples
    Eigen::SparseMatrix<int> mat(rows, cols);
    {
        mat.setFromTriplets(tripleList.begin(), tripleList.end());
        mat.makeCompressed();

        auto duration = (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - beginTime).count()) / 1000.0;
        std::cout << "matrix created " << duration << " sec" << std::endl;
    }
    
    // matrix multiplication, because 3 is good
    {
        mat = mat * mat * mat;
        auto duration = (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - beginTime).count()) / 1000.0;
        std::cout << "first M * M done " << duration << " sec" << std::endl;
        //std::cout << "m =" << std::endl << mat << std::endl;
    }
        
    // iterate along non zero elements to get max element
    {
        int maxValue = 0;
        std::vector<std::pair<int, int>> maxValuesIndexes;
        for (int k = 0; k < mat.outerSize(); ++k)
        {
            for (Eigen::SparseMatrix<int>::InnerIterator it(mat, k); it; ++it)
            {
                if (it.value() > maxValue)
                {
                    maxValue = it.value();
                    maxValuesIndexes.clear();
                    maxValuesIndexes.push_back(std::pair<int, int>(it.row(), it.col()));
                }
                else if (it.value() == maxValue)
                {
                    maxValuesIndexes.push_back(std::pair<int, int>(it.row(), it.col()));
                }
            }
        }
        std::cout << "max edges is " << maxValue << std::endl;

        if (maxValuesIndexes.size() > 6)
        {
            for_each(maxValuesIndexes.begin(), maxValuesIndexes.begin() + 5, [](const std::pair<int, int>& elt) { 
                std::cout << "(" << elt.first << ", " << elt.second << "), "; 
            });
            std::cout << "..." << std::endl;
        }
        else
        {
            for_each(maxValuesIndexes.begin(), maxValuesIndexes.end(), [](const std::pair<int, int>& elt) {
                std::cout << "(" << elt.first << ", " << elt.second << "), ";
                });
            std::cout << std::endl;
        }
    }

    // print timer
    auto duration = (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - beginTime).count()) / 1000.0;
    std::cout << std::endl << "done in " << duration << " s\n";
}
