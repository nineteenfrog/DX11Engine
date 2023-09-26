#include "Camera.h"
#include "Input.h"

Camera::Camera(
	float x, float y, float z, 
	float moveSpeed, 
	float mouslookSpeed, 
	float fov, 
	float aspectRatio)
	:
	moveSpeed(moveSpeed)//sets member variables easily
{
	//setposition

	//update matrices

	//XMLoadFloat3 //grabs them as math type
}

/*
XMMatrix proj = SMMatrixPerspectiveFovLH(
fov,
aspectratio,
0.01f,  //near clip dist
1000.0f); //far clip dist
*/


//update constant buffer 
//add new view and projection matrix in shader and buffer sturct


//getkeyboard input
/*
	(in update)
	Input& input = Input::GetInstance();

	if(input.KeyDown('W')){transform.moveRelative(movement data here)}

	if(input.MouseLeftDown()){
		float xDiff = MouseLookSpeed * input.GetMousedXDelta();

		transform.rotate(xdiff,ydiff,0);
	}

*/