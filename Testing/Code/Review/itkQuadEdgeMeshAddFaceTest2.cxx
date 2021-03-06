/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkQuadEdgeMeshAddFaceTest2.cxx,v $
  Language:  C++
  Date:      $Date: 2007-09-05 18:46:45 $
  Version:   $Revision: 1.5 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#include "itkQuadEdgeMesh.h"
#include "itkQuadEdgeMeshTopologyChecker.h"


int itkQuadEdgeMeshAddFaceTest2(int , char *[])
{
  typedef itk::QuadEdgeMesh< double, 3 >            MeshType;
  typedef MeshType::Pointer                         MeshPointer;
  typedef MeshType::QEType                          QEType;
  typedef MeshType::PointIdentifier                 PointIdentifier;
  typedef MeshType::CellType                        CellType;
  typedef itk::QuadEdgeMeshPolygonCell< CellType >  QEPolygonCellType;

  int numPts = 7;
  int numCells = 4;

  std::cout << "numPts= " << numPts << std::endl;
  std::cout << "numCells= " << numCells << std::endl;

  int oddConnectivityCells[12] =
  { 0, 1, 2,
    3, 4, 5,
    6, 4, 0,
    0, 4, 6, };

  // Configuration of odd_connectivity mesh:
  // numVertices=7, numEdges=9, numFaces=3, numBoundaries=1, Chi=2
  //
  //    5 ---------- 4 ---------- 0 ---------- 1
  //    |  [2]   __/ |   [3]  __/   \__  [1]   |
  //    |     __/    |     __/         \__     |
  //    |  __/       |  __/               \__  |
  //    | /          | /                     \ |
  //    3            6                         2
  //
  // First we add the three triangles [0 1 2], [3 4 5], [6 4 0].
  // Then we try to add a fourth triangle, which is the third triangle
  // inverted i.e. [0 4 6]. This triangle can not be added.

  MeshPointer  mesh = MeshType::New();

  MeshType::PointType pts[7];

  pts[0][0] = 2.0;  pts[0][1] = 1.0;  pts[0][2] = 0.0;
  pts[1][0] = 3.0;  pts[1][1] = 1.0;  pts[1][2] = 0.0;
  pts[2][0] = 3.0;  pts[2][1] = 0.0;  pts[2][2] = 0.0;
  pts[3][0] = 0.0;  pts[3][1] = 0.0;  pts[3][2] = 0.0;
  pts[4][0] = 1.0;  pts[4][1] = 1.0;  pts[4][2] = 0.0;
  pts[5][0] = 0.0;  pts[5][1] = 1.0;  pts[5][2] = 0.0;
  pts[6][0] = 1.0;  pts[6][1] = 0.0;  pts[6][2] = 0.0;

  for(int i=0; i<numPts; i++)
    {
    mesh->SetPoint( i, pts[i] );
    }

  int computedNumPts = mesh->GetNumberOfPoints();
  std::cout << "computedNumPts= " << computedNumPts << std::endl;

  CellType::CellAutoPointer cellpointer;
  QEPolygonCellType *poly;

  for(int i=0; i<numCells; i++)
    {
    poly = new QEPolygonCellType(3);
    cellpointer.TakeOwnership( poly );
    cellpointer->SetPointId( 0, oddConnectivityCells[3*i] );
    cellpointer->SetPointId( 1, oddConnectivityCells[3*i+1] );
    cellpointer->SetPointId( 2, oddConnectivityCells[3*i+2] );
    mesh->SetCell( i, cellpointer );
    }

  int computedNumFaces = mesh->ComputeNumberOfFaces();
  std::cout << "computedNumFaces= " << computedNumFaces << std::endl;

  std::cout << "Test whether the fourth face was rejected" << std::endl;

  typedef itk::QuadEdgeMeshTopologyChecker< MeshType >  TopologyCheckerType;
  TopologyCheckerType::Pointer checker = TopologyCheckerType::New();

  checker->SetMesh( mesh );
  checker->SetExpectedNumberOfPoints( 7 );
  checker->SetExpectedNumberOfEdges( 9 );
  checker->SetExpectedNumberOfFaces( 3 );
  checker->SetExpectedNumberOfBoundaries( 3 );
  checker->SetExpectedGenus( 0 ); // FIXME find the correct genus value
  checker->GetNameOfClass( );
  std::cout << "Testing PrintSelf: " << std::endl;
  std::cout << checker << std::endl;
  std::cout << "[SUCESS]" << std::endl;
 
  if( checker->ValidateEulerCharacteristic() )
    {
    std::cout << "Passed" << std::endl;
    }
  else
    {
    std::cout << "Failed" << std::endl;
    return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}
