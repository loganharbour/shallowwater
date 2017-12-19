# Case 13.2 from
# "Finite Volume Methods for Hyperbolic Equations" by Leveque
# (doc/refs/Leveque_book.pdf)
#
# We have a domain [-5,5] without bathymetry.
#
# The intial height is
#   h(x) = 3, x < 0,
#          1, x > 0
# with a zero-flow initial condition of
#   q(x) = 0.
#
# The zero-flow at the boundary is enforced by
#   q(-5) = q(5) = 0.
#
# Gravity, as used in the book, is g = 1.
#
# Exact solutions are given by the Clawpack software package at
# t = [0.2, 0.4, 0.6, 0.8, 1.0, 1.2, 1.4, 1.6, 1.8, 2.0].

[GlobalParams]
  g = 1.0
  implicit = false
[]

[Mesh]
  type = GeneratedMesh
  dim = 1
  nx = 5000
  xmin = -5
  xmax = 5
[]

[Functions]
  [./initial_height]
    type = ParsedFunction
    value = '3 * (x < 0) + (x > 0)'
  [../]
[]

[Variables]
  [./h]
    family = LAGRANGE
    order = FIRST
    [./InitialCondition]
      type = FunctionIC
      function = initial_height
    [../]
  [../]

  [./q]
    family = LAGRANGE
    order = FIRST
    [./InitialCondition]
      type = ConstantIC
      value = 0
    [../]
  [../]
[]

[AuxVariables]
  [./v]
  [../]
[]

[Kernels]
  [./h_time_derivative]
    type = TimeDerivative
    implicit = true
    variable = h
  [../]

  [./h_continuity]
    type = SVContinuity
    variable = h
    q_x = q
  [../]

  [./h_artificial_viscosity]
    type = SVArtificialViscosity
    variable = h
  [../]

  [./q_time_derivative]
    type = TimeDerivative
    implicit = true
    variable = q
  [../]

  [./q_advection]
    type = SVAdvection
    variable = q
    h = h
    q_x = q
    component = x
  [../]

  [./q_pressure]
    type = SVPressure
    variable = q
    h = h
    component = x
  [../]

  [./q_artificial_viscosity]
    type = SVArtificialViscosity
    variable = q
  [../]
[]

[AuxKernels]
  [./v_kernel]
    type = ParsedAux
    variable = v
    function = 'q / h'
    args = 'q h'
  [../]
[]

[Materials]
  [./sv_material]
    type = SVMaterial
    viscosity_type = FIRST_ORDER
    h = h
    q_x = q
  [../]
[]

[BCs]
  [./BC_q_left]
    type = DirichletBC
    variable = q
    boundary = left
    value = 0
  [../]

  [./BC_q_right]
    type = DirichletBC
    variable = q
    boundary = right
    value = 0
  [../]
[]

[Postprocessors]
  [./dt]
    type = TimeStepCFL
    h = h
    q_x = q
    cfl = 0.25
  [../]
[]

[Executioner]
  type = Transient

  solve_type = LINEAR
  scheme = explicit-euler
  l_tol = 1e-12
  end_time = 2

  [./TimeStepper]
    type = PostprocessorDT
    postprocessor = dt
    dt = 1e-6
  [../]
[]

[Outputs]
  exodus = true
[]
