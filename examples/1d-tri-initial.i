[Mesh]
  type = GeneratedMesh
  dim = 1
  nx = 100
  xmax = 1
[]

[Functions]
  [./initial_height]
    type = ParsedFunction
    value = 'x'
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

  [./q_x]
    family = LAGRANGE
    order = FIRST
    [./InitialCondition]
      type = ConstantIC
      value = 0
    [../]
  [../]
[]

[Kernels]
  [./h_time_derivative]
    type = TimeDerivative
    variable = h
  [../]

  [./h_continuity]
    type = SVContinuity
    variable = h
    q_x = q_x
  [../]

  [./h_viscosity_continuity]
    type = SVArtificialViscosity
    variable = h
  [../]

  [./q_x_time_derivative]
    type = TimeDerivative
    variable = q_x
  [../]

  [./q_x_advection]
    type = SVAdvection
    variable = q_x
    h = h
    q_x = q_x
    component = 0
  [../]

  [./q_x_pressure]
    type = SVPressure
    variable = q_x
    h = h
    component = 0
  [../]

  [./q_x_viscosity_continuity]
    type = SVArtificialViscosity
    variable = q_x
  [../]
[]

[Materials]
  [./sv_material]
    type = SVMaterial
    viscosity_type = FIRST_ORDER
    h = h
    q_x = q_x
  [../]
[]

[BCs]
  [./BC_h_left]
    type = NeumannBC
    variable = h
    boundary = left
  [../]

  [./BC_h_right]
    type = NeumannBC
    variable = h
    boundary = right
  [../]

  [./BC_q_x_left]
    type = DirichletBC
    variable = q_x
    boundary = left
    value = 0
  [../]

  [./BC_q_x_right]
    type = DirichletBC
    variable = q_x
    boundary = right
    value = 0
  [../]
[]

[Postprocessors]
  [./dt]
    type = TimeStepCFL
    h = h
    q_x = q_x
  [../]

  [./h_integral]
    type = ElementIntegralVariablePostprocessor
    variable = h
  [../]
[]

[Executioner]
  type = Transient

  end_time = 10

  [./Quadrature]
    type = GAUSS
    order = SECOND
  [../]

  [./TimeIntegrator]
    type = CrankNicolson
  [../]

  [./TimeStepper]
    type = PostprocessorDT
    postprocessor = dt
    dt = 1e-6
  [../]
[]

[Outputs]
  exodus = true
[]
