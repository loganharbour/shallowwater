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
    variable = h
    q_x = q_x
    q_y = q_y
  [../]

  [./h_artificial_viscosity]
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
    component = x
    h = h
    q_x = q_x
    q_y = q_y
  [../]

  [./q_x_pressure]
    type = SVPressure
    variable = q_x
    component = x
    h = h
  [../]

  [./q_x_artificial_viscosity]
    type = SVArtificialViscosity
    variable = q_x
  [../]

  [./q_y_time_derivative]
    type = TimeDerivative
    variable = q_y
  [../]

  [./q_y_advection]
    type = SVAdvection
    variable = q_y
    component = y
    h = h
    q_x = q_x
    q_y = q_y
  [../]

  [./q_y_pressure]
    type = SVPressure
    variable = q_y
    component = y
    h = h
  [../]

  [./q_y_artificial_viscosity]
    type = SVArtificialViscosity
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
    type = SolidWallBC
    variable = h
    equation = CONTINUITY
    h = h
    boundary = 'top left bottom right'
  [../]

  [./BC_qx]
    type = SolidWallBC
    variable = q_x
    equation = MOMENTUM
    component = x
    h = h
    boundary = 'top left bottom right'
  [../]

  [./BC_qy]
    type = SolidWallBC
    variable = q_y
    equation = MOMENTUM
    component = y
    h = h
    boundary = 'top left bottom right'
  [../]
[]

[Postprocessors]
  [./dt]
    type = TimeStepCFL
    h = h
    q_x = q_x
    q_y = q_y
    cfl = 0.25
  [../]
[]

[Executioner]
  type = Transient

  end_time = 100

  [./TimeStepper]
    type = PostprocessorDT
    postprocessor = dt
    dt = 1e-6
  [../]
[]

[Outputs]
  exodus = true
[]
