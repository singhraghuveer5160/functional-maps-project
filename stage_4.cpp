#include <Eigen/Core>
#include <Eigen/Dense>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;
using namespace Eigen;

//----------------------------------------------------
// Load Matrix
//----------------------------------------------------
bool loadMatrix(const string& filename, MatrixXd& M)
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

    M.resize(rows.size(),rows[0].size());

    for(int i=0;i<(int)rows.size();i++)
        for(int j=0;j<(int)rows[0].size();j++)
            M(i,j)=rows[i][j];

    return true;
}

//----------------------------------------------------
// Load Vector
//----------------------------------------------------
bool loadVector(const string& filename, VectorXd& V)
{
    ifstream in(filename);

    if(!in.is_open())
    {
        cerr<<"Cannot open "<<filename<<endl;
        return false;
    }

    vector<double> values;

    double x;

    while(in>>x)
        values.push_back(x);

    in.close();

    V.resize(values.size());

    for(int i=0;i<(int)values.size();i++)
        V(i)=values[i];

    return true;
}

//----------------------------------------------------
// Save Matrix
//----------------------------------------------------
void saveMatrix(const string& filename,
                const MatrixXd& M)
{
    ofstream out(filename);

    out<<M<<endl;

    out.close();
}


//----------------------------------------------------
// Project descriptors into spectral basis
//----------------------------------------------------
MatrixXd spectralProjection(
        const MatrixXd& eigenvectors,
        const MatrixXd& descriptors)
{
    return eigenvectors.transpose()*descriptors;
}

struct ShapeData
{
    MatrixXd eigenvectors;

    VectorXd eigenvalues;

    MatrixXd hks;
};

bool loadShape(const string& prefix,
               ShapeData& shape)
{
    if(!loadVector(prefix+"_eigenvalues.txt",
                   shape.eigenvalues))
        return false;

    if(!loadMatrix(prefix+"_eigenvectors.txt",
                   shape.eigenvectors))
        return false;

    if(!loadMatrix(prefix+"_hks.txt",
                   shape.hks))
        return false;

    cout<<"Loaded "<<prefix<<endl;

    cout<<"Vertices : "
        <<shape.eigenvectors.rows()
        <<endl;

    cout<<"Eigenvectors : "
        <<shape.eigenvectors.cols()
        <<endl;

    cout<<"HKS columns : "
        <<shape.hks.cols()
        <<endl;

    return true;
}

//----------------------------------------------------
// Compute Functional Map
//----------------------------------------------------
MatrixXd computeFunctionalMap(
        const ShapeData& source,
        const ShapeData& target)
{
    cout << "\nProjecting HKS descriptors..." << endl;

    // Project HKS into spectral basis
    MatrixXd A =
        spectralProjection(
            source.eigenvectors,
            source.hks);

    MatrixXd B =
        spectralProjection(
            target.eigenvectors,
            target.hks);
            
            //------------------------------------------------
// Normalize descriptor columns
//------------------------------------------------
for (int i = 0; i < A.cols(); i++)
{
    double n = A.col(i).norm();

    if (n > 1e-12)
        A.col(i) /= n;
}

for (int i = 0; i < B.cols(); i++)
{
    double n = B.col(i).norm();

    if (n > 1e-12)
        B.col(i) /= n;
}


    cout << "Source descriptor matrix : "
         << A.rows() << " x "
         << A.cols() << endl;

    cout << "Target descriptor matrix : "
         << B.rows() << " x "
         << B.cols() << endl;

    //------------------------------------------------
    // Least-squares solution
    //
    // C = B*Aᵀ*(A*Aᵀ)^(-1)
    //------------------------------------------------

    //------------------------------------------------
// Regularized least-squares
//------------------------------------------------

double lambda = 1e-4;

MatrixXd AAT =
    A * A.transpose() +
    lambda *
    MatrixXd::Identity(A.rows(), A.rows());

MatrixXd RHS =
    B * A.transpose();

MatrixXd C =
    RHS *
    AAT.ldlt().solve(
        MatrixXd::Identity(
            A.rows(),
            A.rows()));
            
            
    return C;
}

//----------------------------------------------------
// Display matrix information
//----------------------------------------------------
void printMapInfo(const MatrixXd& C)
{
    cout << "\nFunctional Map Size : "
         << C.rows()
         << " x "
         << C.cols()
         << endl;

    cout << "\nFirst 5 x 5 block\n";

    int r = min(5,(int)C.rows());
    int c = min(5,(int)C.cols());

    cout << C.block(0,0,r,c) << endl;
}

//----------------------------------------------------
// Save Functional Map
//----------------------------------------------------
void saveFunctionalMap(
        const MatrixXd& C)
{
    saveMatrix("functional_map.txt", C);

    cout << "\nSaved functional_map.txt"
         << endl;
}

//----------------------------------------------------
// Main
//----------------------------------------------------
int main()
{
    ShapeData cat;
    ShapeData lion;

    cout << "====================================" << endl;
    cout << " Stage 4 : Functional Map Estimation" << endl;
    cout << "====================================" << endl;

    //------------------------------------------------
    // Load cat
    //------------------------------------------------
    if(!loadShape("cat",cat))
    {
        cerr << "Failed to load cat data." << endl;
        return EXIT_FAILURE;
    }

    //------------------------------------------------
    // Load lion
    //------------------------------------------------
    if(!loadShape("lion",lion))
    {
        cerr << "Failed to load lion data." << endl;
        return EXIT_FAILURE;
    }

    //------------------------------------------------
    // Check basis size
    //------------------------------------------------
    if(cat.eigenvectors.cols() != lion.eigenvectors.cols())
    {
        cerr << "Error: Both meshes must use the same number "
             << "of eigenvectors." << endl;
        return EXIT_FAILURE;
    }

    //------------------------------------------------
    // Check HKS dimension
    //------------------------------------------------
    if(cat.hks.cols() != lion.hks.cols())
    {
        cerr << "Error: HKS dimensions do not match."
             << endl;
        return EXIT_FAILURE;
    }

    cout << "\nComputing Functional Map..." << endl;

    MatrixXd C =
        computeFunctionalMap(cat, lion);

    printMapInfo(C);

    saveFunctionalMap(C);

    cout << "\n====================================" << endl;
    cout << "Stage 4 Completed Successfully"
         << endl;
    cout << "====================================" << endl;

    return EXIT_SUCCESS;
}


