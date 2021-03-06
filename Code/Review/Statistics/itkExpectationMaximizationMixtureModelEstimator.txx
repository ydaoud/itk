/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkExpectationMaximizationMixtureModelEstimator.txx,v $
  Language:  C++
  Date:      $Date: 2010-05-20 19:26:58 $
  Version:   $Revision: 1.3 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkExpectationMaximizationMixtureModelEstimator_txx
#define __itkExpectationMaximizationMixtureModelEstimator_txx

#include "itkExpectationMaximizationMixtureModelEstimator.h"
#include "itkNumericTraits.h"

namespace itk  { 
namespace Statistics  {
  
template< class TSample >
ExpectationMaximizationMixtureModelEstimator< TSample >
::ExpectationMaximizationMixtureModelEstimator()
{
  m_TerminationCode = NOT_CONVERGED;

  m_MembershipFunctionsObject            = MembershipFunctionVectorObjectType::New();
  m_MembershipFunctionsWeightArrayObject =
                        MembershipFunctionsWeightsArrayObjectType::New();
  m_Sample = 0;
}
 
template< class TSample >
void
ExpectationMaximizationMixtureModelEstimator< TSample >
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);
  os << indent << "Maximum Iteration: "
            << this->GetMaximumIteration() << std::endl;
  os << indent << "Sample: "
            << this->GetSample() << std::endl;
  os << indent << "Number Of Components: "
            << this->GetNumberOfComponents() << std::endl;
  for (unsigned int i = 0; i < this->GetNumberOfComponents(); i++)
    {
    os << indent << "Component Membership Function[" << i << "]: "
       << this->GetComponentMembershipFunction(i) << std::endl;
    }
  os << indent << "Termination Code: "
            << this->GetTerminationCode() << std::endl;
  os << indent << "Initial Proportions: "
            << this->GetInitialProportions() << std::endl;
  os << indent << "Proportions: "
            << this->GetProportions() << std::endl;
  os << indent << "Calculated Expectation: " << this->CalculateExpectation() << std::endl;
}


template< class TSample >
void
ExpectationMaximizationMixtureModelEstimator< TSample >
::SetMaximumIteration(int numberOfIterations) 
{
  m_MaxIteration = numberOfIterations;
}

template< class TSample >
int
ExpectationMaximizationMixtureModelEstimator< TSample >
::GetMaximumIteration() const
{
  return m_MaxIteration;
}

template< class TSample >
void
ExpectationMaximizationMixtureModelEstimator< TSample >
::SetInitialProportions(ProportionVectorType &proportions) 
{
  m_InitialProportions = proportions;
}

template< class TSample >
const typename ExpectationMaximizationMixtureModelEstimator< TSample >::ProportionVectorType&
ExpectationMaximizationMixtureModelEstimator< TSample >
::GetInitialProportions() const
{
  return m_InitialProportions;
}

template< class TSample >
const typename ExpectationMaximizationMixtureModelEstimator< TSample >::ProportionVectorType&
ExpectationMaximizationMixtureModelEstimator< TSample >
::GetProportions() const
{
  return m_Proportions;
}

template< class TSample >
void
ExpectationMaximizationMixtureModelEstimator< TSample >
::SetSample(const TSample* sample) 
{
  m_Sample = sample;
}

template< class TSample >
const TSample *
ExpectationMaximizationMixtureModelEstimator< TSample >
::GetSample() const 
{
  return m_Sample;
}


template< class TSample >
int
ExpectationMaximizationMixtureModelEstimator< TSample >
::AddComponent(ComponentType* component)
{
  m_ComponentVector.push_back(component);
  return static_cast<int>( m_ComponentVector.size() );
}

template< class TSample >
unsigned int
ExpectationMaximizationMixtureModelEstimator< TSample >
::GetNumberOfComponents() const
{
  return m_ComponentVector.size();
}

template< class TSample >
typename ExpectationMaximizationMixtureModelEstimator< TSample >::TERMINATION_CODE
ExpectationMaximizationMixtureModelEstimator< TSample >
::GetTerminationCode() const
{
  return m_TerminationCode;
}

template< class TSample >
typename ExpectationMaximizationMixtureModelEstimator< TSample >::ComponentMembershipFunctionType*
ExpectationMaximizationMixtureModelEstimator< TSample >
::GetComponentMembershipFunction(int componentIndex) const
{
  return (m_ComponentVector[componentIndex])->GetMembershipFunction();
}


template< class TSample >
bool
ExpectationMaximizationMixtureModelEstimator< TSample >
::CalculateDensities()
{
  bool componentModified  = false;

  for (unsigned int i = 0; i < m_ComponentVector.size(); i++)
    {
    if ( (m_ComponentVector[i])->AreParametersModified() )
      {
      componentModified = true;
      }
    }

  if (!componentModified)
    {
    return false;
    }

  double temp;
  int numberOfComponents = static_cast<int>( m_ComponentVector.size() );
  std::vector< double > tempWeights(numberOfComponents);

  typename TSample::ConstIterator iter = m_Sample->Begin();
  typename TSample::ConstIterator last = m_Sample->End();

  int componentIndex;

  typedef typename TSample::AbsoluteFrequencyType FrequencyType;
  FrequencyType frequency;
  FrequencyType zeroFrequency = NumericTraits< FrequencyType >::Zero;
  typename TSample::MeasurementVectorType mvector;
  double density;
  double densitySum;
  double minDouble = NumericTraits< double >::NonpositiveMin();

  long measurementVectorIndex = 0;
  
  while (iter != last)
    {
    mvector = iter.GetMeasurementVector();
    frequency = iter.GetFrequency();
    densitySum = 0.0;
    if ( frequency > zeroFrequency )
      {
      for (componentIndex = 0; componentIndex < numberOfComponents; 
           componentIndex++)
        {
        density = m_Proportions[componentIndex] *
          m_ComponentVector[componentIndex]->Evaluate(mvector);
        tempWeights[componentIndex] = density;
        densitySum += density;
        }
          
      for (componentIndex = 0; componentIndex < numberOfComponents; 
           componentIndex++)
        {
        temp = tempWeights[componentIndex];
        if (densitySum != 0)
          {
          temp /= densitySum;
          }
        m_ComponentVector[componentIndex]->SetWeight(measurementVectorIndex,
                                                     temp); 
        }
      }
    else
      {
      for (componentIndex = 0; componentIndex < numberOfComponents; 
           componentIndex++)
        {
        m_ComponentVector[componentIndex]->SetWeight(measurementVectorIndex,
                                                     minDouble); 
        }
      }

    ++iter;
    ++measurementVectorIndex;
    }
  
  return true;
}

template< class TSample >
double
ExpectationMaximizationMixtureModelEstimator< TSample >
::CalculateExpectation() const
{
  double sum = 0.0;
  if (m_Sample)
    {
    unsigned int componentIndex, measurementVectorIndex;
    unsigned long size = m_Sample->Size();
    double logProportion;
    double temp;
    for (componentIndex = 0; componentIndex < m_ComponentVector.size();
         componentIndex++)
      {
      logProportion = vcl_log(m_Proportions[componentIndex]); 
      for (measurementVectorIndex = 0; measurementVectorIndex < size;
           measurementVectorIndex++)
        {
        temp = m_ComponentVector[componentIndex]->
          GetWeight(measurementVectorIndex);
        sum += temp * ( logProportion + 
                        vcl_log(m_ComponentVector[componentIndex]->
                                GetWeight(measurementVectorIndex) ) );
        }
      }
    }
  return sum;
}

template< class TSample >
bool
ExpectationMaximizationMixtureModelEstimator< TSample >
::UpdateComponentParameters()
{
  unsigned int componentIndex;
  bool updated = false;
  ComponentType* component;

  for (componentIndex = 0; componentIndex < m_ComponentVector.size();
       componentIndex++)
    {
    component = m_ComponentVector[componentIndex];
    component->Update();
    if (component->AreParametersModified())
      {
      updated = true;
      }
    }

  return updated;
}

template< class TSample >
bool
ExpectationMaximizationMixtureModelEstimator< TSample >
::UpdateProportions()
{
  int numberOfComponents = m_ComponentVector.size();
  long sampleSize = m_Sample->Size();
  double totalFrequency = (double) (m_Sample->GetTotalFrequency());
  long i, j;
  double tempSum;
  bool updated = false;

  for (i = 0; i < numberOfComponents; i++)
    {
    tempSum = 0.0;
    for (j = 0; j < sampleSize; j++)
      {
      tempSum += (m_ComponentVector[i]->GetWeight(j) * 
         m_Sample->GetFrequency(j));
      }

    tempSum /= totalFrequency;

    if (tempSum != m_Proportions[i])
      {
      m_Proportions[i] = tempSum; 
      updated = true;
      }
    }

  return updated;
}

template< class TSample >
void
ExpectationMaximizationMixtureModelEstimator< TSample >
::GenerateData() 
{
  m_Proportions = m_InitialProportions;   

  int iteration = 0;
  m_CurrentIteration = 0;
  while (iteration < m_MaxIteration)
    {
    m_CurrentIteration = iteration;
    if (this->CalculateDensities())
      {
      this->UpdateComponentParameters();
      this->UpdateProportions();
      }
    else
      {
      m_TerminationCode = CONVERGED;
      break;
      }
    ++iteration;
    }
  
  m_TerminationCode = NOT_CONVERGED;
}

template< class TSample >
const typename ExpectationMaximizationMixtureModelEstimator< TSample >::MembershipFunctionVectorObjectType * 
ExpectationMaximizationMixtureModelEstimator< TSample > 
::GetOutput() const
{

  unsigned int numberOfComponents = m_ComponentVector.size(); 
  MembershipFunctionVectorType &  membershipFunctionsVector = m_MembershipFunctionsObject->Get();

  typename SampleType::MeasurementVectorSizeType measurementVectorSize =
                                m_Sample->GetMeasurementVectorSize();

  typename GaussianMembershipFunctionType::MeanType    mean;
  MeasurementVectorTraits::SetLength( mean, measurementVectorSize);

  typename GaussianMembershipFunctionType::CovarianceType  covariance;
  covariance.SetSize(measurementVectorSize,measurementVectorSize);

  typename ComponentType::ParametersType                parameters;

  for( unsigned int i=0; i < numberOfComponents; i++ )
    {
    parameters = m_ComponentVector[i]->GetFullParameters(); 
    typename GaussianMembershipFunctionType::Pointer membershipFunction =
                GaussianMembershipFunctionType::New(); 
    membershipFunction->SetMeasurementVectorSize( measurementVectorSize );
    unsigned int parameterIndex = 0;
    for(unsigned int j=0; j < measurementVectorSize; j++)
      {
      mean[j] = parameters[j];
      ++parameterIndex;
      }

    for ( unsigned int ii = 0; ii < measurementVectorSize; ii++ )
      {
      for ( unsigned int jj = 0; jj < measurementVectorSize; jj++ )
        {
        covariance.GetVnlMatrix().put(ii, jj, parameters[parameterIndex]);
        ++parameterIndex;
        }
      }

    membershipFunction->SetMean( mean );
    membershipFunction->SetCovariance( covariance );
    membershipFunctionsVector.push_back( membershipFunction.GetPointer() );
    }

  return static_cast< const MembershipFunctionVectorObjectType*>(m_MembershipFunctionsObject);
}

template< class TSample >
const typename ExpectationMaximizationMixtureModelEstimator< TSample
>::MembershipFunctionsWeightsArrayObjectType * 
ExpectationMaximizationMixtureModelEstimator< TSample > 
::GetMembershipFunctionsWeightsArray() const
{

  unsigned int numberOfComponents = m_ComponentVector.size(); 
  ProportionVectorType &  membershipFunctionsWeightVector =
                                      m_MembershipFunctionsWeightArrayObject->Get();
  membershipFunctionsWeightVector.SetSize( numberOfComponents );
  for( unsigned int i=0; i < numberOfComponents; i++ )
    {
    membershipFunctionsWeightVector[i] = m_Proportions[i];
    }

  return static_cast< const MembershipFunctionsWeightsArrayObjectType*>(m_MembershipFunctionsWeightArrayObject);
}

template< class TSample >
void 
ExpectationMaximizationMixtureModelEstimator< TSample >
::Update()
{
  this->GenerateData();
}
 
} // end of namespace Statistics 
} // end of namespace itk

#endif
