#include <igl/readOBJ.h>
#include <igl/opengl/glfw/Viewer.h>

#include <Eigen/Core>

#include <fstream>
#include <iostream>
#include <vector>

using namespace std;

Eigen::MatrixXd V1, V2;
Eigen::MatrixXi F1, F2;

//------------------------------------------------------------
// Landmark vertices on CAT
//------------------------------------------------------------

vector<int> catLandmarks =
{
    4263,
    4671,
    3853,
    3217,
    2774,
    1670,
    6670,
    5361,
    7117,
    7179
};

//------------------------------------------------------------
// Correspondence loaded from Stage 5
//------------------------------------------------------------

vector<int> correspondence;

int main()
{
    //--------------------------------------------------------
    // Load meshes
    //--------------------------------------------------------

    if (!igl::readOBJ("cat-reference.obj", V1, F1))
    {
        cout << "Cannot load cat\n";
        return 0;
    }

    if (!igl::readOBJ("lion-reference.obj", V2, F2))
    {
        cout << "Cannot load lion\n";
        return 0;
    }

    //--------------------------------------------------------
    // Shift meshes apart
    //--------------------------------------------------------

    V1.col(0).array() += 1.0;
    V2.col(0).array() += 1.5;

     ifstream fin("vertex_correspondence.txt");

    if(!fin)
    {
        cout<<"Cannot open vertex_correspondence.txt"<<endl;
        return 0;
    }

    int catVertex;
    int lionVertex;

    while(fin >> catVertex >> lionVertex)
    {
        correspondence.push_back(lionVertex);
    }

    cout<<"Loaded "
        <<correspondence.size()
        <<" correspondences"<<endl;

            //--------------------------------------------------------
    // Create landmark point matrices
    //--------------------------------------------------------

    Eigen::MatrixXd catPoints(
        catLandmarks.size(),
        3);

    Eigen::MatrixXd lionPoints(
        catLandmarks.size(),
        3);

    for(int i=0;i<(int)catLandmarks.size();i++)
    {
        int catIndex = catLandmarks[i];

        if(catIndex >= correspondence.size())
            continue;

        int lionIndex =
            correspondence[catIndex];

        catPoints.row(i) =
            V1.row(catIndex);

        lionPoints.row(i) =
            V2.row(lionIndex);
    }

    //--------------------------------------------------------
    // Create Viewer
    //--------------------------------------------------------

    igl::opengl::glfw::Viewer viewer;

    //--------------------------------------------------------
    // CAT
    //--------------------------------------------------------

    viewer.data().set_mesh(
        V1,
        F1);

    viewer.data().add_points(
        catPoints,
        Eigen::RowVector3d(1,0,0));

    viewer.data().point_size = 20;

    //--------------------------------------------------------
    // LION
    //--------------------------------------------------------

    viewer.append_mesh();

    viewer.data(1).set_mesh(
        V2,
        F2);

    viewer.data(1).add_points(
        lionPoints,
        Eigen::RowVector3d(1,0,0));

    viewer.data(1).point_size = 20;

    //--------------------------------------------------------
    // Draw correspondence lines
    //--------------------------------------------------------

    for(int i=0;i<(int)catLandmarks.size();i++)
    {
        viewer.data().add_edges(
            catPoints.row(i),
            lionPoints.row(i),
            Eigen::RowVector3d(0,1,0));
    }

        //--------------------------------------------------------
    // Viewer Settings
    //--------------------------------------------------------

    viewer.core().align_camera_center(V1, F1);

    cout << "\n======================================" << endl;
    cout << " Landmark Correspondence Visualization " << endl;
    cout << "======================================" << endl;
    cout << "Red Points   : Landmark vertices" << endl;
    cout << "Green Lines  : Computed correspondence" << endl;
    cout << "======================================" << endl;

    viewer.launch();

    return 0;
}

    