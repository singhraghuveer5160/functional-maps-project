# Functional Map Based Shape Correspondence

## Project Overview

This project implements a Functional Map pipeline for finding dense point-to-point correspondence between two 3D meshes.

Source Mesh:
- Cat

Target Mesh:
- Lion

The implementation follows five major stages:

1. Mesh Preparation
2. Laplace-Beltrami Spectral Analysis
3. Heat Kernel Signature (HKS) Extraction
4. Functional Map Estimation
5. Point Correspondence Recovery

---

## Project Structure

```
myproject/
│
├── stage_2.cpp
├── stage_3.cpp
├── stage_4.cpp
├── stage_5.cpp
├── CMakeLists.txt
│
├── cat-reference.obj
├── lion-reference.obj
│
├── cat_eigenvalues.txt
├── cat_eigenvectors.txt
├── lion_eigenvalues.txt
├── lion_eigenvectors.txt
│
├── cat_hks.txt
├── lion_hks.txt
│
├── functional_map.txt
├── vertex_correspondence.txt
│
└── README.md
```

---

## Stage 1 : Mesh Preparation

Objective:

Prepare clean meshes before spectral processing.

Tasks:

- Load OBJ meshes
- Normalize meshes
- Remove unnecessary artifacts (if required)

Input:

- cat-reference.obj
- lion-reference.obj

Output:

- Normalized meshes

---

## Stage 2 : Laplace-Beltrami Spectral Analysis

Objective:

Compute Laplace-Beltrami spectral basis.

Tasks:

- Load mesh
- Normalize mesh
- Construct Cotangent Laplacian
- Construct Voronoi Mass Matrix
- Compute Eigenvalues
- Compute Eigenvectors

Outputs:

- cat_eigenvalues.txt
- cat_eigenvectors.txt
- lion_eigenvalues.txt
- lion_eigenvectors.txt

---

## Stage 3 : Heat Kernel Signature (HKS)

Objective:

Extract intrinsic spectral descriptors.

Tasks:

- Load Eigenvalues
- Load Eigenvectors
- Compute Heat Kernel Signature

Outputs:

- cat_hks.txt
- lion_hks.txt

---

## Stage 4 : Functional Map Estimation

Objective:

Estimate correspondence between spectral spaces.

Tasks:

- Load HKS
- Project descriptors into spectral basis
- Compute Functional Map

Output:

- functional_map.txt

---

## Stage 5 : Point Correspondence Recovery

Objective:

Recover dense vertex correspondence.

Tasks:

- Load Functional Map
- Map source spectral embedding
- Find nearest target vertex
- Recover dense correspondence

Output:

- vertex_correspondence.txt

---

## Build Instructions

Create build folder

```bash
mkdir build
cd build
```

Configure

```bash
~/tools/cmake-3.30.2-linux-x86_64/bin/cmake ..
```

Compile

```bash
make
```

---

## Execute

Run Stage 2

```bash
./stage2
```

Run Stage 3

```bash
./stage3
```

Run Stage 4

```bash
./stage4
```

Run Stage 5

```bash
./stage5
```

---

## Libraries Used

- Eigen
- libigl
- CMake
- GNU C++ Compiler

---

## Method Used

The project follows the Functional Map framework:

Mesh
↓

Laplace-Beltrami Operator
↓

Eigen Decomposition
↓

Heat Kernel Signature
↓

Functional Map
↓

Dense Point Correspondence

---

## Author

Raghuveer Singh

Computer Science and Engineering

National Institute of Technology Calicut
