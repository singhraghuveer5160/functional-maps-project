#include <Eigen/Core>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;
using namespace Eigen;

//--------------------------------------------------
// Read matrix
//--------------------------------------------------
bool loadMatrix(const string &filename, MatrixXd &M)
{
    ifstream in(filename);

    if (!in.is_open())
    {
        cerr << "Cannot open " << filename << endl;
        return false;
    }

    vector<vector<double>> values;
    string line;

    while (getline(in, line))
    {
        if (line.empty())
            continue;

        stringstream ss(line);

        vector<double> row;

        double x;

        while (ss >> x)
            row.push_back(x);

        values.push_back(row);
    }

    in.close();

    if (values.empty())
        return false;

    M.resize(values.size(), values[0].size());

    for (int i = 0; i < values.size(); i++)
        for (int j = 0; j < values[0].size(); j++)
            M(i, j) = values[i][j];

    return true;
}

//--------------------------------------------------
// Read vector
//--------------------------------------------------
bool loadVector(const string &filename, VectorXd &V)
{
    ifstream in(filename);

    if (!in.is_open())
    {
        cerr << "Cannot open " << filename << endl;
        return false;
    }

    vector<double> temp;

    double x;

    while (in >> x)
        temp.push_back(x);

    in.close();

    V.resize(temp.size());

    for (int i = 0; i < temp.size(); i++)
        V(i) = temp[i];

    return true;
}

//--------------------------------------------------
// Save Matrix
//--------------------------------------------------
void saveMatrix(const string &filename, const MatrixXd &M)
{
    ofstream out(filename);

    out << M << endl;

    out.close();
}

//--------------------------------------------------
// Compute HKS
//--------------------------------------------------
MatrixXd computeHKS(
    const VectorXd &eigenvalues,
    const MatrixXd &eigenvectors,
    int numTimes)
{
    int n = eigenvectors.rows();

    int k = eigenvectors.cols();

    MatrixXd HKS(n, numTimes);

    double tMin = 0.01;

    double tMax = 1.0;

    for (int t = 0; t < numTimes; t++)
    {
        double time =
            tMin +
            (tMax - tMin) *
                t /
                (numTimes - 1);

        for (int v = 0; v < n; v++)
        {
            double hks = 0.0;

            for (int i = 0; i < k; i++)
            {
                hks += exp(-eigenvalues(i) * time) *
                       eigenvectors(v, i) *
                       eigenvectors(v, i);
            }

            HKS(v, t) = hks;
        }
    }

    return HKS;
}

//--------------------------------------------------
// Process Mesh
//--------------------------------------------------
void processMesh(const string &prefix)
{
    VectorXd eigenvalues;

    MatrixXd eigenvectors;

    if (!loadVector(prefix + "_eigenvalues.txt", eigenvalues))
        return;

    if (!loadMatrix(prefix + "_eigenvectors.txt", eigenvectors))
        return;

    cout << "Loaded " << prefix << endl;

    cout << "Vertices : "
         << eigenvectors.rows() << endl;

    cout << "Eigenvectors : "
         << eigenvectors.cols() << endl;

    MatrixXd HKS =
        computeHKS(
            eigenvalues,
            eigenvectors,
            100);

    saveMatrix(prefix + "_hks.txt", HKS);

    cout << "Saved "
         << prefix
         << "_hks.txt"
         << endl;
}

//--------------------------------------------------

int main()
{
    processMesh("cat");

    cout << "-----------------------" << endl;

    processMesh("lion");

    cout << "Stage 3 Complete." << endl;

    return 0;
}
