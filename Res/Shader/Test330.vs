#version 430
layout(location=0)in vec4 position;

layout(binding=0)uniform AliceBuiltinSceneUniforms{
    mat4 ViewMatrix;
    mat4 ProjectionMatrix;
}U_SceneUniforms;

layout(binding=1)uniform AliceBuiltinPrimitiveUniforms{
    mat4 ModelMatrix;
}U_PrimitiveUniforms;

void main(){
    gl_Position=U_SceneUniforms.ProjectionMatrix*U_SceneUniforms.ViewMatrix*U_PrimitiveUniforms.ModelMatrix*position;
}