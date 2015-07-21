/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/
#ifndef COMPUTEVARIABLEEIGENSTRAIN_H
#define COMPUTEVARIABLEEIGENSTRAIN_H

#include "ComputeEigenstrain.h"

/**
 * ComputeVariableEigenstrain computes an Eigenstrain that is a function of a single variable defined by a base tensor and a scalar function defined in a Derivative Material.
 */
class ComputeVariableEigenstrain : public ComputeEigenstrain
{
public:
  ComputeVariableEigenstrain(const InputParameters & parameters);
  ComputeVariableEigenstrain(const std::string & deprecated_name, InputParameters parameters); // DEPRECATED CONSTRUCTOR

protected:
  virtual void computeQpStressFreeStrain();

  /// number of variables the prefactor depends on
  const unsigned int _num_args;

  /// first derivatives of the prefactor w.r.t. to the args
  std::vector<const MaterialProperty<Real> *> _dprefactor;
  /// second derivatives of the prefactor w.r.t. to the args
  std::vector<std::vector<const MaterialProperty<Real> *> > _d2prefactor;

  /// first derivatives of the elastic strain w.r.t. to the args
  std::vector<MaterialProperty<RankTwoTensor> *> _delastic_strain;
  /// second derivatives of the elastic strain w.r.t. to the args
  std::vector<std::vector<MaterialProperty<RankTwoTensor> *> > _d2elastic_strain;
};

#endif //COMPUTEVARIABLEEIGENSTRAIN_H
