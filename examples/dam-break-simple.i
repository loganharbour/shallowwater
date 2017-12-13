[GlobalParams]
  g = 0.05
[]

[Mesh]
  type = GeneratedMesh
  dim = 2
  nx = 100
  ny = 100
  xmax = 4
  ymax = 4
[]

[Functions]
  [./initial_height]
    type = ParsedFunction
    value = '0.05 * (x < (2 + 1e-6)) + 0.01 * (x > (2 + 1e-6))'
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

  [./q_y]
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
    #implicit = false
    variable = h
    q_x = q_x
    q_y = q_y
  [../]

  [./h_artificial_viscosity]
    type = SVArtificialViscosity
    #implicit = false
    variable = h
  [../]

  [./q_x_time_derivative]
    type = TimeDerivative
    variable = q_x
  [../]

  [./q_x_advection]
    type = SVAdvection
    #implicit = false
    variable = q_x
    h = h
    q_x = q_x
    q_y = q_y
    component = 0
  [../]

  [./q_x_pressure]
    type = SVPressure
    #implicit = false
    variable = q_x
    h = h
    component = 0
  [../]

  [./q_x_artificial_viscosity]
    type = SVArtificialViscosity
    #implicit = false
    variable = q_x
  [../]

  [./q_y_time_derivative]
    type = TimeDerivative
    variable = q_y
  [../]

  [./q_y_advection]
    type = SVAdvection
    #implicit = false
    variable = q_y
    h = h
    q_x = q_x
    q_y = q_y
    component = 1
  [../]

  [./q_y_pressure]
    type = SVPressure
    #implicit = false
    variable = q_y
    h = h
    component = 1
  [../]

  [./q_y_artificial_viscosity]
    type = SVArtificialViscosity
    #implicit = false
    variable = q_y
  [../]
[]

[Materials]
  [./sv_material]
    type = SVMaterial
    viscosity_type = FIRST_ORDER
    h = h
    q_x = q_x
    q_y = q_y
  [../]
[]

[BCs]
  [./BC_h]
    type = NeumannBC
    variable = h
    boundary = 'top left bottom right'
  [../]

  [./BC_qx]
    type = DirichletBC
    variable = q_x
    boundary = 'top left bottom right'
    value = 0
  [../]

  [./BC_qy]
    type = DirichletBC
    variable = q_y
    boundary = 'top left bottom right'
    value = 0
  [../]
[]

[Postprocessors]
  [./dt]
    type = TimeStepCFL
    h = h
    q_x = q_x
    q_y = q_y
    cfl = 0.05
  [../]
[]

[Executioner]
  type = Transient

  #scheme = explicit-tvd-rk-2

  end_time = 200
  #num_steps = 50000
  #dt = 1e-3
  [./TimeStepper]
    type = PostprocessorDT
    postprocessor = dt
    dt = 1e-6
  [../]
[]

[Outputs]
  exodus = true
[]
