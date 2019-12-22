#include "Geometry.hpp"


//
//  函数: ComputeModelMatrix(const mat& scale,const mat& rotate,const mat& move)
//
//  目标: 计算模型空间到世界空间的转换矩阵。
//
//    scale	   -  (Sx,Sy,Sz)  x,y,z 轴的缩放倍数		
//    rotate   -  (Rx,Ry,Rz)  x,y,z 轴的旋转度数			
//    move     -  (Dx,Dy,Dz)  x,y,z 轴的原点偏移	
//
mat Geometry::ComputeModelMatrix(const SenceParameters& m)
{
	/*
	  顶点缩放，表示为
			  ——							 ——				——
			  |	x_World = k1 * x_Model;		 |  k1  0	0	 |
		S =   | y_World = k2 * y_Model;  =》 |  0	k2	0	 |
			  | z_World = k3 * z_Model;		 |  0   0	k3	 |
			  ——							 ——				——

	  位移，表示为


			  ——							 ——				  ——
			  |	x_World = d1 + x_Model;		 |  1   0	0	d1 |
	    M =   | y_World = d2 + y_Model;  =》 |  0	1	0	d2 |
			  | z_World = d3 + z_Model;		 |  0   0	1   d3 |
			  |								 |	0   0   0   1  |
			  ——							 ——				 ——

	 旋转，假设在x-y坐标系下，线段长度为r，它起点在原点，终点为p1,与x轴夹角为 a1,
	 旋转角为a2,终点为p2。设旋转点为原点。

	 那么: 设 p1坐标为 (x1,y1)
			 p2 坐标为(x2,y2)

			 x1 = r* cosa1;
			 y1 = r* sina1;

			 x2 = r*cos(a1+a2);
			 y2 = r*sin(a1+a2);

			 展开有：
				x2 = x1* cosa2 - y1 * sina2;
				y2 = x1* sina2 + y1 * cosa2;

			实际上，z轴可视为垂直于x-y片面指向外，因此，这就是绕z轴的旋转方程。
			同理，见 x-y 替换成 x-z 和 y-z即可得到 绕y轴和绕 x轴的旋转方程。

			提取矩阵 Rx,Ry,Rz.
			那么旋转矩阵就为

			R = RzRyRx;		
			不考虑欧拉角万向节锁
		
		此时，模型矩阵	Model =  Move * Rotate * Scale 补齐为齐次矩阵

		注意表面法线的转换
			若包含了非均匀缩放操作，那么它的转换矩阵
			应该为Model逆的转置。
		证明在 https://zhuanlan.zhihu.com/p/72734738 别人写的
	*/
	// 1.计算缩放矩阵
	mat ScaleMatrix = mat::identity(4);
	ScaleMatrix[0][0] = m.gMeshScale.x;
	ScaleMatrix[1][1] = m.gMeshScale.y;
	ScaleMatrix[2][2] = m.gMeshScale.z;

	// 2.计算位移矩阵
	mat MoveMatrix = mat::identity(4);
	MoveMatrix[0][3] = m.gMeshMove.x;
	MoveMatrix[1][3] = m.gMeshMove.y;
	MoveMatrix[2][3] = m.gMeshMove.z;

	// 3.计算旋转矩阵
	mat RotateMatrix = mat::identity(4);
	
	float cosX = std::cos(m.gMeshRotate.x * RtoPi);
	float sinX = std::sin(m.gMeshRotate.x * RtoPi);

	float cosY = std::cos(m.gMeshRotate.y *RtoPi);
	float sinY = std::sin(m.gMeshRotate.y * RtoPi);

	float cosZ = std::cos(m.gMeshRotate.z * RtoPi);
	float sinZ = std::sin(m.gMeshRotate.z * RtoPi);

	RotateMatrix[0][0] = cosY*cosZ;
	RotateMatrix[0][1] = -cosX*sinZ + sinX*sinY*cosZ;
	RotateMatrix[0][2] = sinX*sinZ + cosX*sinY*cosZ;

	RotateMatrix[1][0] = cosY*sinZ;
	RotateMatrix[1][1] = cosX*cosZ + sinX*sinY*sinZ;
	RotateMatrix[1][2] = -sinX*cosZ + cosX*sinY*sinZ;

	RotateMatrix[2][0] = -sinY;
	RotateMatrix[2][1] = sinX*cosY;
	RotateMatrix[2][2] = cosX*cosY;

	return MoveMatrix * RotateMatrix * ScaleMatrix;
}

//
//  函数: ComputeViewMatrix(const vec3f&,const vec3f&,const vec3f)
//
//  目标: 计算视见转换矩阵。
//
//    eye	 -  (Ex,Ey,Ez)  摄像机在世界空间下的坐标	
//    at	 -  (Ax,Ay,Az)  摄像机指向的参考点		
//    up     -  (Dx,Dy,Dz)  相机上方向量，通常定义为y轴
//
//	对应OpenGL的LookAt函数
//  分析在我的文章里有
//
mat Geometry::ComputeViewMatrix(const SenceParameters& m)
{
	// 1.施密特正交化
	vec3f n = (m.gEye - m.gAt).normalize();  //z
	vec3f u = (m.gUp ^ n).normalize();	   //x
	vec3f v = (n ^ u).normalize();	   //y

	mat viewMat = mat::identity(4);
	viewMat[0][0] = u.x;
	viewMat[0][1] = u.y;
	viewMat[0][2] = u.z;
	viewMat[0][3] =  -(m.gEye * u);

	viewMat[1][0] = v.x;
	viewMat[1][1] = v.y;
	viewMat[1][2] = v.z;
	viewMat[1][3] =  -(m.gEye * v);

	viewMat[2][0] = n.x;
	viewMat[2][1] = n.y;
	viewMat[2][2] = n.z;
	viewMat[2][3] =  -(m.gEye * n);

	return viewMat;
}


//
//  函数: ComputeProjectMatrix(float,float,float,float)
//
//  目标: 计算投影矩阵矩阵。
//
//    fovy		-  截平面 y-z 两视锥体连线夹角（角度）	
//    aspect	-  投影平面宽高比	
//    farZ      -  视锥体最远Z平面于原点距离
//	  nearZ		-  视锥体最近Z平面与原点距离
//
mat Geometry::ComputeProjectMatrix(const SenceParameters& m)
{
	mat projMat = mat::identity(4);
	// cot(fovy* pi / 360)
	float cotFovyDiv2 = 1.f / tan(m.gFovy*RtoPi / 2.f);

	//使用左手坐标系
	projMat[0][0] = cotFovyDiv2 / m.gAspect;
	projMat[1][1] = cotFovyDiv2;
	projMat[2][2] = m.gFarZ / (m.gFarZ - m.gNearZ);
	projMat[2][3] = m.gNearZ*m.gFarZ / (m.gFarZ - m.gNearZ);
	projMat[3][2] = -1;
	projMat[3][3] = 0;

	return projMat;
}


//
//  函数: ComputeViewportMatrix(float,float,float,float)
//
//  目标: 计算视口变换矩阵。
//
//    minX		-  屏幕左上角与渲染画布左上角顶点的x距离
//    minY		-  屏幕左上角与渲染画布左上角顶点的y距离
//    minZ      -  NDC矩形盒最小Z深度 （0）
//	  maxZ		-  NDC矩形盒最大Z深度 （1）
//    gwidth/gheight	屏幕宽高
//
mat Geometry::ComputeViewportMatrix(float minX,float minY,float maxZ,float minZ,int gWidth,int gHeight)
{
	mat viewport = mat::identity(4);
	viewport[0][0] = gWidth/2.f;
	viewport[1][1] = gHeight/2.f;
	viewport[2][2] = maxZ - minZ;
	viewport[0][3] = minX + gWidth/2;
	viewport[1][3] = minY + gHeight/2;
	viewport[2][3] = minZ;
	return viewport;
}



// 计算MVP矩阵
mat Geometry::ComputeMVP(const SenceParameters& sence)
{
	mat M = ComputeModelMatrix(sence);
	mat V = ComputeViewMatrix(sence);
	mat P = ComputeProjectMatrix(sence);

	return P * V * M;
}

