#pragma once

class oldcube {
public:
	float verts[48] = {
			//Pos				//color
		//1
			-0.5f,  0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
		//2	    
			-0.5f,  0.5f,  0.5f, 0.5f, 0.5f, 0.5f,
		//3	    
			0.5f ,  0.5f,  0.5f, 0.5f, 0.5f, 0.5f,
		//4	    
			0.5f ,  0.5f, -0.5f, 0.5f, 0.5f, 0.5f,
		//5
			-0.5f, -0.5f, -0.5f, 1.0f, 0.5f, 0.5f,
		//6	   
			-0.5f, -0.5f,  0.5f, 0.5f, 0.5f, 0.5f,
		//7	   
			0.5f , -0.5f,  0.5f, 0.5f, 0.5f, 0.5f,
		//8	   
			0.5f , -0.5f, -0.5f, 0.5f, 0.5f, 0.5f
		};
	unsigned int index[36] = {
		//top
		0, 1, 3,
		1, 2, 3,
		//front
		1, 2, 6,
		1, 5, 6,
		//right
		3, 2, 6,
		3, 7, 6,
		//left
		1, 0, 4,
		1, 5, 4,
		//bottom
		6, 5, 4,
		6, 7, 4,
		//back
		3, 0, 4,
		3, 7, 4
	};
};
class newcube {

};
class testTriangle {
public:
	float verts[18] = {
		// positions			// colors
		 0.000f,  0.1f, 0.0f,	1.0f, 0.0f, 0.0f,   // top
		-0.125f, -0.1f, 0.0f,	0.0f, 1.0f, 0.0f,   // left
		 0.125f, -0.1f, 0.0f,	0.0f, 0.0f, 1.0f    // right 
	};
	testTriangle() {

	};
	testTriangle(float color[], float scale = 1.0f) {
		verts[0] *= scale;
		verts[1] *= scale;
		verts[2] *= scale;

		verts[3] = color[0];
		verts[4] = color[1];
		verts[5] = color[2];

		verts[6] *= scale;
		verts[7] *= scale;
		verts[8] *= scale;

		verts[9] = color[0];
		verts[10] = color[1];
		verts[11] = color[2];

		verts[12] *= scale;
		verts[13] *= scale;
		verts[14] *= scale;

		verts[15] = color[0];
		verts[16] = color[1];
		verts[17] = color[2];
	}
};
class BaseTriangle {

};