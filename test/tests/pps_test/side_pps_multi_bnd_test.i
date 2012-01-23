#
# Tests elemental PPS running on multiple block
#

[Mesh]
	type = StripeMesh
	dim = 2
	xmin = 0
	xmax = 1
	ymin = 0
	ymax = 1
	nx = 3
	ny = 3
	elem_type = QUAD4
	
	stripes = 3
[]

[Functions]
	[./forcing_fn]
		type = ParsedFunction
		value = x*(y+1)
	[../
[]

[Variables]
	[./u]
		family = MONOMIAL
		order = CONSTANT
	[../]
[]

[Kernels]
	[./uv]
		type = Reaction
		variable = u
	[../]
	
	[./fv]
		type = UserForcingFunction
		variable = u
		function = forcing_fn
	[../]
[]

[Postprocessors]
	[./int_0_1]
		type = SideIntegral
		variable = u
		boundary = '0 1'
	[../]
[]

[Executioner]
	type = Steady
[]

[Output]
	exodus = true
[]
