#include "../interface/demo.h"
#include "../core/drivers/events.h"

// ******************************************************************
#define MAX_SHADER_VARS 128	// Max shader variables that can be stored in one shader

typedef struct {
	char		name[128];
	GLint		loc;
	char		equation[1024];
	double		value;
	tExpression	eva;
} varFloat;				// Structure for a evaluation FLOAT

typedef struct {
	char		name[128];
	GLint		loc;
	char		equation[1024];
	float		value[2];
	tExpression	eva;
} varVec2;				// Structure for a evaluation VEC2

typedef struct {
	char		name[128];
	GLint		loc;
	char		equation[1024];
	float		value[3];
	tExpression	eva;
} varVec3;				// Structure for a evaluation VEC3

typedef struct {
	char		name[128];
	GLint		loc;
	char		equation[1024];
	float		value[4];
	tExpression	eva;
} varVec4;				// Structure for a evaluation VEC4

typedef struct {
	char		name[128];
	GLint		loc;
	int			texture;
} varSampler2D;				// Structure for a evaluation Sampler2D (TEXTURE)

typedef struct
	{
	char				m_name[128];
	GLint				m_ShaderUniformID;
	enum_sve_variable	m_SVEVariableID;
	}
script_variable_matrix_4x4;	// Structure for common matrixes (mat4)

typedef struct {
	char		name[128];
	GLint		loc;
	int			texture;
} varSamplerCube;				// Structure for a evaluation samplerCube (TEXTURE)

typedef struct {
	int							program;

	varFloat					vfloat[MAX_SHADER_VARS];
	int							vfloat_num;

	varVec2						vec2[MAX_SHADER_VARS];
	int							vec2_num;

	varVec3						vec3[MAX_SHADER_VARS];
	int							vec3_num;

	varVec4						vec4[MAX_SHADER_VARS];
	int							vec4_num;

	varSampler2D				sampler2D[MAX_SHADER_VARS];
	int							sampler2D_num;

	varSamplerCube				samplerCube[MAX_SHADER_VARS];
	int							samplerCube_num;

	script_variable_matrix_4x4	matrix4x4[MAX_SHADER_VARS];
	int							matrix4x4_num;

} glslshaderbind_section;

static glslshaderbind_section *local;

// ********** COMIENZO DEL CODIGO INCOMPRENSIBLE DE ISAAC ********** //
enum_sve_variable		get_sve_variable_id(const char* pString)
	{
	if (strcmp(pString, "World")==0)
		return sve_variable_matrix_world;
	else if (strcmp(pString, "View")==0)
		return sve_variable_matrix_view;
	else return sve_variable_unknown;
	}

const matrix_t*			get_sve_variable_matrix_4x4f(enum_sve_variable id) {
	return &demoSystem.m_VariableMatrix[id];
}

enum_sve_variable_type	get_sve_variable_type(enum_sve_variable id)
{
	static enum_sve_variable_type VariableType[MAX_SVE_VARIABLE_ID+1];
	// static bool initialized = 0;
	
	//if (!initialized)
	//{
		VariableType[sve_variable_unknown] = sve_variable_type_unknown;
		VariableType[sve_variable_matrix_world] = sve_variable_type_matrix_4x4f;
		VariableType[sve_variable_matrix_view] = sve_variable_type_matrix_4x4f;
		VariableType[sve_variable_matrix_projection] = sve_variable_type_matrix_4x4f;
		VariableType[sve_variable_matrix_world_view] = sve_variable_type_matrix_4x4f;
		VariableType[sve_variable_matrix_world_projection] = sve_variable_type_matrix_4x4f;
		//initialized = 1;
	//}
	
	return VariableType[id];
}

void invalidate_sve_variable(enum_sve_variable id)
{
	//SL_NOT_IMPLEMENTED;
}

void					set_sve_variable_matrix_4x4f(enum_sve_variable id, const matrix_t* pValue)
{
	//invalidate_sve_variable(
	memcpy(&demoSystem.m_VariableMatrix[id], pValue, sizeof(matrix_t));
	
	switch(id)
	{
		case sve_variable_matrix_world:
			invalidate_sve_variable(sve_variable_matrix_world_inverse);
			invalidate_sve_variable(sve_variable_matrix_world_transpose);
			break;
		case sve_variable_matrix_view:
			invalidate_sve_variable(sve_variable_matrix_view_inverse);
			invalidate_sve_variable(sve_variable_matrix_view_transpose);
			break;
		case sve_variable_matrix_projection:
			invalidate_sve_variable(sve_variable_matrix_projection_inverse);
			invalidate_sve_variable(sve_variable_matrix_projection_transpose);
			break;
	}
}


// ********** FIN DEL CODIGO INCOMPRENSIBLE DE ISAAC ********** //

// ******************************************************************

void preload_glslshaderbind () {}

// ******************************************************************

int max_shader_reached (int val)
{
	if (val>=(MAX_SHADER_VARS-1))
	{
		section_error("Too many variables! MAX_SHADER_VARS reached!! you need to recompile the engine or use less variables!");
		val = (MAX_SHADER_VARS-1);
	}
	return val;
}

// ******************************************************************

void remove_spaces(char* pString)
	{
	unsigned int i;
	unsigned int p = 0;

	if (pString == 0)
		return;

	for (i = 0; pString[i] != '\0'; ++i)
		{
		if (pString[i] != ' ')
			pString[p++] = pString[i];
		}

	pString[p] = '\0';
	}

void load_glslshaderbind () {

	int		i;
	int		num;
	char	string_name[128];
	char	string_type[128];
	char	string_value[512];
	
	varFloat*					vfloat;
	varVec2*					vec2;
	varVec3*					vec3;
	varVec4*					vec4;
	varSampler2D*				sampler2D;
	varSamplerCube*				samplerCube;
	script_variable_matrix_4x4*	pVariableMatrix4x4;

	// Exit if shaders not supported
	if (!glDriver.ext.glslshaders)
		return;
	
	// script validation
	// 2 strings needed: Vertex and fragment shader path
	if (mySection->stringNum < 2){
		section_error("At least 2 strings are needed: vertex and fragment shader files");
		return;
	}
	
	local = malloc(sizeof(glslshaderbind_section));
	mySection->vars = (void *) local;
	
	// load program, 2 first strings are vertex and fragment program paths
	local->program = glslshad_load(mySection->strings[0], mySection->strings[1]);
	if (local->program == -1)
		return;
	glslshad_upload(local->program);
	glslshad_bind(local->program);	// We need to use the program (bind it) in order to retrieve it locations (some drivers need it)
	
	// Reset variables
	local->vfloat_num		= 0;
	local->vec2_num			= 0;
	local->vec3_num			= 0;
	local->vec4_num			= 0;
	local->sampler2D_num	= 0;
	local->samplerCube_num	= 0;
	local->matrix4x4_num	= 0;
	num = 0;
	
	// Read the variables
	for (i=2; i<mySection->stringNum; i++)
	{
		sscanf ( mySection->strings[i], "%s %s %s", string_type, string_name, string_value);	// Read one string and store values on temporary strings for frther evaluation
		dkernel_trace("glslshaderbind: string_type [%s], string_name [%s], string_value [%s]", string_type, string_name, string_value);
		remove_spaces(string_value);

		// TODO: convert 'string_type' to low chars, to prevent errors (passar a minœscules)
		// TODO: Com ja obenim el location de la variable, no Žs necessari guardar el "name", o sigui que el podem treure i ens estalviarem mem˜ria
		if (strcmp(string_type,"float")==0)	// FLOAT detected
		{
			num = max_shader_reached ( local->vfloat_num++ );
			vfloat = &(local->vfloat[num]);
			strcpy (vfloat->name, string_name);
			strcpy (vfloat->equation, string_value);
			vfloat->loc = glslshad_getUniformLocation (local->program, vfloat->name);
			vfloat->eva.equation = vfloat->equation;
			initExpression(&vfloat->eva);		// Inits the evaluator
		}
		else if (strcmp(string_type,"vec2")==0)	// VEC2 detected
		{
			num = max_shader_reached ( local->vec2_num++ );
			vec2 = &(local->vec2[num]);
			strcpy (vec2->name, string_name);
			strcpy (vec2->equation, string_value);
			vec2->loc = glslshad_getUniformLocation (local->program, vec2->name);
			vec2->eva.equation = vec2->equation;
			initExpression(&vec2->eva);		// Inits the evaluator
		}
		else if (strcmp(string_type,"vec3")==0)	// VEC3 detected
		{
			num = max_shader_reached ( local->vec3_num++ );
			vec3 = &(local->vec3[num]);
			strcpy (vec3->name, string_name);
			strcpy (vec3->equation, string_value);
			vec3->loc = glslshad_getUniformLocation (local->program, vec3->name);
			vec3->eva.equation = vec3->equation;
			initExpression(&vec3->eva);		// Inits the evaluator
		}
		else if (strcmp(string_type,"vec4")==0)	// VEC4 detected
		{
			num = max_shader_reached ( local->vec4_num++ );
			vec4 = &(local->vec4[num]);
			strcpy (vec4->name, string_name);
			strcpy (vec4->equation, string_value);
			vec4->loc = glslshad_getUniformLocation (local->program, vec4->name);
			vec4->eva.equation = vec4->equation;
			initExpression(&vec4->eva);		// Inits the evaluator
		}
		else if (strcmp(string_type,"sampler2D")==0)	// Texture (sampler2D) detected
		{
			num = max_shader_reached ( local->sampler2D_num++ );
			sampler2D = &(local->sampler2D[num]);
			strcpy (sampler2D->name, string_name);
			if (0 == strncmp("rtt",string_value,3))	// Si volem que la textura sigui un buffer...
			{
				int rttnum;
				sscanf(string_value, "rtt%d",&rttnum);
				if (rttnum<0 || rttnum> (RENDERING_BUFFERS-1))
				{
					section_error("sampler2D buffer not correct, it should be 'rttX', where X=>0 and X<=%d, you chose: %s", (RENDERING_BUFFERS-1), string_value);
					return;
				}
				else
					sampler2D->texture = demoSystem.texRenderingBuffer[rttnum];
			}
			else if (0 == strncmp("fbo",string_value,3))	// Si volem que la textura sigui una fbo...
			{
				int fbonum;
				sscanf(string_value, "fbo%d",&fbonum);
				if (fbonum<0 || fbonum> (FBO_BUFFERS-1))
				{
					section_error("sampler2D fbo not correct, it should be 'fboX', where X=>0 and X<=%d, you chose: %s", (FBO_BUFFERS-1), string_value);
					return;
				}
				else
					sampler2D->texture = demoSystem.fboRenderingBuffer[fbonum];
			}
			else
			{
				sampler2D->texture = tex_load (string_value, USE_CACHE, 0);
				if (sampler2D->texture == -1)
					return;
				tex_properties(sampler2D->texture, NO_MIPMAP);
				tex_upload (sampler2D->texture, USE_CACHE);
			}
			sampler2D->loc = glslshad_getUniformLocation (local->program, sampler2D->name);
		}
		else if (strcmp(string_type,"samplerCube")==0)	// Texture (samplerCube) detected - TODO: Fix!! is this workingÀ?
		{
			num = max_shader_reached ( local->samplerCube_num++ );
			samplerCube = &(local->samplerCube[num]);
			strcpy (samplerCube->name, string_name);
			if (0 == strncmp("rtt",string_value,3))	// Si volem que la textura sigui una layer del sistema...
			{
				int rttnum;
				sscanf(string_value, "rtt%d",&rttnum);
				if (rttnum<0 || rttnum> (RENDERING_BUFFERS-1))
				{
					section_error("SamplerCube buffer not correct, it should be 'rttX', where X=>0 and X<=%d, you chose: %s", (RENDERING_BUFFERS-1), string_value);
					return;
				}
				else
					samplerCube->texture = demoSystem.texRenderingBuffer[rttnum];
			}
			else
			{
				samplerCube->texture = tex_load (string_value, USE_CACHE, 0);
				if (samplerCube->texture == -1)
					return;
				tex_properties(samplerCube->texture, NO_MIPMAP);
				tex_upload (samplerCube->texture, USE_CACHE);
			}
			samplerCube->loc = glslshad_getUniformLocation (local->program, samplerCube->name);
		}
		else if (strcmp(string_type,"mat4")==0)
			{
			num = max_shader_reached( local->matrix4x4_num++ );
			pVariableMatrix4x4 = &local->matrix4x4[num];
			strcpy(pVariableMatrix4x4->m_name, string_name);
				
			// remove the ';'
			if (string_value[0] != '\0')
				string_value[strlen(string_value)-1] = '\0';
				
			pVariableMatrix4x4->m_SVEVariableID = get_sve_variable_id(string_value);
			pVariableMatrix4x4->m_ShaderUniformID = glslshad_getUniformLocation(local->program, pVariableMatrix4x4->m_name);
				
			// check whether the requested SVE Matrix4x4 variable exists
			if (get_sve_variable_type(pVariableMatrix4x4->m_SVEVariableID) != sve_variable_type_matrix_4x4f)
				{
				section_error("\"%s\" is not a valid SVE Engine Matrix4x4 variable", string_value);
				return;
				}
			}
	}
	
	// Unbind any shader used
	glslshad_reset_bind();
	glUseProgramObjectARB(0);
	mySection->loaded=1;
}

// ******************************************************************

void init_glslshaderbind()
	{
	}

// ************************************************************

void render_glslshaderbind()
	{
	varFloat*		vfloat;
	varVec2*		vec2;
	varVec3*		vec3;
	varVec4*		vec4;
	varSampler2D*	sampler2D;
	varSamplerCube*	samplerCube;
	int				i;
	double			d;

	// Exit if shaders not supported
	if (!glDriver.ext.glslshaders)
		return;

	local = (glslshaderbind_section*) mySection->vars;

	// Choose the proper program shader
	glslshad_bind(local->program);
	
	for (i = 0; i < local->vfloat_num; i++)
		if (local->vfloat[i].loc>-1)
			{
			vfloat = &(local->vfloat[i]);
			insertSectionVariables(&vfloat->eva);
			vfloat->eva.err = exprEval(vfloat->eva.o, &vfloat->eva.result);	// Evaluate the equations
			// Check for errors
			if(vfloat->eva.err != EXPR_ERROR_NOERROR) section_error("glslshaderbind: [vfloat] Expression evaluation Error (%d): %s", vfloat->eva.err, vfloat->eva.equation);
			// Retrieve the values and assign them
			exprValListGet(vfloat->eva.v, "v1", &vfloat->value);
			glUniform1f(vfloat->loc, (float)vfloat->value);
			}

	for (i = 0; i < local->vec2_num; i++)
		if (local->vec2[i].loc>-1)
			{
			vec2 = &(local->vec2[i]);
			insertSectionVariables(&vec2->eva);
			vec2->eva.err = exprEval(vec2->eva.o, &vec2->eva.result);		// Evaluate the equations
			// Check for errors
			if(vec2->eva.err != EXPR_ERROR_NOERROR) section_error("glslshaderbind: [vec2] Expression evaluation Error (%d): %s", vec2->eva.err, vec2->eva.equation);
			// Retrieve the values and assign them
			exprValListGet(vec2->eva.v, "v1", &d); vec2->value[0] = (float)d;
			exprValListGet(vec2->eva.v, "v2", &d); vec2->value[1] = (float)d;
			glUniform2fv(vec2->loc, 2, (GLfloat*)vec2->value);
			}
	
	for (i = 0; i < local->vec3_num; i++)
		if (local->vec3[i].loc>-1)
			{
			vec3 = &(local->vec3[i]);
			insertSectionVariables(&vec3->eva);
			vec3->eva.err = exprEval(vec3->eva.o, &vec3->eva.result);		// Evaluate the equations
			// Check for errors
			if(vec3->eva.err != EXPR_ERROR_NOERROR) section_error("glslshaderbind: [vec3] Expression evaluation Error (%d): %s", vec3->eva.err, vec3->eva.equation);
			// Retrieve the values and assign them
			exprValListGet(vec3->eva.v, "v1", &d); vec3->value[0] = (float)d;
			exprValListGet(vec3->eva.v, "v2", &d); vec3->value[1] = (float)d;
			exprValListGet(vec3->eva.v, "v3", &d); vec3->value[2] = (float)d;
			glUniform3fv(vec3->loc, 3, (GLfloat*)vec3->value);
			}

	for (i = 0; i < local->vec4_num; i++)
		if (local->vec4[i].loc>-1)
			{
			vec4 = &(local->vec4[i]);
			insertSectionVariables(&vec4->eva);
			vec4->eva.err = exprEval(vec4->eva.o, &vec4->eva.result);		// Evaluate the equations
			// Check for errors
			if(vec4->eva.err != EXPR_ERROR_NOERROR) section_error("glslshaderbind: [vec4] Expression evaluation Error (%d): %s", vec4->eva.err, vec4->eva.equation);
			// Retrieve the values and assign them
			exprValListGet(vec4->eva.v, "v1", &d); vec4->value[0] = (float)d;
			exprValListGet(vec4->eva.v, "v2", &d); vec4->value[1] = (float)d;
			exprValListGet(vec4->eva.v, "v3", &d); vec4->value[2] = (float)d;
			exprValListGet(vec4->eva.v, "v4", &d); vec4->value[3] = (float)d;
			glUniform4fv(vec4->loc, 4, (GLfloat*)vec4->value);
			}

	for (i = 0; i < local->matrix4x4_num; ++i)
		{
		if (local->matrix4x4[i].m_ShaderUniformID>-1)
			{
			glUniformMatrix4fv
				(
				// Shader Uniform Variable ID
				local->matrix4x4[i].m_ShaderUniformID,
				// 4x4 floats, 16 elements
				16,
				// do not transpose
				0,
				// request to the engine the matrix value and pass it to the shader
				(GLfloat*)get_sve_variable_matrix_4x4f(local->matrix4x4[i].m_SVEVariableID)
				);
			}
		}

	for (i = local->sampler2D_num-1; i >= 0; i--)
		if (local->sampler2D[i].loc>-1)
			{
			sampler2D = &(local->sampler2D[i]);
			glActiveTexture (GL_TEXTURE0 + i);
			tex_bind (sampler2D->texture);
			glUniform1i(sampler2D->loc, (GLuint)i);
			}
		
	for (i = local->samplerCube_num-1; i >= 0; i--)
		if (local->samplerCube[i].loc>-1)
			{
			samplerCube = &(local->samplerCube[i]);
			glActiveTexture (GL_TEXTURE0 + i);
			tex_bind (samplerCube->texture);
			glUniform1i(samplerCube->loc, (GLuint)i);
			}
	}

// ******************************************************************

void end_glslshaderbind () {
}
