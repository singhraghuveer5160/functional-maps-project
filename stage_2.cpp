#include <igl/readOBJ.h>
#include <igl/cotmatrix.h>
#include <igl/massmatrix.h>
#include <igl/eigs.h>

#include <Eigen/Core>
#include <Eigen/Sparse>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <utility>

using namespace std;
using namespace Eigen;

// Normalize mesh
void normalize_mesh(MatrixXd &V)
{
    RowVector3d center = V.colwise().mean();
    V.rowwise() -= center;

    double scale = V.array().abs().maxCoeff();
    V = V / scale;
}

// Save matrix
void save_matrix(const string &filename, const MatrixXd &M)
{
    ofstream out(filename);
    out << M << endl;
    out.close();
}


// Sort eigenvalues in ascending order and
// reorder corresponding eigenvectors
void sortEigenpairs(
    VectorXd &eigenvalues,
    MatrixXd &eigenvectors)
{
    int n = eigenvalues.size();

    vector<pair<double, int>> order;

    for (int i = 0; i < n; i++)
    {
        order.push_back({eigenvalues(i), i});
    }

    sort(order.begin(), order.end());

    VectorXd sortedValues(n);
    MatrixXd sortedVectors(
        eigenvectors.rows(),
        eigenvectors.cols());

    for (int i = 0; i < n; i++)
    {
        sortedValues(i) = order[i].first;
        sortedVectors.col(i) =
            eigenvectors.col(order[i].second);
    }

    eigenvalues = sortedValues;
    eigenvectors = sortedVectors;
}


// Process one mesh
void process_mesh(const string &filename, const string &prefix)
{
    MatrixXd V;
    MatrixXi F;

    if (!igl::readOBJ(filename, V, F))
    {
        cerr << "Error loading " << filename << endl;
        return;
    }

    cout << "Loaded " << filename << endl;
    cout << "Vertices: " << V.rows() << endl;
    cout << "Faces: " << F.rows() << endl;

    // Normalize
    normalize_mesh(V);

    SparseMatrix<double> L, M;

    // Cotangent Laplacian
    igl::cotmatrix(V, F, L);

    // Mass matrix
    igl::massmatrix(V, F, igl::MASSMATRIX_TYPE_VORONOI, M);

    cout << "Matrices created..." << endl;

    int k = 10;   // number of eigenvectors

    MatrixXd eigenvectors;
    VectorXd eigenvalues;

    cout << "Computing eigen decomposition..." << endl;

SparseMatrix<double> negL = -L;

igl::eigs(negL, M, k,
          igl::EIGS_TYPE_SM,
          eigenvectors,
          eigenvalues);
          
          // Sort eigenpairs in ascending order
sortEigenpairs(
    eigenvalues,
    eigenvectors);

    // Save
    save_matrix(prefix + "_eigenvectors.txt", eigenvectors);

    ofstream out(prefix + "_eigenvalues.txt");
    out << eigenvalues << endl;
    out.close();

    cout << "Saved files for " << prefix << endl;
}

int main()
{
    process_mesh("cat-reference.obj", "cat");

    cout << "-------------------" << endl;

    process_mesh("lion-reference.obj", "lion");

    cout << "Done." << endl;

    return 0;
}
