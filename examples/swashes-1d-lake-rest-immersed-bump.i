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

  [./q_x]
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

  [./v_x]
  [../]

  [./h_plus_b]
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

  [./q_x_bathymetry]
    type = SVBathymetry
    variable = q_x
    h = h
    b = grad_b
    component = 0
  [../]
[]

[AuxKernels]
  [./b_kernel]
    type = FunctionAux
    variable = b
    function = b_func
  [../]

  [./grad_b_kernel]
    type = FunctionAux
    variable = grad_b
    function = grad_b_func
  [../]

  [./v_x_kernel]
    type = ParsedAux
    variable = v_x
    function = 'q_x / h'
    args = 'q_x h'
  [../]

  [./h_plus_b_kernel]
    type = ParsedAux
    variable = h_plus_b
    function = 'h + b'
    args = 'h b'
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
    type = DirichletBC
    variable = h
    boundary = left
    value = 0.5
  [../]

  [./BC_h_right]
    type = DirichletBC
    variable = h
    boundary = right
    value = 0.5
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
  petsc_options_iname = '-pc_type'
  petsc_options_value = 'lu'

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
