#pragma once
#include "Maths.hpp"

struct SenceParameters
{
	//模型属性
	vec3f gMeshMove;	 // 模型在世界空间下偏移
	vec3f gMeshRotate;	 // 模型旋转值
	vec3f gMeshScale;	 // 模型缩放值

	//灯光属性
	vec3f gLight;		//灯光位置

	// 相机属性
	vec3f gEye;			//相机在世界空间下的位置
	vec3f gAt;			//相机视点
	vec3f gUp;			//相机向上方向向量
	float gAspect;		//宽高比
	float gFovy;		//视野
	float gFarZ;		//近平面
	float gNearZ;		//远平面
};



namespace Geometry
{
	mat ComputeModelMatrix(const SenceParameters&);
	mat ComputeViewMatrix(const SenceParameters&);
	mat ComputeProjectMatrix(const SenceParameters&);	
	mat ComputeViewportMatrix(float,float,float,float,int,int);
	
	mat ComputeMVP(const SenceParameters&);
	
}

