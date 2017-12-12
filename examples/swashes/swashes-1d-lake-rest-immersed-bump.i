# *NOTE: Need to change once bathymetry hack is removed.
#
# Case 3.1.1 "Lake at rest with an immersed bump" from
# "SWASHES: a compilation of Shallow Water Analytic Solutiosn for Hydraulic
#  and Environmental Studies" by Delestre, et. al (doc/refs/SV_analytic.pdf)
#
# We have a domain of length 25 with bathymetry given by
#   b(x) = 0.2 - 0.05(x - 10)^2,  if 8 < x < 12,
#          0,                     otherwise.
#
# The topography is totally immersed such that the intial height is
#   h(x) = 0.5 - b(x),
# with a zero-flow initial condition of
#   q(x) = 0.
#
# The zero-flow at the boundary is enforced by
#   q(0) = q(25) = 0.
#
# As the initial conditions are the steady state, the solution is solved
# explicitly and the residuals are expected to be 0 at the first time step.

[Mesh]
  type = GeneratedMesh
  dim = 1
  nx = 500
  xmin = 0
  xmax = 25
[]

[Functions]
  [./initial_height]
    type = ParsedFunction
    value = '0.5 - (x > 8) * (x < 12) * (0.2 - 0.05 * (x - 10)^2)'
  [../]

  [./b_func]
    type = ParsedFunction
    value = '(x > 8) * (x < 12) * (0.2 - 0.05 * (x - 10)^2)'
  [../]

  [./grad_b_func]
    type = ParsedFunction
    value = '(x > 8) * (x < 12) * (1 - 0.1 * x)'
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
  [./b]
    [./InitialCondition]
      type = FunctionIC
      function = b_func
    [../]
  [../]

  [./grad_b]
    [./InitialCondition]
      type = FunctionIC
      function = grad_b_func
    [../]
  [../]

  [./v]
  [../]

  [./h_plus_b]
  [../]

  [./h_residual]
  [../]

  [./q_residual]
  [../]
[]

[Kernels]
  [./h_time_derivative]
    type = TimeDerivative
    variable = h
  [../]

  [./h_continuity]
    type = SVContinuity
    implicit = false
    variable = h
    q_x = q
  [../]

  [./q_time_derivative]
    type = TimeDerivative
    variable = q
  [../]

  [./q_advection]
    type = SVAdvection
    implicit = false
    variable = q
    h = h
    q_x = q
    component = 0
  [../]

  [./q_pressure]
    type = SVPressure
    implicit = false
    variable = q
    h = h
    component = 0
  [../]

  [./q_bathymetry]
    type = SVBathymetry
    implicit = false
    variable = q
    h = h
    b = grad_b
    component = 0
  [../]
[]

[AuxKernels]
  [./v_kernel]
    type = ParsedAux
    variable = v
    function = 'q / h'
    args = 'q h'
  [../]

  [./h_plus_b_kernel]
    type = ParsedAux
    variable = h_plus_b
    function = 'h + b'
    args = 'h b'
  [../]

  [./h_residual_kernel]
    type = DebugResidualAux
    variable = h_residual
    debug_variable = h
  [../]

  [./q_residual_kernel]
    type = DebugResidualAux
    variable = q_residual
    debug_variable = q
  [../]
[]

[Materials]
  [./sv_material]
    type = SVMaterial
    viscosity_type = NONE
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

[Executioner]
  type = Transient

  scheme = explicit-euler
  solve_type = LINEAR

  num_steps = 10
  dt = 1e-6
[]

[Outputs]
  exodus = true
[]
