/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/
#ifndef COMPUTEMULTIPLEINELASTICSTRESS_H
#define COMPUTEMULTIPLEINELASTICSTRESS_H

#include "ComputeFiniteStrainElasticStress.h"

class StressUpdateBase;

/**
 * ComputeMultipleInelasticStress computes the stress, the consistent tangent
 * operator (or an approximation to it), and a decomposition of the strain
 * into elastic and inelastic parts.  By default finite strains are assumed.
 *
 * The elastic strain is calculated by subtracting the computed inelastic strain
 * increment tensor from the mechanical strain tensor.  Mechanical strain is
 * considered as the sum of the elastic and inelastic (plastic, creep, ect) strains.
 *
 * This material is used to call the recompute iterative materials of a number
 * of specified inelastic models that inherit from StressUpdateBase.  It iterates
 * over the specified inelastic models until the change in stress is within
 * a user-specified tolerance, in order to produce the stress, the consistent
 * tangent operator and the elastic and inelastic strains for the time increment.
 */

class ComputeMultipleInelasticStress : public ComputeFiniteStrainElasticStress
{
public:
  ComputeMultipleInelasticStress(const InputParameters & parameters);

protected:
  virtual void initQpStatefulProperties() override;
  virtual void initialSetup() override;

  virtual void computeQpStress() override;

  /**
   * Given the _strain_increment[_qp], iterate over all of the user-specified
   * recompute materials in order to find an admissible stress (which is placed
   * into _stress[_qp]) and set of inelastic strains, as well as the tangent operator
   * (which is placed into _Jacobian_mult[_qp]).
   * @param elastic_strain_increment The elastic part of _strain_increment[_qp] after the iterative
   * process has converged
   * @param combined_inelastic_strain_increment The inelastic part of _strain_increment[_qp] after
   * the iterative process has converged.  This is a weighted sum of all the inelastic strains
   * computed by all the plastic models during the Picard iterative scheme.  The weights are
   * dictated by the user using _inelastic_weights
   */
  virtual void updateQpState(RankTwoTensor & elastic_strain_increment,
                             RankTwoTensor & combined_inelastic_strain_increment);

  /**
   * An optimised version of updateQpState that gets used when the number
   * of plastic models is unity.
   * Given the _strain_increment[_qp], find an admissible stress (which is
   * put into _stress[_qp]) and inelastic strain, as well as the tangent operator
   * (which is placed into _Jacobian_mult[_qp])
   * @param elastic_strain_increment The elastic part of _strain_increment[_qp]
   * @param combined_inelastic_strain_increment The inelastic part of _strain_increment[_qp]
   */
  virtual void updateQpStateSingleModel(RankTwoTensor & elastic_strain_increment,
                                        RankTwoTensor & combined_inelastic_strain_increment);

  /**
   * Using _elasticity_tensor[_qp] and the consistent tangent operators,
   * _comsistent_tangent_operator[...] computed by the inelastic models,
   * compute _Jacobian_mult[_qp]
   */
  virtual void computeQpJacobianMult();

  /**
   * Given a trial stress (_stress[_qp]) and a strain increment (elastic_strain_increment)
   * let the model_number model produce an admissible stress (gets placed back in _stress[_qp]), and decompose
   * the strain increment into an elastic part (gets placed back into elastic_strain_increment) and an
   * inelastic part (inelastic_strain_increment), as well as computing the consistent_tangent_operator
   * @param model_number The inelastic model to use
   * @param elastic_strain_increment Upon input, this is the strain increment.  Upon output, it is the elastic part of the strain increment
   * @param inelastic_strain_increment The inelastic strain increment corresponding to the supplied strain increment
   * @param consistent_tangent_operator The consistent tangent operator
   */
  virtual void computeAdmissibleState(unsigned model_number, RankTwoTensor & elastic_strain_increment, RankTwoTensor & inelastic_strain_increment, RankFourTensor & consistent_tangent_operator);

  ///@{Input parameters associated with the recompute iteration to return the stress state to the yield surface
  const unsigned int _max_iterations;
  const Real _relative_tolerance;
  const Real _absolute_tolerance;
  const bool _output_iteration_info;
  ///@}

  /// after updateQpState, rotate the stress, elastic_strain, inelastic_strain and Jacobian_mult using _rotation_increment
  const bool _perform_finite_strain_rotations;

  ///@{ Rank-4 and Rank-2 elasticity and elastic strain tensors
  const MaterialProperty<RankFourTensor> & _elasticity_tensor;
  const MaterialProperty<RankTwoTensor> & _elastic_strain_old;
  const MaterialProperty<RankTwoTensor> & _strain_increment;
  ///@}

  /// The sum of the inelastic strains that come from the plastic models
  MaterialProperty<RankTwoTensor> & _inelastic_strain;

  /// old value of inelastic strain
  const MaterialProperty<RankTwoTensor> & _inelastic_strain_old;

  /// what sort of Tangent operator to calculate
  const enum class TangentOperatorEnum { elastic, nonlinear } _tangent_operator_type;

  /// number of plastic models
  const unsigned _num_models;

  /// _inelastic_strain = sum_i (_inelastic_weights_i * inelastic_strain_from_model_i)
  const std::vector<Real> _inelastic_weights;

  /// the consistent tangent operators computed by each plastic model
  std::vector<RankFourTensor> _consistent_tangent_operator;

  /**
   * The user supplied list of inelastic models to use in the simulation
   *
   * Users should take care to list creep models first and plasticity
   * models last to allow for the case when a creep model relaxes the stress state
   * inside of the yield surface in an iteration.
   */
  std::vector<StressUpdateBase *> _models;
};

#endif // COMPUTEMULTIPLEINELASTICSTRESS_H
