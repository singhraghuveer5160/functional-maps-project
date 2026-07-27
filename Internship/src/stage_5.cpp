#include <Eigen/Core>
#include <Eigen/Dense>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <limits>

using namespace std;
using namespace Eigen;

//------------------------------------------
// Data structure
//------------------------------------------

struct ShapeData
{
    MatrixXd eigenvectors;
};

//------------------------------------------
// Load Matrix
//------------------------------------------

bool loadMatrix(
        const string& filename,
        MatrixXd& M)
{
    ifstream in(filename);

    if(!in.is_open())
    {
        cerr<<"Cannot open "<<filename<<endl;
        return false;
    }

    vector<vector<double>> rows;

    string line;

    while(getline(in,line))
    {
        if(line.empty())
            continue;

        stringstream ss(line);

        vector<double> row;

        double value;

        while(ss>>value)
            row.push_back(value);

        rows.push_back(row);
    }

    in.close();

    if(rows.empty())
        return false;

    M.resize(rows.size(),
             rows[0].size());

    for(int i=0;i<(int)rows.size();i++)
        for(int j=0;j<(int)rows[0].size();j++)
            M(i,j)=rows[i][j];

    return true;
}

//------------------------------------------
// Save correspondence
//------------------------------------------

void saveCorrespondence(
        const string& filename,
        const VectorXi& corr)
{
    ofstream out(filename);

    for(int i=0;i<corr.size();i++)
    {
        out<<i<<" "<<corr(i)<<endl;
    }

    out.close();
}

//------------------------------------------
// Load Shape
//------------------------------------------

bool loadShape(
        const string& prefix,
        ShapeData& shape)
{
    if(!loadMatrix(
            prefix+"_eigenvectors.txt",
            shape.eigenvectors))
        return false;

    cout<<"Loaded "<<prefix<<endl;

    cout<<"Vertices : "
        <<shape.eigenvectors.rows()
        <<endl;

    cout<<"Basis : "
        <<shape.eigenvectors.cols()
        <<endl;

    return true;
}

//------------------------------------------
// Load Functional Map
//------------------------------------------

bool loadFunctionalMap(
        MatrixXd& C)
{
    if(!loadMatrix(
            "functional_map.txt",
            C))
        return false;

    cout<<"Loaded Functional Map"<<endl;

    cout<<"Size : "
        <<C.rows()
        <<" x "
        <<C.cols()
        <<endl;

    return true;
}

//------------------------------------------
// Euclidean distance squared
//------------------------------------------

double distanceSquared(
        const RowVectorXd& a,
        const RowVectorXd& b)
{
    return (a - b).squaredNorm();
}

//------------------------------------------
// Compute dense correspondence
//------------------------------------------

VectorXi computeCorrespondence(
        const ShapeData& source,
        const ShapeData& target,
        const MatrixXd& C)
{
    cout << "\nComputing dense correspondence..."
         << endl;

    //------------------------------------------------
    // Map source spectral embedding
    //
    // Source:  Phi
    // Target:  Psi
    // Functional Map: C
    //
    // mappedSource = Phi * Cᵀ
    //------------------------------------------------

    MatrixXd mappedSource =
        source.eigenvectors * C.transpose();

    VectorXi correspondence(
        mappedSource.rows());

//------------------------------------------------
// Allow each target vertex to be used at most twice
//------------------------------------------------

vector<int> usedCount(
    target.eigenvectors.rows(),
    0);

    //------------------------------------------------
    // For every source vertex
    //------------------------------------------------

    for(int i=0;
        i<mappedSource.rows();
        i++)
    {
        double bestDistance =
            numeric_limits<double>::max();

        int bestIndex = -1;

        //------------------------------------------------
        // Compare against every target vertex
        //------------------------------------------------

        for(int j=0;
            j<target.eigenvectors.rows();
            j++)
        {
             if(usedCount[j] >= 2)
        continue;

            double d =
                distanceSquared(
                    mappedSource.row(i),
                    target.eigenvectors.row(j));

            if(d < bestDistance)
            {
                bestDistance = d;
                bestIndex = j;
            }
        }

        correspondence(i) = bestIndex;

if(bestIndex >= 0)
{
    usedCount[bestIndex]++;
}

        //------------------------------------------------
        // Progress indicator
        //------------------------------------------------

        if(i % 500 == 0)
        {
            cout << "Processed "
                 << i
                 << " / "
                 << mappedSource.rows()
                 << endl;
        }
    }

    return correspondence;
}


//------------------------------------------
// Print first correspondences
//------------------------------------------

void printCorrespondence(
        const VectorXi& corr)
{
    cout << "\nFirst 20 correspondences\n";

    int n =
        min(20,(int)corr.size());

    for(int i=0;i<n;i++)
    {
        cout
            << i
            << " -> "
            << corr(i)
            << endl;
    }
}

//------------------------------------------
// Main
//------------------------------------------

int main()
{
    cout << "====================================" << endl;
    cout << " Stage 5 : Point Correspondence Recovery" << endl;
    cout << "====================================" << endl;

    ShapeData cat;
    ShapeData lion;

    MatrixXd functionalMap;

    //------------------------------------------
    // Load source
    //------------------------------------------

    if(!loadShape("cat", cat))
    {
        cerr << "Failed to load cat data." << endl;
        return EXIT_FAILURE;
    }

    //------------------------------------------
    // Load target
    //------------------------------------------

    if(!loadShape("lion", lion))
    {
        cerr << "Failed to load lion data." << endl;
        return EXIT_FAILURE;
    }

    //------------------------------------------
    // Load Functional Map
    //------------------------------------------

    if(!loadFunctionalMap(functionalMap))
    {
        cerr << "Failed to load functional_map.txt" << endl;
        return EXIT_FAILURE;
    }

    //------------------------------------------
    // Dimension check
    //------------------------------------------

    if(functionalMap.rows() != lion.eigenvectors.cols() ||
       functionalMap.cols() != cat.eigenvectors.cols())
    {
        cerr << "\nERROR: Functional map dimensions do not match "
             << "the spectral basis." << endl;

        cerr << "Functional Map : "
             << functionalMap.rows()
             << " x "
             << functionalMap.cols()
             << endl;

        cerr << "Cat basis : "
             << cat.eigenvectors.cols()
             << endl;

        cerr << "Lion basis : "
             << lion.eigenvectors.cols()
             << endl;

        return EXIT_FAILURE;
    }

    //------------------------------------------
    // Compute correspondence
    //------------------------------------------

    VectorXi correspondence =
        computeCorrespondence(
            cat,
            lion,
            functionalMap);

    //------------------------------------------
    // Display
    //------------------------------------------

    printCorrespondence(correspondence);

    //------------------------------------------
    // Save
    //------------------------------------------

    saveCorrespondence(
        "vertex_correspondence.txt",
        correspondence);

    cout << "\nSaved vertex_correspondence.txt"
         << endl;

    cout << "\n====================================" << endl;
    cout << "Stage 5 Completed Successfully"
         << endl;
    cout << "====================================" << endl;

    return EXIT_SUCCESS;
}


